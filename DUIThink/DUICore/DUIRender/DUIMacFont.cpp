#if !defined(__APPLE__)
#include "StdAfx.h"
#endif
#include "DUIMacFont.h"

#if defined(__APPLE__)

#include <CoreText/CoreText.h>
#include <CoreFoundation/CoreFoundation.h>

int DuiMacFindCjkFontPath(char *szPath, unsigned nSize)
{
	if (NULL == szPath || nSize < 8) return 0;

	static const char *s_pszNames[] = {
		"PingFang SC",
		"PingFangSC-Regular",
		"Hiragino Sans GB",
		"Heiti SC",
		"Songti SC",
		"STHeiti",
		"Arial Unicode MS",
		NULL
	};

	for (int n = 0; s_pszNames[n]; ++n)
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

		const Boolean bOk = CFURLGetFileSystemRepresentation(url, true, (UInt8 *)szPath, (CFIndex)nSize);
		CFRelease(url);
		if (bOk && szPath[0]) return 1;
	}

	return 0;
}

#else

int DuiMacFindCjkFontPath(char *, unsigned)
{
	return 0;
}

#endif
