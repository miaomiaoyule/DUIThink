#include "StdAfx.h"
#include "DUIGdiplusPortable.h"
#include "DUICanvasRaster.h"

#if defined(DuiPlatform_SDL)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Gdiplus
{
//////////////////////////////////////////////////////////////////////////
TextureBrush::TextureBrush(Image *pImage, WrapMode)
	: m_pBmp(dynamic_cast<Bitmap *>(pImage))
	, m_bOwned(false)
{
}

Brush * TextureBrush::Clone() const
{
	TextureBrush *p = new TextureBrush(m_pBmp, WrapModeClamp);
	p->m_matrix = m_matrix;
	return p;
}

//////////////////////////////////////////////////////////////////////////
Bitmap::Bitmap(int nWidth, int nHeight, PixelFormat fmt)
{
	m_format = fmt;
	Reset(nWidth, nHeight);
}

Bitmap::Bitmap(int nWidth, int nHeight, const BYTE *pBGRA)
{
	Reset(nWidth, nHeight);
	if (pBGRA && false == m_vecBits.empty())
	{
		memcpy(m_vecBits.data(), pBGRA, m_vecBits.size());
	}
}

void Bitmap::Reset(int nWidth, int nHeight)
{
	m_nWidth = max(0, nWidth);
	m_nHeight = max(0, nHeight);
	m_vecBits.assign((size_t)m_nWidth * (size_t)m_nHeight * 4, 0);
}

Bitmap * Bitmap::Clone(INT x, INT y, INT nWidth, INT nHeight, INT) const
{
	if (nWidth <= 0 || nHeight <= 0 || x < 0 || y < 0) return NULL;
	if (x + nWidth > m_nWidth || y + nHeight > m_nHeight) return NULL;
	Bitmap *pClone = new Bitmap(nWidth, nHeight, m_format);
	for (int row = 0; row < nHeight; ++row)
	{
		memcpy(pClone->GetBits() + row * nWidth * 4,
			m_vecBits.data() + ((y + row) * m_nWidth + x) * 4,
			(size_t)nWidth * 4);
	}
	return pClone;
}

Status Bitmap::LockBits(const Rect *rect, UINT, PixelFormat format, BitmapData *lockedBitmapData)
{
	if (NULL == lockedBitmapData || m_vecBits.empty()) return InvalidParameter;
	(void)rect;
	lockedBitmapData->Width = (UINT)m_nWidth;
	lockedBitmapData->Height = (UINT)m_nHeight;
	lockedBitmapData->Stride = m_nWidth * 4;
	lockedBitmapData->PixelFormat = format ? format : m_format;
	lockedBitmapData->Scan0 = GetBits();
	return Ok;
}

//////////////////////////////////////////////////////////////////////////
void GraphicsPath::AddLine(INT x1, INT y1, INT x2, INT y2)
{
	POINT a = { x1, y1 }, b = { x2, y2 };
	if (m_pts.empty()) m_pts.push_back(a);
	else if (m_pts.back().x != a.x || m_pts.back().y != a.y) m_pts.push_back(a);
	m_pts.push_back(b);
}

void GraphicsPath::AddArc(INT x, INT y, INT w, INT h, REAL startAngle, REAL sweepAngle)
{
	if (w <= 0 || h <= 0) return;
	const int nSeg = max(8, (int)(fabsf(sweepAngle) / 6.0f));
	const REAL cx = x + w * 0.5f;
	const REAL cy = y + h * 0.5f;
	const REAL rx = w * 0.5f;
	const REAL ry = h * 0.5f;
	for (int i = 0; i <= nSeg; ++i)
	{
		REAL t = startAngle + sweepAngle * (REAL)i / (REAL)nSeg;
		REAL rad = t * (REAL)M_PI / 180.0f;
		POINT pt = { (LONG)(cx + rx * cosf(rad) + 0.5f), (LONG)(cy + ry * sinf(rad) + 0.5f) };
		if (m_pts.empty() || m_pts.back().x != pt.x || m_pts.back().y != pt.y)
			m_pts.push_back(pt);
	}
}

void GraphicsPath::AddEllipse(INT x, INT y, INT w, INT h)
{
	AddArc(x, y, w, h, 0, 360);
	m_closed = true;
}

void GraphicsPath::AddBezier(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4)
{
	const int nSeg = 12;
	for (int i = 0; i <= nSeg; ++i)
	{
		REAL t = (REAL)i / (REAL)nSeg;
		REAL u = 1.0f - t;
		REAL x = u * u * u * x1 + 3 * u * u * t * x2 + 3 * u * t * t * x3 + t * t * t * x4;
		REAL y = u * u * u * y1 + 3 * u * u * t * y2 + 3 * u * t * t * y3 + t * t * t * y4;
		POINT pt = { (LONG)(x + 0.5f), (LONG)(y + 0.5f) };
		if (m_pts.empty() || m_pts.back().x != pt.x || m_pts.back().y != pt.y)
			m_pts.push_back(pt);
	}
}

Status GraphicsPath::GetBounds(RectF *bounds) const
{
	if (NULL == bounds) return InvalidParameter;
	if (m_pts.empty()) { *bounds = RectF(); return Ok; }
	INT minX = m_pts[0].x, maxX = m_pts[0].x, minY = m_pts[0].y, maxY = m_pts[0].y;
	for (size_t i = 1; i < m_pts.size(); ++i)
	{
		minX = min(minX, (INT)m_pts[i].x);
		maxX = max(maxX, (INT)m_pts[i].x);
		minY = min(minY, (INT)m_pts[i].y);
		maxY = max(maxY, (INT)m_pts[i].y);
	}
	*bounds = RectF((REAL)minX, (REAL)minY, (REAL)(maxX - minX), (REAL)(maxY - minY));
	return Ok;
}

//////////////////////////////////////////////////////////////////////////
Graphics::Graphics(HDC hdc)
{
	m_pCanvas = DuiCanvasFromHDC(hdc);
}

Status Graphics::DrawLine(Pen *pen, INT x1, INT y1, INT x2, INT y2)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	m_pCanvas->DrawLine(x1, y1, x2, y2, PenWidth(pen), PenColor(pen), pen->GetStyle());
	return Ok;
}

Status Graphics::DrawRectangle(Pen *pen, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	SIZE sz = {};
	m_pCanvas->DrawRect(rc, PenWidth(pen), PenColor(pen), sz, pen->GetStyle());
	return Ok;
}

Status Graphics::DrawEllipse(Pen *pen, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	m_pCanvas->DrawEllipse(rc, PenWidth(pen), PenColor(pen), pen->GetStyle());
	return Ok;
}

Status Graphics::DrawArc(Pen *pen, INT x, INT y, INT w, INT h, REAL startAngle, REAL sweepAngle)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	m_pCanvas->DrawArc(rc, PenWidth(pen), PenColor(pen), startAngle, sweepAngle, pen->GetStyle());
	return Ok;
}

Status Graphics::DrawPath(Pen *pen, GraphicsPath *path)
{
	if (NULL == m_pCanvas || NULL == pen || NULL == path || path->Points().empty()) return InvalidParameter;
	m_pCanvas->DrawPath(path->Points().data(), (int)path->Points().size(), PenWidth(pen), PenColor(pen), pen->GetStyle());
	return Ok;
}

Status Graphics::FillRectangle(Brush *brush, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == brush) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	Bitmap *pBmp = brush->GetBitmap();
	if (pBmp && pBmp->GetBits())
	{
		RECT rcSrc = { 0, 0, (LONG)pBmp->GetWidth(), (LONG)pBmp->GetHeight() };
		m_pCanvas->DrawImage(pBmp->GetBits(), (int)pBmp->GetWidth(), (int)pBmp->GetHeight(), rc, rcSrc, {}, 255, false, false);
	}
	else
	{
		m_pCanvas->FillRect(rc, brush->GetFillColor(), brush->IsGradient() ? brush->GetGradientColor() : 0);
	}
	return Ok;
}

Status Graphics::FillEllipse(Brush *brush, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == brush) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	m_pCanvas->FillEllipse(rc, brush->GetFillColor(), brush->IsGradient() ? brush->GetGradientColor() : 0);
	return Ok;
}

Status Graphics::FillPath(Brush *brush, GraphicsPath *path)
{
	if (NULL == m_pCanvas || NULL == brush || NULL == path || path->Points().empty()) return InvalidParameter;
	Bitmap *pBmp = brush->GetBitmap();
	if (pBmp && pBmp->GetBits())
	{
		RectF bounds;
		path->GetBounds(&bounds);
		RECT rcDst = { (LONG)bounds.X, (LONG)bounds.Y, (LONG)(bounds.X + bounds.Width), (LONG)(bounds.Y + bounds.Height) };
		RECT rcSrc = { 0, 0, (LONG)pBmp->GetWidth(), (LONG)pBmp->GetHeight() };
		m_pCanvas->Save();
		m_pCanvas->DrawImage(pBmp->GetBits(), (int)pBmp->GetWidth(), (int)pBmp->GetHeight(), rcDst, rcSrc, {}, 255, false, false);
		m_pCanvas->Restore();
	}
	else
	{
		m_pCanvas->FillPolygon(path->Points().data(), (int)path->Points().size(),
			brush->GetFillColor(), brush->IsGradient() ? brush->GetGradientColor() : 0);
	}
	return Ok;
}

Status Graphics::DrawImage(Bitmap *bmp, const Rect &dest, INT srcX, INT srcY, INT srcW, INT srcH, Unit)
{
	if (NULL == m_pCanvas || NULL == bmp || NULL == bmp->GetBits()) return InvalidParameter;
	RECT rcDst = { dest.X, dest.Y, dest.X + dest.Width, dest.Y + dest.Height };
	RECT rcSrc = { srcX, srcY, srcX + srcW, srcY + srcH };
	m_pCanvas->DrawImage(bmp->GetBits(), (int)bmp->GetWidth(), (int)bmp->GetHeight(), rcDst, rcSrc, {}, 255, false, false);
	return Ok;
}

Status Graphics::DrawImage(Bitmap *bmp, PointF *pts, INT count)
{
	if (NULL == m_pCanvas || NULL == bmp || NULL == pts || count < 3) return InvalidParameter;
	const INT x0 = (INT)pts[0].X;
	const INT y0 = (INT)pts[0].Y;
	const INT x1 = (INT)pts[1].X;
	const INT y2 = (INT)pts[2].Y;
	Rect dest(x0, y0, max(1, x1 - x0), max(1, y2 - y0));
	return DrawImage(bmp, dest, 0, 0, (INT)bmp->GetWidth(), (INT)bmp->GetHeight(), UnitPixel);
}

Status Graphics::MeasureString(const WCHAR *str, INT len, const Font *font, const RectF &layout, const StringFormat *fmt, RectF *boundingBox)
{
	if (NULL == boundingBox) return InvalidParameter;
	IDuiFont *pFont = font ? DuiFontFromHFONT(font->GetHFONT()) : NULL;
	CMMString text;
	if (str)
	{
		if (len < 0) text = str;
		else text = CMMString(str, len);
	}
	DWORD dwStyle = DT_LEFT | DT_TOP | DT_WORDBREAK;
	if (fmt)
	{
		if (fmt->GetAlignment() == StringAlignmentCenter) dwStyle = (dwStyle & ~DT_LEFT) | DT_CENTER;
		if (fmt->GetAlignment() == StringAlignmentFar) dwStyle = (dwStyle & ~DT_LEFT) | DT_RIGHT;
		if (fmt->GetLineAlignment() == StringAlignmentCenter) dwStyle |= DT_VCENTER;
		if (fmt->GetLineAlignment() == StringAlignmentFar) dwStyle |= DT_BOTTOM;
		if (fmt->GetFormatFlags() & StringFormatFlagsNoWrap) dwStyle = (dwStyle & ~DT_WORDBREAK) | DT_SINGLELINE;
	}
	SIZE sz = {};
	if (m_pCanvas && pFont) sz = m_pCanvas->MeasureText(pFont, text.c_str(), dwStyle, (int)layout.Width);
	else if (pFont) sz = pFont->MeasureText(text.c_str(), (int)layout.Width, dwStyle);
	boundingBox->X = layout.X;
	boundingBox->Y = layout.Y;
	boundingBox->Width = (REAL)sz.cx;
	boundingBox->Height = (REAL)sz.cy;
	return Ok;
}

Status Graphics::DrawString(const WCHAR *str, INT len, const Font *font, const RectF &layout, const StringFormat *fmt, Brush *brush)
{
	if (NULL == m_pCanvas || NULL == brush) return InvalidParameter;
	IDuiFont *pFont = font ? DuiFontFromHFONT(font->GetHFONT()) : NULL;
	if (NULL == pFont) return GenericError;
	CMMString text;
	if (str)
	{
		if (len < 0) text = str;
		else text = CMMString(str, len);
	}
	RECT rc = { (LONG)layout.X, (LONG)layout.Y, (LONG)(layout.X + layout.Width), (LONG)(layout.Y + layout.Height) };
	DWORD dwStyle = DT_LEFT | DT_TOP | DT_WORDBREAK;
	if (fmt)
	{
		if (fmt->GetAlignment() == StringAlignmentCenter) dwStyle = (dwStyle & ~DT_LEFT) | DT_CENTER;
		if (fmt->GetAlignment() == StringAlignmentFar) dwStyle = (dwStyle & ~DT_LEFT) | DT_RIGHT;
		if (fmt->GetLineAlignment() == StringAlignmentCenter) dwStyle |= DT_VCENTER;
		if (fmt->GetLineAlignment() == StringAlignmentFar) dwStyle |= DT_BOTTOM;
		if (fmt->GetFormatFlags() & StringFormatFlagsNoWrap) dwStyle = (dwStyle & ~DT_WORDBREAK) | DT_SINGLELINE;
	}
	m_pCanvas->DrawText(pFont, rc, text.c_str(), brush->GetFillColor(), dwStyle);
	return Ok;
}

} // namespace Gdiplus

#endif
