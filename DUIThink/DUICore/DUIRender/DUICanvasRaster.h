#ifndef __DUICANVASRASTER_H__
#define __DUICANVASRASTER_H__

#pragma once

#include "IDuiCanvas.h"

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIImageRaster : public IDuiImage
{
public:
	CDUIImageRaster(int nWidth, int nHeight, bool bHasAlpha = true);
	virtual ~CDUIImageRaster();

	int GetWidth() const override;
	int GetHeight() const override;
	LPBYTE GetBits() override;
	int GetPitch() const override;
	bool HasAlpha() const override;
	void SetHasAlpha(bool bAlpha);

protected:
	int m_nWidth = 0;
	int m_nHeight = 0;
	int m_nPitch = 0;
	bool m_bHasAlpha = true;
	std::vector<BYTE> m_vecBits;
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIFontRaster : public IDuiFont
{
public:
	static CDUIFontRaster * Create(LPCTSTR lpszFace, int nPixelSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
	virtual ~CDUIFontRaster();

	int GetPixelSize() const override;
	int GetAscent() const override;
	int GetLineHeight() const override;
	SIZE MeasureText(LPCTSTR lpszText, int nMaxWidth, DWORD dwTextStyle) const override;

	float GetScale() const;
	int GetUnderline() const { return m_bUnderline ? 1 : 0; }
	int GetStrikeOut() const { return m_bStrikeOut ? 1 : 0; }
	const void * GetFontInfo() const;

protected:
	CDUIFontRaster();
	bool Init(LPCTSTR lpszFace, int nPixelSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut);

	int m_nPixelSize = 12;
	int m_nAscent = 10;
	int m_nLineHeight = 14;
	float m_fScale = 1.0f;
	LONG m_lWeight = 400;
	bool m_bItalic = false;
	bool m_bUnderline = false;
	bool m_bStrikeOut = false;
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUICanvasRaster : public IDuiCanvas
{
public:
	CDUICanvasRaster(int nWidth, int nHeight);
	virtual ~CDUICanvasRaster();

	int GetWidth() const override;
	int GetHeight() const override;
	LPBYTE GetBits() override;
	int GetPitch() const override;
	bool Resize(int nWidth, int nHeight);

	void Save() override;
	void Restore() override;
	void ClipRect(const RECT &rc) override;
	void ClipRound(const RECT &rcItem, int nWidth, int nHeight) override;
	void ClipEllipse(const RECT &rcItem) override;

	void ClearRect(const RECT &rc) override;
	void FillRect(const RECT &rc, DWORD dwColor, DWORD dwColorGradient = 0) override;
	void FillRoundRect(const RECT &rc, const RECT &rcRound, DWORD dwColor, DWORD dwColorGradient = 0) override;
	void FillEllipse(const RECT &rc, DWORD dwColor, DWORD dwColorGradient = 0) override;
	void FillPolygon(const POINT *pPts, int nCount, DWORD dwColor, DWORD dwColorGradient = 0) override;

	void DrawLine(int x1, int y1, int x2, int y2, int nWidth, DWORD dwColor, int nStyle = 0) override;
	void DrawRect(const RECT &rc, int nWidth, DWORD dwColor, SIZE szBreakTop, int nStyle = 0) override;
	void DrawRoundRect(const RECT &rc, const RECT &rcRound, int nWidth, DWORD dwColor) override;
	void DrawEllipse(const RECT &rc, int nWidth, DWORD dwColor, int nStyle = 0) override;
	void DrawArc(const RECT &rc, int nWidth, DWORD dwColor, float fStartAngle, float fSweepAngle, int nStyle = 0) override;
	void DrawPath(const POINT *pPts, int nCount, int nWidth, DWORD dwColor, int nStyle = 0) override;

	void DrawImage(const BYTE *pBGRA, int nImgW, int nImgH, const RECT &rcDst, const RECT &rcSrc,
		const RECT &rcCorner, BYTE cbAlpha, bool bTiledX, bool bTiledY) override;
	void DrawText(IDuiFont *pFont, RECT &rc, LPCTSTR lpszText, DWORD dwColor, DWORD dwStyle) override;
	SIZE MeasureText(IDuiFont *pFont, LPCTSTR lpszText, DWORD dwStyle, int nMaxWidth) override;
	void BlitFrom(IDuiCanvas *pSrc, int xDst, int yDst, int nWidth, int nHeight, int xSrc, int ySrc) override;

protected:
	struct ClipState
	{
		RECT rcClip = {};
		int nKind = 0; // 0 rect, 1 round, 2 ellipse
		RECT rcShape = {};
		int nRoundX = 0;
		int nRoundY = 0;
	};

	bool ClipPixel(int x, int y) const;
	RECT IntersectClip(const RECT &rc) const;
	void BlendPixel(int x, int y, DWORD dwColor, BYTE cbCoverage);
	void FillShape(const RECT &rc, DWORD dwColor, DWORD dwColorGradient, int nKind, const RECT &rcRound);
	void StrokeShape(const RECT &rc, int nWidth, DWORD dwColor, int nKind, const RECT &rcRound);
	void BlitImageRect(const BYTE *pBGRA, int nImgW, int nImgH, const RECT &rcDst, const RECT &rcSrc, BYTE cbAlpha);

	int m_nWidth = 0;
	int m_nHeight = 0;
	int m_nPitch = 0;
	std::vector<BYTE> m_vecBits;
	std::vector<ClipState> m_vecClip;
};
#endif
