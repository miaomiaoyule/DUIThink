#ifndef __DUIBUTTONCTRL_H__
#define __DUIBUTTONCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIButtonCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIButtonCtrl = { 0xBC562FB6,0x4F45,0x4DA9,0xAA,0xCD,0x1C,0xE1,0xFD,0x44,0xDB,0x43 };
class DUITHINK_API CDUIButtonCtrl
	: public CDUIStaticCtrl
{
	DuiDeclare_CreateControl(CDUIButtonCtrl)
	MMDeclare_ClassName(CDUIButtonCtrl)

public:
	CDUIButtonCtrl(void);
	~CDUIButtonCtrl(void);

	//attribute
protected:
	//status
	CDUIAttributeGroup					m_AttributeGroupStatus;
	CDUIAttributeColorSwitch			m_AttributeColorNormal;
	CDUIAttributeColorSwitch			m_AttributeColorHot;
	CDUIAttributeColorSwitch			m_AttributeColorPushed;
	CDUIAttributeColorSwitch			m_AttributeColorFocus;
	CDUIAttributeColorSwitch			m_AttributeColorDisabled;
	CDUIAttriImageSection				m_AttributeImageNormal;
	CDUIAttriImageSection				m_AttributeImageHot;
	CDUIAttriImageSection				m_AttributeImagePushed;
	CDUIAttriImageSection				m_AttributeImageFocus;
	CDUIAttriImageSection				m_AttributeImageDisabled;
	CDUIAttributeTextStyle				m_AttributeTextStyleNormal;
	CDUIAttributeTextStyle				m_AttributeTextStyleHot;
	CDUIAttributeTextStyle				m_AttributeTextStylePushed;
	CDUIAttributeTextStyle				m_AttributeTextStyleFocus;
	CDUIAttributeTextStyle				m_AttributeTextStyleDisabled;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//layout 
	CDUISize MeasureString(LPCTSTR lpszText = _T("")) override;

	virtual HFONT GetFontNormal();
	virtual HFONT GetFontHot();
	virtual HFONT GetFontPushed();
	virtual HFONT GetFontFocus();
	virtual HFONT GetFontDisabled();
	virtual ARGB GetTextColorNormal();
	virtual ARGB GetTextColorHot();
	virtual ARGB GetTextColorPushed();
	virtual ARGB GetTextColorFocus();
	virtual ARGB GetTextColorDisabled();
	virtual tagDuiTextStyle GetTextStyle() override;
	virtual tagDuiTextStyle GetTextStyleNormal();
	virtual tagDuiTextStyle GetTextStyleHot();
	virtual tagDuiTextStyle GetTextStylePushed();
	virtual tagDuiTextStyle GetTextStyleFocus();
	virtual tagDuiTextStyle GetTextStyleDisabled();
	virtual tagDuiTextStyle GetTextStyleActive() override;
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle) override;
	virtual void SetTextStyleNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleHot(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStylePushed(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleFocus(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleDisabled(const tagDuiTextStyle &TextStyle);

	virtual vector<CMMString> GetColorResSwitchNormal();
	virtual vector<CMMString> GetColorResSwitchHot();
	virtual vector<CMMString> GetColorResSwitchPushed();
	virtual vector<CMMString> GetColorResSwitchFocus();
	virtual vector<CMMString> GetColorResSwitchDisabled();
	virtual void SetColorResSwitchNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchHot(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchPushed(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchFocus(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchDisabled(const vector<CMMString> &vecResSwitch);

	virtual tagDuiImageSection GetImageSectionNormal();
	virtual tagDuiImageSection GetImageSectionHot();
	virtual tagDuiImageSection GetImageSectionPushed();
	virtual tagDuiImageSection GetImageSectionFocus();
	virtual tagDuiImageSection GetImageSectionDisabled();
	virtual void SetImageSectionNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionPushed(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionFocus(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionDisabled(const tagDuiImageSection &ImageSection);

	//message
protected:
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//proprety
protected:
	void InitProperty() override;

	//paint
protected:
	void PaintStatusColor(HDC hDC) override;
	void PaintStatusImage(HDC hDC) override;
	void PaintText(HDC hDC) override;

	//help
protected:
	CDUIAttributeTextStyle * GetAttributeTextStyleActive() override;
	CDUIAttributeColorSwitch * GetAttributeStatusColor() override;
	CDUIAttriImageSection * GetAttributeStatusImage() override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif