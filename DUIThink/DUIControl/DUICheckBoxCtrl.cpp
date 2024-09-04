#include "StdAfx.h"
#include "DUICheckBoxCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUICheckBoxCtrl)
MMImplement_ClassName(CDUICheckBoxCtrl)

CDUICheckBoxCtrl::CDUICheckBoxCtrl(void)
{
}

CDUICheckBoxCtrl::~CDUICheckBoxCtrl(void)
{
}

bool CDUICheckBoxCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeIsSelected)
	{
		Select(IsSelected());

		return true;
	}

	return false;
}

LPVOID CDUICheckBoxCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUICheckBoxCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUICheckBoxCtrl::GetDescribe() const
{
	return Dui_Ctrl_CheckBox;
}

bool CDUICheckBoxCtrl::Active()
{
	if (false == __super::Active()) return false;

	Select(!IsSelected());

	return true;
}

bool CDUICheckBoxCtrl::IsSelected()
{
	return m_AttributeIsSelected.GetValue();
}

void CDUICheckBoxCtrl::Select(bool bSelect, bool bNotify)
{
#ifndef DUI_DESIGN
	if (bSelect == IsSelected()) return;
#endif

	m_AttributeIsSelected.SetValue(bSelect);

	if (m_pWndOwner)
	{
		if (bNotify)
		{
			m_pWndOwner->SendNotify(this, DuiNotify_SelectChanged);
		}
	}

	Invalidate();

	return;
}

tagDuiImageSection CDUICheckBoxCtrl::GetImageSectionSelNormal()
{
	return m_AttributeImageSelNormal.GetImageSection();
}

tagDuiImageSection CDUICheckBoxCtrl::GetImageSectionSelHot()
{
	return m_AttributeImageSelHot.GetImageSection();
}

tagDuiImageSection CDUICheckBoxCtrl::GetImageSectionSelPushed()
{
	return m_AttributeImageSelPushed.GetImageSection();
}

tagDuiImageSection CDUICheckBoxCtrl::GetImageSectionSelFocus()
{
	return m_AttributeImageSelFocus.GetImageSection();
}

tagDuiImageSection CDUICheckBoxCtrl::GetImageSectionSelDisabled()
{
	return m_AttributeImageSelDisabled.GetImageSection();
}

void CDUICheckBoxCtrl::SetImageSectionSelNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetImageSectionSelHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetImageSectionSelPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelPushed.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetImageSectionSelFocus(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelFocus.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetImageSectionSelDisabled(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelDisabled.SetImageSection(ImageSection);

	Invalidate();

	return;
}

tagDuiTextStyle CDUICheckBoxCtrl::GetTextStyleSelNormal()
{
	return m_AttributeTextStyleSelNormal.GetTextStyle();
}

tagDuiTextStyle CDUICheckBoxCtrl::GetTextStyleSelHot()
{
	return m_AttributeTextStyleSelHot.GetTextStyle();
}

tagDuiTextStyle CDUICheckBoxCtrl::GetTextStyleSelPushed()
{
	return m_AttributeTextStyleSelPushed.GetTextStyle();
}

tagDuiTextStyle CDUICheckBoxCtrl::GetTextStyleSelFocus()
{
	return m_AttributeTextStyleSelFocus.GetTextStyle();
}

tagDuiTextStyle CDUICheckBoxCtrl::GetTextStyleSelDisabled()
{
	return m_AttributeTextStyleSelDisabled.GetTextStyle();
}

void CDUICheckBoxCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyle(TextStyle);

	SetTextStyleSelNormal(TextStyle);
	SetTextStyleSelHot(TextStyle);
	SetTextStyleSelPushed(TextStyle);
	SetTextStyleSelFocus(TextStyle);
	SetTextStyleSelDisabled(TextStyle);

	return;
}

void CDUICheckBoxCtrl::SetTextStyleSelNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleSelNormal.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetTextStyleSelHot(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleSelHot.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetTextStyleSelPushed(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleSelPushed.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetTextStyleSelFocus(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleSelFocus.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::SetTextStyleSelDisabled(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleSelDisabled.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUICheckBoxCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupSelStatus, _T("SelectedStatus"));
	DuiCreateAttribute(m_AttributeColorSelNormal, _T("ColorSelNormal"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeColorSelHot, _T("ColorSelHot"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeColorSelPushed, _T("ColorSelPushed"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeColorSelFocus, _T("ColorSelFocus"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeColorSelDisabled, _T("ColorSelDisabled"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeImageSelNormal, _T("ImageSelNormal"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeImageSelHot, _T("ImageSelHot"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeImageSelPushed, _T("ImageSelPushed"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeImageSelFocus, _T("ImageSelFocus"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeImageSelDisabled, _T("ImageSelDisabled"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeTextStyleSelNormal, _T("TextStyleSelNormal"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeTextStyleSelHot, _T("TextStyleSelHot"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeTextStyleSelPushed, _T("TextStyleSelPushed"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeTextStyleSelFocus, _T("TextStyleSelFocus"), _T(""), m_AttributeGroupSelStatus);
	DuiCreateAttribute(m_AttributeTextStyleSelDisabled, _T("TextStyleSelDisabled"), _T(""), m_AttributeGroupSelStatus);

	DuiCreateGroupAttribute(m_AttributeGroupSelect, _T("Select"));
	DuiCreateAttribute(m_AttributeIsSelected, _T("IsSelected"), _T(""), m_AttributeGroupSelect);

	return;
}

void CDUICheckBoxCtrl::InitNormalSkin()
{
	auto pImageBaseCheckBoxUnSelect = CDUIGlobal::GetInstance()->GetImageResource(Name_ImageCheckBoxUnSelect);
	auto pImageBaseCheckBoxSelect = CDUIGlobal::GetInstance()->GetImageResource(Name_ImageCheckBoxSelect);

	tagDuiImageSection ImageSection;
	ImageSection.ImageSourceType = ImageSource_Part;
	ImageSection.vecImageResSwitch.push_back(pImageBaseCheckBoxUnSelect->GetResourceName());
	ImageSection.HorizImageAlign = HorizImageAlign_Left;
	ImageSection.VertImageAlign = VertImageAlign_Center;
	ImageSection.cbPartAll = 4;
	ImageSection.cbPartSel = 1;
	SetImageSectionNormal(ImageSection);

	ImageSection.cbPartSel = 2;
	SetImageSectionHot(ImageSection);

	ImageSection.cbPartSel = 3;
	SetImageSectionPushed(ImageSection);

	ImageSection.cbPartSel = 4;
	SetImageSectionDisabled(ImageSection);

	ImageSection.vecImageResSwitch.clear();
	ImageSection.vecImageResSwitch.push_back(pImageBaseCheckBoxSelect->GetResourceName());
	ImageSection.cbPartSel = 1;
	SetImageSectionSelNormal(ImageSection);

	ImageSection.cbPartSel = 2;
	SetImageSectionSelHot(ImageSection);

	ImageSection.cbPartSel = 3;
	SetImageSectionSelPushed(ImageSection);

	ImageSection.cbPartSel = 4;
	SetImageSectionSelDisabled(ImageSection);

	CMMString strText = _T("¸´Ñ¡¿ò");
	tagDuiTextStyle TextStyle;
	TextStyle.dwTextStyle |= DT_VCENTER;
	SetText(strText);
	SetTextPadding({ pImageBaseCheckBoxUnSelect->GetWidth() / 4, 0, 0, 0 });
	CDUIStaticCtrl::SetTextStyle(TextStyle);

	return;
}

void CDUICheckBoxCtrl::PaintStatusColor(HDC hDC)
{
	if (IsSelected())
	{
		PaintStatusColorSelected(hDC);
	}
	else
	{
		__super::PaintStatusColor(hDC);
	}

	return;
}

void CDUICheckBoxCtrl::PaintStatusImage(HDC hDC)
{
	if (IsSelected())
	{
		PaintStatusImageSelected(hDC);
	}
	else
	{
		__super::PaintStatusImage(hDC);
	}

	return;
}

void CDUICheckBoxCtrl::PaintText(HDC hDC)
{
	if (IsSelected())
	{
		PaintTextSelected(hDC);
	}
	else
	{
		__super::PaintText(hDC);
	}

	return;
}

bool CDUICheckBoxCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_nControlStatus & ControlStatus_Pushed);

	__super::OnDuiLButtonUp(pt, Msg);

	if (bClick)
	{
		Select(!IsSelected());
	}

	Invalidate();

	return true;
}

void CDUICheckBoxCtrl::PaintStatusColorSelected(HDC hDC)
{
	CDUIAttributeColorSwitch *pAttribute = NULL;
	if (false == IsEnabled())
	{
		pAttribute = &m_AttributeColorSelDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeColorSelPushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeColorSelHot;
	}
	else if (IsFocused())
	{
		pAttribute = &m_AttributeColorSelFocus;
	}
	else
	{
		pAttribute = &m_AttributeColorSelNormal;
	}

	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeColorSelNormal : pAttribute;
	if (NULL == pAttribute) return;

	CDUIRect rcBorderRound = GetRoundCorner();
	if (rcBorderRound.left > 0 
		|| rcBorderRound.top > 0
		|| rcBorderRound.right > 0
		|| rcBorderRound.bottom > 0)
	{
		pAttribute->FillRoundRect(hDC, m_rcAbsolute, 0, rcBorderRound, IsColorHSL());

		return;
	}

	pAttribute->FillRect(hDC, m_rcAbsolute, IsColorHSL());

	return;
}

void CDUICheckBoxCtrl::PaintStatusImageSelected(HDC hDC)
{
	CDUIAttriImageSection *pAttribute = NULL;
	if (false == IsEnabled())
	{
		pAttribute = &m_AttributeImageSelDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeImageSelPushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeImageSelHot;
	}
	else if (IsFocused())
	{
		pAttribute = &m_AttributeImageSelFocus;
	}
	else
	{
		pAttribute = &m_AttributeImageSelNormal;
	}

	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeImageSelNormal : pAttribute;
	if (NULL == pAttribute) return;

	pAttribute->Draw(hDC, m_rcAbsolute, m_rcPaint);

	return;
}

void CDUICheckBoxCtrl::PaintTextSelected(HDC hDC)
{
	CMMString strText = GetText();
	if (strText.empty() || NULL == m_pWndOwner) return;

	CDUIRect rcRange = GetAbsoluteRect();
	CDUIRect rcTextPadding = GetTextPadding();

	rcRange.left += rcTextPadding.left;
	rcRange.right -= rcTextPadding.right;
	rcRange.top += rcTextPadding.top;
	rcRange.bottom -= rcTextPadding.bottom;
	rcRange.CheckRect();

	//textstyle
	CDUIAttributeTextStyle *pAttribute = NULL;
	if (false == IsEnabled())
	{
		pAttribute = &m_AttributeTextStyleSelDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeTextStyleSelPushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeTextStyleSelHot;
	}
	else if (IsFocused())
	{
		pAttribute = &m_AttributeTextStyleSelFocus;
	}
	else
	{
		pAttribute = &m_AttributeTextStyleSelNormal;
	}

	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyleSelNormal : pAttribute;
	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyle : pAttribute;
	if (NULL == pAttribute) return;

	pAttribute->Draw(hDC, rcRange, strText, m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), IsShadowText());

	return;
}