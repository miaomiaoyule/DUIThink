#ifndef __IDUICANVAS_H__
#define __IDUICANVAS_H__

#pragma once

#ifdef DuiPlatform_SDL
// Portable 2D canvas. HDC on DuiPlatform_SDL is IDuiCanvas*.
// Phase 1: CDUICanvasRaster (CPU BGRA) + SDL_Texture present.
// Phase 2: swap implementation to Skia GPU (GL / GLES / Metal).

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API IDuiNativeGdi
{
public:
	enum enKind
	{
		Kind_Canvas = 1,
		Kind_Image = 2,
		Kind_Font = 3,
	};

	virtual ~IDuiNativeGdi() {}
	virtual enKind GetNativeKind() const = 0;
	virtual int GetWidth() const { return 0; }
	virtual int GetHeight() const { return 0; }
	virtual LPBYTE GetBits() { return NULL; }
	virtual int GetPitch() const { return 0; }
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API IDuiImage : public IDuiNativeGdi
{
public:
	enKind GetNativeKind() const override { return Kind_Image; }
	virtual bool HasAlpha() const = 0;
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API IDuiFont : public IDuiNativeGdi
{
public:
	enKind GetNativeKind() const override { return Kind_Font; }
	virtual int GetPixelSize() const = 0;
	virtual int GetAscent() const = 0;
	virtual int GetLineHeight() const = 0;
	virtual SIZE MeasureText(LPCTSTR lpszText, int nMaxWidth, DWORD dwTextStyle) const = 0;
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API IDuiCanvas : public IDuiNativeGdi
{
public:
	enKind GetNativeKind() const override { return Kind_Canvas; }

	virtual void Save() = 0;
	virtual void Restore() = 0;
	virtual void ClipRect(const RECT &rc) = 0;
	virtual void ClipRound(const RECT &rcItem, int nWidth, int nHeight) = 0;
	virtual void ClipEllipse(const RECT &rcItem) = 0;

	virtual void ClearRect(const RECT &rc) = 0;
	virtual void FillRect(const RECT &rc, DWORD dwColor, DWORD dwColorGradient = 0) = 0;
	virtual void FillRoundRect(const RECT &rc, const RECT &rcRound, DWORD dwColor, DWORD dwColorGradient = 0) = 0;
	virtual void FillEllipse(const RECT &rc, DWORD dwColor, DWORD dwColorGradient = 0) = 0;
	virtual void FillPolygon(const POINT *pPts, int nCount, DWORD dwColor, DWORD dwColorGradient = 0) = 0;

	virtual void DrawLine(int x1, int y1, int x2, int y2, int nWidth, DWORD dwColor, int nStyle = 0) = 0;
	virtual void DrawRect(const RECT &rc, int nWidth, DWORD dwColor, SIZE szBreakTop, int nStyle = 0) = 0;
	virtual void DrawRoundRect(const RECT &rc, const RECT &rcRound, int nWidth, DWORD dwColor) = 0;
	virtual void DrawEllipse(const RECT &rc, int nWidth, DWORD dwColor, int nStyle = 0) = 0;
	virtual void DrawArc(const RECT &rc, int nWidth, DWORD dwColor, float fStartAngle, float fSweepAngle, int nStyle = 0) = 0;
	virtual void DrawPath(const POINT *pPts, int nCount, int nWidth, DWORD dwColor, int nStyle = 0) = 0;

	virtual void DrawImage(const BYTE *pBGRA, int nImgW, int nImgH, const RECT &rcDst, const RECT &rcSrc,
		const RECT &rcCorner, BYTE cbAlpha, bool bTiledX, bool bTiledY) = 0;
	virtual void DrawText(IDuiFont *pFont, RECT &rc, LPCTSTR lpszText, DWORD dwColor, DWORD dwStyle) = 0;
	virtual SIZE MeasureText(IDuiFont *pFont, LPCTSTR lpszText, DWORD dwStyle, int nMaxWidth) = 0;
	virtual void BlitFrom(IDuiCanvas *pSrc, int xDst, int yDst, int nWidth, int nHeight, int xSrc, int ySrc) = 0;
	virtual void StretchBlitFrom(IDuiCanvas *pSrc, int xDst, int yDst, int nWidth, int nHeight, int xSrc, int ySrc, int wSrc, int hSrc) = 0;
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API IDuiPaintScene
{
public:
	virtual ~IDuiPaintScene() {}
	virtual IDuiCanvas * GetCanvas() = 0;
	virtual HDC GetCanvasHDC() = 0;
	virtual LPBYTE GetBits() = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual bool Resize(int nWidth, int nHeight) = 0;
	virtual void Present() = 0;
	virtual void Present(const RECT &rcDirty) = 0;
};

//////////////////////////////////////////////////////////////////////////
inline IDuiCanvas * DuiCanvasFromHDC(HDC hDC)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)hDC;
	return (pNative && IDuiNativeGdi::Kind_Canvas == pNative->GetNativeKind()) ? (IDuiCanvas *)pNative : NULL;
}

inline IDuiImage * DuiImageFromHBITMAP(HBITMAP hBitmap)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)hBitmap;
	return (pNative && IDuiNativeGdi::Kind_Image == pNative->GetNativeKind()) ? (IDuiImage *)pNative : NULL;
}

inline IDuiFont * DuiFontFromHFONT(HFONT hFont)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)hFont;
	return (pNative && IDuiNativeGdi::Kind_Font == pNative->GetNativeKind()) ? (IDuiFont *)pNative : NULL;
}

inline HDC DuiHDCFromCanvas(IDuiCanvas *pCanvas)
{
	return (HDC)pCanvas;
}

//////////////////////////////////////////////////////////////////////////
#endif
#endif
