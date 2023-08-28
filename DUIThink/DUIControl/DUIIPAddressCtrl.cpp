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
	if (pAttributeObj == &m_AttributeTextStyle)
	{
		RefreshEditTextStyle();
	}

	if (__super::OnAttributeChange(pAttributeObj)) return true;

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
		CDUIRect rcDot = { rcEdit.right + 2, rcEdit.top, rcNext.left, rcEdit.bottom };
		m_vecIPDot.push_back(rcDot);
	}

	return;
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
	return false;
}

CDUIThinkEditCtrl * CDUIIPAddressCtrl::GetChildAt(int nIndex) const
{
	return MMInterfaceHelper(CDUIThinkEditCtrl, __super::GetChildAt(nIndex));
}

DWORD CDUIIPAddressCtrl::GetIPAddrValue()
{
	return m_dwIPAddr;
}

void CDUIIPAddressCtrl::SetIPAddrValue(DWORD dwIPAddr)
{
	if (dwIPAddr == GetIPAddrValue()) return;

	m_dwIPAddr = dwIPAddr;

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIThinkEditCtrl *pEditCtrl = GetChildAt(n);
		if (NULL == pEditCtrl) continue;

		pEditCtrl->SetText(CMMStrHelp::FormatW(_T("%d"), (LPBYTE)&m_dwIPAddr + n).c_str());
	}

	Invalidate();

	return;
}

CMMString CDUIIPAddressCtrl::GetIPAddress()
{
	return CMMService::IPAddrToString(m_dwIPAddr);
}

void CDUIIPAddressCtrl::SetIPAddress(CMMString strIPAddr)
{
	if (strIPAddr == GetIPAddress()) return;

	return SetIPAddrValue(CMMService::StringToIPAddr(strIPAddr));
}

void CDUIIPAddressCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	RefreshEditTextStyle();

	return;
}

void CDUIIPAddressCtrl::InitProperty()
{
	//ip edit
	if (GetChildCount() <= 0)
	{
		for (int n = 0; n < COUNT_IPVALUE; n++)
		{
			__super::InsertChild(new CDUIThinkEditCtrl());
		}
	}

	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupText, _T("Text"));
	DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextStyleIPDot, _T("TextStyleIPDot"), _T(""), m_AttributeGroupText);

	return;
}

void CDUIIPAddressCtrl::InitAttriValue()
{
	return __super::InitAttriValue();
}

void CDUIIPAddressCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupAnimation, false);
	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);
	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeGroupChild, false);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIThinkEditCtrl *pEditCtrl = GetChildAt(n);
		if (NULL == pEditCtrl) continue;

		pEditCtrl->SetEditTextType(EditText_NumberInt);
		pEditCtrl->SetNumberMinLimit(0);
		pEditCtrl->SetNumberMaxLimit(255);
	}

	EnableScrollBar(false, false);

	return;
}

void CDUIIPAddressCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pWndManager) return;

	for (int n = 0; n < m_vecIPDot.size(); n++)
	{
		CDUIRect &rcDot = m_vecIPDot[n];
		m_AttributeTextStyleIPDot.Draw(hDC, rcDot, TEXT_IPDOT, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), false);
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
		pEditCtrl->SetVertAlignType(VertAlign_Bottom);
		pEditCtrl->SetPaddingB(szMeasure.cy);
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