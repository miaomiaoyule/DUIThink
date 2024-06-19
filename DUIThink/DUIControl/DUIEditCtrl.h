#ifndef __DUIEDITCTRL_H__
#define __DUIEDITCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIEditWnd;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIEditCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIEditCtrl = { 0x1ABA5E56,0x3740,0x457E,0xAE,0x24,0x91,0xE2,0x33,0xF9,0x3A,0x6B };
class DUITHINK_API CDUIEditCtrl
	: public CDUIButtonCtrl
{
	friend class CDUIEditWnd;

	DuiDeclare_CreateControl(CDUIEditCtrl)
	MMDeclare_ClassName(CDUIEditCtrl)

public:
	CDUIEditCtrl(void);
	virtual ~CDUIEditCtrl(void);

	//attribute
protected:
	//edit
	CDUIAttributeGroup					m_AttributeGroupEdit;
	CDUIAttributeBool					m_AttributeReadOnly;
	CDUIAttributeBool					m_AttributeNumberOnly;
	CDUIAttributeBool					m_AttributePasswordMode;
	CDUIAttributeBool					m_AttributeAutoSelAll;
	CDUIAttributeBool					m_AttributeDesktopEdit;
	CDUIAttributeLong					m_AttributeMaxChar = 255;
	CDUIAttributeLong					m_AttributeNumberMinLimit = 0x80000000;
	CDUIAttributeLong					m_AttributeNumberMaxLimit = 0x7fffffff;
	CDUIAttributeText					m_AttributePasswordChar = _T("*");
	CDUIAttributeText					m_AttributeTipText;
	CDUIAttributeTextStyle				m_AttributeTextStyleTipTextNormal;

	//variant
protected:
	CDUIEditWnd	*						m_pEditWindow = NULL;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//override
	void SetVisible(bool bVisible = true) override;
	void SetEnabled(bool bEnable = true) override;
	bool SetText(LPCTSTR lpszText) override;

	//refresh
	void RefreshView() override;

	//function
	virtual void SetSel(long nStartChar, long nEndChar);
	virtual void SetSelAll();
	virtual void SetReplaceSel(LPCTSTR lpszReplace);

	//attribute
	virtual bool IsReadOnly();
	virtual void SetReadOnly(bool bReadOnly);
	virtual bool IsNumberOnly();
	virtual void SetNumberOnly(bool bNumberOnly);
	virtual bool IsPasswordMode();
	virtual void SetPasswordMode(bool bPasswordMode);
	virtual bool IsAutoSelAll();
	virtual void SetAutoSelAll(bool bAutoSelAll);
	virtual bool IsDesktopEdit();
	virtual void SetDesktopEdit(bool bDesktopEdit);
	virtual UINT GetMaxChar();
	virtual void SetMaxChar(UINT uMax);
	virtual int GetNumberMinLimit();
	virtual void SetNumberMinLimit(int nMinNum);
	virtual int GetNumberMaxLimit();
	virtual void SetNumberMaxLimit(int nMaxNum);
	virtual TCHAR GetPasswordChar();
	virtual void SetPasswordChar(TCHAR chPasswordChar);
	virtual CMMString GetTipText();
	virtual void SetTipText(LPCTSTR lpszTipText);
	virtual tagDuiTextStyle GetTextStyleTipTextNormal();
	virtual ARGB GetTextColorTipTextNormal();
	virtual void SetTextStyleTipTextNormal(const tagDuiTextStyle &TextStyle);

	//message
protected:
	bool OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiSetFocus() override;
	bool OnDuiKillFocus() override;

	//property
protected:
	void InitProperty() override;

	//paint
protected:
	void PaintText(HDC hDC) override;

	//help
protected:
	void SetInternVisible(bool bVisible = true, bool bTraversal = false) override;
	void OnEditDestroy();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif