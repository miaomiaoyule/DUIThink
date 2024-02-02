#ifndef __DUICHECKBOXCTRL_H__
#define __DUICHECKBOXCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUICheckBoxCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUICheckBoxCtrl = { 0x9E913B03,0x3F46,0x497E,0x89,0xB6,0x4A,0x60,0x95,0x60,0x9E,0x7C };
class DUITHINK_API CDUICheckBoxCtrl
	: public CDUIButtonCtrl
{
	DuiDeclare_CreateControl(CDUICheckBoxCtrl)
	MMDeclare_ClassName(CDUICheckBoxCtrl)

public:
	CDUICheckBoxCtrl(void);
	~CDUICheckBoxCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupSelStatus;
	CDUIAttributeColorSwitch			m_AttributeColorSelNormal;
	CDUIAttributeColorSwitch			m_AttributeColorSelHot;
	CDUIAttributeColorSwitch			m_AttributeColorSelPushed;
	CDUIAttributeColorSwitch			m_AttributeColorSelFocus;
	CDUIAttributeColorSwitch			m_AttributeColorSelDisabled;
	CDUIAttriImageSection				m_AttributeImageSelNormal;
	CDUIAttriImageSection				m_AttributeImageSelHot;
	CDUIAttriImageSection				m_AttributeImageSelPushed;
	CDUIAttriImageSection				m_AttributeImageSelFocus;
	CDUIAttriImageSection				m_AttributeImageSelDisabled;
	CDUIAttributeTextStyle				m_AttributeTextStyleSelNormal;
	CDUIAttributeTextStyle				m_AttributeTextStyleSelHot;
	CDUIAttributeTextStyle				m_AttributeTextStyleSelPushed;
	CDUIAttributeTextStyle				m_AttributeTextStyleSelFocus;
	CDUIAttributeTextStyle				m_AttributeTextStyleSelDisabled;

	CDUIAttributeGroup					m_AttributeGroupSelect;
	CDUIAttributeBool					m_AttributeIsSelected;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//override
	bool Active() override;

	//select
	virtual bool IsSelected();
	virtual void Select(bool bSelect = true, bool bNotify = true);
	virtual tagDuiImageSection GetImageSectionSelNormal();
	virtual tagDuiImageSection GetImageSectionSelHot();
	virtual tagDuiImageSection GetImageSectionSelPushed();
	virtual tagDuiImageSection GetImageSectionSelFocus();
	virtual tagDuiImageSection GetImageSectionSelDisabled();
	virtual void SetImageSectionSelNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelPushed(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelFocus(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelDisabled(const tagDuiImageSection &ImageSection);
	virtual tagDuiTextStyle GetTextStyleSelNormal();
	virtual tagDuiTextStyle GetTextStyleSelHot();
	virtual tagDuiTextStyle GetTextStyleSelPushed();
	virtual tagDuiTextStyle GetTextStyleSelFocus();
	virtual tagDuiTextStyle GetTextStyleSelDisabled();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle) override;
	virtual void SetTextStyleSelNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleSelHot(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleSelPushed(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleSelFocus(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleSelDisabled(const tagDuiTextStyle &TextStyle);

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitNormalSkin() override;

	//paint
protected:
	void PaintStatusColor(HDC hDC) override;
	void PaintStatusImage(HDC hDC) override;
	void PaintText(HDC hDC) override;
	virtual void PaintStatusColorSelected(HDC hDC);
	virtual void PaintStatusImageSelected(HDC hDC);
	virtual void PaintTextSelected(HDC hDC);

	//message
protected:
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif