#include "StdAfx.h"
#include "DUIButtonCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIButtonCtrl)
MMImplement_ClassName(CDUIButtonCtrl)

CDUIButtonCtrl::CDUIButtonCtrl(void)
{
}

CDUIButtonCtrl::~CDUIButtonCtrl(void)
{
}

LPVOID CDUIButtonCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIButtonCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIButtonCtrl::GetDescribe() const
{
	return Dui_Ctrl_Button;
}

CDUISize CDUIButtonCtrl::MeasureString(LPCTSTR lpszText)
{
	if (m_AttributeTextStyleNormal.IsEmpty()) return __super::MeasureString(lpszText);

	return m_AttributeTextStyleNormal.MeasureString(MMInvalidString(lpszText) ? GetText() : lpszText);
}

HFONT CDUIButtonCtrl::GetFontNormal()
{
	return m_AttributeTextStyleNormal.GetFont();
}

HFONT CDUIButtonCtrl::GetFontHot()
{
	return m_AttributeTextStyleHot.GetFont();
}

HFONT CDUIButtonCtrl::GetFontPushed()
{
	return m_AttributeTextStylePushed.GetFont();
}

HFONT CDUIButtonCtrl::GetFontFocus()
{
	return m_AttributeTextStyleFocus.GetFont();
}

HFONT CDUIButtonCtrl::GetFontDisabled()
{
	return m_AttributeTextStyleDisabled.GetFont();
}

ARGB CDUIButtonCtrl::GetTextColorNormal()
{
	return m_AttributeTextStyleNormal.GetTextColor();
}

ARGB CDUIButtonCtrl::GetTextColorHot()
{
	return m_AttributeTextStyleHot.GetTextColor();
}

ARGB CDUIButtonCtrl::GetTextColorPushed()
{
	return m_AttributeTextStylePushed.GetTextColor();
}

ARGB CDUIButtonCtrl::GetTextColorFocus()
{
	return m_AttributeTextStyleFocus.GetTextColor();
}

ARGB CDUIButtonCtrl::GetTextColorDisabled()
{
	return m_AttributeTextStyleDisabled.GetTextColor();
}

tagDuiTextStyle CDUIButtonCtrl::GetTextStyle()
{
	if (m_AttributeTextStyle.IsEmpty()) return GetTextStyleNormal();

	return __super::GetTextStyle();
}

tagDuiTextStyle CDUIButtonCtrl::GetTextStyleNormal()
{
	return m_AttributeTextStyleNormal.IsEmpty() ? __super::GetTextStyle() : m_AttributeTextStyleNormal.GetTextStyle();
}

tagDuiTextStyle CDUIButtonCtrl::GetTextStyleHot()
{
	return m_AttributeTextStyleHot.GetTextStyle();
}

tagDuiTextStyle CDUIButtonCtrl::GetTextStylePushed()
{
	return m_AttributeTextStylePushed.GetTextStyle();
}

tagDuiTextStyle CDUIButtonCtrl::GetTextStyleFocus()
{
	return m_AttributeTextStyleFocus.GetTextStyle();
}

tagDuiTextStyle CDUIButtonCtrl::GetTextStyleDisabled()
{
	return m_AttributeTextStyleDisabled.GetTextStyle();
}

tagDuiTextStyle CDUIButtonCtrl::GetTextStyleActive()
{
	CDUIAttributeTextStyle *pAttribute = GetAttributeTextStyleActive();

	return pAttribute ? pAttribute->GetTextStyle() : tagDuiTextStyle();
}

void CDUIButtonCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyle(TextStyle);

	SetTextStyleNormal(TextStyle);
	SetTextStyleHot(TextStyle);
	SetTextStylePushed(TextStyle);
	SetTextStyleFocus(TextStyle);
	SetTextStyleDisabled(TextStyle);

	return;
}

void CDUIButtonCtrl::SetTextStyleNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleNormal.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetTextStyleHot(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleHot.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetTextStylePushed(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStylePushed.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetTextStyleFocus(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleFocus.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetTextStyleDisabled(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleDisabled.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

vector<CMMString> CDUIButtonCtrl::GetColorResSwitchNormal()
{
	return m_AttributeColorNormal.GetColorResSwitch();
}

vector<CMMString> CDUIButtonCtrl::GetColorResSwitchHot()
{
	return m_AttributeColorHot.GetColorResSwitch();
}

vector<CMMString> CDUIButtonCtrl::GetColorResSwitchPushed()
{
	return m_AttributeColorPushed.GetColorResSwitch();
}

vector<CMMString> CDUIButtonCtrl::GetColorResSwitchFocus()
{
	return m_AttributeColorFocus.GetColorResSwitch();
}

vector<CMMString> CDUIButtonCtrl::GetColorResSwitchDisabled()
{
	return m_AttributeColorDisabled.GetColorResSwitch();
}

void CDUIButtonCtrl::SetColorResSwitchNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetColorResSwitchHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetColorResSwitchPushed(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorPushed.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetColorResSwitchFocus(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorFocus.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetColorResSwitchDisabled(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorDisabled.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

tagDuiImageSection CDUIButtonCtrl::GetImageSectionNormal()
{
	return m_AttributeImageNormal.GetImageSection();
}

tagDuiImageSection CDUIButtonCtrl::GetImageSectionHot()
{
	return m_AttributeImageHot.GetImageSection();
}

tagDuiImageSection CDUIButtonCtrl::GetImageSectionPushed()
{
	return m_AttributeImagePushed.GetImageSection();
}

tagDuiImageSection CDUIButtonCtrl::GetImageSectionFocus()
{
	return m_AttributeImageFocus.GetImageSection();
}

tagDuiImageSection CDUIButtonCtrl::GetImageSectionDisabled()
{
	return m_AttributeImageDisabled.GetImageSection();
}

void CDUIButtonCtrl::SetImageSectionNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetImageSectionHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetImageSectionPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImagePushed.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetImageSectionFocus(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageFocus.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIButtonCtrl::SetImageSectionDisabled(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDisabled.SetImageSection(ImageSection);

	Invalidate();

	return;
}

bool CDUIButtonCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_nControlStatus & ControlStatus_Pushed);

	__super::OnDuiLButtonUp(pt, Msg);

	if (m_pWndOwner && bClick)
	{
		m_pWndOwner->SendNotify(this, DuiNotify_Click, Msg.wParam, Msg.lParam);
	}

	return true;
}

bool CDUIButtonCtrl::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_nControlStatus & ControlStatus_Pushed);

	if (false == __super::OnDuiRButtonUp(pt, Msg)) return false;

	if (m_pWndOwner && bClick)
	{
		m_pWndOwner->SendNotify(this, DuiNotify_RClick, Msg.wParam, Msg.lParam);
	}

	return true;
}

bool CDUIButtonCtrl::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseEnter(pt, Msg);

	Invalidate();

	return true;
}

void CDUIButtonCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseLeave(pt, Msg);

	Invalidate();

	return;
}

void CDUIButtonCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupStatus, _T("Status"));
	DuiCreateAttribute(m_AttributeColorNormal, _T("ColorNormal"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeColorHot, _T("ColorHot"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeColorPushed, _T("ColorPushed"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeColorFocus, _T("ColorFocus"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeColorDisabled, _T("ColorDisabled"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImageNormal, _T("ImageNormal"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImageHot, _T("ImageHot"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImagePushed, _T("ImagePushed"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImageFocus, _T("ImageFocus"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeImageDisabled, _T("ImageDisabled"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeTextStyleNormal, _T("TextStyleNormal"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeTextStyleHot, _T("TextStyleHot"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeTextStylePushed, _T("TextStylePushed"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeTextStyleFocus, _T("TextStyleFocus"), _T(""), m_AttributeGroupStatus);
	DuiCreateAttribute(m_AttributeTextStyleDisabled, _T("TextStyleDisabled"), _T(""), m_AttributeGroupStatus);

	return;
}

void CDUIButtonCtrl::PaintStatusColor(HDC hDC)
{
	__super::PaintStatusColor(hDC);

	CDUIAttributeColorSwitch *pAttribute = GetAttributeStatusColor();

	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& false == m_AttributeColorNormal.IsEmpty())
	{
		pAttribute = &m_AttributeColorNormal;
	}
	if (NULL == pAttribute)
	{
		return;
	}

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

void CDUIButtonCtrl::PaintStatusImage(HDC hDC)
{
	__super::PaintStatusImage(hDC);

	CDUIAttriImageSection *pAttribute = GetAttributeStatusImage();

	NULL == pAttribute || pAttribute->IsEmpty() ? pAttribute = &m_AttributeImageNormal : pAttribute;
	if (NULL == pAttribute) return;

	pAttribute->Draw(hDC, m_rcAbsolute, m_rcPaint, false == IsEnabled() && pAttribute != &m_AttributeImageDisabled);

	return;
}

void CDUIButtonCtrl::PaintText(HDC hDC)
{
	if (GetText().empty() || NULL == m_pWndOwner)
	{
		return __super::PaintText(hDC);
	}

	CDUIRect rcRange = GetTextRange();

	//textstyle
	CDUIAttributeTextStyle *pAttribute = NULL;
	if (false == IsEnabled())
	{
		pAttribute = &m_AttributeTextStyleDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeTextStylePushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeTextStyleHot;
	}
	else if (IsFocused())
	{
		pAttribute = &m_AttributeTextStyleFocus;
	}
	else
	{
		pAttribute = &m_AttributeTextStyleNormal;
	}

	NULL == pAttribute || pAttribute->IsEmpty() ? pAttribute = &m_AttributeTextStyleNormal : pAttribute;
	NULL == pAttribute || pAttribute->IsEmpty() ? pAttribute = &m_AttributeTextStyle : pAttribute;
	if (NULL == pAttribute) return;

	pAttribute->Draw(hDC, rcRange, GetText(), m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), IsShadowText());

	return;
}

CDUIAttributeTextStyle * CDUIButtonCtrl::GetAttributeTextStyleActive()
{
	CDUIAttributeTextStyle *pAttribute = &m_AttributeTextStyleFocus;
	NULL == pAttribute || pAttribute->IsEmpty() ? pAttribute = &m_AttributeTextStyleNormal : pAttribute;
	NULL == pAttribute || pAttribute->IsEmpty() ? pAttribute = &m_AttributeTextStyle : pAttribute;

	return pAttribute;
}

CDUIAttributeColorSwitch * CDUIButtonCtrl::GetAttributeStatusColor()
{
	if (false == IsEnabled())
	{
		return &m_AttributeColorDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		return &m_AttributeColorPushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		return &m_AttributeColorHot;
	}
	else if (IsFocused())
	{
		return &m_AttributeColorFocus;
	}
	else
	{
		return &m_AttributeColorNormal;
	}

	return NULL;
}

CDUIAttriImageSection * CDUIButtonCtrl::GetAttributeStatusImage()
{
	if (false == IsEnabled())
	{
		return &m_AttributeImageDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		return &m_AttributeImagePushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		return &m_AttributeImageHot;
	}
	else if (IsFocused())
	{
		return &m_AttributeImageFocus;
	}
	else
	{
		return &m_AttributeImageNormal;
	}

	return NULL;
}