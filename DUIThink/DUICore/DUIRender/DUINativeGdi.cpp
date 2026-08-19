#include "StdAfx.h"
#include "DUICanvasRaster.h"

#if defined(DuiPlatform_SDL)

//////////////////////////////////////////////////////////////////////////
BOOL DeleteObject(HGDIOBJ hObject)
{
	if (NULL == hObject) return FALSE;
	delete (IDuiNativeGdi *)hObject;
	return TRUE;
}

BOOL DeleteDC(HDC hdc)
{
	return DeleteObject((HGDIOBJ)hdc);
}

HDC CreateCompatibleDC(HDC hdc)
{
	IDuiCanvas *pSrc = DuiCanvasFromHDC(hdc);
	const int nWidth = pSrc ? max(1, pSrc->GetWidth()) : 1;
	const int nHeight = pSrc ? max(1, pSrc->GetHeight()) : 1;
	return DuiHDCFromCanvas(new CDUICanvasRaster(nWidth, nHeight));
}

HGDIOBJ SelectObject(HDC, HGDIOBJ)
{
	return NULL;
}

BOOL BitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD)
{
	IDuiCanvas *pDst = DuiCanvasFromHDC(hdcDest);
	IDuiCanvas *pSrc = DuiCanvasFromHDC(hdcSrc);
	if (NULL == pDst || NULL == pSrc) return FALSE;
	pDst->BlitFrom(pSrc, nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc);
	return TRUE;
}

DWORD GetObjectType(HGDIOBJ h)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)h;
	if (NULL == pNative) return 0;
	switch (pNative->GetNativeKind())
	{
		case IDuiNativeGdi::Kind_Canvas: return OBJ_MEMDC;
		case IDuiNativeGdi::Kind_Image: return OBJ_BITMAP;
		case IDuiNativeGdi::Kind_Font: return OBJ_FONT;
		default: break;
	}
	return 0;
}

int SaveDC(HDC hdc)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas) return 0;
	pCanvas->Save();
	return 1;
}

BOOL RestoreDC(HDC hdc, int)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas) return FALSE;
	pCanvas->Restore();
	return TRUE;
}

HFONT CreateFontIndirect(const LOGFONT *lplf)
{
	if (NULL == lplf) return NULL;
	int nSize = lplf->lfHeight;
	if (nSize < 0) nSize = -nSize;
	if (nSize == 0) nSize = 12;
	return (HFONT)CDUIFontRaster::Create(lplf->lfFaceName, nSize, lplf->lfWeight,
		lplf->lfItalic != 0, lplf->lfUnderline != 0, lplf->lfStrikeOut != 0);
}

HGDIOBJ GetStockObject(int)
{
	return NULL;
}

int GetObject(HGDIOBJ h, int c, LPVOID pv)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)h;
	if (NULL == pNative || NULL == pv || c < (int)sizeof(BITMAP)) return 0;
	if (IDuiNativeGdi::Kind_Image != pNative->GetNativeKind()
		&& IDuiNativeGdi::Kind_Canvas != pNative->GetNativeKind())
	{
		return 0;
	}

	BITMAP *pBmp = (BITMAP *)pv;
	memset(pBmp, 0, sizeof(BITMAP));
	pBmp->bmWidth = pNative->GetWidth();
	pBmp->bmHeight = pNative->GetHeight();
	pBmp->bmWidthBytes = pNative->GetPitch();
	pBmp->bmPlanes = 1;
	pBmp->bmBitsPixel = 32;
	pBmp->bmBits = pNative->GetBits();
	return sizeof(BITMAP);
}

int GetDeviceCaps(HDC, int nIndex)
{
	if (LOGPIXELSX == nIndex || LOGPIXELSY == nIndex) return 96;
	return 0;
}

BOOL UpdateLayeredWindow(HWND, HDC, POINT *, SIZE *, HDC, POINT *, DWORD, BLENDFUNCTION *, DWORD)
{
	return FALSE;
}

#endif
