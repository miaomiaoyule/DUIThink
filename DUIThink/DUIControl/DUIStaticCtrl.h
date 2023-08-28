#ifndef __DUISTATICCTRL_H__
#define __DUISTATICCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIStaticCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIStaticCtrl = { 0x54F14AD6,0x882C,0x432C,0xB6,0x39,0xC5,0xA7,0x42,0x28,0xB4,0x3B };
class DUITHINK_API CDUIStaticCtrl
	: public CDUIControlBase
{
	friend class CDUIListViewCtrl;

	DuiDeclare_CreateControl(CDUIStaticCtrl)
	MMDeclare_ClassName(CDUIStaticCtrl)

public:
	CDUIStaticCtrl(void);
	~CDUIStaticCtrl(void);

	//attribute
protected:
	//position
	CDUIAttributeBool					m_AttributeAutoCalcWidth;
	CDUIAttributeBool					m_AttributeAutoCalcHeight;

	//text
	CDUIAttributeGroup					m_AttributeGroupText;
	CDUIAttributeBool					m_AttribuetShadow;
	CDUIAttributeText					m_AttributeText;
	CDUIAttributeTextStyle				m_AttributeTextStyle;
	CDUIAttributeRichText				m_AttributeRichText;
	CDUIAttributeLong					m_AttributeRichTextLineSpace;
	CDUIAttributeRect					m_AttributeTextPadding;

	//variant
protected:
	bool								m_bNeedMeasureSize = true;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//layout
	bool SetAbsoluteRect(CDUIRect &rcAbsolute) override; 
	void SetMinWidth(long lWidth) override;
	void SetMaxWidth(long lWidth) override;
	void SetMinHeight(long lHeight) override;
	void SetMaxHeight(long lHeight) override;

	//refresh
	void RefreshView() override;
	virtual void NeedMeasureSize();

	//layout
	virtual bool IsAutoCalcWidth();
	virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
	virtual bool IsAutoCalcHeight();
	virtual void SetAutoCalcHeight(bool bAutoCalcHeight);
	virtual CDUISize MeasureString(LPCTSTR lpszText = _T(""));

	//text
	virtual CMMString GetText();
	virtual bool SetText(LPCTSTR lpszText);
	virtual VecDuiRichTextItem GetRichTextItem();
	virtual bool SetRichTextItem(const VecDuiRichTextItem &vecRichTextItem);
	virtual bool IsShadowText() override;
	virtual void SetShadowText(bool bShadow);
	virtual HFONT GetFont();
	virtual ARGB GetTextColor();
	virtual tagDuiTextStyle GetTextStyle();
	virtual tagDuiTextStyle GetTextStyleActive();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);
	virtual int GetRichTextLineSpace();
	virtual void SetRichTextLineSpace(int nLineSpace);
	virtual RECT GetTextPadding();
	virtual void SetTextPadding(RECT rcPadding);

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;

	//paint
protected:
	void PaintText(HDC hDC) override;

	//message
protected:
	void OnDuiSize(CDUIRect rcParentAbs) override;

	//help
protected:
	virtual CDUIAttributeTextStyle * GetAttributeTextStyleActive();
	virtual CDUIRect GetTextRange();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif