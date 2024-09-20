#include "StdAfx.h"
#include "DUIIPAddressCtrl.h"

//////////////////////////////////////////////////////////////////////////
#define COUNT_IPVALUE					(4)

#define TEXT_IPDOT						_T(".")

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIIPAddressCtrl)
MMImplement_ClassName(CDUIIPAddressCtrl)

CDUIIPAddressCtrl::CDUIIPAddressCtrl(void)
{
	return;
}

CDUIIPAddressCtrl::~CDUIIPAddressCtrl(void)
{
	return;
}

bool CDUIIPAddressCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeIP)
	{
		SetIPAddress(GetIPAddress());

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStyle)
	{
		RefreshEditTextStyle();

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStyleIPDot)
	{
		RefreshIPDotWidth();

		return true;
	}

	return false;
}

LPVOID CDUIIPAddressCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIIPAddressCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIIPAddressCtrl::GetDescribe() const
{
	return Dui_Ctrl_IPAddress;
}

void CDUIIPAddressCtrl::RefreshView()
{
	__super::RefreshView();

	m_vecIPDot.clear();
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIThinkEditCtrl *pEditCtrl = GetChildAt(n);
		CDUIThinkEditCtrl *pEditNextCtrl = GetChildAt(n + 1);
		if (NULL == pEditCtrl || NULL == pEditNextCtrl) continue;

		CDUIRect rcEdit = pEditCtrl->GetAbsoluteRect();
		CDUIRect rcNext = pEditNextCtrl->GetAbsoluteRect();
		CDUIRect rcDot = { rcEdit.right, rcEdit.top, rcNext.left, rcEdit.bottom };
		m_vecIPDot.push_back(rcDot);
	}

	return;
}

HFONT CDUIIPAddressCtrl::GetFont()
{
	return m_AttributeTextStyle.GetFont();
}

ARGB CDUIIPAddressCtrl::GetTextColor()
{
	return m_AttributeTextStyle.GetTextColor();
}

tagDuiTextStyle CDUIIPAddressCtrl::GetTextStyle()
{
	return m_AttributeTextStyle.GetTextStyle();
}

void CDUIIPAddressCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyle.SetTextStyle(TextStyle);

	Invalidate();

	RefreshEditTextStyle();

	return;
}

bool CDUIIPAddressCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (GetChildCount() >= COUNT_IPVALUE || NULL == MMInterfaceHelper(CDUIThinkEditCtrl, pChild)) return false;

	return __super::InsertChild(pChild, nPos);
}

CDUIThinkEditCtrl * CDUIIPAddressCtrl::GetChildAt(int nIndex) const
{
	return MMInterfaceHelper(CDUIThinkEditCtrl, __super::GetChildAt(nIndex));
}

DWORD CDUIIPAddressCtrl::GetIPAddrValue()
{
	return CMMService::StringToIPAddr(GetIPAddress());
}

void CDUIIPAddressCtrl::SetIPAddrValue(DWORD dwIPAddr)
{
#ifndef DUI_DESIGN
	if (dwIPAddr == GetIPAddrValue()) return;
#endif

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIThinkEditCtrl *pEditCtrl = GetChildAt(n);
		if (NULL == pEditCtrl) continue;

		pEditCtrl->SetText(CMMStrHelp::FormatW(_T("%d"), *((LPBYTE)&dwIPAddr + n)).c_str());
	}

	Invalidate();

	return;
}

CMMString CDUIIPAddressCtrl::GetIPAddress()
{
	return m_AttributeIP.GetValue();
}

void CDUIIPAddressCtrl::SetIPAddress(CMMString strIPAddr)
{
#ifndef DUI_DESIGN
	if (strIPAddr == GetIPAddress()) return;
#endif

	return SetIPAddrValue(CMMService::StringToIPAddr(strIPAddr));
}

void CDUIIPAddressCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	RefreshIPDotWidth();
	RefreshEditTextStyle();

	return;
}

void CDUIIPAddressCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupIP, _T("IP"));
	DuiCreateAttribute(m_AttributeIP, _T("IP"), _T(""), m_AttributeGroupIP);
	DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupIP);
	DuiCreateAttribute(m_AttributeTextStyleIPDot, _T("TextStyleIPDot"), _T(""), m_AttributeGroupIP);

	return;
}

void CDUIIPAddressCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupAnimation, false);
	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);
	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeGroupChild, false);

	if (GetChildCount() <= 0)
	{
		for (int n = 0; n < COUNT_IPVALUE; n++)
		{
			CDUIThinkEditCtrl *pEditCtrl = new CDUIThinkEditCtrl();
			pEditCtrl->Init();
			pEditCtrl->SetEditTextType(EditText_NumberInt);
			pEditCtrl->SetNumberMinLimit(0);
			pEditCtrl->SetNumberMaxLimit(255);

			InsertChild(pEditCtrl);
		}
	}

	EnableScrollBar(false, false);

	return;
}

void CDUIIPAddressCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pWndOwner) return;

	for (int n = 0; n < m_vecIPDot.size(); n++)
	{
		CDUIRect &rcDot = m_vecIPDot[n];
		m_AttributeTextStyleIPDot.Draw(hDC, rcDot, TEXT_IPDOT, m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), false);
	}

	return;
}

void CDUIIPAddressCtrl::RefreshEditTextStyle()
{
	m_AttributeTextStyle.SetVAlignTop(true);

	CDUISize szMeasure = m_AttributeTextStyle.MeasureString(_T("255"));
	if (szMeasure.cy <= 0) return;

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIThinkEditCtrl *pEditCtrl = GetChildAt(n);
		if (NULL == pEditCtrl) continue;

		pEditCtrl->SetTextStyle(m_AttributeTextStyle.GetTextStyle());
		pEditCtrl->SetTextStyleNormal(m_AttributeTextStyle.GetTextStyle());
		pEditCtrl->SetTextStyleHot(m_AttributeTextStyle.GetTextStyle());
		pEditCtrl->SetTextStylePushed(m_AttributeTextStyle.GetTextStyle());
		pEditCtrl->SetTextStyleFocus(m_AttributeTextStyle.GetTextStyle());
		pEditCtrl->SetVertAlignType(VertAlign_Middle);
		pEditCtrl->SetFixedHeight(szMeasure.cy);
	}

	return;
}

void CDUIIPAddressCtrl::RefreshIPDotWidth()
{
	CDUISize szMeasure = m_AttributeTextStyleIPDot.MeasureString(TEXT_IPDOT);
	SetChildPadding(szMeasure.cx, GetChildPaddingV());

	return;
}