#ifndef __DUIMACFONT_H__
#define __DUIMACFONT_H__

#ifdef __cplusplus
extern "C" {
#endif

// Resolve a CJK font file via Core Text. Returns 1 and writes a filesystem
// path on success. Isolated from DUI headers: CoreText/MacTypes collide with
// Gdiplus::Point/Rect and the project's `interface` macro.
int DuiMacFindCjkFontPath(char *szPath, unsigned nSize);

#ifdef __cplusplus
}
#endif

#endif
