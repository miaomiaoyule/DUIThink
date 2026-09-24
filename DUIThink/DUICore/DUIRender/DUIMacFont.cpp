#include "StdAfx.h"
#include "DUIMacFont.h"

#if defined(__APPLE__)

#include <CoreText/CoreText.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <string.h>
#include <stdio.h>

enum { DuiMacFontPathMax = 16, DuiMacFontPathLen = 1024 };

struct tagDuiMacGlyphCache
{
	int x0 = 0;
	int y0 = 0;
	int w = 0;
	int h = 0;
	int nAdvance = 0;
	std::vector<unsigned char> vecBits;
};

struct tagDuiMacFont
{
	CTFontRef ctFont = NULL;
	int nPixelSize = 16;
	int nAscent = 12;
	std::unordered_map<unsigned, tagDuiMacGlyphCache> mapGlyph;
};

static int DuiMacCollectCjkFontPaths(char szPaths[][DuiMacFontPathLen], int nMax)
{
	// stb fallback only: prefer glyf/CFF faces it can actually InitFont.
	static const char *s_pszNames[] = {
		"Hiragino Sans GB",
		"HiraginoSansGB-W3",
		"STHeitiSC-Medium",
		"Songti SC",
		"Arial Unicode MS",
		"PingFangSC-Regular",
		"PingFang SC",
		"Heiti SC",
		NULL
	};

	int nCount = 0;
	for (int n = 0; s_pszNames[n] && nCount < nMax; ++n)
	{
		CFStringRef cfName = CFStringCreateWithCString(kCFAllocatorDefault, s_pszNames[n], kCFStringEncodingUTF8);
		if (NULL == cfName) continue;

		CTFontRef ctFont = CTFontCreateWithName(cfName, 32.0, NULL);
		CFRelease(cfName);
		if (NULL == ctFont) continue;

		CTFontDescriptorRef desc = CTFontCopyFontDescriptor(ctFont);
		CFRelease(ctFont);
		if (NULL == desc) continue;

		CFURLRef url = (CFURLRef)CTFontDescriptorCopyAttribute(desc, kCTFontURLAttribute);
		CFRelease(desc);
		if (NULL == url) continue;

		char szPath[DuiMacFontPathLen] = {};
		const Boolean bOk = CFURLGetFileSystemRepresentation(url, true, (UInt8 *)szPath, (CFIndex)sizeof(szPath));
		CFRelease(url);
		if (false == bOk || 0 == szPath[0]) continue;

		int nDup = 0;
		for (int i = 0; i < nCount; ++i)
		{
			if (0 == strcmp(szPaths[i], szPath)) { nDup = 1; break; }
		}
		if (nDup) continue;

		snprintf(szPaths[nCount], DuiMacFontPathLen, "%s", szPath);
		++nCount;
	}

	return nCount;
}

int DuiMacFindCjkFontPath(char *szPath, unsigned nSize, int nWhich)
{
	if (NULL == szPath || nSize < 8 || nWhich < 0) return 0;

	static char s_szPaths[DuiMacFontPathMax][DuiMacFontPathLen];
	static int s_nCount = -1;
	if (s_nCount < 0)
	{
		s_nCount = DuiMacCollectCjkFontPaths(s_szPaths, DuiMacFontPathMax);
	}
	if (nWhich >= s_nCount) return 0;

	snprintf(szPath, nSize, "%s", s_szPaths[nWhich]);
	return 1;
}

static bool DuiMacIsWindowsCjkName(const char *pszName)
{
	if (NULL == pszName || 0 == pszName[0]) return true;

	static const char *s_pszAlias[] = {
		"微软雅黑", "微软正黑体", "黑体", "宋体", "楷体", "仿宋", "等线",
		"Microsoft YaHei", "Microsoft JhengHei",
		"SimHei", "SimSun", "NSimSun", "KaiTi", "FangSong", "DengXian",
		"msyh", "simhei", "simsun",
		NULL
	};
	for (int i = 0; s_pszAlias[i]; ++i)
	{
		if (NULL != strcasestr(pszName, s_pszAlias[i])) return true;
	}
	return false;
}

static bool DuiMacIsFallbackFace(CTFontRef ctFont)
{
	if (NULL == ctFont) return true;

	CFStringRef ps = CTFontCopyPostScriptName(ctFont);
	if (NULL == ps) return true;

	char szPs[128] = {};
	CFStringGetCString(ps, szPs, sizeof(szPs), kCFStringEncodingUTF8);
	CFRelease(ps);

	return (0 == strcasecmp(szPs, "Helvetica")
		|| 0 == strcasecmp(szPs, "TimesNewRomanPSMT")
		|| 0 == strcasecmp(szPs, "LastResort")
		|| 0 == strcasecmp(szPs, ".LastResort")
		|| NULL != strcasestr(szPs, "LastResort"));
}

static CTFontRef DuiMacTryCreateNamed(const char *pszName, CGFloat fSize, const CGAffineTransform *pMatrix)
{
	if (NULL == pszName || 0 == pszName[0]) return NULL;

	CFStringRef cfName = CFStringCreateWithCString(kCFAllocatorDefault, pszName, kCFStringEncodingUTF8);
	if (NULL == cfName) return NULL;

	CTFontRef ctFont = CTFontCreateWithName(cfName, fSize, pMatrix);
	CFRelease(cfName);
	return ctFont;
}

static CTFontRef DuiMacCreateCTFont(const char *pszUtf8Face, int nPixelSize, int nWeight, int bItalic)
{
	const CGFloat fSize = (CGFloat)max(1, nPixelSize);
	CGAffineTransform mx = CGAffineTransformIdentity;
	if (bItalic) mx.c = 0.20;

	const char *pszNames[16] = {};
	int nNames = 0;
	auto add = [&](const char *psz)
	{
		if (NULL == psz || 0 == psz[0] || nNames >= 16) return;
		for (int i = 0; i < nNames; ++i)
		{
			if (0 == strcmp(pszNames[i], psz)) return;
		}
		pszNames[nNames++] = psz;
	};

	if (pszUtf8Face && pszUtf8Face[0] && false == DuiMacIsWindowsCjkName(pszUtf8Face))
	{
		add(pszUtf8Face);
	}

	if (nWeight >= 600)
	{
		add("PingFangSC-Semibold");
		add("PingFangSC-Medium");
		add("HiraginoSansGB-W6");
		add("STHeitiSC-Medium");
	}
	else if (nWeight >= 500)
	{
		add("PingFangSC-Medium");
		add("PingFangSC-Regular");
		add("HiraginoSansGB-W3");
		add("STHeitiSC-Medium");
	}
	else
	{
		add("PingFangSC-Regular");
		add("PingFang SC");
		add("HiraginoSansGB-W3");
		add("Hiragino Sans GB");
		add("STHeitiSC-Medium");
	}

	for (int i = 0; i < nNames; ++i)
	{
		CTFontRef ctFont = DuiMacTryCreateNamed(pszNames[i], fSize, bItalic ? &mx : NULL);
		if (NULL == ctFont) continue;

		const bool bRequested = (pszUtf8Face && 0 == strcmp(pszNames[i], pszUtf8Face));
		if (DuiMacIsFallbackFace(ctFont) && false == bRequested)
		{
			CFRelease(ctFont);
			continue;
		}
		return ctFont;
	}

	CFStringRef cfLang = CFStringCreateWithCString(kCFAllocatorDefault, "zh-Hans", kCFStringEncodingUTF8);
	CTFontRef ctSys = CTFontCreateUIFontForLanguage(kCTFontUIFontSystem, fSize, cfLang);
	if (cfLang) CFRelease(cfLang);
	if (ctSys && bItalic)
	{
		CTFontRef ctItalic = CTFontCreateCopyWithAttributes(ctSys, fSize, &mx, NULL);
		CFRelease(ctSys);
		return ctItalic;
	}
	return ctSys;
}

static CFIndex DuiMacCpToUnichar(unsigned cp, UniChar *pOut)
{
	if (NULL == pOut) return 0;
	if (cp >= 0x10000)
	{
		const unsigned n = cp - 0x10000;
		pOut[0] = (UniChar)(0xD800 + (n >> 10));
		pOut[1] = (UniChar)(0xDC00 + (n & 0x3FF));
		return 2;
	}
	pOut[0] = (UniChar)cp;
	return 1;
}

static CTFontRef DuiMacFontForCodepoint(CTFontRef ctPrimary, unsigned cp)
{
	if (NULL == ctPrimary) return NULL;

	UniChar u[2] = {};
	const CFIndex n = DuiMacCpToUnichar(cp, u);
	CGGlyph glyphs[2] = {};
	if (CTFontGetGlyphsForCharacters(ctPrimary, u, glyphs, n) && glyphs[0] != 0)
	{
		return (CTFontRef)CFRetain(ctPrimary);
	}

	CFStringRef cfStr = CFStringCreateWithCharacters(kCFAllocatorDefault, u, n);
	if (NULL == cfStr) return (CTFontRef)CFRetain(ctPrimary);

	CTFontRef ctFb = CTFontCreateForString(ctPrimary, cfStr, CFRangeMake(0, n));
	CFRelease(cfStr);
	if (NULL == ctFb) return (CTFontRef)CFRetain(ctPrimary);
	return ctFb;
}

static int DuiMacRasterize(tagDuiMacFont *pFont, unsigned cp, tagDuiMacGlyphCache &g)
{
	g = tagDuiMacGlyphCache();
	if (NULL == pFont || NULL == pFont->ctFont) return 0;

	CTFontRef ctUse = DuiMacFontForCodepoint(pFont->ctFont, cp);
	if (NULL == ctUse) return 0;

	UniChar u[2] = {};
	const CFIndex n = DuiMacCpToUnichar(cp, u);
	CGGlyph glyphs[2] = {};
	CTFontGetGlyphsForCharacters(ctUse, u, glyphs, n);
	const CGGlyph glyph = glyphs[0];

	CGSize adv = {};
	if (glyph != 0)
	{
		CTFontGetAdvancesForGlyphs(ctUse, kCTFontOrientationDefault, &glyphs[0], &adv, 1);
	}
	else
	{
		adv.width = (CGFloat)max(1, pFont->nPixelSize / 2);
	}
	g.nAdvance = (int)floor(adv.width + 0.5);
	if (g.nAdvance < 0) g.nAdvance = 0;

	if (0 == glyph)
	{
		CFRelease(ctUse);
		return 1;
	}

	CGRect bb = CTFontGetBoundingRectsForGlyphs(ctUse, kCTFontOrientationDefault, &glyphs[0], NULL, 1);
	if (bb.size.width < 0.01 || bb.size.height < 0.01)
	{
		CFRelease(ctUse);
		return 1;
	}

	const int nPad = 1;
	g.x0 = (int)floor(bb.origin.x) - nPad;
	g.y0 = (int)floor(-(bb.origin.y + bb.size.height)) - nPad;
	const int x1 = (int)ceil(bb.origin.x + bb.size.width) + nPad;
	const int y1 = (int)ceil(-bb.origin.y) + nPad;
	g.w = x1 - g.x0;
	g.h = y1 - g.y0;
	if (g.w < 1 || g.h < 1)
	{
		g.w = 0;
		g.h = 0;
		CFRelease(ctUse);
		return 1;
	}

	g.vecBits.assign((size_t)g.w * (size_t)g.h, 0);
	CGColorSpaceRef cs = CGColorSpaceCreateDeviceGray();
	CGContextRef ctx = CGBitmapContextCreate(g.vecBits.data(), (size_t)g.w, (size_t)g.h, 8, (size_t)g.w, cs,
		kCGImageAlphaNone);
	if (cs) CGColorSpaceRelease(cs);
	if (NULL == ctx)
	{
		g.vecBits.clear();
		g.w = 0;
		g.h = 0;
		CFRelease(ctUse);
		return 1;
	}

	CGContextSetShouldAntialias(ctx, true);
	CGContextSetShouldSmoothFonts(ctx, false);
	CGContextSetAllowsFontSmoothing(ctx, false);
	CGContextSetGrayFillColor(ctx, 1.0, 1.0);

	// CGBitmapContext data is top-down (row 0 = top), while the default CTM is
	// bottom-left origin. Draw with baseline at y1 from the bottom; do not flip
	// rows afterwards or glyphs appear upside-down on the canvas.
	const CGPoint pt = { (CGFloat)(-g.x0), (CGFloat)y1 };
	CTFontDrawGlyphs(ctUse, &glyphs[0], &pt, 1, ctx);
	CGContextFlush(ctx);
	CGContextRelease(ctx);
	CFRelease(ctUse);
	return 1;
}

static tagDuiMacGlyphCache *DuiMacEnsureGlyph(tagDuiMacFont *pFont, unsigned cp)
{
	if (NULL == pFont) return NULL;

	auto it = pFont->mapGlyph.find(cp);
	if (it != pFont->mapGlyph.end()) return &it->second;

	tagDuiMacGlyphCache g;
	if (0 == DuiMacRasterize(pFont, cp, g)) return NULL;

	auto ins = pFont->mapGlyph.emplace(cp, std::move(g));
	return &ins.first->second;
}

void *DuiMacCreateFont(const char *pszUtf8Face, int nPixelSize, int nWeight, int bItalic)
{
	CTFontRef ctFont = DuiMacCreateCTFont(pszUtf8Face, nPixelSize, nWeight, bItalic);
	if (NULL == ctFont) return NULL;

	tagDuiMacFont *pFont = new tagDuiMacFont();
	pFont->ctFont = ctFont;
	pFont->nPixelSize = max(1, nPixelSize);
	pFont->nAscent = (int)floor(CTFontGetAscent(ctFont) + 0.5);
	if (pFont->nAscent < 1) pFont->nAscent = max(1, (pFont->nPixelSize * 4) / 5);
	if (pFont->nAscent > pFont->nPixelSize - 1) pFont->nAscent = max(1, pFont->nPixelSize - 1);
	return pFont;
}

void *DuiMacCreateFontW(const wchar_t *pszFace, int nPixelSize, int nWeight, int bItalic)
{
	char szUtf8[256] = {};
	if (pszFace && pszFace[0])
	{
		const size_t nWide = wcslen(pszFace);
		CFStringRef cfFace = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)pszFace,
			nWide * sizeof(wchar_t), (sizeof(wchar_t) == 2) ? kCFStringEncodingUTF16LE : kCFStringEncodingUTF32LE, false);
		if (cfFace)
		{
			CFStringGetCString(cfFace, szUtf8, sizeof(szUtf8), kCFStringEncodingUTF8);
			CFRelease(cfFace);
		}
	}
	return DuiMacCreateFont(szUtf8, nPixelSize, nWeight, bItalic);
}

void DuiMacReleaseFont(void *pFont)
{
	tagDuiMacFont *pMac = (tagDuiMacFont *)pFont;
	if (NULL == pMac) return;
	if (pMac->ctFont) CFRelease(pMac->ctFont);
	delete pMac;
}

int DuiMacFontAscent(void *pFont)
{
	tagDuiMacFont *pMac = (tagDuiMacFont *)pFont;
	return pMac ? pMac->nAscent : 0;
}

int DuiMacGetAdvance(void *pFont, unsigned cp)
{
	tagDuiMacGlyphCache *pG = DuiMacEnsureGlyph((tagDuiMacFont *)pFont, cp);
	return pG ? pG->nAdvance : 0;
}

int DuiMacGetGlyph(void *pFont, unsigned cp, DuiMacGlyph *pOut)
{
	if (NULL == pOut) return 0;
	memset(pOut, 0, sizeof(*pOut));

	tagDuiMacGlyphCache *pG = DuiMacEnsureGlyph((tagDuiMacFont *)pFont, cp);
	if (NULL == pG) return 0;

	pOut->x0 = pG->x0;
	pOut->y0 = pG->y0;
	pOut->w = pG->w;
	pOut->h = pG->h;
	pOut->nAdvance = pG->nAdvance;
	pOut->pBits = pG->vecBits.empty() ? NULL : pG->vecBits.data();
	return 1;
}

#else

int DuiMacFindCjkFontPath(char *, unsigned, int)
{
	return 0;
}

void *DuiMacCreateFont(const char *, int, int, int)
{
	return NULL;
}

void *DuiMacCreateFontW(const wchar_t *, int, int, int)
{
	return NULL;
}

void DuiMacReleaseFont(void *)
{
}

int DuiMacFontAscent(void *)
{
	return 0;
}

int DuiMacGetAdvance(void *, unsigned)
{
	return 0;
}

int DuiMacGetGlyph(void *, unsigned, DuiMacGlyph *)
{
	return 0;
}

#endif
