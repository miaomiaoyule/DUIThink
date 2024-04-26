#include "StdAfx.h"
#include "DUIListHeaderItemCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIListHeaderItemCtrl)
MMImplement_ClassName(CDUIListHeaderItemCtrl)

CDUIListHeaderItemCtrl::CDUIListHeaderItemCtrl(void)
{
	m_nSepStatus = ControlStatus_Normal;

	return;
}

CDUIListHeaderItemCtrl::~CDUIListHeaderItemCtrl(void)
{
}

bool CDUIListHeaderItemCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeSepWidth)
	{
		SetSepWidth(GetSepWidth());

		return true;
	}

	return false;
}

LPVOID CDUIListHeaderItemCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIListHeaderItemCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIListHeaderItemCtrl::GetDescribe() const
{
	return Dui_Ctrl_ListHeaderItem;
}

UINT CDUIListHeaderItemCtrl::GetControlFlags()
{
	if (IsEnabled() && m_rcSep.GetWidth() != 0) return DUIFLAG_SETCURSOR;
	else return 0;
}

void CDUIListHeaderItemCtrl::SetOwner(CDUIListHeaderCtrl *pOwner)
{
	m_pOwner = pOwner;

	return;
}

int CDUIListHeaderItemCtrl::GetSepWidth()
{
	return m_AttributeSepWidth.GetValue();
}

void CDUIListHeaderItemCtrl::SetSepWidth(int nWidth)
{
#ifndef DUI_DESIGN
	if (nWidth == GetSepWidth()) return;
#endif

	//headeritem sel
	if (Dui_CtrlIDInner_HeaderItemSelect == GetCtrlID())
	{
		nWidth = 0;
	}

	m_AttributeSepWidth.SetValue(nWidth);

	NeedRefreshView();

	return;
}

float CDUIListHeaderItemCtrl::GetSepScale()
{
	return m_AttributeSepScale.GetValue();
}

void CDUIListHeaderItemCtrl::SetSepScale(float fScale)
{
	if (fScale == GetSepScale()) return;

	m_AttributeSepScale.SetValue(fScale);
}

RECT CDUIListHeaderItemCtrl::GetSepRect()
{
	return m_rcSep;
}

void CDUIListHeaderItemCtrl::SetFixedWidth(int nWidth)
{
	return __super::SetFixedWidth(nWidth);
}

bool CDUIListHeaderItemCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (false == __super::InsertChild(pChild, nPos)) return false;

	if (pChild)
	{
		pChild->SetOwnerModelCtrl(this);
	}

	return true;
}

bool CDUIListHeaderItemCtrl::Remove(CDUIControlBase *pControl)
{
	if (NULL == pControl || Dui_CtrlIDInner_HeaderItemCheck == pControl->GetCtrlID()) return false;

	return __super::Remove(pControl);
}

bool CDUIListHeaderItemCtrl::RemoveAt(int nIndex)
{
	CDUIControlBase *pControl = GetChildAt(nIndex);
	if (NULL == pControl || Dui_CtrlIDInner_HeaderItemCheck == pControl->GetCtrlID()) return false;

	return __super::RemoveAt(nIndex);
}

void CDUIListHeaderItemCtrl::RemoveAll()
{
	for (int n = GetChildCount() - 1; n >= 0; n--)
	{
		CDUIControlBase *pControl = GetChildAt(n);
		if (NULL == pControl || Dui_CtrlIDInner_HeaderItemCheck == pControl->GetCtrlID()) continue;

		RemoveAt(n);
	}

	return;
}

void CDUIListHeaderItemCtrl::RefreshView()
{
	__super::RefreshView();

	CalcSepRect();

	return;
}

CMMString CDUIListHeaderItemCtrl::GetText()
{
	return m_AttributeText.GetValue();
}

bool CDUIListHeaderItemCtrl::SetText(LPCTSTR lpszText)
{
	if (lpszText == GetText()) return false;

	m_AttributeText.SetValue(lpszText);

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_TextChanged);
	}

	Invalidate();

	return true;
}

HFONT CDUIListHeaderItemCtrl::GetFont()
{
	return m_AttributeTextStyle.GetFont();
}

ARGB CDUIListHeaderItemCtrl::GetTextColor()
{
	return m_AttributeTextStyle.GetTextColor();
}

tagDuiTextStyle CDUIListHeaderItemCtrl::GetTextStyle()
{
	return m_AttributeTextStyle.GetTextStyle();
}

tagDuiTextStyle CDUIListHeaderItemCtrl::GetTextStyleActive()
{
	return GetTextStyle();
}

void CDUIListHeaderItemCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyle.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

RECT CDUIListHeaderItemCtrl::GetTextPadding()
{
	return DuiDpiScaleCtrl(m_AttributeTextPadding.GetValue());
}

void CDUIListHeaderItemCtrl::SetTextPadding(RECT rcPadding)
{
	if (rcPadding == GetTextPadding()) return;

	m_AttributeTextPadding.SetValue(rcPadding);

	Invalidate();

	return;
}

void CDUIListHeaderItemCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupSep, _T("Separation"));
	DuiCreateAttribute(m_AttributeSepColor, _T("SepColor"), _T("·Ö¸ôÌõÑÕÉ«"), m_AttributeGroupSep);
	DuiCreateAttribute(m_AttributeSepWidth, _T("SepWidth"), _T("separator width"), m_AttributeGroupSep);
	DuiCreateAttribute(m_AttributeSepScale, _T("SepScale"), _T("·Ö¸ôÌõ±ÈÀý"), m_AttributeGroupSep);
	DuiCreateAttribute(m_AttributeSepImage, _T("SepImage"), _T("·Ö¸ôÌõ±³¾°Í¼Æ¬"), m_AttributeGroupSep);

	DuiCreateGroupAttribute(m_AttributeGroupStatus, _T("Status"));
	DuiCreateAttribute(m_AttributeColorNormal, _T("ColorNormal"), _T("item normal back color"), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeColorHot, _T("ColorHot"), _T("item hot back color"), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeColorPushed, _T("ColorPushed"), _T("item pushed back color"), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeColorFocus, _T("ColorFocus"), _T("item focus back color"), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImageNormal, _T("ImageNormal"), _T("item normal back image"), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImageHot, _T("ImageHot"), _T("item hot back image"), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImagePushed, _T("ImagePushed"), _T("item pushed back image"), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImageFocus, _T("ImageFocus"), _T("item focus back image"), m_AttributeGroupStatus);

	DuiCreateGroupAttribute(m_AttributeGroupText, _T("Text"));
	DuiCreateAttribute(m_AttributeText, _T("Text"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextPadding, _T("TextPadding"), _T(""), m_AttributeGroupText);

	return;
}

void CDUIListHeaderItemCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	DuiInitAttriValue(m_AttributeSepWidth, 1);
	DuiInitAttriValue(m_AttributeUseHorizScrollBar, false);
	DuiInitAttriValue(m_AttributeUseVertScrollBar, false);
	DuiInitAttriColorRes(m_AttributeSepColor, Name_ColorBlack);

	return;
}

void CDUIListHeaderItemCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupScroll, false);

	return;
}

//»æÖÆ
void CDUIListHeaderItemCtrl::PaintStatusColor(HDC hDC)
{
	__super::PaintStatusColor(hDC);

	//back
	CDUIAttributeColorSwitch *pAttribute = NULL;
	if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeColorPushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeColorHot;
	}
	else if (IsFocused() && false == m_AttributeColorFocus.empty())
	{
		pAttribute = &m_AttributeColorFocus;
	}
	else
	{
		pAttribute = &m_AttributeColorNormal;
	}

	//äÖÈ¾
	if (pAttribute)
	{
		pAttribute->FillRect(hDC, m_rcAbsolute, IsColorHSL());
	}

	//sep
	m_AttributeSepColor.FillRect(hDC, m_rcSep, IsColorHSL());

	return;
}

void CDUIListHeaderItemCtrl::PaintStatusImage(HDC hDC)
{
	__super::PaintStatusImage(hDC);

	//back
	CDUIAttriImageSection *pAttribute = NULL;
	if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeImagePushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeImageHot;
	}
	else if (IsFocused() && false == m_AttributeImageFocus.empty())
	{
		pAttribute = &m_AttributeImageFocus;
	}
	else
	{
		pAttribute = &m_AttributeImageNormal;
	}

	//äÖÈ¾
	if (pAttribute)
	{
		pAttribute->Draw(hDC, m_rcAbsolute, m_rcPaint);
	}

	//sep
	m_AttributeSepImage.Draw(hDC, m_rcSep, m_rcPaint);

	return;
}

void CDUIListHeaderItemCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pOwner || NULL == m_pWndManager) return;

	CDUIAttributeTextStyle *pAttribute = &m_AttributeTextStyle;
	if (NULL == pAttribute) return;

	//self text
	CMMString strText = GetText();
	if (strText.empty()) return;

	if (false == m_AttributeTextStyle.empty()
		&& 0 == (m_nControlStatus & ControlStatus_Hot))
	{
		pAttribute = &m_AttributeTextStyle;
	}

	CDUIRect rcRange = GetTextRange();

	pAttribute->Draw(hDC, rcRange, strText, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), false);

	return;
}

bool CDUIListHeaderItemCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_HeadItemButtonDown, Msg.wParam, Msg.lParam);
	}
	if (m_rcSep.PtInRect(pt))
	{
		m_nSepStatus |= ControlStatus_Pushed;
		m_ptLastMouse = pt;

		Invalidate();

		return true;
	}

	return __super::OnDuiLButtonDown(pt, Msg);
}

bool CDUIListHeaderItemCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_nControlStatus & ControlStatus_Pushed);

	__super::OnDuiLButtonUp(pt, Msg);

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_HeadItemButtonUp, Msg.wParam, Msg.lParam);

		if (bClick)
		{
			m_pWndManager->SendNotify(this, DuiNotify_HeadItemClick, Msg.wParam, Msg.lParam);
		}
	}

	m_nSepStatus &= ~ControlStatus_Pushed;

	return true;
}

bool CDUIListHeaderItemCtrl::OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (m_rcSep.PtInRect(pt))
	{
		::SetCursor(::LoadCursor(nullptr, IDC_SIZEWE));
	}
	else
	{
		__super::OnDuiSetCursor(pt, Msg);
	}

	return true;
}

bool CDUIListHeaderItemCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	//×´Ì¬
	if (0 == (m_nSepStatus & ControlStatus_Pushed))
	{
		__super::OnDuiMouseMove(pt, Msg);

		if (m_rcSep.PtInRect(pt))
		{
			m_nSepStatus |= ControlStatus_Hot;
		}
		else
		{
			m_nSepStatus &= ~ControlStatus_Hot;
		}

		return true;
	}

	//ÍÏ×§
	if (GetSepWidth() > 0)
	{
		CDUIRect rcTemp = GetAbsoluteRect();
		rcTemp.right += (pt.x - m_ptLastMouse.x);
		rcTemp.right = max(rcTemp.left + GetSepWidth(), rcTemp.right);
		rcTemp.CheckRect();
		int nWidth = max(min(rcTemp.GetWidth(), GetMaxWidth()), GetMinWidth());
		m_ptLastMouse = pt;

		SetFixedWidth(DuiDpiScaleBackCtrl(nWidth));

		if (m_pOwner)
		{
			SetSepScale(m_pOwner->GetScaleValue(nWidth));
		}

		return true;
	}

	return __super::OnDuiMouseMove(pt, Msg);
}

void CDUIListHeaderItemCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nSepStatus &= ~ControlStatus_Hot;

	Invalidate();

	return __super::OnDuiMouseLeave(pt, Msg);
}

void CDUIListHeaderItemCtrl::OnDuiSize(CDUIRect rcParentAbs)
{
	CDUIRect rcThis = GetAbsoluteRect();

	__super::OnDuiSize(rcParentAbs);

	//refresh subitem
	if (rcThis != GetAbsoluteRect())
	{
		if (m_pOwner)
		{
			CDUIListViewCtrl *pListView = m_pOwner->GetOwner();

			if (pListView)
			{
				pListView->NeedRefreshView();
			}
		}
	}

	return;
}

void CDUIListHeaderItemCtrl::CalcSepRect()
{
	int nSepWidth = GetSepWidth();
	if (nSepWidth <= 0)
	{
		m_rcSep.right = m_rcSep.left;

		return;
	}

	nSepWidth = min(nSepWidth, GetWidth());

	m_rcSep = m_rcAbsolute;
	m_rcSep.left = m_rcSep.right - nSepWidth;
	m_rcSep.CheckRect();

	return;
}

CDUIRect CDUIListHeaderItemCtrl::GetTextRange()
{
	CDUIRect rcRange = GetAbsoluteRect();
	CDUIRect rcTextPadding = GetTextPadding();

	rcRange.left += rcTextPadding.left;
	rcRange.right -= rcTextPadding.right;
	rcRange.top += rcTextPadding.top;
	rcRange.bottom -= rcTextPadding.bottom;
	rcRange.CheckRect();

	return rcRange;
}