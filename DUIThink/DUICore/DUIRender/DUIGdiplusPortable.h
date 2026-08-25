#ifndef __DUIGDIPLUSPORTABLE_H__
#define __DUIGDIPLUSPORTABLE_H__

#pragma once

#if defined(DuiPlatform_SDL)

// GDI+ API stand-in for DuiPlatform_SDL.
// Graphics draws into IDuiCanvas* obtained from HDC (CDUICanvasRaster).
class IDuiCanvas;

namespace Gdiplus
{
	typedef float REAL;
	typedef DWORD ARGB;
	typedef ULONG PROPID;
	typedef INT PixelFormat;

	enum Status { Ok = 0, GenericError = 1, InvalidParameter = 2, OutOfMemory = 3 };

	enum SmoothingMode { SmoothingModeInvalid = -1, SmoothingModeDefault = 0, SmoothingModeHighQuality = 2 };
	enum InterpolationMode { InterpolationModeInvalid = -1, InterpolationModeDefault = 0, InterpolationModeHighQualityBicubic = 7 };
	enum PixelOffsetMode { PixelOffsetModeInvalid = -1, PixelOffsetModeDefault = 0, PixelOffsetModeHighQuality = 2 };
	enum CompositingQuality { CompositingQualityInvalid = -1, CompositingQualityDefault = 0, CompositingQualityHighQuality = 2 };
	enum TextRenderingHint
	{
		TextRenderingHintSystemDefault = 0,
		TextRenderingHintSingleBitPerPixelGridFit = 1,
		TextRenderingHintSingleBitPerPixel = 2,
		TextRenderingHintAntiAliasGridFit = 3,
		TextRenderingHintAntiAlias = 4,
		TextRenderingHintClearTypeGridFit = 5,
	};
	enum DashStyle { DashStyleSolid = 0, DashStyleDash = 1, DashStyleDot = 2, DashStyleDashDot = 3, DashStyleDashDotDot = 4 };
	enum PenAlignment { PenAlignmentCenter = 0, PenAlignmentInset = 1 };
	enum WrapMode { WrapModeTile = 0, WrapModeClamp = 4 };
	enum Unit { UnitWorld = 0, UnitPixel = 2 };
	enum MatrixOrder { MatrixOrderPrepend = 0, MatrixOrderAppend = 1 };
	enum StringAlignment { StringAlignmentNear = 0, StringAlignmentCenter = 1, StringAlignmentFar = 2 };
	enum StringTrimming { StringTrimmingNone = 0, StringTrimmingEllipsisCharacter = 3, StringTrimmingEllipsisPath = 5 };
	enum StringFormatFlags { StringFormatFlagsNoClip = 0x2000, StringFormatFlagsNoWrap = 0x1000 };
	enum ImageLockMode { ImageLockModeRead = 0x0001, ImageLockModeWrite = 0x0002 };

	const PixelFormat PixelFormat32bppARGB = 0x26200A;
	const PixelFormat PixelFormat32bppPARGB = 0xE200B;

	struct BitmapData
	{
		UINT Width;
		UINT Height;
		INT Stride;
		PixelFormat PixelFormat;
		VOID *Scan0;
		UINT_PTR Reserved;
		BitmapData() : Width(0), Height(0), Stride(0), PixelFormat(0), Scan0(NULL), Reserved(0) {}
	};
#ifndef PropertyTagFrameDelay
#define PropertyTagFrameDelay 0x5100
#endif

	struct PropertyItem { PROPID id; ULONG length; WORD type; VOID *value; };
	static const GUID FrameDimensionTime = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

	class Point
	{
	public:
		INT X, Y;
		Point() : X(0), Y(0) {}
		Point(INT x, INT y) : X(x), Y(y) {}
	};

	class PointF
	{
	public:
		REAL X, Y;
		PointF() : X(0), Y(0) {}
		PointF(REAL x, REAL y) : X(x), Y(y) {}
	};

	class Rect
	{
	public:
		INT X, Y, Width, Height;
		Rect() : X(0), Y(0), Width(0), Height(0) {}
		Rect(INT x, INT y, INT w, INT h) : X(x), Y(y), Width(w), Height(h) {}
	};

	class RectF
	{
	public:
		REAL X, Y, Width, Height;
		RectF() : X(0), Y(0), Width(0), Height(0) {}
		RectF(REAL x, REAL y, REAL w, REAL h) : X(x), Y(y), Width(w), Height(h) {}
	};

	class Color
	{
	public:
		Color() : m_argb(0) {}
		Color(ARGB argb) : m_argb(argb) {}
		Color(BYTE a, BYTE r, BYTE g, BYTE b) : m_argb(((ARGB)a << 24) | ((ARGB)r << 16) | ((ARGB)g << 8) | b) {}
		ARGB GetValue() const { return m_argb; }
		BYTE GetA() const { return (BYTE)((m_argb >> 24) & 0xff); }
		BYTE GetR() const { return (BYTE)((m_argb >> 16) & 0xff); }
		BYTE GetG() const { return (BYTE)((m_argb >> 8) & 0xff); }
		BYTE GetB() const { return (BYTE)(m_argb & 0xff); }
	private:
		ARGB m_argb;
	};

	class Matrix
	{
	public:
		Matrix() : m_sx(1), m_sy(1), m_tx(0), m_ty(0) {}
		Status Scale(REAL sx, REAL sy, MatrixOrder = MatrixOrderPrepend) { m_sx *= sx; m_sy *= sy; return Ok; }
		Status Translate(REAL dx, REAL dy, MatrixOrder order = MatrixOrderPrepend)
		{
			if (MatrixOrderAppend == order) { m_tx += dx; m_ty += dy; }
			else { m_tx = m_tx * m_sx + dx; m_ty = m_ty * m_sy + dy; }
			return Ok;
		}
		REAL m_sx, m_sy, m_tx, m_ty;
	};

	class Image;
	class Bitmap;

	class Brush
	{
	public:
		virtual ~Brush() {}
		virtual Brush * Clone() const = 0;
		virtual DWORD GetFillColor() const { return 0xFF000000; }
		virtual Bitmap * GetBitmap() const { return NULL; }
		virtual bool IsGradient() const { return false; }
		virtual DWORD GetGradientColor() const { return GetFillColor(); }
	};

	class SolidBrush : public Brush
	{
	public:
		SolidBrush(const Color &c) : m_color(c) {}
		SolidBrush(ARGB argb) : m_color(argb) {}
		Brush * Clone() const override { return new SolidBrush(m_color); }
		DWORD GetFillColor() const override { return m_color.GetValue(); }
	private:
		Color m_color;
	};

	class LinearGradientBrush : public Brush
	{
	public:
		LinearGradientBrush(const Point &p1, const Point &p2, const Color &c1, const Color &c2)
			: m_c1(c1), m_c2(c2) { (void)p1; (void)p2; }
		Brush * Clone() const override { return new LinearGradientBrush(Point(), Point(), m_c1, m_c2); }
		DWORD GetFillColor() const override { return m_c1.GetValue(); }
		bool IsGradient() const override { return true; }
		DWORD GetGradientColor() const override { return m_c2.GetValue(); }
	private:
		Color m_c1, m_c2;
	};

	class TextureBrush : public Brush
	{
	public:
		TextureBrush(Image *pImage, WrapMode = WrapModeTile);
		Brush * Clone() const override;
		Bitmap * GetBitmap() const override { return m_pBmp; }
		Status SetTransform(const Matrix *m) { if (m) m_matrix = *m; return Ok; }
	private:
		Bitmap *m_pBmp;
		bool m_bOwned;
		Matrix m_matrix;
	};

	class Pen
	{
	public:
		Pen(const Color &c, REAL width = 1.0f) : m_color(c), m_width(width), m_dash(DashStyleSolid), m_align(PenAlignmentCenter) {}
		void SetDashStyle(DashStyle s) { m_dash = s; }
		void SetAlignment(PenAlignment a) { m_align = a; }
		Color GetColor() const { return m_color; }
		REAL GetWidth() const { return m_width; }
		int GetStyle() const { return (int)m_dash; }
	private:
		Color m_color;
		REAL m_width;
		DashStyle m_dash;
		PenAlignment m_align;
	};

	class GraphicsPath
	{
	public:
		GraphicsPath() {}
		void Reset() { m_pts.clear(); m_closed = false; }
		void AddLine(INT x1, INT y1, INT x2, INT y2);
		void AddLine(REAL x1, REAL y1, REAL x2, REAL y2) { AddLine((INT)x1, (INT)y1, (INT)x2, (INT)y2); }
		void AddArc(INT x, INT y, INT w, INT h, REAL startAngle, REAL sweepAngle);
		void AddEllipse(INT x, INT y, INT w, INT h);
		void AddBezier(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);
		void StartFigure() {}
		void CloseFigure() { m_closed = true; }
		Status GetBounds(RectF *bounds) const;
		const std::vector<POINT> & Points() const { return m_pts; }
		bool IsClosed() const { return m_closed; }
	private:
		std::vector<POINT> m_pts;
		bool m_closed = false;
	};

	class Image
	{
	public:
		virtual ~Image() {}
		virtual UINT GetWidth() const = 0;
		virtual UINT GetHeight() const = 0;
	};

	class Bitmap : public Image
	{
	public:
		Bitmap(int nWidth = 0, int nHeight = 0, PixelFormat = PixelFormat32bppPARGB);
		Bitmap(int nWidth, int nHeight, const BYTE *pBGRA);
		virtual ~Bitmap() {}

		UINT GetWidth() const override { return (UINT)m_nWidth; }
		UINT GetHeight() const override { return (UINT)m_nHeight; }
		BYTE * GetBits() { return m_vecBits.empty() ? NULL : m_vecBits.data(); }
		const BYTE * GetBits() const { return m_vecBits.empty() ? NULL : m_vecBits.data(); }
		int GetPitch() const { return m_nWidth * 4; }
		INT GetPixelFormat() const { return m_format; }
		Status LockBits(const Rect *rect, UINT, PixelFormat, BitmapData *lockedBitmapData);
		Status UnlockBits(BitmapData *) { return Ok; }

		Bitmap * Clone(INT x, INT y, INT nWidth, INT nHeight, INT) const;
		UINT GetFrameDimensionsCount() const { return 0; }
		Status GetFrameDimensionsList(GUID *, UINT) const { return GenericError; }
		UINT GetFrameCount(const GUID *) const { return 0; }
		UINT GetPropertyItemSize(PROPID) const { return 0; }
		Status GetPropertyItem(PROPID, UINT, PropertyItem *) const { return GenericError; }
		Status SelectActiveFrame(const GUID *, UINT) { return GenericError; }
		static Bitmap * FromStream(void *) { return NULL; }

	protected:
		void Reset(int nWidth, int nHeight);
		int m_nWidth = 0;
		int m_nHeight = 0;
		PixelFormat m_format = PixelFormat32bppPARGB;
		std::vector<BYTE> m_vecBits;
	};

	class Font
	{
	public:
		Font(HDC, HFONT hFont) : m_hFont(hFont) {}
		HFONT GetHFONT() const { return m_hFont; }
	private:
		HFONT m_hFont;
	};

	class StringFormat
	{
	public:
		StringFormat() : m_align(StringAlignmentNear), m_lineAlign(StringAlignmentNear), m_trim(StringTrimmingNone), m_flags(0) {}
		static StringFormat GenericTypographic() { return StringFormat(); }
		void SetAlignment(StringAlignment a) { m_align = a; }
		void SetLineAlignment(StringAlignment a) { m_lineAlign = a; }
		void SetTrimming(StringTrimming t) { m_trim = t; }
		void SetFormatFlags(INT f) { m_flags = f; }
		StringAlignment GetAlignment() const { return m_align; }
		StringAlignment GetLineAlignment() const { return m_lineAlign; }
		INT GetFormatFlags() const { return m_flags; }
	private:
		StringAlignment m_align, m_lineAlign;
		StringTrimming m_trim;
		INT m_flags;
	};

	class Graphics
	{
	public:
		Graphics(HDC hdc);
		~Graphics() {}

		void SetSmoothingMode(SmoothingMode) {}
		void SetInterpolationMode(InterpolationMode) {}
		void SetPixelOffsetMode(PixelOffsetMode) {}
		void SetCompositingQuality(CompositingQuality) {}
		void SetTextRenderingHint(TextRenderingHint) {}

		Status DrawLine(Pen *pen, INT x1, INT y1, INT x2, INT y2);
		Status DrawRectangle(Pen *pen, INT x, INT y, INT w, INT h);
		Status DrawEllipse(Pen *pen, INT x, INT y, INT w, INT h);
		Status DrawArc(Pen *pen, INT x, INT y, INT w, INT h, REAL startAngle, REAL sweepAngle);
		Status DrawPath(Pen *pen, GraphicsPath *path);
		Status FillRectangle(Brush *brush, INT x, INT y, INT w, INT h);
		Status FillEllipse(Brush *brush, INT x, INT y, INT w, INT h);
		Status FillPath(Brush *brush, GraphicsPath *path);
		Status DrawImage(Bitmap *bmp, const Rect &dest, INT srcX, INT srcY, INT srcW, INT srcH, Unit);
		Status DrawImage(Bitmap *bmp, PointF *pts, INT count);
		Status MeasureString(const WCHAR *str, INT len, const Font *font, const RectF &layout, const StringFormat *fmt, RectF *boundingBox);
		Status DrawString(const WCHAR *str, INT len, const Font *font, const RectF &layout, const StringFormat *fmt, Brush *brush);

	private:
		IDuiCanvas *m_pCanvas;
		DWORD PenColor(Pen *pen) const { return pen ? pen->GetColor().GetValue() : 0; }
		int PenWidth(Pen *pen) const { return pen ? max(1, (int)(pen->GetWidth() + 0.5f)) : 1; }
	};
}

#endif // DuiPlatform_SDL
#endif
