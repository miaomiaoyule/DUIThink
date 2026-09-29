#ifndef __DUIMACFONT_H__
#define __DUIMACFONT_H__

#ifdef __cplusplus
extern "C" {
#endif

// Resolve CJK font files via Core Text. nWhich is 0-based among unique
// filesystem URLs (cached after the first call). Returns 1 on success.
int DuiMacFindCjkFontPath(char *szPath, unsigned nSize, int nWhich);

typedef struct DuiMacGlyph
{
	int x0;
	int y0;
	int w;
	int h;
	int nAdvance;
	const unsigned char *pBits;
} DuiMacGlyph;

void *DuiMacCreateFont(const char *pszUtf8Face, int nPixelSize, int nWeight, int bItalic);
void *DuiMacCreateFontW(const wchar_t *pszFace, int nPixelSize, int nWeight, int bItalic);
void DuiMacReleaseFont(void *pFont);
int DuiMacFontAscent(void *pFont);
int DuiMacGetAdvance(void *pFont, unsigned cp);
int DuiMacGetGlyph(void *pFont, unsigned cp, DuiMacGlyph *pOut);

#ifdef __cplusplus
}
#endif

#endif
