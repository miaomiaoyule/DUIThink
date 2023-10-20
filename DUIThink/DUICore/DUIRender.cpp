#include "StdAfx.h"
#include <atlimage.h>

/////////////////////////////////////////////////////////////////////////////////////
CDUIRenderClip::~CDUIRenderClip()
{
	if (NULL == hDC) return;

	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	ASSERT(::GetObjectType(hRgn) == OBJ_REGION);
	ASSERT(::GetObjectType(hOldRgn) == OBJ_REGION);
	::SelectClipRgn(hDC, hOldRgn);
	MMSafeDeleteObject(hOldRgn);
	MMSafeDeleteObject(hRgn);

	return;
}

void CDUIRenderClip::GenerateClip(HDC hDC, RECT rc, CDUIRenderClip &clip)
{
	RECT rcClip = {};
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;

	return;
}

void CDUIRenderClip::GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CDUIRenderClip &clip)
{
	RECT rcClip = {};
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
	::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;
	MMSafeDeleteObject(hRgnItem);
}

void CDUIRenderClip::GenerateEllipseClip(HDC hDC, RECT rcPaint, RECT rcItem, CDUIRenderClip &clip)
{
	RECT rcClip = {};
	::GetClipBox(hDC, &rcClip);

	clip.hOldRgn = ::CreateEllipticRgnIndirect(&rcClip);
	clip.hRgn = ::CreateEllipticRgnIndirect(&rcPaint);
	HRGN hRgnItem = ::CreateEllipticRgnIndirect(&rcItem);

	::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);

	clip.hDC = hDC;
	clip.rcItem = rcPaint;
	MMSafeDeleteObject(hRgnItem);
}

void CDUIRenderClip::UseOldClipBegin(HDC hDC, CDUIRenderClip &clip)
{
	::SelectClipRgn(hDC, clip.hOldRgn);
}

void CDUIRenderClip::UseOldClipEnd(HDC hDC, CDUIRenderClip &clip)
{
	::SelectClipRgn(hDC, clip.hRgn);
}

/////////////////////////////////////////////////////////////////////////////////////
static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
	return RGB(GetRValue(clrSrc) * src_darken + GetRValue(clrDest) * dest_darken,
		GetGValue(clrSrc) * src_darken + GetGValue(clrDest) * dest_darken,
		GetBValue(clrSrc) * src_darken + GetBValue(clrDest) * dest_darken);
}

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
	int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
	HDC hTempDC = ::CreateCompatibleDC(hDC);
	if (NULL == hTempDC)
		return FALSE;

	//Creates Source DIB
	COLORREF *pSrcBits = NULL;
	HBITMAP hSrcDib = CDUIRenderEngine::CreateARGB32Bitmap(hSrcDC, dwWidth, dwHeight, (BYTE **)&pSrcBits, false);
	if ((NULL == hSrcDib) || (NULL == pSrcBits))
	{
		MMSafeDeleteDC(hTempDC);
		return FALSE;
	}

	HBITMAP hOldTempBmp = (HBITMAP)::SelectObject(hTempDC, hSrcDib);
	::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
	::SelectObject(hTempDC, hOldTempBmp);

	//Creates Destination DIB
	COLORREF *pDestBits = NULL;
	HBITMAP hDestDib = CDUIRenderEngine::CreateARGB32Bitmap(hSrcDC, dwWidth, dwHeight, (BYTE **)&pDestBits, false);
	if ((NULL == hDestDib) || (NULL == pDestBits))
	{
		MMSafeDeleteObject(hSrcDib);
		MMSafeDeleteDC(hTempDC);
		return FALSE;
	}

	::SelectObject(hTempDC, hDestDib);
	::BitBlt(hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
	::SelectObject(hTempDC, hOldTempBmp);

	double src_darken;
	BYTE nAlpha;

	for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
	{
		nAlpha = LOBYTE(*pSrcBits >> 24);
		src_darken = (double)(nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
		if (src_darken < 0.0) src_darken = 0.0;
		*pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
	} //for

	::SelectObject(hTempDC, hDestDib);
	::BitBlt(hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
	::SelectObject(hTempDC, hOldTempBmp);

	MMSafeDeleteObject(hDestDib);
	MMSafeDeleteObject(hSrcDib);
	MMSafeDeleteDC(hTempDC);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void CDUIRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
	BYTE cbAlpha, bool bAlpha, bool bHole, bool bTiledX, bool bTiledY)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (NULL == hDC || NULL == hBitmap) return;

	//verify
	CDUIRect rcTemp;
	CDUIRect rcDest;
	if (false == IntersectRect(&rcTemp, &rcItem, &rcPaint)) return;

	//info
	typedef BOOL(WINAPI *LPALPHABLEND)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = AlphaBlend;
	if (NULL == lpAlphaBlend) lpAlphaBlend = AlphaBitBlt;

	HDC hCloneDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCloneDC, hBitmap);
	int nSaveDC = SaveDC(hDC);

	//draw
	bool bAlphaBlend = (lpAlphaBlend && (bAlpha || cbAlpha < 255));
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, cbAlpha, AC_SRC_ALPHA };

	if (rcItem.GetWidth() == rcBmpPart.GetWidth() \
		&& rcItem.GetHeight() == rcBmpPart.GetHeight() \
		&& rcCorner.left == 0 && rcCorner.right == 0 && rcCorner.top == 0 && rcCorner.bottom == 0)
	{
		rcDest = rcItem;
		if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
		{
			if (bAlphaBlend)
			{
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(),
					hCloneDC, rcBmpPart.left, rcBmpPart.top, rcBmpPart.GetWidth(), rcBmpPart.GetHeight(), bf);
			}
			else
			{
				::BitBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), \
					hCloneDC, rcBmpPart.left, rcBmpPart.top, SRCCOPY);
			}
		}
	}
	else
	{
		//middle
		if (false == bHole)
		{
			do
			{
				rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
				rcDest.left += rcCorner.left;
				rcDest.top += rcCorner.top;
				rcDest.right = rcItem.right - rcCorner.right;
				rcDest.bottom = rcItem.bottom - rcCorner.bottom;
				if (false == ::IntersectRect(&rcTemp, &rcPaint, &rcDest)) break;

				if (!bTiledX && !bTiledY)
				{
					if (bAlphaBlend)
					{
						lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
							rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, \
							rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
					}
					else
					{
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
							rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, \
							rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
					}

					break;
				}

				if (bTiledX && bTiledY)
				{
					LONG lWidth = rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right;
					LONG lHeight = rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom;
					int iTimesX = (rcDest.GetWidth() + lWidth - 1) / lWidth;
					int iTimesY = (rcDest.GetHeight() + lHeight - 1) / lHeight;
					for (int j = 0; j < iTimesY; ++j)
					{
						LONG lDestTop = rcDest.top + lHeight * j;
						LONG lDestBottom = rcDest.top + lHeight * (j + 1);
						LONG lDrawHeight = lHeight;
						if (lDestBottom > rcDest.bottom)
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						for (int i = 0; i < iTimesX; ++i)
						{
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if (lDestRight > rcDest.right)
							{
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}

							if (bAlphaBlend)
							{
								lpAlphaBlend(hDC, lDestLeft, lDestTop, lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC,
									rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, lDrawWidth, lDrawHeight, bf);
							}
							else
							{
								::BitBlt(hDC, lDestLeft, lDestTop, lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
									rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, SRCCOPY);
							}
						}
					}

					break;
				}

				if (bTiledX)
				{
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorner.left - rcCorner.right;
					int iTimes = (rcDest.GetWidth() + lWidth - 1) / lWidth;
					for (int i = 0; i < iTimes; ++i)
					{
						LONG lDestLeft = rcDest.left + lWidth * i;
						LONG lDestRight = rcDest.left + lWidth * (i + 1);
						LONG lDrawWidth = lWidth;
						if (lDestRight > rcDest.right)
						{
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}
						if (bAlphaBlend)
						{
							lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.GetHeight(),
								hCloneDC, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
								lDrawWidth, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
						}
						else
						{
							::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.GetHeight(),
								hCloneDC, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
								lDrawWidth, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
						}
					}

					break;
				}

				//bTiledY
				{
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorner.top - rcCorner.bottom;
					int iTimes = (rcDest.GetHeight() + lHeight - 1) / lHeight;
					for (int i = 0; i < iTimes; ++i)
					{
						LONG lDestTop = rcDest.top + lHeight * i;
						LONG lDestBottom = rcDest.top + lHeight * (i + 1);
						LONG lDrawHeight = lHeight;
						if (lDestBottom > rcDest.bottom)
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						if (bAlphaBlend)
						{
							lpAlphaBlend(hDC, rcDest.left, lDestTop, rcDest.GetWidth(), lDestBottom - lDestTop,
								hCloneDC, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
								rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, lDrawHeight, bf);
						}
						else
						{
							::StretchBlt(hDC, rcDest.left, lDestTop, rcDest.GetWidth(), lDestBottom - lDestTop,
								hCloneDC, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
								rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, lDrawHeight, SRCCOPY);
						}
					}
				}

			} while (false);
		}

		//left-top
		if (rcCorner.left > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left, rcBmpPart.top, rcCorner.left, rcCorner.top, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left, rcBmpPart.top, rcCorner.left, rcCorner.top, SRCCOPY);
				}
			}
		}
		//left
		if (rcCorner.left > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.top += rcCorner.top;
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left, rcBmpPart.top + rcCorner.top,
						rcCorner.left, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left, rcBmpPart.top + rcCorner.top,
						rcCorner.left, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
				}
			}
		}
		//top
		if (rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.top,
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.top, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.top,
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.top, SRCCOPY);
				}
			}
		}

		//right-bottom
		if (rcCorner.right > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.bottom - rcDest.bottom);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.bottom - rcCorner.bottom,
						rcCorner.right, rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.bottom - rcCorner.bottom,
						rcCorner.right, rcCorner.bottom, SRCCOPY);
				}
			}
		}
		//right
		if (rcCorner.right > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top += rcCorner.top;
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top + rcCorner.top,
						rcCorner.right, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top + rcCorner.top,
						rcCorner.right, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
				}
			}
		}
		//bottom
		if (rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.bottom - rcCorner.bottom, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.bottom - rcCorner.bottom, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.bottom, SRCCOPY);
				}
			}
		}

		//right-top
		if (rcCorner.right > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top, rcCorner.right, rcCorner.top, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top, rcCorner.right, rcCorner.top, SRCCOPY);
				}
			}
		}

		//left-bottom
		if (rcCorner.left > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.right = rcDest.left + rcCorner.left;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hCloneDC, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, SRCCOPY);
				}
			}
		}
	}

	::SelectObject(hCloneDC, hOldBitmap);
	MMSafeDeleteDC(hCloneDC);
	RestoreDC(hDC, nSaveDC);

	return;
}

void CDUIRenderEngine::DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (NULL == hDC || NULL == pBmp) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	Gp.DrawImage(pBmp, Gdiplus::Rect(rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight()), 0, 0, pBmp->GetWidth(), pBmp->GetHeight(), UnitPixel);

	return;
}

void CDUIRenderEngine::DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
	BYTE cbAlpha, bool bAlpha, bool bHole, bool bTiledX, bool bTiledY)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (NULL == hDC || NULL == pBmp) return;

	//verify
	CDUIRect rcTemp;
	CDUIRect rcDest;
	if (false == IntersectRect(&rcTemp, &rcItem, &rcPaint)) return;

	//info
	Gdiplus::Graphics Gp(hDC);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//draw
	bool bAlphaBlend = (bAlpha || cbAlpha < 255);
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, cbAlpha, AC_SRC_ALPHA };

	if (rcItem.GetWidth() == rcBmpPart.GetWidth() \
		&& rcItem.GetHeight() == rcBmpPart.GetHeight() \
		&& rcCorner.left == 0 && rcCorner.right == 0 && rcCorner.top == 0 && rcCorner.bottom == 0)
	{
		rcDest = rcItem;
		if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
		{
			Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()),
				rcBmpPart.left, rcBmpPart.top, rcBmpPart.GetWidth(), rcBmpPart.GetHeight(), UnitPixel);
		}
	}
	else
	{
		//middle
		if (false == bHole)
		{
			do
			{
				rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
				rcDest.left += rcCorner.left;
				rcDest.top += rcCorner.top;
				rcDest.right = rcItem.right - rcCorner.right;
				rcDest.bottom = rcItem.bottom - rcCorner.bottom;
				if (false == ::IntersectRect(&rcTemp, &rcPaint, &rcDest)) break;

				if (!bTiledX && !bTiledY)
				{
					Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
						rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, \
						rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, UnitPixel);

					break;
				}

				if (bTiledX && bTiledY)
				{
					LONG lWidth = rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right;
					LONG lHeight = rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom;
					int iTimesX = (rcDest.GetWidth() + lWidth - 1) / lWidth;
					int iTimesY = (rcDest.GetHeight() + lHeight - 1) / lHeight;
					for (int j = 0; j < iTimesY; ++j)
					{
						LONG lDestTop = rcDest.top + lHeight * j;
						LONG lDestBottom = rcDest.top + lHeight * (j + 1);
						LONG lDrawHeight = lHeight;
						if (lDestBottom > rcDest.bottom)
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						for (int i = 0; i < iTimesX; ++i)
						{
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if (lDestRight > rcDest.right)
							{
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}

							Gp.DrawImage(pBmp, Gdiplus::Rect(lDestLeft, lDestTop, lDestRight - lDestLeft, lDestBottom - lDestTop),
								rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, lDrawWidth, lDrawHeight, UnitPixel);
						}
					}

					break;
				}

				if (bTiledX)
				{
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorner.left - rcCorner.right;
					int iTimes = (rcDest.GetWidth() + lWidth - 1) / lWidth;
					for (int i = 0; i < iTimes; ++i)
					{
						LONG lDestLeft = rcDest.left + lWidth * i;
						LONG lDestRight = rcDest.left + lWidth * (i + 1);
						LONG lDrawWidth = lWidth;
						if (lDestRight > rcDest.right)
						{
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}
						
						Gp.DrawImage(pBmp, Gdiplus::Rect(lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.GetHeight()),
							rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							lDrawWidth, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, UnitPixel);

					}

					break;
				}

				//bTiledY
				{
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorner.top - rcCorner.bottom;
					int iTimes = (rcDest.GetHeight() + lHeight - 1) / lHeight;
					for (int i = 0; i < iTimes; ++i)
					{
						LONG lDestTop = rcDest.top + lHeight * i;
						LONG lDestBottom = rcDest.top + lHeight * (i + 1);
						LONG lDrawHeight = lHeight;
						if (lDestBottom > rcDest.bottom)
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						
						Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, lDestTop, rcDest.GetWidth(), lDestBottom - lDestTop),
							rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, lDrawHeight, UnitPixel);
					}
				}

			} while (false);
		}

		//left-top
		if (rcCorner.left > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left, rcBmpPart.top, rcCorner.left, rcCorner.top, UnitPixel);
			}
		}
		//left
		if (rcCorner.left > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.top += rcCorner.top;
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left, rcBmpPart.top + rcCorner.top,
					rcCorner.left, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, UnitPixel);
			}
		}
		//top
		if (rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left + rcCorner.left, rcBmpPart.top,
					rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.top, UnitPixel);
			}
		}

		//right-bottom
		if (rcCorner.right > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.bottom - rcDest.bottom);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.right - rcCorner.right, rcBmpPart.bottom - rcCorner.bottom,
					rcCorner.right, rcCorner.bottom, UnitPixel);
			}
		}
		//right
		if (rcCorner.right > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top += rcCorner.top;
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.right - rcCorner.right, rcBmpPart.top + rcCorner.top,
					rcCorner.right, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, UnitPixel);
			}
		}
		//bottom
		if (rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left + rcCorner.left, rcBmpPart.bottom - rcCorner.bottom, \
					rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.bottom, UnitPixel);
			}
		}

		//right-top
		if (rcCorner.right > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.right - rcCorner.right, rcBmpPart.top, rcCorner.right, rcCorner.top, UnitPixel);
			}
		}

		//left-bottom
		if (rcCorner.left > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.right = rcDest.left + rcCorner.left;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left, rcBmpPart.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, UnitPixel);
			}
		}
	}

	return;
}

void CDUIRenderEngine::DrawAnimateImage(HDC hDC, Gdiplus::Bitmap *pBmpAnimate, const CDUIRect &rcItem, int nFrameCur)
{
	if (NULL == pBmpAnimate) return;

	GUID pageGuid = Gdiplus::FrameDimensionTime;
	Gdiplus::Graphics Gp(hDC);
	pBmpAnimate->SelectActiveFrame(&pageGuid, nFrameCur);
	Gp.DrawImage(pBmpAnimate, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight());

	return;
}

void CDUIRenderEngine::DrawLine(HDC hDC, const CDUIRect &rcItem, int nLineSize, DWORD dwPenColor, int nStyle /*= PS_SOLID*/)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	Gdiplus::Graphics Gp(hDC);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetDashStyle((Gdiplus::DashStyle)nStyle);
	Gp.DrawLine(&Pen, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);

	return;
}

void CDUIRenderEngine::DrawRect(HDC hDC, const CDUIRect &rcItem, int nLineSize, DWORD dwPenColor)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	//gdi+
	//if (bGdiplusRender)
	{
		Gdiplus::Graphics Gp(hDC);
		Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), (Gdiplus::REAL)nLineSize);
		Pen.SetAlignment(Gdiplus::PenAlignmentInset);

		Gp.DrawRectangle(&Pen, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight());

		return;
	}

	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nLineSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
	::SelectObject(hDC, hOldPen);
	MMSafeDeleteObject(hPen);

	return;
}

void CDUIRenderEngine::DrawRound(HDC hDC, const CDUIRect &rcItem, int nXRound, int nYRound, int nLineSize, DWORD dwPenColor)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetCompositingQuality(CompositingQuality::CompositingQualityHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//left top right bottom
	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Gp.DrawArc(&Pen, rcItem.left, rcItem.top, nXRound * 2, nYRound * 2, 180, 90); // 左上圆角
	Gp.DrawArc(&Pen, rcItem.right - nXRound * 2 - nLineSize, rcItem.top, nXRound * 2, nYRound * 2, 270, 90); // 右上圆角
	Gp.DrawArc(&Pen, rcItem.right - nXRound * 2 - nLineSize, rcItem.bottom - nYRound * 2 - nLineSize, nXRound * 2, nYRound * 2, 0, 90); // 右下圆角
	Gp.DrawArc(&Pen, rcItem.left, rcItem.bottom - nYRound * 2 - nLineSize, nXRound * 2, nYRound * 2, 90, 90); // 左下圆角

	return;
}

void CDUIRenderEngine::DrawRoundRect(HDC hDC, const CDUIRect &rcItem, int nXRound, int nYRound, int nLineSize, DWORD dwPenColor)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	//gid+
	{
		Gdiplus::Graphics Gp(hDC);
		Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		Gp.SetCompositingQuality(CompositingQuality::CompositingQualityHighQuality);
		Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

		//adjust
		CDUIRect rcDraw = rcItem;
		rcDraw.top += nLineSize;
		rcDraw.left += nLineSize;

		//path
		Gdiplus::GraphicsPath Path;
		Path.AddLine(rcDraw.left + nXRound, rcDraw.top, rcDraw.right - nXRound, rcDraw.top); //顶部横线
		Path.AddArc(rcDraw.right - nXRound * 2 - nLineSize, rcDraw.top, nXRound * 2, nYRound * 2, 270, 90); //右上圆角
		Path.AddLine(rcDraw.right - nLineSize, rcDraw.top + nYRound, rcDraw.right - nLineSize, rcDraw.bottom - nYRound); //右侧竖线
		Path.AddArc(rcDraw.right - nXRound * 2 - nLineSize, rcDraw.bottom - nYRound * 2 - nLineSize, nXRound * 2, nYRound * 2, 0, 90); //右下圆角
		Path.AddLine(rcDraw.right - nXRound, rcDraw.bottom - nLineSize, rcDraw.left + nXRound, rcDraw.bottom - nLineSize); //底部横线
		Path.AddArc(rcDraw.left, rcDraw.bottom - nYRound * 2 - nLineSize, nXRound * 2, nYRound * 2, 90, 90); //左下圆角
		Path.AddLine(rcDraw.left, rcDraw.bottom - nYRound, rcDraw.left, rcDraw.top + nYRound); //左侧竖线
		Path.AddArc(rcDraw.left, rcDraw.top, nXRound * 2, nYRound * 2, 180, 90); //左上圆角

		Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
		Gp.DrawPath(&Pen, &Path);

		return;
	}

	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nLineSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::RoundRect(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom, nXRound, nYRound);
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);
	MMSafeDeleteObject(hPen);

	return;
}

void CDUIRenderEngine::FillRect(HDC hDC, const CDUIRect &rcItem, DWORD dwColor)
{
	if (dwColor <= 0x00ffffff) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::SolidBrush Brush(Gdiplus::Color((Gdiplus::ARGB)dwColor));

	Gp.FillRectangle(&Brush, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight());

	return;
}

void CDUIRenderEngine::FillRectGradient(HDC hDC, const CDUIRect &rcItem, DWORD dwFirstColor, DWORD dwSecondColor, bool bVertical, int nSteps)
{
	typedef BOOL(WINAPI *LPALPHABLEND)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
	if (lpAlphaBlend == NULL) lpAlphaBlend = AlphaBitBlt;
	typedef BOOL(WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
	static PGradientFill lpGradientFill = (PGradientFill)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

	BYTE bAlpha = DUIARGBGetA(dwFirstColor);
	if (bAlpha == 0) return;

	int cx = rcItem.right - rcItem.left;
	int cy = rcItem.bottom - rcItem.top;
	RECT rcPaint = rcItem;
	HDC hPaintDC = hDC;
	HBITMAP hPaintBitmap = NULL;
	HBITMAP hOldPaintBitmap = NULL;
	if (bAlpha < 255)
	{
		rcPaint.left = rcPaint.top = 0;
		rcPaint.right = cx;
		rcPaint.bottom = cy;
		hPaintDC = ::CreateCompatibleDC(hDC);
		hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
		ASSERT(hPaintDC);
		ASSERT(hPaintBitmap);
		hOldPaintBitmap = (HBITMAP)::SelectObject(hPaintDC, hPaintBitmap);
	}
	if (lpGradientFill != NULL)
	{
		TRIVERTEX triv[2] =
		{
			{ rcPaint.left, rcPaint.top, DUIARGBGetR(dwFirstColor) << 8, DUIARGBGetG(dwFirstColor) << 8, DUIARGBGetB(dwFirstColor) << 8, 0xFF00 },
			{ rcPaint.right, rcPaint.bottom, DUIARGBGetR(dwSecondColor) << 8, DUIARGBGetG(dwSecondColor) << 8, DUIARGBGetB(dwSecondColor) << 8, 0xFF00 }
		};

		GRADIENT_RECT grc = { 0, 1 };
		lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
	}
	else
	{
		// Determine how many shades
		int nShift = 1;
		if (nSteps >= 64) nShift = 6;
		else if (nSteps >= 32) nShift = 5;
		else if (nSteps >= 16) nShift = 4;
		else if (nSteps >= 8) nShift = 3;
		else if (nSteps >= 4) nShift = 2;
		int nLines = 1 << nShift;
		for (int i = 0; i < nLines; i++)
		{
			// Do a little alpha blending
			BYTE bR = (BYTE)((DUIARGBGetR(dwSecondColor) * (nLines - i) + DUIARGBGetR(dwFirstColor) * i) >> nShift);
			BYTE bG = (BYTE)((DUIARGBGetG(dwSecondColor) * (nLines - i) + DUIARGBGetG(dwFirstColor) * i) >> nShift);
			BYTE bB = (BYTE)((DUIARGBGetB(dwSecondColor) * (nLines - i) + DUIARGBGetB(dwFirstColor) * i) >> nShift);
			// ... then paint with the resulting color
			HBRUSH hBrush = ::CreateSolidBrush(RGB(bR, bG, bB));
			RECT r2 = rcPaint;
			if (bVertical)
			{
				r2.bottom = rcItem.bottom - ((i * (rcItem.bottom - rcItem.top)) >> nShift);
				r2.top = rcItem.bottom - (((i + 1) * (rcItem.bottom - rcItem.top)) >> nShift);
				if ((r2.bottom - r2.top) > 0) ::FillRect(hDC, &r2, hBrush);
			}
			else
			{
				r2.left = rcItem.right - (((i + 1) * (rcItem.right - rcItem.left)) >> nShift);
				r2.right = rcItem.right - ((i * (rcItem.right - rcItem.left)) >> nShift);
				if ((r2.right - r2.left) > 0) ::FillRect(hPaintDC, &r2, hBrush);
			}
			MMSafeDeleteObject(hBrush);
		}
	}
	if (bAlpha < 255)
	{
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
		lpAlphaBlend(hDC, rcItem.left, rcItem.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
		::SelectObject(hPaintDC, hOldPaintBitmap);
		MMSafeDeleteObject(hPaintBitmap);
		MMSafeDeleteDC(hPaintDC);
	}

	return;
}

void CDUIRenderEngine::FillRoundRect(HDC hDC, const CDUIRect &rcItem, int nXRound, int nYRound, int nLineSize, DWORD dwColor)
{
	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//adjust
	CDUIRect rcDraw = rcItem;
	rcDraw.top += nLineSize;
	rcDraw.left += nLineSize;

	//path
	Gdiplus::GraphicsPath Path;
	Path.AddLine(rcDraw.left + nXRound, rcDraw.top, rcDraw.right - nXRound, rcDraw.top); //顶部横线
	Path.AddArc(rcDraw.right - nXRound * 2 - nLineSize, rcDraw.top, nXRound * 2, nYRound * 2, 270, 90); //右上圆角
	Path.AddLine(rcDraw.right - nLineSize, rcDraw.top + nYRound, rcDraw.right - nLineSize, rcDraw.bottom - nYRound); //右侧竖线
	Path.AddArc(rcDraw.right - nXRound * 2 - nLineSize, rcDraw.bottom - nYRound * 2 - nLineSize, nXRound * 2, nYRound * 2, 0, 90); //右下圆角
	Path.AddLine(rcDraw.right - nXRound, rcDraw.bottom - nLineSize, rcDraw.left + nXRound, rcDraw.bottom - nLineSize); //底部横线
	Path.AddArc(rcDraw.left, rcDraw.bottom - nYRound * 2 - nLineSize, nXRound * 2, nYRound * 2, 90, 90); //左下圆角
	Path.AddLine(rcDraw.left, rcDraw.bottom - nYRound, rcDraw.left, rcDraw.top + nYRound); //左侧竖线
	Path.AddArc(rcDraw.left, rcDraw.top, nXRound * 2, nYRound * 2, 180, 90); //左上圆角

	Gdiplus::SolidBrush Brush(dwColor);
	Gp.FillPath(&Brush, &Path);

	return;
}

void CDUIRenderEngine::DrawText(HDC hDC, HFONT hFont, CDUIRect &rcItem, LPCTSTR lpszText, DWORD dwTextColor, DWORD dwTextStyle,
	bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, bool bCeilFloat, bool bShadow)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (MMInvalidString(lpszText) || NULL == hFont) return;

	//shadow
	if (bShadow)
	{
//#ifdef _DLL
		do
		{
			{
				int nSaveDC = SaveDC(hDC);
				::SetBkMode(hDC, TRANSPARENT);
				::SetTextColor(hDC, RGB(DUIARGBGetR(dwTextColor), DUIARGBGetG(dwTextColor), DUIARGBGetB(dwTextColor)));
				HFONT hFontOld = (HFONT)::SelectObject(hDC, hFont);
				::DrawShadowText(hDC, lpszText, -1, &rcItem, dwTextStyle | DT_NOPREFIX, RGB(DUIARGBGetR(dwTextColor), DUIARGBGetG(dwTextColor), DUIARGBGetB(dwTextColor)), RGB(0, 0, 0), 2, 2);
				::SelectObject(hDC, hFontOld);
				RestoreDC(hDC, nSaveDC);

				if (dwTextStyle & DT_CALCRECT)
				{
					rcItem.right += 4;
					rcItem.bottom += 4;
				}

				break;
			}

			LPBYTE pBmpBits = NULL;
			HDC hMemDC = ::CreateCompatibleDC(hDC);
			HBITMAP hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(hDC, rcItem.GetWidth(), rcItem.GetHeight(), &pBmpBits);
			if (NULL == hMemDC || NULL == hBitmap || NULL == pBmpBits) break;

			CDUIRect rcDraw(0, 0, rcItem.GetWidth(), rcItem.GetHeight());
			HBITMAP hBmpOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
			::SetBkMode(hMemDC, TRANSPARENT);
			HFONT hFontOld = (HFONT)::SelectObject(hMemDC, hFont);
			::DrawShadowText(hMemDC, lpszText, -1, &rcDraw, dwTextStyle | DT_NOPREFIX, RGB(DUIARGBGetR(dwTextColor), DUIARGBGetG(dwTextColor), DUIARGBGetB(dwTextColor)), RGB(0, 0, 0), 2, 2);

			BLENDFUNCTION Blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			RestorePixelAlpha(pBmpBits, rcDraw.GetWidth(), rcDraw);
			AlphaBitBlt(hDC, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight(),
				hMemDC, rcDraw.left, rcDraw.top, rcDraw.GetWidth(), rcDraw.GetHeight(), Blend);

			::SelectObject(hMemDC, hFontOld);
			::SelectObject(hMemDC, hBmpOld);
			MMSafeDeleteObject(hBitmap);
			MMSafeDeleteDC(hMemDC);

		} while (false);

		return;
//#endif
	}

	//gdiplus
	if (bGdiplusRender)
	{
		Gdiplus::Graphics Gp(hDC);
		Gdiplus::Font font(hDC, hFont);
		Gdiplus::RectF rectF((Gdiplus::REAL)rcItem.left, (Gdiplus::REAL)rcItem.top, (Gdiplus::REAL)(rcItem.GetWidth()), (Gdiplus::REAL)(rcItem.GetHeight()));
		Gdiplus::SolidBrush brush(Gdiplus::Color((Gdiplus::ARGB)dwTextColor));
		Gp.SetTextRenderingHint(RenderType);
		Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

		Gdiplus::StringFormat stringFormat = stringFormat.GenericTypographic();

		if ((dwTextStyle & DT_END_ELLIPSIS) != 0)
		{
			stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
		}

		int formatFlags = 0;
		if ((dwTextStyle & DT_NOCLIP) != 0)
		{
			formatFlags |= Gdiplus::StringFormatFlagsNoClip;
		}
		if ((dwTextStyle & DT_SINGLELINE) != 0)
		{
			formatFlags |= Gdiplus::StringFormatFlagsNoWrap;
		}

		stringFormat.SetFormatFlags(formatFlags);

		if ((dwTextStyle & DT_LEFT) != 0)
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((dwTextStyle & DT_CENTER) != 0)
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((dwTextStyle & DT_RIGHT) != 0)
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
		}
		else
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}

		if ((dwTextStyle & DT_TOP) != 0)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((dwTextStyle & DT_VCENTER) != 0)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((dwTextStyle & DT_BOTTOM) != 0)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		}
		else
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}

		if ((dwTextStyle & DT_CALCRECT) != 0)
		{
			Gdiplus::RectF bounds;
			Gp.MeasureString(lpszText, -1, &font, rectF, &stringFormat, &bounds);

			if (bounds.Width < 1)
			{
				if ('\t' == *lpszText)
				{
					bounds.Width = 60;
				}
				else if (' ' == *lpszText)
				{
					bounds.Width = 8;
				}
			}
			if (bCeilFloat)
			{
				rcItem.bottom = rcItem.top + ceil(bounds.Height);
				rcItem.right = rcItem.left + ceil(bounds.Width);
			}
			else
			{
				rcItem.bottom = rcItem.top + (long)round(bounds.Height);
				rcItem.right = rcItem.left + (long)round(bounds.Width);
			}
		}
		else
		{
			Gp.DrawString(lpszText, (int)-1, &font, rectF, &stringFormat, &brush);
		}

		return;
	}

	//gdi
	int nSaveDC = SaveDC(hDC);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(DUIARGBGetR(dwTextColor), DUIARGBGetG(dwTextColor), DUIARGBGetB(dwTextColor)));
	HFONT hFontOld = (HFONT)::SelectObject(hDC, hFont);
	::DrawText(hDC, lpszText, -1, &rcItem, dwTextStyle | DT_NOPREFIX);
	::SelectObject(hDC, hFontOld);
	RestoreDC(hDC, nSaveDC);

	//tab width
	if (rcItem.GetWidth() < 1 && '\t' == *lpszText)
	{
		rcItem.right = rcItem.left + 60;
	}

	return;
}

void CDUIRenderEngine::DrawRichText(HDC hDC, CDUIRect &rcItem, const tagDuiRichText &RichText,
	bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, int nLineSpace, bool bShadow)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (RichText.vecRichTextItem.empty()) return;

	DWORD dwTextStyle = RichText;
	if (0 == (DT_CALCRECT & dwTextStyle))
	{
		tagDuiRichText RichTextTemp = RichText;
		RichTextTemp.dwTextStyle |= DT_CALCRECT;

		CDUIRect rcMeasure(rcItem.left, rcItem.top, rcItem.left, rcItem.top);
		MapLineVecDuiRichTextDraw mapLineVecRichTextDraw;
		CDUIRenderHelp::MeasureRichText(hDC, RichTextTemp, mapLineVecRichTextDraw, rcMeasure, rcItem, bGdiplusRender, RenderType, nLineSpace);
		CDUIRenderHelp::AdjustRichText(RichTextTemp, rcItem, mapLineVecRichTextDraw, rcMeasure);

		CDUIRenderEngine::DrawRichText(hDC, rcItem, mapLineVecRichTextDraw, dwTextStyle, bGdiplusRender, RenderType, bShadow);

		return;
	}

	CDUIRect rcMeasure(rcItem.left, rcItem.top, rcItem.left, rcItem.top);
	MapLineVecDuiRichTextDraw mapLineVecRichTextDraw;
	CDUIRenderHelp::MeasureRichText(hDC, RichText, mapLineVecRichTextDraw, rcMeasure, rcItem, bGdiplusRender, RenderType, nLineSpace);

	rcItem = rcMeasure;

	return;
}

void CDUIRenderEngine::DrawRichText(HDC hDC, CDUIRect &rcItem, const MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, DWORD dwTextStyle,
	bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, bool bShadow)
{
	DWORD dwColorBk[] = { 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff };
	int nColorBk = 0;
	bool bEndEllipsis = (dwTextStyle & DT_END_ELLIPSIS);
	dwTextStyle = dwTextStyle & ~DT_WORDBREAK & ~DT_END_ELLIPSIS | DT_SINGLELINE;
	dwTextStyle = 0 == (dwTextStyle & DT_RIGHT) ? dwTextStyle & ~DT_CENTER | DT_LEFT : dwTextStyle;
	dwTextStyle = 0 == (dwTextStyle & DT_BOTTOM) ? dwTextStyle & ~DT_VCENTER | DT_BOTTOM : dwTextStyle;

	for (int nLine = 0; nLine < mapLineVecRichTextDraw.size(); nLine++)
	{
		auto FindIt = mapLineVecRichTextDraw.find(nLine);
		if (FindIt == mapLineVecRichTextDraw.end()) continue;

		//line verify
		CDUIRect rcLine = CDUIRenderHelp::GetLineRange(mapLineVecRichTextDraw, nLine);
		if (false == ::IntersectRect(&rcLine, &rcLine, &rcItem)) continue;

		//draw line
		auto &vecRichTextDraw = FindIt->second;
		for (int n = 0; n < vecRichTextDraw.size(); n++)
		{
			tagDuiRichTextDraw RichTextDrawItem = vecRichTextDraw[n];
			if (false == ::IntersectRect(&CDUIRect(), &RichTextDrawItem.rcDraw, &rcLine)) continue;

			switch (RichTextDrawItem.ItemType)
			{
				case RichTextItem_Text:
				{
					if (n == vecRichTextDraw.size() - 1
						&& mapLineVecRichTextDraw.rbegin()->first == nLine
						&& bEndEllipsis)
					{
						dwTextStyle |= DT_END_ELLIPSIS;
					}

					//DrawColor(hDC, RichTextDraw.rcText, dwColorBk[nColorBk++]);
					HFONT hFont = RichTextDrawItem.pFontBase ? RichTextDrawItem.pFontBase->GetHandle() : NULL;
					DrawText(hDC, hFont, RichTextDrawItem.rcDraw, RichTextDrawItem.strText, RichTextDrawItem.dwColor, dwTextStyle, bGdiplusRender, RenderType, true, bShadow);

					break;
				}
				case RichTextItem_Image:
				{
					CDUIImageBase *pImageBase = CDUIGlobal::GetInstance()->GetImageResource(RichTextDrawItem.strImageResName);
					if (NULL == pImageBase) break;

					tagDuiAnimateImageInfo AnimateImageInfo = pImageBase->GetAnimateImageInfo();
					switch (AnimateImageInfo.AnimateImageType)
					{
						case AnimateImage_Gif:
						{
							CDUIRenderEngine::DrawAnimateImage(hDC, pImageBase->GetAnimateImage(), RichTextDrawItem.rcDraw, RichTextDrawItem.nAnimateFrameCur);

							break;
						}
						default:
						{
							CDUIRect rcBmpPart(0, 0, pImageBase->GetWidth(), pImageBase->GetHeight());
							CDUIRenderEngine::DrawImage(hDC, pImageBase->GetHandle(), RichTextDrawItem.rcDraw, RichTextDrawItem.rcDraw, rcBmpPart, CDUIRect(), 255, pImageBase->IsAlpha());

							break;
						}
					}

					break;
				}
				default:
					break;
			}
		}
	}

	return;
}

SIZE CDUIRenderEngine::GetTextSize(HDC hDC, HFONT hFont, LPCTSTR lpszText, DWORD dwTextStyle)
{
	SIZE size = { 0,0 };
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (MMInvalidString(lpszText) || NULL == hFont) return size;

	int nSaveDC = SaveDC(hDC);
	::SetBkMode(hDC, TRANSPARENT);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	GetTextExtentPoint32(hDC, lpszText, _tcslen(lpszText), &size);
	::SelectObject(hDC, hOldFont);
	RestoreDC(hDC, nSaveDC);

	return size;
}

void CDUIRenderEngine::RGBToHSL(IN DWORD ARGB, OUT float *pH, OUT float *pS, OUT float *pL)
{
	float R = (float)(DUIARGBGetR(ARGB) / 255.0f);  //RGB from 0 to 255
	float G = (float)(DUIARGBGetG(ARGB) / 255.0f);
	float B = (float)(DUIARGBGetB(ARGB) / 255.0f);

	float fMin = min(R, min(G, B));		//Min. value of RGB
	float fMax = max(R, max(G, B));		//Max. value of RGB
	float fDelta = fMax - fMin;				//Delta RGB value

	float H = 0.0f, S = 0.0f, L = (fMax + fMin) / 2.0f;

	if (fDelta != 0)
	{
		//Chromatic data...
		float del_R, del_G, del_B;

		if (L < 0.5) S = fDelta / (fMax + fMin);
		else S = fDelta / (2.0f - fMax - fMin);

		del_R = (((fMax - R) / 6.0f) + (fDelta / 2.0f)) / fDelta;
		del_G = (((fMax - G) / 6.0f) + (fDelta / 2.0f)) / fDelta;
		del_B = (((fMax - B) / 6.0f) + (fDelta / 2.0f)) / fDelta;

		if (R == fMax) H = del_B - del_G;
		else if (G == fMax) H = (1.0f / 3.0f) + del_R - del_B;
		else if (B == fMax) H = (2.0f / 3.0f) + del_G - del_R;

		if (H < 0.0f) H += 1.0f;
		if (H > 1.0f)  H -= 1.0f;
	}

	*pH = H;
	*pS = S;
	*pL = L;

	return;
}

DWORD CDUIRenderEngine::HSLToRGB(float H, float S, float L)
{
	static const float OneThird = 1.0f / 3;

	DWORD dwColor = 0;
	const float
		q = 2 * L < 1 ? L * (1 + S) : (L + S - L * S),
		p = 2 * L - q,
		h = H / 360,
		tr = h + OneThird,
		tg = h,
		tb = h - OneThird,
		ntr = tr < 0 ? tr + 1 : (tr > 1 ? tr - 1 : tr),
		ntg = tg < 0 ? tg + 1 : (tg > 1 ? tg - 1 : tg),
		ntb = tb < 0 ? tb + 1 : (tb > 1 ? tb - 1 : tb),
		R = 255 * (6 * ntr < 1 ? p + (q - p) * 6 * ntr : (2 * ntr < 1 ? q : (3 * ntr < 2 ? p + (q - p) * 6 * (2.0f * OneThird - ntr) : p))),
		G = 255 * (6 * ntg < 1 ? p + (q - p) * 6 * ntg : (2 * ntg < 1 ? q : (3 * ntg < 2 ? p + (q - p) * 6 * (2.0f * OneThird - ntg) : p))),
		B = 255 * (6 * ntb < 1 ? p + (q - p) * 6 * ntb : (2 * ntb < 1 ? q : (3 * ntb < 2 ? p + (q - p) * 6 * (2.0f * OneThird - ntb) : p)));

	BYTE cbR = R < 0 ? 0 : (R > 255 ? 255 : R);
	BYTE cbG = G < 0 ? 0 : (G > 255 ? 255 : G);
	BYTE cbB = B < 0 ? 0 : (B > 255 ? 255 : B);

	return DUIARGB(0xff, cbR, cbG, cbB);
}

DWORD CDUIRenderEngine::AdjustColor(DWORD dwColor, short H, short S, short L)
{
	if (H == 180 && S == 100 && L == 100) return dwColor;
	float fH, fS, fL;
	float S1 = S / 100.0f;
	float L1 = L / 100.0f;
	RGBToHSL(dwColor, &fH, &fS, &fL);
	fH += (H - 180);
	fH = fH > 0 ? fH : fH + 360;
	fS *= S1;
	fL *= L1;
	dwColor = HSLToRGB(fH, fS, fL);
	return dwColor;
}

void CDUIRenderEngine::AdjustImage(CDUIImageBase *pImageBase, short H, short S, short L)
{
	if (NULL == pImageBase || NULL == pImageBase->GetHandle()) return;
	if (H == 180 && S == 100 && L == 100) return;

	BITMAP Bmp = {};
	GetObject(pImageBase->GetHandle(), sizeof(Bmp), &Bmp);
	if (NULL == Bmp.bmBits || 32 != Bmp.bmBitsPixel) return;

	float fH, fS, fL;
	float S1 = S / 100.0f;
	float L1 = L / 100.0f;
	for (int i = 0; i < Bmp.bmWidth * Bmp.bmHeight; i++)
	{
		RGBToHSL(*((DWORD *)Bmp.bmBits + i), &fH, &fS, &fL);
		fH += (H - 180);
		fH = fH > 0 ? fH : fH + 360;
		fS *= S1;
		fL *= L1;
		*((DWORD *)Bmp.bmBits + i) = HSLToRGB(fH, fS, fL);
	}

	return;
}

void CDUIRenderEngine::AdjustImage(HBITMAP hBitmap, DWORD dwColorSrc, DWORD dwColorDest)
{
	if (NULL == hBitmap || dwColorSrc < 0x00ffffff || dwColorSrc == dwColorDest) return;

	BITMAP Bmp = {};
	GetObject(hBitmap, sizeof(Bmp), &Bmp);
	if (NULL == Bmp.bmBits || 32 != Bmp.bmBitsPixel) return;

	for (int nHeight = 0; nHeight < Bmp.bmHeight; nHeight++)
	{
		BYTE *pBits = (BYTE *)Bmp.bmBits + (Bmp.bmWidthBytes * nHeight);
		for (int nWidth = 0; nWidth < Bmp.bmWidth; nWidth++)
		{
			DWORD &dwColor = *((DWORD *)pBits + nWidth);
			if (dwColor != dwColorSrc) continue;

			dwColor = dwColorDest;
		}
	}

	return;
}

HBITMAP CDUIRenderEngine::CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE **pBits, bool bPositive)
{
	BITMAPINFO BmpSrc = {};
	BmpSrc.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmpSrc.bmiHeader.biWidth = cx;
	BmpSrc.bmiHeader.biHeight = bPositive ? -cy : cy;
	BmpSrc.bmiHeader.biPlanes = 1;
	BmpSrc.bmiHeader.biBitCount = 32;
	BmpSrc.bmiHeader.biCompression = BI_RGB;
	BmpSrc.bmiHeader.biSizeImage = cx * cy * sizeof(ARGB);
	BmpSrc.bmiHeader.biXPelsPerMeter = 0;
	BmpSrc.bmiHeader.biYPelsPerMeter = 0;
	BmpSrc.bmiHeader.biClrUsed = 0;
	BmpSrc.bmiHeader.biClrImportant = 0;

	HBITMAP hBitmap = CreateDIBSection(hDC, &BmpSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);

	return hBitmap;
}

HBITMAP CDUIRenderEngine::GenerateBitmap(CDUIWndManager *pManager, CDUIControlBase *pControl, const CDUIRect &rcItem, DWORD dwFilterColor)
{
	HDC hPaintDC = ::CreateCompatibleDC(pManager->GetWndDC());
	HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetWndDC(), rcItem.right, rcItem.bottom);
	ASSERT(hPaintDC);
	ASSERT(hPaintBitmap);
	HBITMAP hOldPaintBitmap = (HBITMAP)::SelectObject(hPaintDC, hPaintBitmap);
	pControl->OnDraw(hPaintDC, rcItem, true);
	AdjustImage(hPaintBitmap, dwFilterColor, 0);

	LPBYTE pBmpBits = NULL;
	HDC hCloneDC = ::CreateCompatibleDC(pManager->GetWndDC());
	HBITMAP hBitmap = CreateARGB32Bitmap(pManager->GetWndDC(), rcItem.GetWidth(), rcItem.GetHeight(), &pBmpBits);
	ASSERT(hCloneDC);
	ASSERT(hBitmap);
	if (hBitmap != NULL)
	{
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCloneDC, hBitmap);
		::BitBlt(hCloneDC, 0, 0, rcItem.GetWidth(), rcItem.GetHeight(), hPaintDC, rcItem.left, rcItem.top, SRCCOPY);
		::SelectObject(hCloneDC, hOldBitmap);
		MMSafeDeleteDC(hCloneDC);
		::GdiFlush();
	}

	// Cleanup
	::SelectObject(hPaintDC, hOldPaintBitmap);
	MMSafeDeleteObject(hPaintBitmap);
	MMSafeDeleteDC(hPaintDC);

	return hBitmap;
}

HBITMAP CDUIRenderEngine::CopyBitmap(HDC hDC, const CDUIRect &rcItem, DWORD dwFilterColor)
{
	LPBYTE pBmpBits = NULL;
	HDC hPaintDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateARGB32Bitmap(hDC, rcItem.GetWidth(), rcItem.GetHeight(), &pBmpBits);
	ASSERT(hPaintDC);
	ASSERT(hBitmap);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hPaintDC, hBitmap);
	::BitBlt(hPaintDC, 0, 0, rcItem.GetWidth(), rcItem.GetHeight(), hDC, rcItem.left, rcItem.top, SRCCOPY);
	AdjustImage(hBitmap, dwFilterColor, 0);

	// Cleanup
	::SelectObject(hPaintDC, hOldBitmap);
	MMSafeDeleteDC(hPaintDC);

	return hBitmap;
}

HBITMAP CDUIRenderEngine::CopyBitmap(HBITMAP hBitmap, DWORD dwFilterColor/* = 0*/)
{
	Bitmap *pBmp = GetAlphaBitmap(hBitmap);
	if (NULL == pBmp) return NULL;

	HBITMAP hBmpClone = NULL;
	pBmp->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBmpClone);
	if (NULL == hBmpClone) return NULL;

	if (0 != dwFilterColor)
	{
		AdjustImage(hBmpClone, dwFilterColor, 0);
	}

	return hBmpClone;
}

Bitmap *CDUIRenderEngine::GetAlphaBitmap(HBITMAP hBitmap)
{
	BITMAP bmp = {};
	if (GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0) return NULL;

	LONG cbSize = bmp.bmWidthBytes * bmp.bmHeight;
	std::vector<BYTE> vecPixel;
	vecPixel.resize(cbSize);
	if (vecPixel.empty()) return NULL;

	BITMAPINFO bmpInfo = {};
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
	bmpInfo.bmiHeader.biHeight = bmp.bmHeight;	//正数说明数据从下到上，负数说明从上到下
	bmpInfo.bmiHeader.biPlanes = bmp.bmPlanes;
	bmpInfo.bmiHeader.biBitCount = bmp.bmBitsPixel;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HDC hDCScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	LONG cbCopied = GetDIBits(hDCScreen, hBitmap, 0, bmp.bmHeight, vecPixel.data(), &bmpInfo, DIB_RGB_COLORS);
	MMSafeDeleteDC(hDCScreen);

	if (cbCopied == 0) return NULL;

	Bitmap *pBitmap = new Bitmap(bmp.bmWidth, bmp.bmHeight, PixelFormat32bppARGB);
	if (NULL == pBitmap) return NULL;

	BitmapData bitmapData;
	Rect rect(0, 0, bmp.bmWidth, bmp.bmHeight);
	if (Ok != pBitmap->LockBits(&rect, ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData))
	{
		MMSafeDelete(pBitmap);

		return NULL;
	}

	BYTE *pixelsDest = (BYTE *)bitmapData.Scan0;
	int nLinesize = bmp.bmWidth * sizeof(UINT);
	int nHeight = bmp.bmHeight;

	for (int y = 0; y < nHeight; y++)
	{
		//从下到上复制数据，因为前面设置高度为正数
		memcpy_s(
			(pixelsDest + y * nLinesize),
			nLinesize,
			(vecPixel.data() + (nHeight - y - 1) * nLinesize),
			nLinesize);
	}

	if (Ok != pBitmap->UnlockBits(&bitmapData))
	{
		MMSafeDelete(pBitmap);
	}

	return pBitmap;
}

bool CDUIRenderEngine::SaveImage(HBITMAP hBitmap, CMMString strFile, enDuiSaveImageType SaveImageType, bool bAlpahImage)
{
	if (hBitmap == NULL || strFile.IsEmpty()) return false;

	GUID ImageFormat = GUID_NULL;
	switch (SaveImageType)
	{
		case SaveImage_Bmp:
		{
			ImageFormat = Gdiplus::ImageFormatBMP;
			break;
		}
		case SaveImage_Emf:
		{
			ImageFormat = Gdiplus::ImageFormatEMF;
			break;
		}
		case SaveImage_Exif:
		{
			ImageFormat = Gdiplus::ImageFormatEXIF;
			break;
		}
		case SaveImage_Gif:
		{
			ImageFormat = Gdiplus::ImageFormatGIF;
			break;
		}
		case SaveImage_Icon:
		{
			ImageFormat = Gdiplus::ImageFormatIcon;
			break;
		}
		case SaveImage_Jpeg:
		{
			ImageFormat = Gdiplus::ImageFormatJPEG;
			break;
		}
		case SaveImage_Png:
		{
			ImageFormat = Gdiplus::ImageFormatPNG;
			break;
		}
		case SaveImage_Tiff:
		{
			ImageFormat = Gdiplus::ImageFormatTIFF;
			break;
		}
		case SaveImage_Wmf:
		{
			ImageFormat = Gdiplus::ImageFormatWMF;
			break;
		}
	}

	CImage ImageObj;
	ImageObj.Attach(hBitmap);
	ImageObj.SetHasAlphaChannel(bAlpahImage);
	if (FAILED(ImageObj.Save(strFile, ImageFormat))) return false;
	ImageObj.Detach();

	return true;
}

void CDUIRenderEngine::CheckAlphaColor(DWORD &dwColor)
{
	//RestoreAlphaColor认为0x00000000是真正的透明，其它都是GDI绘制导致的
	//所以在GDI绘制中不能用0xFF000000这个颜色值，现在处理是让它变成RGB(0,0,1)
	//RGB(0,0,1)与RGB(0,0,0)很难分出来
	if ((0x00FFFFFF & dwColor) == 0)
	{
		dwColor += 1;
	}

	return;
}

void CDUIRenderEngine::ClearPixel(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear)
{
	if (NULL == pBits) return;

	for (int nTop = rcClear.top; nTop < rcClear.bottom; ++nTop)
	{
		memset((ARGB *)pBits + nWidthBitmap * nTop + rcClear.left, 0, rcClear.GetWidth() * sizeof(ARGB));
	}

	return;
}

void CDUIRenderEngine::ClearPixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear)
{
	if (NULL == pBits) return;

	for (int nTop = rcClear.top; nTop < rcClear.bottom; ++nTop)
	{
		for (int nLeft = rcClear.left; nLeft < rcClear.right; ++nLeft)
		{
			ARGB &wColor = *((ARGB *)pBits + nWidthBitmap * nTop + nLeft);
			((LPBYTE)&wColor)[3] = 0;
		}
	}

	return;
}

void CDUIRenderEngine::RestorePixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcRestore)
{
	if (NULL == pBits) return;

	for (int nTop = rcRestore.top; nTop < rcRestore.bottom; ++nTop)
	{
		for (int nLeft = rcRestore.left; nLeft < rcRestore.right; ++nLeft)
		{
			LPBYTE lpColor = (LPBYTE)((ARGB *)pBits + nWidthBitmap * nTop + nLeft);
			if ((lpColor[3] == 0) && (lpColor[0] != 0 || lpColor[1] != 0 || lpColor[2] != 0))
				lpColor[3] = 255;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
CDUIMemDC::CDUIMemDC(HDC hDC, const CDUIRect rcDest, const CDUIRect rcClient, bool bCopyBitmap)
{
	m_hDC = hDC;
	m_rcDest = rcDest;
	m_rcClient = rcClient;
	m_bCopyBitmap = bCopyBitmap;

	m_hMemDC = ::CreateCompatibleDC(m_hDC);
	m_hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(m_hDC, m_rcClient.GetWidth(), m_rcClient.GetHeight(), &m_pBmpBits);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);

	if (m_bCopyBitmap)
	{
		::BitBlt(m_hMemDC, m_rcDest.left, m_rcDest.top, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
			m_hDC, m_rcDest.left, m_rcDest.top, SRCCOPY);
	}

	return;
}

CDUIMemDC::CDUIMemDC(HDC hDC, const CDUIRect rcDest, bool bCopyBitmap)
{
	m_bFromStart = true;
	m_hDC = hDC;
	m_rcDest = rcDest;
	m_bCopyBitmap = bCopyBitmap;

	m_hMemDC = ::CreateCompatibleDC(m_hDC);
	m_hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(m_hDC, m_rcDest.GetWidth(), m_rcDest.GetHeight(), &m_pBmpBits);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);

	if (m_bCopyBitmap)
	{
		::BitBlt(m_hMemDC, 0, 0, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
			m_hDC, m_rcDest.left, m_rcDest.top, SRCCOPY);
	}

	return;
}

CDUIMemDC::~CDUIMemDC(void)
{
	if (m_bCopyBitmap)
	{
		if (false == m_bFromStart)
		{
			::BitBlt(m_hDC, m_rcDest.left, m_rcDest.top, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
				m_hMemDC, m_rcDest.left, m_rcDest.top, SRCCOPY);
		}
		else
		{
			::BitBlt(m_hDC, m_rcDest.left, m_rcDest.top, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
				m_hMemDC, 0, 0, SRCCOPY);
		}
	}

	SelectObject(m_hMemDC, m_hOldBitmap);
	MMSafeDeleteObject(m_hBitmap);
	MMSafeDeleteObject(m_hMemDC);
	m_pBmpBits = NULL;

	return;
}

CDUIMemDC::operator HDC() const
{
	return m_hMemDC;
}

HDC CDUIMemDC::GetSrcHDC()
{
	return m_hDC;
}

HDC CDUIMemDC::GetMemHDC()
{
	return m_hMemDC;
}

HBITMAP CDUIMemDC::GetMemBitmap()
{
	return m_hBitmap;
}

LPBYTE CDUIMemDC::GetMemBmpBits()
{
	return m_pBmpBits;
}

CDUIRect CDUIMemDC::GetPaint()
{
	return m_rcDest;
}

CDUIRect CDUIMemDC::GetClient()
{
	return m_rcClient;
}

const CDUIMemDC &CDUIMemDC::operator = (CDUIMemDC)
{
	return *this;
}

//////////////////////////////////////////////////////////////////////////