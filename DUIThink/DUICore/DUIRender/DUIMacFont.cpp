#include "StdAfx.h"
#include "DUIMacFont.h"

#if defined(__APPLE__)

#include <CoreText/CoreText.h>
#include <CoreFoundation/CoreFoundation.h>
#include <string.h>
#include <stdio.h>

enum { DuiMacFontPathMax = 16, DuiMacFontPathLen = 1024 };

static int DuiMacCollectCjkFontPaths(char szPaths[][DuiMacFontPathLen], int nMax)
{
	// Prefer faces stb_truetype can rasterize. PingFang UI on modern macOS is
	// a private CFF2 collection (PingFangUI.ttc) that InitFont often fails.
	static const char *s_pszNames[] = {
		"Heiti SC",
		"STHeiti",
		"Songti SC",
		"Hiragino Sans GB",
		"Arial Unicode MS",
		"PingFang SC",
		"PingFangSC-Regular",
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

#else

int DuiMacFindCjkFontPath(char *, unsigned, int)
{
	return 0;
}

#endif
