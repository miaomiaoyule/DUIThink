#include "StdAfx.h"
#include "DUIColorPaletteCtrl.h"

//////////////////////////////////////////////////////////////////////////
#define DragPart_None						(0)
#define DragPart_Ring						(1)
#define DragPart_Triangle					(2)

//////////////////////////////////////////////////////////////////////////
static bool BaryOf(float fX, float fY, float fAx, float fAy, float fBx, float fBy, float fCx, float fCy, float &fA, float &fB, float &fC)
{
	const float fDen = (fBy - fCy) * (fAx - fCx) + (fCx - fBx) * (fAy - fCy);
	if (fabsf(fDen) < 0.001f) return false;

	fA = ((fBy - fCy) * (fX - fCx) + (fCx - fBx) * (fY - fCy)) / fDen;
	fB = ((fCy - fAy) * (fX - fCx) + (fAx - fCx) * (fY - fCy)) / fDen;
	fC = 1.0f - fA - fB;
	return true;
}

static void ClampBary(float &fA, float &fB, float &fC)
{
	if (fA >= 0.0f && fB >= 0.0f && fC >= 0.0f) return;
	if (fA < 0.0f && fB < 0.0f) { fA = 0.0f; fB = 0.0f; fC = 1.0f; return; }
	if (fA < 0.0f && fC < 0.0f) { fA = 0.0f; fC = 0.0f; fB = 1.0f; return; }
	if (fB < 0.0f && fC < 0.0f) { fB = 0.0f; fC = 0.0f; fA = 1.0f; return; }
	if (fA < 0.0f)
	{
		fA = 0.0f;
		const float fSum = fB + fC;
		if (fSum <= 0.0001f) { fB = 1.0f; fC = 0.0f; }
		else { fB /= fSum; fC /= fSum; }
		return;
	}
	if (fB < 0.0f)
	{
		fB = 0.0f;
		const float fSum = fA + fC;
		if (fSum <= 0.0001f) { fA = 1.0f; fC = 0.0f; }
		else { fA /= fSum; fC /= fSum; }
		return;
	}

	fC = 0.0f;
	const float fSum = fA + fB;
	if (fSum <= 0.0001f) { fA = 1.0f; fB = 0.0f; }
	else { fA /= fSum; fB /= fSum; }
}

static float HueFromOffset(float fDx, float fDy)
{
	float fHue = atan2f(fDy, fDx) * 57.2957795f + 90.0f;
	if (fHue < 0.0f) fHue += 360.0f;
	if (fHue >= 360.0f) fHue -= 360.0f;
	return fHue;
}

static float EdgeCover(float fInside)
{
	if (fInside >= 0.75f) return 1.0f;
	if (fInside <= -0.75f) return 0.0f;

	const float fU = (fInside + 0.75f) / 1.5f;
	return fU * fU * (3.0f - 2.0f * fU);
}

static void SampleWheel(float fSx, float fSy, float fDx, float fDy,
	float fAx, float fAy, float fBx, float fBy, float fCx, float fCy,
	float fAlt, float fInner, float fOuter, float fHue, bool bRing,
	float &fR, float &fG, float &fB, float &fCover)
{
	fR = 0.0f;
	fG = 0.0f;
	fB = 0.0f;
	fCover = 0.0f;

	if (false == bRing)
	{
		float fWa = 0.0f, fWb = 0.0f, fWc = 0.0f;
		if (false == BaryOf(fSx, fSy, fAx, fAy, fBx, fBy, fCx, fCy, fWa, fWb, fWc)) return;

		const float fTri = EdgeCover(fWa * fAlt) * EdgeCover(fWb * fAlt) * EdgeCover(fWc * fAlt);
		if (fTri <= 0.0f) return;

		ClampBary(fWa, fWb, fWc);
		const float fVal = max(0.0f, min(1.0f, fWa + fWb));
		const float fSat = (fVal > 0.001f) ? max(0.0f, min(1.0f, fWa / fVal)) : 0.0f;
		const Gdiplus::ARGB dwColor = CDUIRenderEngine::HSVToRGB(fHue, fSat, fVal);
		fR = DUIARGBGetR(dwColor);
		fG = DUIARGBGetG(dwColor);
		fB = DUIARGBGetB(dwColor);
		fCover = fTri;
		return;
	}

	const float fDist = sqrtf(fDx * fDx + fDy * fDy);
	const float fRing = EdgeCover(fOuter - fDist) * EdgeCover(fDist - fInner);
	if (fRing <= 0.0f) return;

	const Gdiplus::ARGB dwColor = CDUIRenderEngine::HSVToRGB(HueFromOffset(fDx, fDy), 1.0f, 1.0f);
	fR = DUIARGBGetR(dwColor);
	fG = DUIARGBGetG(dwColor);
	fB = DUIARGBGetB(dwColor);
	fCover = fRing;
}

static bool SameBitmapSize(Gdiplus::Bitmap *pBmp, int nWidth, int nHeight)
{
	return NULL != pBmp && (int)pBmp->GetWidth() == nWidth && (int)pBmp->GetHeight() == nHeight;
}

static void WritePremul(BYTE *pPixel, float fB, float fG, float fR, float fCov)
{
	const BYTE cbA = (BYTE)min(255.0f, max(0.0f, fCov * 255.0f + 0.5f));
	pPixel[0] = (BYTE)min(255.0f, fB * cbA / 255.0f + 0.5f);
	pPixel[1] = (BYTE)min(255.0f, fG * cbA / 255.0f + 0.5f);
	pPixel[2] = (BYTE)min(255.0f, fR * cbA / 255.0f + 0.5f);
	pPixel[3] = cbA;
}

static void DrawCachedBmp(HDC hDC, Gdiplus::Bitmap *pBmp, int nLeft, int nTop, int nWidth, int nHeight)
{
	if (NULL == hDC || NULL == pBmp || nWidth <= 0 || nHeight <= 0) return;

	HBITMAP hBmp = CDUIRenderEngine::GetHBITMAP(pBmp, true);
	if (NULL == hBmp) return;

	CDUIRect rcDest(nLeft, nTop, nLeft + nWidth, nTop + nHeight);
	CDUIRect rcSrc(0, 0, (int)pBmp->GetWidth(), (int)pBmp->GetHeight());
	CDUIRenderEngine::DrawImage(hDC, hBmp, rcDest, rcDest, rcSrc, CDUIRect(), 255, true);
	MMSafeDeleteObject(hBmp);
}

static void DrawMarkerBmp(HDC hDC, Gdiplus::Bitmap *pBmp, int nSize, float fX, float fY)
{
	if (NULL == pBmp || nSize <= 0) return;

	const int nLeft = (int)floorf(fX - nSize * 0.5f + 0.5f);
	const int nTop = (int)floorf(fY - nSize * 0.5f + 0.5f);
	DrawCachedBmp(hDC, pBmp, nLeft, nTop, nSize, nSize);
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIColorPaletteCtrl)
MMImplement_ClassName(CDUIColorPaletteCtrl)

CDUIColorPaletteCtrl::CDUIColorPaletteCtrl(void)
{
	return;
}

CDUIColorPaletteCtrl::~CDUIColorPaletteCtrl(void)
{
	if (m_pEditColorCtrl)
	{
		m_pEditColorCtrl->UnRegisterControlCallBack(this);
		m_pEditColorCtrl = NULL;
	}

	MMSafeDelete(m_pBmpWheel);
	MMSafeDelete(m_pBmpTri);
	MMSafeDelete(m_pBmpMark);

	return;
}

bool CDUIColorPaletteCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeColor)
	{
		SetPaletteColor(GetPaletteColor());

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStyle)
	{
		SetTextStyle(GetTextStyle());

		return true;
	}
	if (pAttributeObj == &m_AttributeEditHeight)
	{
		SetEditHeight(GetEditHeight());

		return true;
	}

	return false;
}

void CDUIColorPaletteCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (pControl == m_pEditColorCtrl && DuiNotify_TextChanged == Notify.NotifyType)
	{
		CMMString strText = m_pEditColorCtrl->GetText();
		strText.Trim(_T("0x"));
		strText.Trim(_T("0X"));
		strText.Trim(_T("#"));
		Gdiplus::ARGB dwColor = _tcstoul(strText.c_str(), NULL, 16);

		SetPaletteColor(dwColor);

		return;
	}

	return IDuiControlCallBack::OnNotify(pControl, Notify);
}

void CDUIColorPaletteCtrl::OnRelease(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	pControl->UnRegisterControlCallBack(this);
	if (pControl == m_pEditColorCtrl)
	{
		m_pEditColorCtrl = NULL;
	}

	return IDuiControlCallBack::OnRelease(pControl);
}

LPVOID CDUIColorPaletteCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIColorPaletteCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIColorPaletteCtrl::GetDescribe() const
{
	return Dui_Ctrl_ColorPalette;
}

void CDUIColorPaletteCtrl::RefreshView()
{
	if (m_pEditColorCtrl)
	{
		m_pEditColorCtrl->SetFixedHeight(max(1, GetEditHeight()));
		m_pEditColorCtrl->SetVertAlignType(VertAlign_Bottom);
	}

	__super::RefreshView();

	SyncPaletteRect();

	return;
}

bool CDUIColorPaletteCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (GetChildCount() >= 1 || NULL == MMInterfaceHelper(CDUIXEditCtrl, pChild)) return false;

	return __super::InsertChild(pChild, nPos);
}

CDUIXEditCtrl * CDUIColorPaletteCtrl::GetChildAt(int nIndex) const
{
	return MMInterfaceHelper(CDUIXEditCtrl, __super::GetChildAt(nIndex));
}

HFONT CDUIColorPaletteCtrl::GetFont()
{
	return m_AttributeTextStyle.GetFont();
}

Gdiplus::ARGB CDUIColorPaletteCtrl::GetTextColor()
{
	return m_AttributeTextStyle.GetTextColor();
}

tagDuiTextStyle CDUIColorPaletteCtrl::GetTextStyle()
{
	return m_AttributeTextStyle.GetTextStyle();
}

void CDUIColorPaletteCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyle.SetTextStyle(TextStyle);

	if (NULL == m_pEditColorCtrl) return;

	m_pEditColorCtrl->SetTextStyle(m_AttributeTextStyle.GetTextStyle());

	Invalidate();

	return;
}

Gdiplus::ARGB CDUIColorPaletteCtrl::GetPaletteColor()
{
	return m_AttributeColor.GetColorValue();
}

void CDUIColorPaletteCtrl::SetPaletteColor(Gdiplus::ARGB dwColor)
{
	m_AttributeColor.SetColor(dwColor);
	
	ApplyColor(dwColor);

	return;
}

CDUIXEditCtrl * CDUIColorPaletteCtrl::GetEditColorCtrl()
{
	return m_pEditColorCtrl;
}

int CDUIColorPaletteCtrl::GetEditHeight()
{
	return (int)m_AttributeEditHeight.GetValue();
}

void CDUIColorPaletteCtrl::SetEditHeight(int nHeight)
{
	nHeight = max(1, nHeight);
	m_AttributeEditHeight.SetValue(nHeight);
	if (m_pEditColorCtrl)
	{
		m_pEditColorCtrl->SetFixedHeight(nHeight);
	}

	return;
}

bool CDUIColorPaletteCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiLButtonDown(pt, Msg);

	m_nDragPart = HitPart(pt);
	if (DragPart_None != m_nDragPart)
	{
		UpdateColorFromPoint(pt);
	}

	return true;
}

bool CDUIColorPaletteCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	const int nDragPart = m_nDragPart;
	m_nDragPart = DragPart_None;
	if (DragPart_None != nDragPart)
	{
		SetPaletteColor(MakeColor());
	}

	return __super::OnDuiLButtonUp(pt, Msg);
}

bool CDUIColorPaletteCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if ((m_cbControlStatus & ControlStatus_Pushed) && DragPart_None != m_nDragPart)
	{
		UpdateColorFromPoint(pt);
	}

	return __super::OnDuiMouseMove(pt, Msg);
}

void CDUIColorPaletteCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	return;
}

void CDUIColorPaletteCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupPalette, _T("ColorPalette"));
	DuiCreateAttribute(m_AttributeColor, _T("Color"), _T(""), m_AttributeGroupPalette);
	DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupPalette);
	DuiCreateAttribute(m_AttributeEditHeight, _T("EditHeight"), _T(""), m_AttributeGroupPalette);

	return;
}

void CDUIColorPaletteCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeGroupChild, false);

	if (NULL == m_pEditColorCtrl)
	{
		m_pEditColorCtrl = MMInterfaceHelper(CDUIXEditCtrl, FindSubControlThisView(Dui_CtrlIDInner_ColorPaletteEdit));
	}
	if (NULL == m_pEditColorCtrl)
	{
		m_pEditColorCtrl = new CDUIXEditCtrl();
		m_pEditColorCtrl->Init();
		m_pEditColorCtrl->SetCtrlID(Dui_CtrlIDInner_ColorPaletteEdit);
		InsertChild(m_pEditColorCtrl);
	}
	if (m_pEditColorCtrl)
	{
		m_pEditColorCtrl->SetEditTextType(EditText_None);
		m_pEditColorCtrl->SetMaxChar(9);
		m_pEditColorCtrl->SetAutoSelAll(true);
		m_pEditColorCtrl->SetTipText(_T("#AARRGGBB"));
		m_pEditColorCtrl->SetVertAlignType(VertAlign_Bottom);
		m_pEditColorCtrl->SetPaddingT(6);
		m_pEditColorCtrl->SetFixedHeight(max(1, GetEditHeight()));
		m_pEditColorCtrl->RegisterControlCallBack(this);
	}

	EnableScrollBar(false, false);
	ApplyColor(GetPaletteColor());

	return;
}

void CDUIColorPaletteCtrl::PaintBkImage(HDC hDC)
{
	__super::PaintBkImage(hDC);

	if (m_nWheelSize < 16 || m_fOuter <= 1.0f || NULL == m_pBmpWheel)
	{
		SyncPaletteRect();
	}
	if (m_nWheelSize < 16 || m_fOuter <= 1.0f) return;

	DrawCachedBmp(hDC, m_pBmpWheel, m_nWheelLeft, m_nWheelTop, m_nWheelSize, m_nWheelSize);

	EnsureTriangleBitmap();
	DrawCachedBmp(hDC, m_pBmpTri, m_nWheelLeft, m_nWheelTop, m_nWheelSize, m_nWheelSize);

	EnsureMarker();
	const float fTheta = (m_fHue - 90.0f) * 0.0174532925f;
	const float fMid = (m_fInner + m_fOuter) * 0.5f;
	DrawMarkerBmp(hDC, m_pBmpMark, m_nMarkSize, m_fCx + cosf(fTheta) * fMid, m_fCy + sinf(fTheta) * fMid);

	float fAx = 0.0f, fAy = 0.0f, fBx = 0.0f, fBy = 0.0f, fCx = 0.0f, fCy = 0.0f;
	TriangleVertex(fAx, fAy, fBx, fBy, fCx, fCy);
	const float fA = m_fSat * m_fVal;
	const float fB = m_fVal * (1.0f - m_fSat);
	const float fC = 1.0f - m_fVal;
	DrawMarkerBmp(hDC, m_pBmpMark, m_nMarkSize, fA * fAx + fB * fBx + fC * fCx, fA * fAy + fB * fBy + fC * fCy);

	return;
}

void CDUIColorPaletteCtrl::ApplyColor(Gdiplus::ARGB dwColor)
{
	float fH = 0.0f, fS = 0.0f, fV = 0.0f;
	CDUIRenderEngine::RGBToHSV(dwColor, &fH, &fS, &fV);
	if (fS > 0.001f) m_fHue = fH;
	m_fSat = fS;
	m_fVal = fV;
	m_cbAlpha = DUIARGBGetA(dwColor);

	CMMString strColor = CMMStrHelp::Format(_T("%08x"), MakeColor());
	if (strColor == m_pEditColorCtrl->GetText()) return;

	m_pEditColorCtrl->SetText(strColor);

	Invalidate();

	return;
}

void CDUIColorPaletteCtrl::SyncPaletteRect()
{
	CDUIRect rcRange = GetLayoutRangeOfItem();
	if (m_pEditColorCtrl && m_pEditColorCtrl->IsVisible())
	{
		const CDUIRect rcEdit = m_pEditColorCtrl->GetAbsoluteRect();
		if (rcEdit.top > rcRange.top && rcEdit.top < rcRange.bottom)
		{
			rcRange.bottom = rcEdit.top;
		}
	}

	const int nInset = 4;
	rcRange.left += nInset;
	rcRange.top += nInset;
	rcRange.right -= nInset;
	rcRange.bottom -= nInset;
	if (rcRange.GetWidth() <= 0 || rcRange.GetHeight() <= 0)
	{
		m_rcPalette.Clear();
	}
	else
	{
		m_rcPalette = rcRange;
	}

	LayoutWheel();

	return;
}

void CDUIColorPaletteCtrl::LayoutWheel()
{
	m_fOuter = 0.0f;
	m_fInner = 0.0f;
	m_fTriR = 0.0f;
	m_nWheelSize = 0;
	if (m_rcPalette.GetWidth() < 16 || m_rcPalette.GetHeight() < 16) 
	{
		MMSafeDelete(m_pBmpWheel);
		MMSafeDelete(m_pBmpTri);
		m_fBmpHue = -1.0f;
		return;
	}

	const int nMargin = 3;
	m_nWheelSize = min(m_rcPalette.GetWidth(), m_rcPalette.GetHeight()) - nMargin * 2;
	if (m_nWheelSize < 16) { m_nWheelSize = 0; MMSafeDelete(m_pBmpWheel); MMSafeDelete(m_pBmpTri); m_fBmpHue = -1.0f; return; }
	m_nWheelLeft = m_rcPalette.left + (m_rcPalette.GetWidth() - m_nWheelSize) / 2;
	m_nWheelTop = m_rcPalette.top + (m_rcPalette.GetHeight() - m_nWheelSize) / 2;
	m_fCx = m_nWheelLeft + m_nWheelSize * 0.5f;
	m_fCy = m_nWheelTop + m_nWheelSize * 0.5f;
	m_fOuter = m_nWheelSize * 0.5f - 2.0f;
	m_fInner = m_fOuter * 0.76f;
	m_fTriR = m_fInner * 0.78f;
	EnsureRingBitmap();

	return;
}

void CDUIColorPaletteCtrl::UpdateColorFromPoint(const CDUIPoint &pt)
{
	if (DragPart_Ring == m_nDragPart)
	{
		m_fHue = HueFromOffset(pt.x - m_fCx, pt.y - m_fCy);
	}
	else if (DragPart_Triangle == m_nDragPart)
	{
		float fA = 0.0f, fB = 0.0f, fC = 0.0f;
		if (false == Barycentric((float)pt.x, (float)pt.y, fA, fB, fC)) return;
		ClampBary(fA, fB, fC);
		m_fVal = max(0.0f, min(1.0f, fA + fB));
		if (m_fVal > 0.001f) m_fSat = max(0.0f, min(1.0f, fA / m_fVal));
	}
	else
	{
		return;
	}

	const Gdiplus::ARGB dwColor = MakeColor();
	SetPaletteColor(dwColor);

	return;
}

void CDUIColorPaletteCtrl::EnsureRingBitmap()
{
	if (m_nWheelSize < 16) return;
	if (SameBitmapSize(m_pBmpWheel, m_nWheelSize, m_nWheelSize)) return;

	MMSafeDelete(m_pBmpWheel);
	MMSafeDelete(m_pBmpTri);
	m_fBmpHue = -1.0f;

	const int nSize = m_nWheelSize;
	Gdiplus::Bitmap *pBmp = new Gdiplus::Bitmap(nSize, nSize, PixelFormat32bppARGB);
	Gdiplus::BitmapData data = {};
	Gdiplus::Rect rcLock(0, 0, nSize, nSize);
	if (Gdiplus::Ok != pBmp->LockBits(&rcLock, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data) || NULL == data.Scan0)
	{
		if (NULL != data.Scan0) pBmp->UnlockBits(&data);
		MMSafeDelete(pBmp);
		return;
	}

	const float fBmpMid = nSize * 0.5f;
	const float fIn2 = max(0.0f, m_fInner - 2.0f);
	const float fOut2 = m_fOuter + 2.0f;
	const float fInSq = fIn2 * fIn2;
	const float fOutSq = fOut2 * fOut2;
	static const float fSub[2] = { 0.25f, 0.75f };

	for (int y = 0; y < nSize; ++y)
	{
		BYTE *pRow = (BYTE *)data.Scan0 + y * data.Stride;
		memset(pRow, 0, (size_t)nSize * 4);
		for (int x = 0; x < nSize; ++x)
		{
			const float fDx0 = (x + 0.5f) - fBmpMid;
			const float fDy0 = (y + 0.5f) - fBmpMid;
			const float fDist2 = fDx0 * fDx0 + fDy0 * fDy0;
			if (fDist2 < fInSq || fDist2 > fOutSq) continue;

			float fAccR = 0.0f, fAccG = 0.0f, fAccB = 0.0f, fAccA = 0.0f;
			for (int nY = 0; nY < 2; ++nY)
			{
				const float fOy = fSub[nY];
				const float fDy = (y + fOy) - fBmpMid;
				for (int nX = 0; nX < 2; ++nX)
				{
					const float fOx = fSub[nX];
					float fR = 0.0f, fG = 0.0f, fB = 0.0f, fCover = 0.0f;
					SampleWheel(m_nWheelLeft + x + fOx, m_nWheelTop + y + fOy, (x + fOx) - fBmpMid, fDy,
						0, 0, 0, 0, 0, 0, 0, m_fInner, m_fOuter, 0, true, fR, fG, fB, fCover);
					fAccA += fCover;
					fAccR += fR * fCover;
					fAccG += fG * fCover;
					fAccB += fB * fCover;
				}
			}
			if (fAccA <= 0.004f) continue;

			float fCov = fAccA * 0.25f;
			if (fCov < 1.0f) fCov = powf(fCov, 1.0f / 2.2f);
			BYTE *pPixel = pRow + x * 4;
			WritePremul(pPixel, fAccB / fAccA, fAccG / fAccA, fAccR / fAccA, fCov);
		}
	}
	pBmp->UnlockBits(&data);

	m_pBmpWheel = pBmp;

	return;
}

void CDUIColorPaletteCtrl::EnsureTriangleBitmap()
{
	if (m_nWheelSize < 16) return;
	if (DragPart_Ring == m_nDragPart && NULL != m_pBmpTri) return;
	if (SameBitmapSize(m_pBmpTri, m_nWheelSize, m_nWheelSize) && fabsf(m_fBmpHue - m_fHue) < 0.05f) return;

	MMSafeDelete(m_pBmpTri);

	const int nSize = m_nWheelSize;
	Gdiplus::Bitmap *pBmp = new Gdiplus::Bitmap(nSize, nSize, PixelFormat32bppARGB);
	Gdiplus::BitmapData data = {};
	Gdiplus::Rect rcLock(0, 0, nSize, nSize);
	if (Gdiplus::Ok != pBmp->LockBits(&rcLock, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data) || NULL == data.Scan0)
	{
		if (NULL != data.Scan0) pBmp->UnlockBits(&data);
		MMSafeDelete(pBmp);
		return;
	}

	float fAx = 0.0f, fAy = 0.0f, fBx = 0.0f, fBy = 0.0f, fCx = 0.0f, fCy = 0.0f;
	TriangleVertex(fAx, fAy, fBx, fBy, fCx, fCy);
	const float fAlt = m_fTriR * 1.5f;
	const float fBmpMid = nSize * 0.5f;
	int nLeft = max(0, (int)floorf(min(fAx, min(fBx, fCx)) - m_nWheelLeft - 2.0f));
	int nTop = max(0, (int)floorf(min(fAy, min(fBy, fCy)) - m_nWheelTop - 2.0f));
	int nRight = min(nSize, (int)ceilf(max(fAx, max(fBx, fCx)) - m_nWheelLeft + 2.0f));
	int nBottom = min(nSize, (int)ceilf(max(fAy, max(fBy, fCy)) - m_nWheelTop + 2.0f));
	static const float fSub[2] = { 0.25f, 0.75f };

	for (int y = 0; y < nSize; ++y)
	{
		BYTE *pRow = (BYTE *)data.Scan0 + y * data.Stride;
		memset(pRow, 0, (size_t)nSize * 4);
		if (y < nTop || y >= nBottom) continue;
		for (int x = nLeft; x < nRight; ++x)
		{
			float fAccR = 0.0f, fAccG = 0.0f, fAccB = 0.0f, fAccA = 0.0f;
			for (int nY = 0; nY < 2; ++nY)
			{
				const float fOy = fSub[nY];
				const float fDy = (y + fOy) - fBmpMid;
				for (int nX = 0; nX < 2; ++nX)
				{
					const float fOx = fSub[nX];
					float fR = 0.0f, fG = 0.0f, fB = 0.0f, fCover = 0.0f;
					SampleWheel(m_nWheelLeft + x + fOx, m_nWheelTop + y + fOy, (x + fOx) - fBmpMid, fDy,
						fAx, fAy, fBx, fBy, fCx, fCy, fAlt, m_fInner, m_fOuter, m_fHue, false, fR, fG, fB, fCover);
					fAccA += fCover;
					fAccR += fR * fCover;
					fAccG += fG * fCover;
					fAccB += fB * fCover;
				}
			}
			if (fAccA <= 0.004f) continue;

			float fCov = fAccA * 0.25f;
			if (fCov < 1.0f) fCov = powf(fCov, 1.0f / 2.2f);
			BYTE *pPixel = pRow + x * 4;
			WritePremul(pPixel, fAccB / fAccA, fAccG / fAccA, fAccR / fAccA, fCov);
		}
	}
	pBmp->UnlockBits(&data);

	m_pBmpTri = pBmp;
	m_fBmpHue = m_fHue;

	return;
}

void CDUIColorPaletteCtrl::EnsureMarker()
{
	const float fRing = m_fOuter - m_fInner;
	float fR = 2.5f;
	if (fRing > 1.0f && m_fTriR > 1.0f)
	{
		const float fFit = (m_fOuter - m_fInner) * 0.5f - 1.2f;
		fR = min(fRing * 0.46f, m_fTriR * 0.20f);
		if (fFit > 2.0f) fR = min(fR, fFit);
		if (fR < 2.5f) fR = 2.5f;
	}
	const int nMark = max(8, (int)ceilf((fR + 2.0f) * 2.0f));
	if (SameBitmapSize(m_pBmpMark, nMark, nMark))
	{
		m_nMarkSize = nMark;
		m_fMarkR = fR;
		return;
	}

	MMSafeDelete(m_pBmpMark);
	m_fMarkR = fR;
	m_nMarkSize = nMark;

	Gdiplus::Bitmap *pBmp = new Gdiplus::Bitmap(m_nMarkSize, m_nMarkSize, PixelFormat32bppARGB);
	Gdiplus::BitmapData data = {};
	Gdiplus::Rect rcLock(0, 0, m_nMarkSize, m_nMarkSize);
	if (Gdiplus::Ok != pBmp->LockBits(&rcLock, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data) || NULL == data.Scan0)
	{
		if (NULL != data.Scan0) pBmp->UnlockBits(&data);
		MMSafeDelete(pBmp);
		m_nMarkSize = 0;
		return;
	}

	const float fMid = m_nMarkSize * 0.5f;
	const float fStroke = max(1.7f, fR * 0.38f);
	const float fIn = max(0.8f, fR - fStroke);
	static const float fSub[2] = { 0.25f, 0.75f };
	for (int y = 0; y < m_nMarkSize; ++y)
	{
		BYTE *pRow = (BYTE *)data.Scan0 + y * data.Stride;
		for (int x = 0; x < m_nMarkSize; ++x)
		{
			BYTE *pPixel = pRow + x * 4;
			float fAccR = 0.0f, fAccG = 0.0f, fAccB = 0.0f, fAccA = 0.0f;
			for (int nY = 0; nY < 2; ++nY)
			{
				const float fDy = (y + fSub[nY]) - fMid;
				for (int nX = 0; nX < 2; ++nX)
				{
					const float fDx = (x + fSub[nX]) - fMid;
					const float fDist = sqrtf(fDx * fDx + fDy * fDy);
					const float fDisk = EdgeCover(fR + 1.15f - fDist);
					const float fHole = EdgeCover(fIn - fDist);
					const float fWhite = EdgeCover(fR - 0.15f - fDist) * (1.0f - fHole);
					const float fDark = max(0.0f, fDisk * (1.0f - fHole) - fWhite);
					const float fCover = fWhite + fDark;
					fAccA += fCover;
					fAccR += (255.0f * fWhite + 28.0f * fDark);
					fAccG += (255.0f * fWhite + 28.0f * fDark);
					fAccB += (255.0f * fWhite + 28.0f * fDark);
				}
			}
			if (fAccA <= 0.004f)
			{
				pPixel[0] = pPixel[1] = pPixel[2] = pPixel[3] = 0;
				continue;
			}
			float fCov = fAccA * 0.25f;
			if (fCov < 1.0f) fCov = powf(max(0.0f, fCov), 1.0f / 2.2f);
			WritePremul(pPixel, fAccB / fAccA, fAccG / fAccA, fAccR / fAccA, fCov);
		}
	}
	pBmp->UnlockBits(&data);
	m_pBmpMark = pBmp;

	return;
}

void CDUIColorPaletteCtrl::TriangleVertex(float &fAx, float &fAy, float &fBx, float &fBy, float &fCx, float &fCy) const
{
	const float fTheta = (m_fHue - 90.0f) * 0.0174532925f;
	fAx = m_fCx + cosf(fTheta) * m_fTriR;
	fAy = m_fCy + sinf(fTheta) * m_fTriR;
	fBx = m_fCx + cosf(fTheta + 4.1887902f) * m_fTriR;
	fBy = m_fCy + sinf(fTheta + 4.1887902f) * m_fTriR;
	fCx = m_fCx + cosf(fTheta + 2.0943951f) * m_fTriR;
	fCy = m_fCy + sinf(fTheta + 2.0943951f) * m_fTriR;

	return;
}

bool CDUIColorPaletteCtrl::Barycentric(float fX, float fY, float &fA, float &fB, float &fC) const
{
	float fAx = 0.0f, fAy = 0.0f, fBx = 0.0f, fBy = 0.0f, fCx = 0.0f, fCy = 0.0f;
	TriangleVertex(fAx, fAy, fBx, fBy, fCx, fCy);

	return BaryOf(fX, fY, fAx, fAy, fBx, fBy, fCx, fCy, fA, fB, fC);
}

int CDUIColorPaletteCtrl::HitPart(const CDUIPoint &pt) const
{
	if (m_fOuter <= 1.0f) return DragPart_None;

	float fA = 0.0f, fB = 0.0f, fC = 0.0f;
	if (Barycentric((float)pt.x, (float)pt.y, fA, fB, fC) && fA >= -0.02f && fB >= -0.02f && fC >= -0.02f)
	{
		return DragPart_Triangle;
	}

	const float fDx = pt.x - m_fCx;
	const float fDy = pt.y - m_fCy;
	const float fDist2 = fDx * fDx + fDy * fDy;
	const float fIn = m_fInner - 1.5f;
	const float fOut = m_fOuter + 2.0f;
	if (fDist2 >= fIn * fIn && fDist2 <= fOut * fOut) return DragPart_Ring;

	return DragPart_None;
}

Gdiplus::ARGB CDUIColorPaletteCtrl::MakeColor() const
{
	return CDUIRenderEngine::HSVToRGB(m_fHue, m_fSat, m_fVal, m_cbAlpha);
}
