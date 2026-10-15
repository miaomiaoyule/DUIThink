#include "StdAfx.h"
#include "DUIColorPaletteCtrl.h"

//////////////////////////////////////////////////////////////////////////
#define DragPart_None						(0)
#define DragPart_Hex						(1)
#define DragPart_Value						(2)

//////////////////////////////////////////////////////////////////////////
static float HueFromOffset(float fDx, float fDy)
{
	float fHue = atan2f(-fDy, fDx) * 57.2957795f;
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

static float HexEdgeRadius(float fHue, float fOuter)
{
	float fMod = fmodf(fHue, 60.0f);
	if (fMod < 0.0f) fMod += 60.0f;
	const float fCos = cosf((fMod - 30.0f) * 0.0174532925f);
	if (fCos < 0.001f) return fOuter;

	return fOuter * 0.8660254f / fCos;
}

static void SampleHex(float fDx, float fDy, float fOuter, float &fR, float &fG, float &fB, float &fCover)
{
	const float fDist = sqrtf(fDx * fDx + fDy * fDy);
	float fHue = 0.0f;
	if (fDist > 0.001f) fHue = HueFromOffset(fDx, fDy);
	const float fEdge = HexEdgeRadius(fHue, fOuter);
	fCover = EdgeCover(fEdge - fDist);
	if (fCover <= 0.0f)
	{
		fR = 0.0f;
		fG = 0.0f;
		fB = 0.0f;
		return;
	}

	float fSat = 0.0f;
	if (fEdge > 0.001f) fSat = min(1.0f, fDist / fEdge);
	const Gdiplus::ARGB dwColor = CDUIRenderEngine::HSVToRGB(fHue, fSat, 1.0f);
	fR = DUIARGBGetR(dwColor);
	fG = DUIARGBGetG(dwColor);
	fB = DUIARGBGetB(dwColor);
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
	MMSafeDelete(m_pBmpValue);
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
	if (pControl == m_pEditColorCtrl && DuiNotify_Edited == Notify.NotifyType)
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
	const float fHue = m_fHue;
	const float fSat = m_fSat;
	const float fVal = m_fVal;
	m_nDragPart = DragPart_None;
	if (DragPart_None != nDragPart)
	{
		SetPaletteColor(MakeColor());
		m_fHue = fHue;
		m_fSat = fSat;
		m_fVal = fVal;
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
		tagDuiTextStyle TextStyle = {};
		TextStyle.dwTextStyle = DT_CENTER | DT_VCENTER;
		m_pEditColorCtrl->SetEditTextType(EditText_None);
		m_pEditColorCtrl->SetMaxChar(9);
		m_pEditColorCtrl->SetAutoSelAll(true);
		m_pEditColorCtrl->SetTipText(_T("#AARRGGBB"));
		m_pEditColorCtrl->SetVertAlignType(VertAlign_Bottom);
		m_pEditColorCtrl->SetPaddingT(6);
		m_pEditColorCtrl->SetFixedHeight(max(1, GetEditHeight()));
		m_pEditColorCtrl->SetTextStyle(TextStyle);
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

	EnsureMarker();
	const float fTheta = -m_fHue * 0.0174532925f;
	const float fRad = max(0.0f, min(1.0f, m_fSat)) * HexEdgeRadius(m_fHue, m_fOuter);
	DrawMarkerBmp(hDC, m_pBmpMark, m_nMarkSize, m_fCx + cosf(fTheta) * fRad, m_fCy + sinf(fTheta) * fRad);

	EnsureValueBitmap();
	if (m_pBmpValue && m_rcValue.GetWidth() > 0 && m_rcValue.GetHeight() > 0)
	{
		DrawCachedBmp(hDC, m_pBmpValue, m_rcValue.left, m_rcValue.top, m_rcValue.GetWidth(), m_rcValue.GetHeight());
		const float fX = (m_rcValue.left + m_rcValue.right) * 0.5f;
		const float fY = m_rcValue.top + (1.0f - max(0.0f, min(1.0f, m_fVal))) * m_rcValue.GetHeight();
		DrawMarkerBmp(hDC, m_pBmpMark, m_nMarkSize, fX, fY);
	}

	if (DragPart_None != m_nDragPart)
	{
		SyncEditText();
	}

	return;
}

void CDUIColorPaletteCtrl::ApplyColor(Gdiplus::ARGB dwColor)
{
	float fH = 0.0f, fS = 0.0f, fV = 0.0f;
	CDUIRenderEngine::RGBToHSV(dwColor, &fH, &fS, &fV);
	if (DragPart_None == m_nDragPart)
	{
		if (fS > 0.001f) m_fHue = fH;
		m_fSat = fS;
		m_fVal = fV;
	}
	m_cbAlpha = DUIARGBGetA(dwColor);
	SyncEditText();
	Invalidate();

	return;
}

void CDUIColorPaletteCtrl::SyncEditText()
{
	if (m_bSyncEdit || NULL == m_pEditColorCtrl) return;

	m_bSyncEdit = true;
	CMMString strColor = CMMStrHelp::Format(_T("%08x"), MakeColor());
	m_pEditColorCtrl->SetText(strColor.c_str());
	m_pEditColorCtrl->RefreshView();
	m_bSyncEdit = false;

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
	m_nWheelSize = 0;
	m_rcValue.Clear();
	if (m_rcPalette.GetWidth() < 16 || m_rcPalette.GetHeight() < 16)
	{
		MMSafeDelete(m_pBmpWheel);
		MMSafeDelete(m_pBmpValue);
		m_dwValueTop = 0;
		return;
	}

	const int nMargin = 3;
	const int nGap = 10;
	const int nSlider = 18;
	const int nAvailW = m_rcPalette.GetWidth() - nMargin * 2;
	const int nAvailH = m_rcPalette.GetHeight() - nMargin * 2;
	m_nWheelSize = min(nAvailH, nAvailW - nGap - nSlider);
	if (m_nWheelSize < 16)
	{
		m_nWheelSize = 0;
		MMSafeDelete(m_pBmpWheel);
		MMSafeDelete(m_pBmpValue);
		m_dwValueTop = 0;
		return;
	}

	const int nGroupW = m_nWheelSize + nGap + nSlider;
	const int nGroupLeft = m_rcPalette.left + (m_rcPalette.GetWidth() - nGroupW) / 2;
	const int nGroupTop = m_rcPalette.top + (m_rcPalette.GetHeight() - m_nWheelSize) / 2;
	m_nWheelLeft = nGroupLeft;
	m_nWheelTop = nGroupTop;
	m_fCx = m_nWheelLeft + m_nWheelSize * 0.5f;
	m_fCy = m_nWheelTop + m_nWheelSize * 0.5f;
	m_fOuter = m_nWheelSize * 0.5f - 2.0f;

	int nThumbPad = 8;
	if (m_nWheelSize - nThumbPad * 2 < 8) nThumbPad = 2;
	m_rcValue.left = m_nWheelLeft + m_nWheelSize + nGap;
	m_rcValue.right = m_rcValue.left + nSlider;
	m_rcValue.top = m_nWheelTop + nThumbPad;
	m_rcValue.bottom = m_nWheelTop + m_nWheelSize - nThumbPad;
	EnsureWheelBitmap();

	return;
}

void CDUIColorPaletteCtrl::UpdateColorFromPoint(const CDUIPoint &pt)
{
	if (DragPart_Hex == m_nDragPart)
	{
		if (m_fOuter <= 1.0f) return;

		const float fDx = pt.x - m_fCx;
		const float fDy = pt.y - m_fCy;
		const float fDist = sqrtf(fDx * fDx + fDy * fDy);
		if (fDist > 0.001f) m_fHue = HueFromOffset(fDx, fDy);
		const float fEdge = HexEdgeRadius(m_fHue, m_fOuter);
		m_fSat = (fEdge > 0.001f) ? max(0.0f, min(1.0f, fDist / fEdge)) : 0.0f;
	}
	else if (DragPart_Value == m_nDragPart)
	{
		const int nHeight = m_rcValue.GetHeight();
		if (nHeight <= 0) return;

		const float fT = (pt.y - m_rcValue.top) / (float)nHeight;
		m_fVal = max(0.0f, min(1.0f, 1.0f - fT));
	}
	else
	{
		return;
	}

	SetPaletteColor(MakeColor());
	if ((m_cbControlStatus & ControlStatus_Pushed) && m_pWndOwner)
	{
		UpdateWindow(m_pWndOwner->GetWndHandle());
	}

	return;
}

void CDUIColorPaletteCtrl::EnsureWheelBitmap()
{
	if (m_nWheelSize < 16) return;
	if (SameBitmapSize(m_pBmpWheel, m_nWheelSize, m_nWheelSize)) return;

	MMSafeDelete(m_pBmpWheel);

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
	const float fInside = m_fOuter * 0.8660254f - 1.5f;
	const float fOutside = m_fOuter + 1.5f;
	static const float fSub[2] = { 0.25f, 0.75f };

	for (int y = 0; y < nSize; ++y)
	{
		BYTE *pRow = (BYTE *)data.Scan0 + y * data.Stride;
		memset(pRow, 0, (size_t)nSize * 4);
		for (int x = 0; x < nSize; ++x)
		{
			const float fDx = (x + 0.5f) - fBmpMid;
			const float fDy = (y + 0.5f) - fBmpMid;
			const float fDist = sqrtf(fDx * fDx + fDy * fDy);
			BYTE *pPixel = pRow + x * 4;
			if (fDist < fInside)
			{
				float fR = 0.0f, fG = 0.0f, fB = 0.0f, fCover = 0.0f;
				SampleHex(fDx, fDy, m_fOuter, fR, fG, fB, fCover);
				WritePremul(pPixel, fB, fG, fR, fCover);
				continue;
			}
			if (fDist > fOutside) continue;

			float fAccR = 0.0f, fAccG = 0.0f, fAccB = 0.0f, fAccA = 0.0f;
			for (int nY = 0; nY < 2; ++nY)
			{
				for (int nX = 0; nX < 2; ++nX)
				{
					float fR = 0.0f, fG = 0.0f, fB = 0.0f, fCover = 0.0f;
					SampleHex((x + fSub[nX]) - fBmpMid, (y + fSub[nY]) - fBmpMid, m_fOuter, fR, fG, fB, fCover);
					fAccA += fCover;
					fAccR += fR * fCover;
					fAccG += fG * fCover;
					fAccB += fB * fCover;
				}
			}
			if (fAccA <= 0.004f) continue;

			float fCov = fAccA * 0.25f;
			if (fCov < 1.0f) fCov = powf(fCov, 1.0f / 2.2f);
			WritePremul(pPixel, fAccB / fAccA, fAccG / fAccA, fAccR / fAccA, fCov);
		}
	}
	pBmp->UnlockBits(&data);

	m_pBmpWheel = pBmp;

	return;
}

void CDUIColorPaletteCtrl::EnsureValueBitmap()
{
	const int nW = m_rcValue.GetWidth();
	const int nH = m_rcValue.GetHeight();
	if (nW < 4 || nH < 8)
	{
		MMSafeDelete(m_pBmpValue);
		m_dwValueTop = 0;
		return;
	}

	const Gdiplus::ARGB dwTop = CDUIRenderEngine::HSVToRGB(m_fHue, m_fSat, 1.0f, 255);
	if (SameBitmapSize(m_pBmpValue, nW, nH) && dwTop == m_dwValueTop) return;

	MMSafeDelete(m_pBmpValue);
	m_dwValueTop = dwTop;

	Gdiplus::Bitmap *pBmp = new Gdiplus::Bitmap(nW, nH, PixelFormat32bppARGB);
	Gdiplus::BitmapData data = {};
	Gdiplus::Rect rcLock(0, 0, nW, nH);
	if (Gdiplus::Ok != pBmp->LockBits(&rcLock, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data) || NULL == data.Scan0)
	{
		if (NULL != data.Scan0) pBmp->UnlockBits(&data);
		MMSafeDelete(pBmp);
		m_dwValueTop = 0;
		return;
	}

	const float fR = DUIARGBGetR(dwTop);
	const float fG = DUIARGBGetG(dwTop);
	const float fB = DUIARGBGetB(dwTop);
	for (int y = 0; y < nH; ++y)
	{
		BYTE *pRow = (BYTE *)data.Scan0 + y * data.Stride;
		const float fV = 1.0f - (y + 0.5f) / (float)nH;
		BYTE *pPixel = pRow;
		WritePremul(pPixel, fB * fV, fG * fV, fR * fV, 1.0f);
		for (int x = 1; x < nW; ++x)
		{
			BYTE *pNext = pRow + x * 4;
			pNext[0] = pPixel[0];
			pNext[1] = pPixel[1];
			pNext[2] = pPixel[2];
			pNext[3] = pPixel[3];
		}
	}
	pBmp->UnlockBits(&data);
	m_pBmpValue = pBmp;

	return;
}

void CDUIColorPaletteCtrl::EnsureMarker()
{
	float fR = 4.0f;
	if (m_fOuter > 1.0f)
	{
		fR = m_fOuter * 0.045f;
		if (fR < 3.0f) fR = 3.0f;
		if (fR > 7.0f) fR = 7.0f;
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

int CDUIColorPaletteCtrl::HitPart(const CDUIPoint &pt) const
{
	if (m_rcValue.GetWidth() > 0 && m_rcValue.GetHeight() > 0)
	{
		CDUIRect rcHit = m_rcValue;
		rcHit.left -= 6;
		rcHit.right += 6;
		rcHit.top -= 8;
		rcHit.bottom += 8;
		if (rcHit.PtInRect(pt)) return DragPart_Value;
	}

	if (m_fOuter <= 1.0f) return DragPart_None;

	const float fDx = pt.x - m_fCx;
	const float fDy = pt.y - m_fCy;
	const float fDist = sqrtf(fDx * fDx + fDy * fDy);
	const float fHue = (fDist > 0.001f) ? HueFromOffset(fDx, fDy) : m_fHue;
	if (fDist <= HexEdgeRadius(fHue, m_fOuter) + 1.5f) return DragPart_Hex;

	return DragPart_None;
}

Gdiplus::ARGB CDUIColorPaletteCtrl::MakeColor() const
{
	return CDUIRenderEngine::HSVToRGB(m_fHue, m_fSat, m_fVal, m_cbAlpha);
}
