#include "StdAfx.h"
#include "DUIGroupCtrl.h"

DuiImplement_CreateControl(CDUIGroupCtrl)
MMImplement_ClassName(CDUIGroupCtrl)

CDUIGroupCtrl::CDUIGroupCtrl(void)
{
	m_pHorizScrollBarCtrl = NULL;
	m_pVertScrollBarCtrl = NULL;

	return;
}

CDUIGroupCtrl::~CDUIGroupCtrl(void)
{
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		MMSafeDelete(pChild);
	}

	m_vecChilds.clear();
	MMSafeDelete(m_pHorizScrollBarCtrl);
	MMSafeDelete(m_pVertScrollBarCtrl);

	return;
}

LPVOID CDUIGroupCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIContainerCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIGroupCtrl::GetDescribe() const
{
	return Dui_Ctrl_Group;
}

void CDUIGroupCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupText, _T("Text"));
	DuiCreateAttribute(m_AttributeText, _T("Text"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttribuetShadow, _T("Shadow"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupText);

	return;
}

void CDUIGroupCtrl::RefreshView()
{
	__super::RefreshView();

	//measure
	m_szText = MeasureString();
	m_szText.cx = min(GetMaxWidth(), max(m_szText.cx, GetMinWidth()));
	m_szText.cy = min(GetMaxHeight(), max(m_szText.cy, GetMinHeight()));

	return;
}

CDUISize CDUIGroupCtrl::MeasureString(LPCTSTR lpszText)
{
	return m_AttributeTextStyle.MeasureString(MMInvalidString(lpszText) ? GetText() : lpszText);
}

CMMString CDUIGroupCtrl::GetText()
{
	return m_AttributeText.GetValue();
}

bool CDUIGroupCtrl::SetText(LPCTSTR lpszText)
{
	if (lpszText == GetText()) return true;

	m_AttributeText.SetValue(lpszText);

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_TextChanged);
	}

	Invalidate();

	return true;
}

bool CDUIGroupCtrl::IsShadowText()
{
	return m_AttribuetShadow.GetValue();
}

void CDUIGroupCtrl::SetShadowText(bool bShadow)
{
	if (bShadow == IsShadowText()) return;

	m_AttribuetShadow.SetValue(bShadow);

	Invalidate();

	return;
}

HFONT CDUIGroupCtrl::GetFont()
{
	return m_AttributeTextStyle.GetFont();
}

ARGB CDUIGroupCtrl::GetTextColor()
{
	return m_AttributeTextStyle.GetTextColor();
}

tagDuiTextStyle CDUIGroupCtrl::GetTextStyle()
{
	return m_AttributeTextStyle.GetTextStyle();
}

void CDUIGroupCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyle.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIGroupCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pWndManager) return;

	//range
	CDUIRect rcRange = GetAbsoluteRect();
	rcRange.left += GetBorderLine().left;
	rcRange.left += GetRoundSize().cx;
	rcRange.left += Dui_Offset_GroupBorderTopBreak;
	rcRange.right = rcRange.left + m_szText.cx;
	rcRange.bottom = rcRange.top + m_szText.cy;

	//text
	CMMString strText = GetText();
	if (strText.empty()) return;

	m_AttributeTextStyle.Draw(hDC, rcRange, strText, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), IsShadowText());

	return;
}

CDUIRect CDUIGroupCtrl::GetBorderRect()
{
	CDUIRect rcBorder = __super::GetBorderRect();
	rcBorder.top += m_szText.cy / 2;

	return rcBorder;
}

CDUISize CDUIGroupCtrl::GetBorderBreakTop()
{
	return { Dui_Offset_GroupBorderTopBreak, m_szText.cx };
}