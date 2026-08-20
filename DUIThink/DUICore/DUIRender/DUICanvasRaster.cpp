#include "StdAfx.h"
#include "DUICanvasRaster.h"

#if defined(DuiPlatform_SDL)

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "../../DUIUtils/stb_truetype.h"

#include <cmath>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//////////////////////////////////////////////////////////////////////////
static unsigned DuiNextCodepoint(LPCTSTR &s)
{
	if (NULL == s || 0 == *s) return 0;

#if defined(_WIN32) || defined(_WIN64)
	unsigned c = (unsigned)(unsigned short)*s++;
	if (c >= 0xD800 && c <= 0xDBFF && *s)
	{
		unsigned c2 = (unsigned)(unsigned short)*s;
		if (c2 >= 0xDC00 && c2 <= 0xDFFF)
		{
			++s;
			c = 0x10000 + ((c - 0xD800) << 10) + (c2 - 0xDC00);
		}
	}
	return c;
#else
	return (unsigned)(unsigned long)*s++;
#endif
}

static BYTE DuiClampByte(int n)
{
	if (n < 0) return 0;
	if (n > 255) return 255;
	return (BYTE)n;
}

static DWORD DuiPremulColor(DWORD dwColor, BYTE cbCoverage)
{
	const BYTE a = (BYTE)((DUIARGBGetA(dwColor) * cbCoverage + 127) / 255);
	const BYTE r = (BYTE)((DUIARGBGetR(dwColor) * a + 127) / 255);
	const BYTE g = (BYTE)((DUIARGBGetG(dwColor) * a + 127) / 255);
	const BYTE b = (BYTE)((DUIARGBGetB(dwColor) * a + 127) / 255);
	return DUIARGB(a, r, g, b);
}

static DWORD DuiLerpColor(DWORD c0, DWORD c1, float f)
{
	if (f <= 0.0f) return c0;
	if (f >= 1.0f) return c1;
	const int a = (int)(DUIARGBGetA(c0) + (DUIARGBGetA(c1) - DUIARGBGetA(c0)) * f);
	const int r = (int)(DUIARGBGetR(c0) + (DUIARGBGetR(c1) - DUIARGBGetR(c0)) * f);
	const int g = (int)(DUIARGBGetG(c0) + (DUIARGBGetG(c1) - DUIARGBGetG(c0)) * f);
	const int b = (int)(DUIARGBGetB(c0) + (DUIARGBGetB(c1) - DUIARGBGetB(c0)) * f);
	return DUIARGB(DuiClampByte(a), DuiClampByte(r), DuiClampByte(g), DuiClampByte(b));
}

static RECT DuiIntersectRect(const RECT &a, const RECT &b)
{
	RECT rc = {};
	IntersectRect(&rc, &a, &b);
	return rc;
}

static bool DuiPointInRoundRect(float x, float y, const RECT &rc, int rx, int ry)
{
	if (x < rc.left || x >= rc.right || y < rc.top || y >= rc.bottom) return false;
	rx = max(0, min(rx, (int)((rc.right - rc.left) / 2)));
	ry = max(0, min(ry, (int)((rc.bottom - rc.top) / 2)));
	if (rx <= 0 || ry <= 0) return true;

	float cx = x;
	float cy = y;
	if (x < rc.left + rx) cx = (float)(rc.left + rx);
	else if (x >= rc.right - rx) cx = (float)(rc.right - rx - 1);
	if (y < rc.top + ry) cy = (float)(rc.top + ry);
	else if (y >= rc.bottom - ry) cy = (float)(rc.bottom - ry - 1);

	if (cx == x || cy == y) return true;
	const float dx = (x - cx) / (float)rx;
	const float dy = (y - cy) / (float)ry;
	return dx * dx + dy * dy <= 1.0f;
}

static bool DuiPointInEllipse(float x, float y, const RECT &rc)
{
	const float cx = (rc.left + rc.right - 1) * 0.5f;
	const float cy = (rc.top + rc.bottom - 1) * 0.5f;
	const float rx = max(0.5f, (rc.right - rc.left) * 0.5f);
	const float ry = max(0.5f, (rc.bottom - rc.top) * 0.5f);
	const float dx = (x - cx) / rx;
	const float dy = (y - cy) / ry;
	return dx * dx + dy * dy <= 1.0f;
}

static float DuiDistToSegment(float x, float y, float x1, float y1, float x2, float y2)
{
	const float vx = x2 - x1;
	const float vy = y2 - y1;
	const float len2 = vx * vx + vy * vy;
	float t = 0.0f;
	if (len2 > 0.0001f)
	{
		t = ((x - x1) * vx + (y - y1) * vy) / len2;
		t = max(0.0f, min(1.0f, t));
	}
	const float px = x1 + vx * t - x;
	const float py = y1 + vy * t - y;
	return sqrtf(px * px + py * py);
}

//////////////////////////////////////////////////////////////////////////
struct tagDuiSharedFont
{
	std::vector<unsigned char> vecFile;
	stbtt_fontinfo Info = {};
	bool bReady = false;
};

static tagDuiSharedFont & DuiSharedFont()
{
	static tagDuiSharedFont s_Font;
	return s_Font;
}

static bool DuiLoadFontFile(const char *lpszPath, tagDuiSharedFont &Font)
{
	std::ifstream ifs(lpszPath, std::ios::binary);
	if (false == ifs.is_open()) return false;

	ifs.seekg(0, std::ios::end);
	const std::streamoff nSize = ifs.tellg();
	if (nSize <= 0) return false;
	ifs.seekg(0, std::ios::beg);

	Font.vecFile.resize((size_t)nSize);
	ifs.read((char *)Font.vecFile.data(), nSize);
	if ((std::streamoff)ifs.gcount() != nSize) return false;

	const int nOffset = stbtt_GetFontOffsetForIndex(Font.vecFile.data(), 0);
	if (0 == stbtt_InitFont(&Font.Info, Font.vecFile.data(), nOffset < 0 ? 0 : nOffset))
	{
		Font.vecFile.clear();
		return false;
	}

	Font.bReady = true;
	return true;
}

static bool DuiEnsureSharedFont()
{
	tagDuiSharedFont &Font = DuiSharedFont();
	if (Font.bReady) return true;

	static const char *s_pszFonts[] =
	{
#if defined(_WIN32) || defined(_WIN64)
		"C:\\Windows\\Fonts\\msyh.ttc",
		"C:\\Windows\\Fonts\\msyh.ttf",
		"C:\\Windows\\Fonts\\simhei.ttf",
		"C:\\Windows\\Fonts\\arialuni.ttf",
		"C:\\Windows\\Fonts\\arial.ttf",
#elif defined(__APPLE__)
		"/System/Library/Fonts/PingFang.ttc",
		"/System/Library/Fonts/STHeiti Light.ttc",
		"/System/Library/Fonts/Hiragino Sans GB.ttc",
		"/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
		"/Library/Fonts/Arial Unicode.ttf",
#elif defined(__ANDROID__)
		"/system/fonts/NotoSansCJK-Regular.ttc",
		"/system/fonts/NotoSansSC-Regular.otf",
		"/system/fonts/DroidSansFallback.ttf",
		"/system/fonts/NotoSansCJK-Regular.otf",
		"/system/fonts/Roboto-Regular.ttf",
#else
		"/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
		"/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
		"/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
		"/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc",
		"/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf",
		"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
		"/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
		"/usr/share/fonts/truetype/freefont/FreeSans.ttf",
#endif
		NULL
	};

	for (int n = 0; s_pszFonts[n]; ++n)
	{
		if (DuiLoadFontFile(s_pszFonts[n], Font)) return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
CDUIImageRaster::CDUIImageRaster(int nWidth, int nHeight, bool bHasAlpha)
	: m_nWidth(max(0, nWidth))
	, m_nHeight(max(0, nHeight))
	, m_bHasAlpha(bHasAlpha)
{
	m_nPitch = m_nWidth * 4;
	m_vecBits.assign((size_t)m_nPitch * (size_t)m_nHeight, 0);
}

CDUIImageRaster::~CDUIImageRaster()
{
}

int CDUIImageRaster::GetWidth() const { return m_nWidth; }
int CDUIImageRaster::GetHeight() const { return m_nHeight; }
LPBYTE CDUIImageRaster::GetBits() { return m_vecBits.empty() ? NULL : m_vecBits.data(); }
int CDUIImageRaster::GetPitch() const { return m_nPitch; }
bool CDUIImageRaster::HasAlpha() const { return m_bHasAlpha; }
void CDUIImageRaster::SetHasAlpha(bool bAlpha) { m_bHasAlpha = bAlpha; }

//////////////////////////////////////////////////////////////////////////
CDUIFontRaster::CDUIFontRaster()
{
}

CDUIFontRaster::~CDUIFontRaster()
{
}

CDUIFontRaster * CDUIFontRaster::Create(LPCTSTR lpszFace, int nPixelSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut)
{
	CDUIFontRaster *pFont = new CDUIFontRaster();
	pFont->Init(lpszFace, nPixelSize, lWeight, bItalic, bUnderline, bStrikeOut);
	return pFont;
}

bool CDUIFontRaster::Init(LPCTSTR, int nPixelSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut)
{
	m_nPixelSize = max(1, nPixelSize);
	m_lWeight = lWeight;
	m_bItalic = bItalic;
	m_bUnderline = bUnderline;
	m_bStrikeOut = bStrikeOut;

	DuiEnsureSharedFont();
	tagDuiSharedFont &Font = DuiSharedFont();
	if (Font.bReady)
	{
		m_fScale = stbtt_ScaleForPixelHeight(&Font.Info, (float)m_nPixelSize);
		int nAscent = 0, nDescent = 0, nLineGap = 0;
		stbtt_GetFontVMetrics(&Font.Info, &nAscent, &nDescent, &nLineGap);
		m_nAscent = (int)floorf(nAscent * m_fScale + 0.5f);
		m_nLineHeight = (int)floorf((nAscent - nDescent + nLineGap) * m_fScale + 0.5f);
		m_nLineHeight = max(m_nPixelSize, m_nLineHeight);
	}
	else
	{
		m_fScale = 1.0f;
		m_nAscent = m_nPixelSize;
		m_nLineHeight = m_nPixelSize + 2;
	}

	return true;
}

int CDUIFontRaster::GetPixelSize() const { return m_nPixelSize; }
int CDUIFontRaster::GetAscent() const { return m_nAscent; }
int CDUIFontRaster::GetLineHeight() const { return m_nLineHeight; }
float CDUIFontRaster::GetScale() const { return m_fScale; }
const void * CDUIFontRaster::GetFontInfo() const
{
	return DuiSharedFont().bReady ? &DuiSharedFont().Info : NULL;
}

SIZE CDUIFontRaster::MeasureText(LPCTSTR lpszText, int nMaxWidth, DWORD dwTextStyle) const
{
	SIZE sz = {};
	if (NULL == lpszText) return sz;

	const stbtt_fontinfo *pInfo = (const stbtt_fontinfo *)GetFontInfo();
	int nLineW = 0;
	int nLines = 1;
	LPCTSTR p = lpszText;
	int nLastAdvance = 0;
	while (unsigned cp = DuiNextCodepoint(p))
	{
		if (L'\n' == cp)
		{
			sz.cx = max(sz.cx, nLineW);
			nLineW = 0;
			nLastAdvance = 0;
			++nLines;
			continue;
		}

		int nAdvance = m_nPixelSize / 2;
		if (pInfo)
		{
			int nAdv = 0, nLsb = 0;
			stbtt_GetCodepointHMetrics(pInfo, (int)cp, &nAdv, &nLsb);
			nAdvance = (int)floorf(nAdv * m_fScale + 0.5f);
		}

		if ((dwTextStyle & DT_WORDBREAK) && nMaxWidth > 0 && nLineW + nAdvance > nMaxWidth && nLineW > 0)
		{
			sz.cx = max(sz.cx, nLineW);
			nLineW = 0;
			++nLines;
		}

		nLineW += nAdvance;
		nLastAdvance = nAdvance;
	}

	sz.cx = max(sz.cx, nLineW);
	sz.cy = nLines * m_nLineHeight;
	(void)nLastAdvance;
	return sz;
}

//////////////////////////////////////////////////////////////////////////
CDUICanvasRaster::CDUICanvasRaster(int nWidth, int nHeight)
{
	Resize(nWidth, nHeight);
}

CDUICanvasRaster::~CDUICanvasRaster()
{
}

int CDUICanvasRaster::GetWidth() const { return m_nWidth; }
int CDUICanvasRaster::GetHeight() const { return m_nHeight; }
LPBYTE CDUICanvasRaster::GetBits() { return m_vecBits.empty() ? NULL : m_vecBits.data(); }
int CDUICanvasRaster::GetPitch() const { return m_nPitch; }

bool CDUICanvasRaster::Resize(int nWidth, int nHeight)
{
	m_nWidth = max(1, nWidth);
	m_nHeight = max(1, nHeight);
	m_nPitch = m_nWidth * 4;
	m_vecBits.assign((size_t)m_nPitch * (size_t)m_nHeight, 0);
	m_vecClip.clear();

	ClipState State;
	State.rcClip = { 0, 0, m_nWidth, m_nHeight };
	m_vecClip.push_back(State);
	return true;
}

void CDUICanvasRaster::Save()
{
	if (false == m_vecClip.empty())
	{
		m_vecClip.push_back(m_vecClip.back());
	}
}

void CDUICanvasRaster::Restore()
{
	if (m_vecClip.size() > 1)
	{
		m_vecClip.pop_back();
	}
}

void CDUICanvasRaster::ClipRect(const RECT &rc)
{
	if (m_vecClip.empty()) return;
	ClipState &State = m_vecClip.back();
	State.rcClip = DuiIntersectRect(State.rcClip, rc);
	State.nKind = 0;
}

void CDUICanvasRaster::ClipRound(const RECT &rcItem, int nWidth, int nHeight)
{
	if (m_vecClip.empty()) return;
	ClipState &State = m_vecClip.back();
	State.rcClip = DuiIntersectRect(State.rcClip, rcItem);
	State.nKind = 1;
	State.rcShape = rcItem;
	State.nRoundX = nWidth;
	State.nRoundY = nHeight;
}

void CDUICanvasRaster::ClipEllipse(const RECT &rcItem)
{
	if (m_vecClip.empty()) return;
	ClipState &State = m_vecClip.back();
	State.rcClip = DuiIntersectRect(State.rcClip, rcItem);
	State.nKind = 2;
	State.rcShape = rcItem;
}

bool CDUICanvasRaster::ClipPixel(int x, int y) const
{
	if (m_vecClip.empty()) return false;
	const ClipState &State = m_vecClip.back();
	if (x < State.rcClip.left || x >= State.rcClip.right || y < State.rcClip.top || y >= State.rcClip.bottom)
	{
		return false;
	}
	if (1 == State.nKind)
	{
		return DuiPointInRoundRect((float)x + 0.5f, (float)y + 0.5f, State.rcShape, State.nRoundX, State.nRoundY);
	}
	if (2 == State.nKind)
	{
		return DuiPointInEllipse((float)x + 0.5f, (float)y + 0.5f, State.rcShape);
	}
	return true;
}

RECT CDUICanvasRaster::IntersectClip(const RECT &rc) const
{
	RECT rcBound = { 0, 0, m_nWidth, m_nHeight };
	RECT rcClip = m_vecClip.empty() ? rcBound : m_vecClip.back().rcClip;
	return DuiIntersectRect(DuiIntersectRect(rc, rcClip), rcBound);
}

void CDUICanvasRaster::BlendPixel(int x, int y, DWORD dwColor, BYTE cbCoverage)
{
	if (0 == cbCoverage || false == ClipPixel(x, y)) return;

	DWORD dwSrc = DuiPremulColor(dwColor, cbCoverage);
	const BYTE sa = DUIARGBGetA(dwSrc);
	if (0 == sa) return;

	BYTE *p = m_vecBits.data() + y * m_nPitch + x * 4;
	const BYTE sr = DUIARGBGetR(dwSrc);
	const BYTE sg = DUIARGBGetG(dwSrc);
	const BYTE sb = DUIARGBGetB(dwSrc);
	const int inv = 255 - sa;

	p[0] = (BYTE)(sb + ((p[0] * inv + 127) / 255));
	p[1] = (BYTE)(sg + ((p[1] * inv + 127) / 255));
	p[2] = (BYTE)(sr + ((p[2] * inv + 127) / 255));
	p[3] = (BYTE)(sa + ((p[3] * inv + 127) / 255));
}

void CDUICanvasRaster::ClearRect(const RECT &rc)
{
	const RECT rcDraw = IntersectClip(rc);
	for (int y = rcDraw.top; y < rcDraw.bottom; ++y)
	{
		memset(m_vecBits.data() + y * m_nPitch + rcDraw.left * 4, 0, (rcDraw.right - rcDraw.left) * 4);
	}
}

void CDUICanvasRaster::FillShape(const RECT &rc, DWORD dwColor, DWORD dwColorGradient, int nKind, const RECT &rcRound)
{
	if (DUIARGBGetA(dwColor) == 0 && (0 == dwColorGradient || DUIARGBGetA(dwColorGradient) == 0)) return;

	const RECT rcDraw = IntersectClip(rc);
	const float fH = (float)max(1, rc.bottom - rc.top);
	for (int y = rcDraw.top; y < rcDraw.bottom; ++y)
	{
		const float f = 0 == dwColorGradient ? 0.0f : (y - rc.top) / fH;
		const DWORD dwRow = 0 == dwColorGradient ? dwColor : DuiLerpColor(dwColor, dwColorGradient, f);
		for (int x = rcDraw.left; x < rcDraw.right; ++x)
		{
			bool bInside = true;
			if (1 == nKind) bInside = DuiPointInRoundRect((float)x + 0.5f, (float)y + 0.5f, rc, rcRound.left, rcRound.top);
			else if (2 == nKind) bInside = DuiPointInEllipse((float)x + 0.5f, (float)y + 0.5f, rc);
			if (bInside) BlendPixel(x, y, dwRow, 255);
		}
	}
}

void CDUICanvasRaster::FillRect(const RECT &rc, DWORD dwColor, DWORD dwColorGradient)
{
	FillShape(rc, dwColor, dwColorGradient, 0, {});
}

void CDUICanvasRaster::FillRoundRect(const RECT &rc, const RECT &rcRound, DWORD dwColor, DWORD dwColorGradient)
{
	RECT rcUse = rcRound;
	if (0 == rcUse.left && 0 == rcUse.top && 0 == rcUse.right && 0 == rcUse.bottom)
	{
		FillRect(rc, dwColor, dwColorGradient);
		return;
	}
	const int nR = max(rcUse.left, max(rcUse.top, max(rcUse.right, rcUse.bottom)));
	FillShape(rc, dwColor, dwColorGradient, 1, { nR, nR, nR, nR });
}

void CDUICanvasRaster::FillEllipse(const RECT &rc, DWORD dwColor, DWORD dwColorGradient)
{
	FillShape(rc, dwColor, dwColorGradient, 2, {});
}

void CDUICanvasRaster::FillPolygon(const POINT *pPts, int nCount, DWORD dwColor, DWORD dwColorGradient)
{
	if (NULL == pPts || nCount < 3) return;

	RECT rc = { pPts[0].x, pPts[0].y, pPts[0].x, pPts[0].y };
	for (int n = 1; n < nCount; ++n)
	{
		rc.left = min(rc.left, pPts[n].x);
		rc.top = min(rc.top, pPts[n].y);
		rc.right = max(rc.right, pPts[n].x);
		rc.bottom = max(rc.bottom, pPts[n].y);
	}

	const RECT rcDraw = IntersectClip(rc);
	const float fH = (float)max(1, rc.bottom - rc.top);
	for (int y = rcDraw.top; y < rcDraw.bottom; ++y)
	{
		const float fScan = y + 0.5f;
		int nHits[32] = {};
		int nHitCount = 0;
		for (int n = 0; n < nCount && nHitCount < 32; ++n)
		{
			const POINT &a = pPts[n];
			const POINT &b = pPts[(n + 1) % nCount];
			if ((a.y <= fScan && b.y > fScan) || (b.y <= fScan && a.y > fScan))
			{
				const float t = (fScan - a.y) / (float)(b.y - a.y);
				nHits[nHitCount++] = (int)floorf(a.x + t * (b.x - a.x));
			}
		}
		for (int i = 0; i < nHitCount - 1; ++i)
		{
			for (int j = i + 1; j < nHitCount; ++j)
			{
				if (nHits[j] < nHits[i])
				{
					int tmp = nHits[i];
					nHits[i] = nHits[j];
					nHits[j] = tmp;
				}
			}
		}

		const float f = 0 == dwColorGradient ? 0.0f : (y - rc.top) / fH;
		const DWORD dwRow = 0 == dwColorGradient ? dwColor : DuiLerpColor(dwColor, dwColorGradient, f);
		for (int n = 0; n + 1 < nHitCount; n += 2)
		{
			const int x0 = max(rcDraw.left, nHits[n]);
			const int x1 = min(rcDraw.right, nHits[n + 1]);
			for (int x = x0; x < x1; ++x) BlendPixel(x, y, dwRow, 255);
		}
	}
}

void CDUICanvasRaster::StrokeShape(const RECT &rc, int nWidth, DWORD dwColor, int nKind, const RECT &rcRound)
{
	if (nWidth <= 0) return;
	const RECT rcDraw = IntersectClip({ rc.left - nWidth, rc.top - nWidth, rc.right + nWidth, rc.bottom + nWidth });
	const float fHalf = nWidth * 0.5f;
	for (int y = rcDraw.top; y < rcDraw.bottom; ++y)
	{
		for (int x = rcDraw.left; x < rcDraw.right; ++x)
		{
			const float px = x + 0.5f;
			const float py = y + 0.5f;
			bool bRing = false;
			if (0 == nKind)
			{
				const bool bOuter = px >= rc.left && px < rc.right && py >= rc.top && py < rc.bottom;
				const bool bInner = px >= rc.left + nWidth && px < rc.right - nWidth && py >= rc.top + nWidth && py < rc.bottom - nWidth;
				bRing = bOuter && false == bInner;
			}
			else if (1 == nKind)
			{
				const int nR = max(rcRound.left, rcRound.top);
				const bool bOuter = DuiPointInRoundRect(px, py, rc, nR, nR);
				RECT rcInner = { rc.left + nWidth, rc.top + nWidth, rc.right - nWidth, rc.bottom - nWidth };
				const bool bInner = DuiPointInRoundRect(px, py, rcInner, max(0, nR - nWidth), max(0, nR - nWidth));
				bRing = bOuter && false == bInner;
			}
			else
			{
				const bool bOuter = DuiPointInEllipse(px, py, rc);
				RECT rcInner = { rc.left + nWidth, rc.top + nWidth, rc.right - nWidth, rc.bottom - nWidth };
				const bool bInner = DuiPointInEllipse(px, py, rcInner);
				bRing = bOuter && false == bInner;
			}
			if (bRing) BlendPixel(x, y, dwColor, 255);
		}
	}
	(void)fHalf;
}

void CDUICanvasRaster::DrawLine(int x1, int y1, int x2, int y2, int nWidth, DWORD dwColor, int)
{
	nWidth = max(1, nWidth);
	RECT rc = { min(x1, x2) - nWidth, min(y1, y2) - nWidth, max(x1, x2) + nWidth + 1, max(y1, y2) + nWidth + 1 };
	const RECT rcDraw = IntersectClip(rc);
	const float fHalf = nWidth * 0.5f + 0.25f;
	for (int y = rcDraw.top; y < rcDraw.bottom; ++y)
	{
		for (int x = rcDraw.left; x < rcDraw.right; ++x)
		{
			const float d = DuiDistToSegment(x + 0.5f, y + 0.5f, (float)x1, (float)y1, (float)x2, (float)y2);
			if (d <= fHalf)
			{
				const float fCov = max(0.0f, min(1.0f, fHalf - d + 0.5f));
				BlendPixel(x, y, dwColor, (BYTE)(fCov * 255.0f));
			}
		}
	}
}

void CDUICanvasRaster::DrawRect(const RECT &rc, int nWidth, DWORD dwColor, SIZE szBreakTop, int)
{
	nWidth = max(1, nWidth);
	const int nBreakL = szBreakTop.cx;
	const int nBreakR = szBreakTop.cy;
	DrawLine(rc.left, rc.top, rc.left, rc.bottom, nWidth, dwColor, 0);
	DrawLine(rc.right - 1, rc.top, rc.right - 1, rc.bottom, nWidth, dwColor, 0);
	DrawLine(rc.left, rc.bottom - 1, rc.right, rc.bottom - 1, nWidth, dwColor, 0);
	if (nBreakR > nBreakL)
	{
		DrawLine(rc.left, rc.top, rc.left + nBreakL, rc.top, nWidth, dwColor, 0);
		DrawLine(rc.left + nBreakR, rc.top, rc.right, rc.top, nWidth, dwColor, 0);
	}
	else
	{
		DrawLine(rc.left, rc.top, rc.right, rc.top, nWidth, dwColor, 0);
	}
}

void CDUICanvasRaster::DrawRoundRect(const RECT &rc, const RECT &rcRound, int nWidth, DWORD dwColor)
{
	StrokeShape(rc, nWidth, dwColor, 1, rcRound);
}

void CDUICanvasRaster::DrawEllipse(const RECT &rc, int nWidth, DWORD dwColor, int)
{
	StrokeShape(rc, nWidth, dwColor, 2, {});
}

void CDUICanvasRaster::DrawArc(const RECT &rc, int nWidth, DWORD dwColor, float fStartAngle, float fSweepAngle, int)
{
	nWidth = max(1, nWidth);
	const float cx = (rc.left + rc.right) * 0.5f;
	const float cy = (rc.top + rc.bottom) * 0.5f;
	const float rx = max(0.5f, (rc.right - rc.left) * 0.5f);
	const float ry = max(0.5f, (rc.bottom - rc.top) * 0.5f);
	const int nSeg = max(8, (int)(fabsf(fSweepAngle) / 6.0f));
	float xPrev = cx + cosf(fStartAngle * (float)M_PI / 180.0f) * rx;
	float yPrev = cy + sinf(fStartAngle * (float)M_PI / 180.0f) * ry;
	for (int n = 1; n <= nSeg; ++n)
	{
		const float fA = fStartAngle + fSweepAngle * n / (float)nSeg;
		const float x = cx + cosf(fA * (float)M_PI / 180.0f) * rx;
		const float y = cy + sinf(fA * (float)M_PI / 180.0f) * ry;
		DrawLine((int)xPrev, (int)yPrev, (int)x, (int)y, nWidth, dwColor, 0);
		xPrev = x;
		yPrev = y;
	}
}

void CDUICanvasRaster::DrawPath(const POINT *pPts, int nCount, int nWidth, DWORD dwColor, int)
{
	if (NULL == pPts || nCount < 2) return;
	for (int n = 0; n < nCount - 1; ++n)
	{
		DrawLine(pPts[n].x, pPts[n].y, pPts[n + 1].x, pPts[n + 1].y, nWidth, dwColor, 0);
	}
}

void CDUICanvasRaster::BlitImageRect(const BYTE *pBGRA, int nImgW, int nImgH, const RECT &rcDst, const RECT &rcSrc, BYTE cbAlpha)
{
	if (NULL == pBGRA || nImgW <= 0 || nImgH <= 0 || cbAlpha == 0) return;
	if (rcDst.right <= rcDst.left || rcDst.bottom <= rcDst.top) return;
	if (rcSrc.right <= rcSrc.left || rcSrc.bottom <= rcSrc.top) return;

	const RECT rcDraw = IntersectClip(rcDst);
	const float fSrcW = (float)(rcSrc.right - rcSrc.left);
	const float fSrcH = (float)(rcSrc.bottom - rcSrc.top);
	const float fDstW = (float)(rcDst.right - rcDst.left);
	const float fDstH = (float)(rcDst.bottom - rcDst.top);

	for (int y = rcDraw.top; y < rcDraw.bottom; ++y)
	{
		const float fV = (y - rcDst.top + 0.5f) / fDstH;
		const int nSrcY = min(nImgH - 1, max(0, rcSrc.top + (int)(fV * fSrcH)));
		for (int x = rcDraw.left; x < rcDraw.right; ++x)
		{
			if (false == ClipPixel(x, y)) continue;
			const float fU = (x - rcDst.left + 0.5f) / fDstW;
			const int nSrcX = min(nImgW - 1, max(0, rcSrc.left + (int)(fU * fSrcW)));
			const BYTE *pS = pBGRA + (nSrcY * nImgW + nSrcX) * 4;
			const BYTE sa = (BYTE)((pS[3] * cbAlpha + 127) / 255);
			if (0 == sa || false == ClipPixel(x, y)) continue;
			BYTE *pD = m_vecBits.data() + y * m_nPitch + x * 4;
			const BYTE inv = (BYTE)(255 - sa);
			pD[0] = (BYTE)((pS[0] * cbAlpha + 127) / 255 + (pD[0] * inv + 127) / 255);
			pD[1] = (BYTE)((pS[1] * cbAlpha + 127) / 255 + (pD[1] * inv + 127) / 255);
			pD[2] = (BYTE)((pS[2] * cbAlpha + 127) / 255 + (pD[2] * inv + 127) / 255);
			pD[3] = (BYTE)(sa + (pD[3] * inv + 127) / 255);
		}
	}
}

void CDUICanvasRaster::DrawImage(const BYTE *pBGRA, int nImgW, int nImgH, const RECT &rcDst, const RECT &rcSrc,
	const RECT &rcCorner, BYTE cbAlpha, bool bTiledX, bool bTiledY)
{
	if (NULL == pBGRA) return;
	cbAlpha = cbAlpha == 0 ? 0 : cbAlpha;

	const bool bNine = rcCorner.left > 0 || rcCorner.top > 0 || rcCorner.right > 0 || rcCorner.bottom > 0;
	if (false == bNine && false == bTiledX && false == bTiledY)
	{
		BlitImageRect(pBGRA, nImgW, nImgH, rcDst, rcSrc, cbAlpha);
		return;
	}

	if (bTiledX || bTiledY)
	{
		const int nTileW = max(1, rcSrc.right - rcSrc.left);
		const int nTileH = max(1, rcSrc.bottom - rcSrc.top);
		for (int y = rcDst.top; y < rcDst.bottom; y += (bTiledY ? nTileH : (rcDst.bottom - rcDst.top)))
		{
			for (int x = rcDst.left; x < rcDst.right; x += (bTiledX ? nTileW : (rcDst.right - rcDst.left)))
			{
				RECT rcTile = { x, y, min(rcDst.right, x + nTileW), min(rcDst.bottom, y + nTileH) };
				RECT rcS = rcSrc;
				rcS.right = rcS.left + (rcTile.right - rcTile.left);
				rcS.bottom = rcS.top + (rcTile.bottom - rcTile.top);
				BlitImageRect(pBGRA, nImgW, nImgH, rcTile, rcS, cbAlpha);
				if (false == bTiledX) break;
			}
			if (false == bTiledY) break;
		}
		return;
	}

	const int nL = rcCorner.left;
	const int nT = rcCorner.top;
	const int nR = rcCorner.right;
	const int nB = rcCorner.bottom;
	const RECT rcC = rcSrc;

	const RECT rcDstMid = { rcDst.left + nL, rcDst.top + nT, rcDst.right - nR, rcDst.bottom - nB };
	const RECT rcSrcMid = { rcC.left + nL, rcC.top + nT, rcC.right - nR, rcC.bottom - nB };

	auto Blit = [&](RECT dst, RECT src)
	{
		if (dst.right <= dst.left || dst.bottom <= dst.top) return;
		if (src.right <= src.left || src.bottom <= src.top) return;
		BlitImageRect(pBGRA, nImgW, nImgH, dst, src, cbAlpha);
	};

	Blit({ rcDst.left, rcDst.top, rcDst.left + nL, rcDst.top + nT }, { rcC.left, rcC.top, rcC.left + nL, rcC.top + nT });
	Blit({ rcDst.right - nR, rcDst.top, rcDst.right, rcDst.top + nT }, { rcC.right - nR, rcC.top, rcC.right, rcC.top + nT });
	Blit({ rcDst.left, rcDst.bottom - nB, rcDst.left + nL, rcDst.bottom }, { rcC.left, rcC.bottom - nB, rcC.left + nL, rcC.bottom });
	Blit({ rcDst.right - nR, rcDst.bottom - nB, rcDst.right, rcDst.bottom }, { rcC.right - nR, rcC.bottom - nB, rcC.right, rcC.bottom });
	Blit({ rcDstMid.left, rcDst.top, rcDstMid.right, rcDst.top + nT }, { rcSrcMid.left, rcC.top, rcSrcMid.right, rcC.top + nT });
	Blit({ rcDstMid.left, rcDst.bottom - nB, rcDstMid.right, rcDst.bottom }, { rcSrcMid.left, rcC.bottom - nB, rcSrcMid.right, rcC.bottom });
	Blit({ rcDst.left, rcDstMid.top, rcDst.left + nL, rcDstMid.bottom }, { rcC.left, rcSrcMid.top, rcC.left + nL, rcSrcMid.bottom });
	Blit({ rcDst.right - nR, rcDstMid.top, rcDst.right, rcDstMid.bottom }, { rcC.right - nR, rcSrcMid.top, rcC.right, rcSrcMid.bottom });
	Blit(rcDstMid, rcSrcMid);
}

SIZE CDUICanvasRaster::MeasureText(IDuiFont *pFont, LPCTSTR lpszText, DWORD dwStyle, int nMaxWidth)
{
	SIZE sz = {};
	if (NULL == pFont || NULL == lpszText) return sz;
	return pFont->MeasureText(lpszText, nMaxWidth, dwStyle);
}

void CDUICanvasRaster::DrawText(IDuiFont *pFont, RECT &rc, LPCTSTR lpszText, DWORD dwColor, DWORD dwStyle)
{
	if (NULL == pFont || NULL == lpszText || 0 == *lpszText) return;

	CDUIFontRaster *pRaster = (CDUIFontRaster *)pFont;
	const int nMaxW = (dwStyle & DT_CALCRECT) ? 0 : (int)(rc.right - rc.left);
	SIZE sz = pRaster->MeasureText(lpszText, (dwStyle & DT_WORDBREAK) ? max(1, (int)(rc.right - rc.left)) : 0, dwStyle);

	if (dwStyle & DT_CALCRECT)
	{
		rc.right = rc.left + sz.cx;
		rc.bottom = rc.top + sz.cy;
		return;
	}

	int x = rc.left;
	int y = rc.top;
	if (dwStyle & DT_CENTER) x = rc.left + max(0, ((int)(rc.right - rc.left) - sz.cx) / 2);
	else if (dwStyle & DT_RIGHT) x = rc.right - sz.cx;
	if (dwStyle & DT_VCENTER) y = rc.top + max(0, ((int)(rc.bottom - rc.top) - sz.cy) / 2);
	else if (dwStyle & DT_BOTTOM) y = rc.bottom - sz.cy;

	const stbtt_fontinfo *pInfo = (const stbtt_fontinfo *)pRaster->GetFontInfo();
	const float fScale = pRaster->GetScale();
	const int nAscent = pRaster->GetAscent();
	const int nLineH = pRaster->GetLineHeight();
	int nPenX = x;
	int nPenY = y + nAscent;
	const int nWrapW = (dwStyle & DT_WORDBREAK) ? (int)(rc.right - rc.left) : 0;

	auto DrawGlyph = [&](unsigned cp)
	{
		if (NULL == pInfo)
		{
			RECT rcG = { nPenX, nPenY - nAscent, nPenX + max(4, pRaster->GetPixelSize() / 2), nPenY };
			FillRect(rcG, dwColor, 0);
			nPenX += max(4, pRaster->GetPixelSize() / 2);
			return;
		}

		int nAdv = 0, nLsb = 0;
		stbtt_GetCodepointHMetrics(pInfo, (int)cp, &nAdv, &nLsb);
		const int nAdvance = (int)floorf(nAdv * fScale + 0.5f);

		if (nWrapW > 0 && nPenX + nAdvance > rc.right && nPenX > x)
		{
			nPenX = x;
			nPenY += nLineH;
		}

		int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
		stbtt_GetCodepointBitmapBox(pInfo, (int)cp, fScale, fScale, &x0, &y0, &x1, &y1);
		const int gw = x1 - x0;
		const int gh = y1 - y0;
		if (gw > 0 && gh > 0)
		{
			std::vector<BYTE> vecGlyph((size_t)gw * (size_t)gh);
			stbtt_MakeCodepointBitmap(pInfo, vecGlyph.data(), gw, gh, gw, fScale, fScale, (int)cp);
			const int nDestX = nPenX + x0;
			const int nDestY = nPenY + y0;
			for (int gy = 0; gy < gh; ++gy)
			{
				for (int gx = 0; gx < gw; ++gx)
				{
					BlendPixel(nDestX + gx, nDestY + gy, dwColor, vecGlyph[gy * gw + gx]);
				}
			}
		}
		nPenX += nAdvance;
	};

	LPCTSTR p = lpszText;
	while (unsigned cp = DuiNextCodepoint(p))
	{
		if (L'\n' == cp)
		{
			nPenX = x;
			nPenY += nLineH;
			continue;
		}
		DrawGlyph(cp);
	}

	(void)nMaxW;
}

void CDUICanvasRaster::BlitFrom(IDuiCanvas *pSrc, int xDst, int yDst, int nWidth, int nHeight, int xSrc, int ySrc)
{
	if (NULL == pSrc || NULL == pSrc->GetBits()) return;
	RECT rcDst = { xDst, yDst, xDst + nWidth, yDst + nHeight };
	RECT rcSrc = { xSrc, ySrc, xSrc + nWidth, ySrc + nHeight };
	BlitImageRect(pSrc->GetBits(), pSrc->GetWidth(), pSrc->GetHeight(), rcDst, rcSrc, 255);
}

void CDUICanvasRaster::StretchBlitFrom(IDuiCanvas *pSrc, int xDst, int yDst, int nWidth, int nHeight, int xSrc, int ySrc, int wSrc, int hSrc)
{
	if (NULL == pSrc || NULL == pSrc->GetBits()) return;
	if (nWidth <= 0 || nHeight <= 0 || wSrc <= 0 || hSrc <= 0) return;

	RECT rcDst = { xDst, yDst, xDst + nWidth, yDst + nHeight };
	RECT rcSrc = { xSrc, ySrc, xSrc + wSrc, ySrc + hSrc };
	BlitImageRect(pSrc->GetBits(), pSrc->GetWidth(), pSrc->GetHeight(), rcDst, rcSrc, 255);
}

#endif
