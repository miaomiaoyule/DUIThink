#ifndef __DUIDATETIMECTRL_H__
#define __DUIDATETIMECTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIDateTimeCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIDateTimeCtrl = { 0x5ACF56A6,0x9149,0x4C00,0xB6,0xA9,0x3D,0x52,0x41,0x50,0xED,0x06 };
class DUITHINK_API CDUIDateTimeCtrl 
	: public CDUIHorizontalLayoutCtrl
	, public IDuiControlCallBack
{
	DuiDeclare_CreateControl(CDUIDateTimeCtrl)
	MMDeclare_ClassName(CDUIDateTimeCtrl)

public:
	CDUIDateTimeCtrl(void);
	~CDUIDateTimeCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupDateTime;
	CDUIAttributeDuiSelect				m_AttributeBindCalendarDui;

	//variant
protected:
	CDUIThinkEditCtrl *					m_pEditDateTimeCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnDateTimeCtrl = NULL;
	SYSTEMTIME							m_tDateTime = {};

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//override
protected:
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void RefreshView() override;

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	virtual CDUIControlBase * DetachChild(int nIndex) override;
	virtual CDUIControlBase * DetachChild(CDUIControlBase *pControl) override;
	virtual bool SetChildIndex(CDUIControlBase *pControl, int nIndex) override;
	virtual bool SwapChild(CDUIControlBase *pFirstCtrl, CDUIControlBase *pNextCtrl, bool bNeedRefreshView = true) override;
	virtual bool SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView = true) override;
	virtual void SetChildPadding(int nChildPaddingH, int nChildPaddingV) override;
	virtual bool Remove(CDUIControlBase *pControl) override;
	virtual bool RemoveAt(int nIndex) override;
	virtual void RemoveAll() override;

	//date time
	virtual CDUIThinkEditCtrl * GetDataTimeEditCtrl();
	virtual CDUIButtonCtrl * GetDateTimeBtnCtrl();
	virtual CMMString GetBindCalendarDui();
	virtual void SetBindCalendarDui(CMMString strCalendarDui);
	virtual SYSTEMTIME GetDateTime();
	virtual void SetDateTime(SYSTEMTIME tDateTime);
	virtual CDUISize MeasureString(LPCTSTR lpszText = _T(""));
	virtual HFONT GetFont();
	virtual HFONT GetFontNormal();
	virtual HFONT GetFontHot();
	virtual HFONT GetFontPushed();
	virtual HFONT GetFontFocus();
	virtual HFONT GetFontDisabled();
	virtual ARGB GetTextColor();
	virtual ARGB GetTextColorNormal();
	virtual ARGB GetTextColorHot();
	virtual ARGB GetTextColorPushed();
	virtual ARGB GetTextColorFocus();
	virtual ARGB GetTextColorDisabled();
	virtual tagDuiTextStyle GetTextStyle();
	virtual tagDuiTextStyle GetTextStyleNormal();
	virtual tagDuiTextStyle GetTextStyleHot();
	virtual tagDuiTextStyle GetTextStylePushed();
	virtual tagDuiTextStyle GetTextStyleFocus();
	virtual tagDuiTextStyle GetTextStyleDisabled();
	virtual tagDuiTextStyle GetTextStyleActive();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleHot(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStylePushed(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleFocus(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleDisabled(const tagDuiTextStyle &TextStyle);

	virtual tagDuiImageSection GetBtnImageSectionNormal();
	virtual tagDuiImageSection GetBtnImageSectionHot();
	virtual tagDuiImageSection GetBtnImageSectionPushed();
	virtual tagDuiImageSection GetBtnImageSectionFocus();
	virtual tagDuiImageSection GetBtnImageSectionDisabled();
	virtual void SetBtnImageSectionNormal(const tagDuiImageSection &ImageSection);
	virtual void SetBtnImageSectionHot(const tagDuiImageSection &ImageSection);
	virtual void SetBtnImageSectionPushed(const tagDuiImageSection &ImageSection);
	virtual void SetBtnImageSectionFocus(const tagDuiImageSection &ImageSection);
	virtual void SetBtnImageSectionDisabled(const tagDuiImageSection &ImageSection);

	//message
protected:
	void OnDuiWndManagerAttach() override;

	//proprety
protected:
	void InitProperty();
	void InitComplete();

	//paint
protected:

	//help
protected:
	void PerformActiveCalendar();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
