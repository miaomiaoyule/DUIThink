#ifndef __DUIHOTKEYCTRL_H__
#define __DUIHOTKEYCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIHotKeyWnd;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIHotKeyCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIHotKeyCtrl = { 0x899DE908,0x305F,0x49D6,0xB6,0xDD,0x6D,0x32,0x25,0x77,0xA4,0xBD };
class DUITHINK_API CDUIHotKeyCtrl
	: public CDUIButtonCtrl
	, public IDUIPreMessage
{
	friend class CDUIHotKeyWnd;

	DuiDeclare_CreateControl(CDUIHotKeyCtrl)
	MMDeclare_ClassName(CDUIHotKeyCtrl)

public:
	CDUIHotKeyCtrl(void);
	~CDUIHotKeyCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupHotKey;
	CDUIAttributeBool					m_AttributeAutoSelAll;
	CDUIAttributeHotKey					m_AttributeHotKey;

	//variant
protected:
	CDUIHotKeyWnd *						m_pHotKeyWindow = NULL;

	//override
protected:
	LRESULT OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//override
	void SetVisible(bool bVisible = true) override;
	void SetEnabled(bool bEnable = true) override;

	//refresh
	void RefreshView() override;

	//hotkey
	virtual void GetHotKey(BYTE &cbVKeyCode, BYTE &cbModifierFlag);
	virtual WORD GetHotKey();
	virtual void SetHotKey(BYTE cbVKeyCode, BYTE cbModifierFlag);
	virtual void SetHotKey(WORD wHotKey);
	virtual CMMString GetKeyName(UINT uVKeyCode, UINT uMapType = MAPVK_VK_TO_VSC, bool bDistinguishLeftRight = false);
	virtual CMMString GetHotKeyName();
	virtual bool IsAutoSelAll();
	virtual void SetAutoSelAll(bool bAutoSelAll);

	//message
protected:
	bool OnDuiSetFocus() override;
	bool OnDuiKillFocus() override;
	void OnDuiWndManagerAttach() override;
	void OnDuiWndManagerDetach() override;

	//property 
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:

	//help
protected:
	void SetInternVisible(bool bVisible = true, bool bTraversal = false) override;
	virtual BYTE HotkeyToMod(BYTE cbModifierFlag);
	virtual BYTE ModToHotkey(BYTE cbModifierFlag);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif