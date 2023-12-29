#ifndef __DUIMENUCTRL_H__
#define __DUIMENUCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIMenuWnd;
class CDUIMenuCtrl;
class CDUIMenuItemCtrl;

//////////////////////////////////////////////////////////////////////////
class CDUIMenuWnd 
	: public CDUIWnd
	, public IDUIControlCallBack
{
	MMDeclare_ClassName()
	DuiDeclare_Message_Map()

public:
	CDUIMenuWnd();
	CDUIMenuWnd(CDUIMenuItemCtrl *pOwner, CMMString strDuiName);
	virtual ~CDUIMenuWnd();

protected:
	CDUIMenuItemCtrl *					m_pOwner = NULL;
	CDUIWndManager *					m_pWndManagerOwner = NULL;
	CDUIMenuCtrl *						m_pShowMenuView = NULL;
	CDUIPoint							m_ptTrack;

	//method
public:
	void Init(HWND hWndParent);
	void Init(HWND hWndParent, CDUIPoint ptTrack);
	void UnInit();
	CDUIMenuCtrl * GetMenuView();
	void SetMenuView(CDUIMenuCtrl *pMenuView);
	CDUIMenuItemCtrl * GetOwner();

	//override
protected:
	void OnSize(CDUIControlBase *pControl) override;

	//override
protected:
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled);
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam, bool &bHandled);
	LRESULT OnKeyDown(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled);
	LRESULT OnClose(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled);
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/);

	//message
protected:
	LRESULT OnWMDuiResizeMenu(WPARAM wParam, LPARAM lParam);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);

	//help
protected:
	void ResizeMenu();
	void ResizeSubMenu();
};

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIMenuItemCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIMenuItemCtrl = { 0x07174720,0xD8C2,0x4B3C,0x8C,0x9E,0xA2,0x28,0x07,0xF1,0xD3,0xEF };
class DUITHINK_API CDUIMenuItemCtrl
	: public CDUIListItemCtrl
{
	DuiDeclare_CreateControl(CDUIMenuItemCtrl)
	MMDeclare_ClassName(CDUIMenuItemCtrl)

public:
	CDUIMenuItemCtrl(void);
	~CDUIMenuItemCtrl(void);

	//attibute
protected:
	CDUIAttributeGroup					m_AttributeMenuGroup;
	CDUIAttributeBool					m_AttributeLineMenu;
	CDUIAttributeColorSwitch			m_AttributeLineColor;
	CDUIAttributeText					m_AttributeGroupID;
	CDUIAttriImageSection				m_AttributeIconChecked;
	CDUIAttributeLong					m_AttributeIconCheckedLeftPadding;
	CDUIAttriImageSection				m_AttributeIconNormal;
	CDUIAttributeLong					m_AttributeIconNormalLeftPadding;
	CDUIAttriImageSection				m_AttributeIconExpand;
	CDUIAttributeLong					m_AttributeIconExpandRightPadding;
	CDUIAttributeBool					m_AttributeHasExpandMenu;
	CDUIAttributeText					m_AttributeExpandViewDuiName;

	//variant
protected:
	CDUIMenuWnd *						m_pExpandMenuWnd = NULL;
	CDUIRect							m_rcLineMenu;
	CDUIRect							m_rcIconChecked;
	CDUIRect							m_rcIconNormal;
	CDUIRect							m_rcIconExpand;
	CDUIRect							m_rcItemText;
	bool								m_bCheckMenu = false;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	bool IsSelected() const override;
	bool Select(bool bSelect = true) override;

	//method
public:
	//override
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;
	void RefreshView() override;

	//override
	virtual bool IsExpanded() const;
	virtual bool Expand(bool bExpand = true);
	virtual bool CheckMenu(bool bCheck = true);
	virtual bool IsChecked();

	//function
	virtual CDUIMenuCtrl * GetExpandMenuView() const;
	virtual CDUIMenuItemCtrl * GetExpandMenuItemFromIndex(int nIndex) const;
	virtual CDUIMenuItemCtrl * GetExpandMenuItemFromID(UINT uCtrlID) const;

	//attribute
	virtual bool IsLineMenu();
	virtual void SetLineMenu(bool bLineMenu);
	virtual void SetLineColorResSwitch(const CMMString &strResSwitch);
	virtual CMMString GetMenuGroupID();
	virtual void SetMenuGroupID(CMMString strGroupID);
	virtual void SetIconCheckedResSwitch(const CMMString &strResSwitch);
	virtual void SetIconNormalResSwitch(const CMMString &strResSwitch);
	virtual void SetIconExpandResSwitch(const CMMString &strResSwitch);
	virtual int GetIconCheckedLeftPadding();
	virtual void SetIconCheckedLeftPadding(int nX);
	virtual int GetIconNormalLeftPadding();
	virtual void SetIconNormalLeftPadding(int nX);
	virtual int GetIconExpandRightPadding();
	virtual void SetIconExpandRightPadding(int nX);
	virtual bool IsHasExpandMenu();
	virtual void SetHasExpandMenu(bool bHasExpandMenu);

	//message
protected:
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
	LRESULT OnDuiKeyDown(const DuiMessage &Msg) override;

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//refresh
protected:
	virtual void PaintLineMenu(HDC hDC);
	virtual void PaintIconChecked(HDC hDC);
	virtual void PaintIconNormal(HDC hDC);
	virtual void PaintIconExpand(HDC hDC);

	//help
protected:
	CDUIRect GetTextRange() override;
	void ActiveExpandMenu();
	void UnActiveExpandMenu();
	void LoadExpandMenu();
	void InitExpandMenu(bool bActive = false);
	void CalcItemRect();
};

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIMenuCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIMenuCtrl = { 0x98178BB2,0x30BC,0x4E93,0x95,0x58,0xB4,0x6C,0x16,0xC4,0xA6,0x22 };
class DUITHINK_API CDUIMenuCtrl
	: public CDUIListViewCtrl
{
	DuiDeclare_CreateControl(CDUIMenuCtrl)
	MMDeclare_ClassName(CDUIMenuCtrl)

public:
	CDUIMenuCtrl(void);
	~CDUIMenuCtrl(void);

	//attribute
protected:

	//variant
protected:

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	//override
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//menu
	virtual int GetMenuItemCount() const;
	virtual bool InsertMenuItem(CDUIMenuItemCtrl *pMenuItem, int nPos = -1);
	virtual CDUIMenuItemCtrl * GetMenuItem(int nIndex) const;
	virtual void CheckMenu(int nIndex);

	//refresh
	void RefreshView() override;

	//override
protected:
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIMenuItemCtrl * GetChildAt(int nIndex) const override;

	//property
protected:
	void InitComplete() override;

	//message
protected:

	//help
protected:
	CDUIListItemCtrl * ProductItemDefault() override;
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIMenu
{
public:
	CDUIMenu();
	virtual ~CDUIMenu();

protected:

	//method
public:
	virtual void LoadMenu(LPCTSTR lpszMenu);
	virtual void DestroyMenu();
	virtual CDUIMenuCtrl * GetRootMenu();
	virtual tagDuiMenuCmd TrackPopupMenu(HWND hWndParent, CDUIPoint pt);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif