#ifndef __DUIGROUPCTRL_H__
#define __DUIGROUPCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIGroupCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIGroupCtrl = { 0x902AD2D4,0xBCE0,0x4337,0x8A,0x99,0xF3,0x90,0x4F,0xAA,0xCE,0xB2 }; 
class DUITHINK_API CDUIGroupCtrl
	: public CDUIContainerCtrl
{
	DuiDeclare_CreateControl(CDUIGroupCtrl)
	MMDeclare_ClassName(CDUIGroupCtrl)

public:
	CDUIGroupCtrl(void);
	~CDUIGroupCtrl(void);

	//attribute
protected:
	//group
	CDUIAttributeGroup					m_AttributeGroupText;
	CDUIAttributeBool					m_AttribuetShadow;
	CDUIAttributeText					m_AttributeText;
	CDUIAttributeTextStyle				m_AttributeTextStyle;

	//variant
protected:
	CDUISize							m_szText;

	//override
protected:

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void RefreshView() override;

	//layout
	virtual CDUISize MeasureString(LPCTSTR lpszText = _T(""));

	//text
	virtual CMMString GetText();
	virtual bool SetText(LPCTSTR lpszText);
	virtual bool IsShadowText() override;
	virtual void SetShadowText(bool bShadow);
	virtual HFONT GetFont();
	virtual ARGB GetTextColor();
	virtual tagDuiTextStyle GetTextStyle();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);

	//property
protected:
	void InitProperty() override;

	//message
protected:

	//paint
protected:
	void PaintText(HDC hDC) override;

	//help
protected:
	CDUIRect GetBorderRect() override;
	CDUISize GetBorderBreakTop() override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif