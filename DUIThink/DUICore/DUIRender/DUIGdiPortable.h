#pragma once

#if defined(DuiPlatform_SDL)

//////////////////////////////////////////////////////////////////////////
BOOL DeleteObject(HGDIOBJ hObject);
BOOL DeleteDC(HDC hdc);
HDC CreateCompatibleDC(HDC hdc);
HGDIOBJ SelectObject(HDC hdc, HGDIOBJ hgdiobj);
BOOL BitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
BOOL StretchBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, int wSrc, int hSrc, DWORD rop);
DWORD GetObjectType(HGDIOBJ h);
int SaveDC(HDC hdc);
BOOL RestoreDC(HDC hdc, int nSavedDC);
HFONT CreateFontIndirect(const LOGFONT *lplf);
HGDIOBJ GetStockObject(int i);
int GetObject(HGDIOBJ h, int c, LPVOID pv);
int GetDeviceCaps(HDC hdc, int nIndex);
BOOL UpdateLayeredWindow(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, DWORD crKey, BLENDFUNCTION *pblend, DWORD dwFlags);

HRGN CreateRectRgnIndirect(const RECT *lprect);
HRGN CreateEllipticRgnIndirect(const RECT *lprect);
HRGN CreateRoundRectRgn(int x1, int y1, int x2, int y2, int w, int h);
int CombineRgn(HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2, int fnCombineMode);
int SelectClipRgn(HDC hdc, HRGN hRgn);
int GetClipBox(HDC hdc, LPRECT lprect);
HPEN CreatePen(int iStyle, int cWidth, DWORD color);
BOOL Rectangle(HDC hdc, int left, int top, int right, int bottom);
BOOL RoundRect(HDC hdc, int left, int top, int right, int bottom, int width, int height);
int SetBkMode(HDC hdc, int mode);
int SetTextColor(HDC hdc, DWORD color);
int DrawText(HDC hdc, LPCTSTR lpchText, int cchText, LPRECT lprc, UINT dwStyle);
HDC GetDC(HWND hWnd);
int ReleaseDC(HWND hWnd, HDC hDC);
void GdiFlush();
int GetDIBits(HDC hdc, HBITMAP hbm, UINT start, UINT cLines, LPVOID lpvBits, LPBITMAPINFO lpbi, UINT usage);
HBITMAP CreateCompatibleBitmap(HDC hdc, int cx, int cy);
HBITMAP CreateDIBSection(HDC hdc, const BITMAPINFO *pbmi, UINT usage, void **ppvBits, HANDLE hSection, DWORD offset);
BOOL WINAPI AlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
	HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn);
int DrawShadowText(HDC hdc, LPCTSTR lpchText, int cchText, LPRECT lprc, UINT dwStyle,
	DWORD crText, DWORD crShadow, int dx, int dy);
BOOL GetTextExtentPoint32(HDC hdc, LPCTSTR lpString, int c, LPSIZE psizl);

//////////////////////////////////////////////////////////////////////////
#endif