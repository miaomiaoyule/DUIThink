#ifndef __DUIMACFONT_H__
#define __DUIMACFONT_H__

#ifdef __cplusplus
extern "C" {
#endif

// Resolve CJK font files via Core Text. nWhich is 0-based among unique
// filesystem URLs (cached after the first call). Returns 1 on success.
int DuiMacFindCjkFontPath(char *szPath, unsigned nSize, int nWhich);

#ifdef __cplusplus
}
#endif

#endif
