#ifndef __DUILISTVIEWHEADERITEMCTRL_H__
#define __DUILISTVIEWHEADERITEMCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIListHeaderItemCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIListHeaderItemCtrl = { 0xFD6C81B4,0xCA0A,0x4176,0xA1,0xCD,0xB9,0xA9,0x0C,0x51,0x6E,0x15 };
class DUITHINK_API CDUIListHeaderItemCtrl 
	: public CDUIContainerCtrl
{
	friend class CDUIListHeaderCtrl;

	DuiDeclare_CreateControl(CDUIListHeaderItemCtrl)
	MMDeclare_ClassName(CDUIListHeaderItemCtrl)

public:
	CDUIListHeaderItemCtrl(void);
	~CDUIListHeaderItemCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupSep;
	CDUIAttributeLong					m_AttributeSepWidth = 1;
	CDUIAttributeFloat					m_AttributeSepScale;
	CDUIAttributeColorSwitch			m_AttributeSepColor = Name_ColorBlack;
	CDUIAttriImageSection				m_AttributeSepImage;

	CDUIAttributeGroup					m_AttributeGroupStatus;
	CDUIAttributeColorSwitch			m_AttributeColorNormal;
	CDUIAttributeColorSwitch			m_AttributeColorHot;
	CDUIAttributeColorSwitch			m_AttributeColorPushed;
	CDUIAttributeColorSwitch			m_AttributeColorFocus;
	CDUIAttriImageSection				m_AttributeImageNormal;
	CDUIAttriImageSection				m_AttributeImageHot;
	CDUIAttriImageSection				m_AttributeImagePushed;
	CDUIAttriImageSection				m_AttributeImageFocus;

	//text
	CDUIAttributeGroup					m_AttributeGroupText;
	CDUIAttributeText					m_AttributeText;
	CDUIAttributeTextStyle				m_AttributeTextStyle;
	CDUIAttributeRect					m_AttributeTextPadding;

	//variant
protected:
	CDUIListHeaderCtrl *				m_pOwner = NULL;
	CDUIRect							m_rcSep;							//·Ö¸ôÇøÓò
	int									m_nSepStatus;						//·Ö¸ô×´Ì¬
	CDUIPoint							m_ptLastMouse;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	UINT GetControlFlags() override;

	//header
	virtual void SetOwner(CDUIListHeaderCtrl *pOwner);
	virtual int GetSepWidth();
	virtual void SetSepWidth(int nWidth);
	virtual float GetSepScale();
	virtual void SetSepScale(float fScale);
	virtual RECT GetSepRect();

	//layout
	void SetFixedWidth(int nWidth) override;

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;

	//refresh
	void RefreshView() override;

	//text
	virtual CMMString GetText();
	virtual bool SetText(LPCTSTR lpszText);
	virtual HFONT GetFont();
	virtual ARGB GetTextColor();
	virtual tagDuiTextStyle GetTextStyle();
	virtual tagDuiTextStyle GetTextStyleActive();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);
	virtual RECT GetTextPadding();
	virtual void SetTextPadding(RECT rcPadding);

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:	   
	void PaintStatusColor(HDC hDC) override;
	void PaintStatusImage(HDC hDC) override;
	void PaintText(HDC hDC) override;

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;

	void OnDuiSize(CDUIRect rcParentAbs) override;

	//help
protected:
	virtual void CalcSepRect();
	virtual CDUIRect GetTextRange();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif