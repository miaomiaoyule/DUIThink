#ifndef __DUIRENDER_H__
#define __DUIRENDER_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIRenderClip
{
public:
	~CDUIRenderClip();
	RECT rcItem = {};
	HDC hDC = NULL;
	HRGN hRgn = NULL;
	HRGN hOldRgn = NULL;

	static void GenerateClip(HDC hDC, RECT rc, CDUIRenderClip &clip);
	static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CDUIRenderClip &clip);
	static void GenerateEllipseClip(HDC hDC, RECT rcPaint, RECT rcItem, CDUIRenderClip &clip);
	static void UseOldClipBegin(HDC hDC, CDUIRenderClip &clip);
	static void UseOldClipEnd(HDC hDC, CDUIRenderClip &clip);
};

/////////////////////////////////////////////////////////////////////////////////////
//
class DUITHINK_API CDUIRenderEngine
{
public:
	//draw argb
	static void DrawImage(HDC hDC, HBITMAP hBitmap, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
		BYTE cbAlpha = 255, bool bAlpha = false, bool bHole = false, bool bTiledX = false, bool bTiledY = false);
	static void DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem);
	static void DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
		BYTE cbAlpha = 255, bool bAlpha = false, bool bHole = false, bool bTiledX = false, bool bTiledY = false);
	static void DrawAnimateImage(HDC hDC, Gdiplus::Bitmap *pBmpAnimate, const CDUIRect &rcItem, int nFrameCur, const CDUIRect &rcRound = {});
	static void DrawLine(HDC hDC, const CDUIRect &rcItem, int nLineSize, DWORD dwPenColor, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawRect(HDC hDC, const CDUIRect &rcItem, int nLineSize, DWORD dwPenColor, CDUISize szBreakTop = {}, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawPath(HDC hDC, const std::vector<CDUIPoint> &vecPtList, int nLineSize, DWORD dwPenColor, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawRound(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, DWORD dwPenColor);
	static void DrawRoundRect(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, DWORD dwPenColor, CDUISize szBreakTop = {});
	static void FillRect(HDC hDC, const CDUIRect &rcItem, DWORD dwColor, DWORD dwColorGradient = 0);
	static void FillRoundRect(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, DWORD dwColor, DWORD dwColorGradient = 0);

	//draw text
	static void DrawText(HDC hDC, HFONT hFont, CDUIRect &rcItem, LPCTSTR lpszText, DWORD dwTextColor, DWORD dwTextStyle, bool bGdiplusRender = false, Gdiplus::TextRenderingHint RenderType = TextRenderingHintSystemDefault, bool bCeilFloat = true, bool bShadow = false);
	static void DrawRichText(HDC hDC, CDUIRect &rcItem, const tagDuiRichText &RichText, bool bGdiplusRender = false, Gdiplus::TextRenderingHint RenderType = TextRenderingHintSystemDefault, int nLineSpace = 0, bool bShadow = false);
	static void DrawRichText(HDC hDC, CDUIRect &rcItem, const MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, DWORD dwTextStyle, bool bGdiplusRender = false, Gdiplus::TextRenderingHint RenderType = TextRenderingHintSystemDefault, bool bShadow = false);
	static SIZE GetTextSize(HDC hDC, HFONT hFont, LPCTSTR lpszText, DWORD dwTextStyle);
	
	//help
	static void RGBToHSL(IN DWORD ARGB, OUT float *pH, OUT float *pS, OUT float *pL);
	static DWORD HSLToRGB(float H, float S, float L);
	static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
	static void AdjustImage(CDUIImageBase *pImageBase, short H, short S, short L);
	static void AdjustImage(HBITMAP hBitmap, DWORD dwColorSrc, DWORD dwColorDest);
	static HBITMAP CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE** pBits, bool bPositive = true);
	static HBITMAP GenerateBitmap(CDUIWnd *pWnd, CDUIControlBase *pControl, const CDUIRect &rcItem, DWORD dwFilterColor = 0);
	static Bitmap * GenerateBitmap(Bitmap *pBmp, const CDUISize szGenerate);
	static HBITMAP CopyBitmap(HDC hDC, const CDUIRect &rcItem, DWORD dwFilterColor = 0);
	static HBITMAP CopyBitmap(HBITMAP hBitmap, DWORD dwFilterColor = 0);
	static Bitmap * GetAlphaBitmap(HBITMAP hBitmap, bool bPARGB = true);
	static bool SaveImage(HBITMAP hBitmap, CMMString strFile, bool bAlpahImage = false);

	//alpha help
	static void CheckAlphaColor(DWORD &dwColor);
	static void ClearPixel(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear);
	static void ClearPixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear);
	static void RestorePixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcRestore);
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIMemDC
{
public:
	//************************************
	// Description:	create membitmap size is Width of rcClient, Heigh of rcClient
	// if bCopyBitmap is true, copy hDC from rcDest to membitmap rcDest
	// if bCopyBitmap is true, finaly copy membitmap from rcDest to dc rcDest
	//************************************
	CDUIMemDC(HDC hDC, const CDUIRect rcDest, const CDUIRect rcClient, bool bCopyBitmap = true);

	//************************************
	// Description:	create membitmap size is Width of rcDest, Heigh of rcDest
	// if bCopyBitmap is true, copy hDC from rcDest to membitmap point(0,0)
	// finaly copy membitmap from point(0,0) to dc rcDest
	//************************************
	CDUIMemDC(HDC hDC, const CDUIRect rcDest, bool bCopyBitmap = true);

	virtual ~CDUIMemDC(void);

public:
	operator HDC() const;

	HDC GetSrcHDC();
	HDC GetMemHDC();
	HBITMAP GetMemBitmap();
	LPBYTE GetMemBmpBits();

	CDUIRect GetPaint();
	CDUIRect GetClient();

protected:
	HDC									m_hDC = NULL;
	HDC									m_hMemDC = NULL;

	HBITMAP								m_hBitmap = NULL;
	HBITMAP								m_hOldBitmap = NULL;
	LPBYTE								m_pBmpBits = NULL;

	CDUIRect							m_rcDest;
	CDUIRect							m_rcClient;

	bool								m_bFromStart = false;
	bool								m_bCopyBitmap = true;

private:
	const CDUIMemDC & operator = (CDUIMemDC);
};

//////////////////////////////////////////////////////////////////////////

#endif 
