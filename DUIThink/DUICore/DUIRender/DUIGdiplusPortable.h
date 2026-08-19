#ifndef __DUIGDIPLUSPORTABLE_H__
#define __DUIGDIPLUSPORTABLE_H__

#pragma once

// Minimal Gdiplus stand-in so existing Bitmap* / TextRenderingHint call sites compile on SDL.
// Pixel storage is BGRA8888, top-down, matching the raster canvas.

namespace Gdiplus
{
	typedef float REAL;
	typedef DWORD ARGB;
	typedef ULONG PROPID;

	enum Status
	{
		Ok = 0,
		GenericError = 1,
	};

	enum TextRenderingHint
	{
		TextRenderingHintSystemDefault = 0,
		TextRenderingHintSingleBitPerPixelGridFit = 1,
		TextRenderingHintSingleBitPerPixel = 2,
		TextRenderingHintAntiAliasGridFit = 3,
		TextRenderingHintAntiAlias = 4,
		TextRenderingHintClearTypeGridFit = 5,
	};

	const INT PixelFormat32bppPARGB = 0xE200B;
#ifndef PropertyTagFrameDelay
#define PropertyTagFrameDelay 0x5100
#endif

	struct PropertyItem
	{
		PROPID id;
		ULONG length;
		WORD type;
		VOID *value;
	};

	static const GUID FrameDimensionTime = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

	struct PointF
	{
		REAL X;
		REAL Y;
		PointF() : X(0), Y(0) {}
		PointF(REAL x, REAL y) : X(x), Y(y) {}
	};

	class Bitmap;

	class Graphics
	{
	public:
		Graphics(HDC) {}
		void DrawImage(Bitmap *, PointF *, int) {}
	};

	class Bitmap
	{
	public:
		Bitmap(int nWidth = 0, int nHeight = 0)
		{
			Reset(nWidth, nHeight);
		}

		Bitmap(int nWidth, int nHeight, const BYTE *pBGRA)
		{
			Reset(nWidth, nHeight);
			if (pBGRA && false == m_vecBits.empty())
			{
				memcpy(m_vecBits.data(), pBGRA, m_vecBits.size());
			}
		}

		~Bitmap() {}

		UINT GetWidth() const { return (UINT)m_nWidth; }
		UINT GetHeight() const { return (UINT)m_nHeight; }
		BYTE * GetBits() { return m_vecBits.empty() ? NULL : m_vecBits.data(); }
		const BYTE * GetBits() const { return m_vecBits.empty() ? NULL : m_vecBits.data(); }
		int GetPitch() const { return m_nWidth * 4; }

		INT GetPixelFormat() const { return PixelFormat32bppPARGB; }

		Bitmap * Clone(INT x, INT y, INT nWidth, INT nHeight, INT) const
		{
			if (nWidth <= 0 || nHeight <= 0 || x < 0 || y < 0) return NULL;
			if (x + nWidth > m_nWidth || y + nHeight > m_nHeight) return NULL;

			Bitmap *pClone = new Bitmap(nWidth, nHeight);
			for (int row = 0; row < nHeight; ++row)
			{
				memcpy(pClone->GetBits() + row * nWidth * 4,
					m_vecBits.data() + ((y + row) * m_nWidth + x) * 4,
					nWidth * 4);
			}
			return pClone;
		}

		UINT GetFrameDimensionsCount() const { return 0; }
		Status GetFrameDimensionsList(GUID *, UINT) const { return GenericError; }
		UINT GetFrameCount(const GUID *) const { return 0; }
		UINT GetPropertyItemSize(PROPID) const { return 0; }
		Status GetPropertyItem(PROPID, UINT, PropertyItem *) const { return GenericError; }
		Status SelectActiveFrame(const GUID *, UINT) { return GenericError; }
		static Bitmap * FromStream(void *) { return NULL; }

	protected:
		void Reset(int nWidth, int nHeight)
		{
			m_nWidth = max(0, nWidth);
			m_nHeight = max(0, nHeight);
			m_vecBits.assign((size_t)m_nWidth * (size_t)m_nHeight * 4, 0);
		}

		int m_nWidth = 0;
		int m_nHeight = 0;
		std::vector<BYTE> m_vecBits;
	};
}

#endif
