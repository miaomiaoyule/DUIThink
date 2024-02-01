#ifndef __DUICOMBOXCTRL_H__
#define __DUICOMBOXCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIComboxWnd;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIComboxCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIComboxCtrl = { 0xA82E0774,0xAC2A,0x493E,0x81,0x23,0x51,0x1F,0x55,0x23,0xF3,0x9C };
class DUITHINK_API CDUIComboxCtrl
	: public CDUICheckBoxCtrl
{
	DuiDeclare_CreateControl(CDUIComboxCtrl)
	MMDeclare_ClassName(CDUIComboxCtrl)

public:
	CDUIComboxCtrl(void);
	~CDUIComboxCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupCombox;
	CDUIAttributeBool					m_AttributeEditEnable;
	CDUIAttriViewSelect					m_AttributeComboxView;
	CDUIAttributeLong					m_AttributeWaveHeight;
	CDUIAttributeSize					m_AttributeComboxSize;

	//variant
protected:
	CDUIComboxWnd *						m_pComboxWindow = NULL;
	CDUIListViewCtrl *					m_pShowListView = NULL;
	CDUIThinkEditCtrl *					m_pEditCtrl = NULL;

	//override
protected:
	bool RegisterControlListen(IDUIInterface *pIControlListen) override;
	bool UnRegisterControlListen(IDUIInterface *pIControlListen) override;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	void OnResourceDelete(CDUIResourceBase *pResourceObj) override;
	void OnResourceSwitch(int nIndexRes) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	CDUIComboxCtrl * Clone(bool bIncludeChild = true, bool bRefreshCtrlID = true) override;

	//basic
	UINT InitCtrlID() override;
	void RefreshCtrlID() override;
	bool SetWndManager(CDUIWndManager *pWndManager) override;
	void SetVisible(bool bVisible = true) override;
	bool SetText(LPCTSTR lpszText) override;

	//refresh
	void RefreshView() override;
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//override
	void SetTextStyleNormal(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleHot(const tagDuiTextStyle &TextStyle) override;
	void SetTextStylePushed(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleFocus(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleDisabled(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleSelNormal(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleSelHot(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleSelPushed(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleSelFocus(const tagDuiTextStyle &TextStyle) override;
	void SetTextStyleSelDisabled(const tagDuiTextStyle &TextStyle) override;

	//combox
	virtual bool IsEditEnable();
	virtual void SetEditEnable(bool bEnable);
	virtual CMMString GetComboxView();
	virtual void SetComboxView(CMMString strComboxView);
	virtual int GetWaveHeight();
	virtual void SetWaveHeight(int nHeight);
	virtual SIZE GetComboxSize();
	virtual void SetComboxSize(SIZE szDropBox);

	virtual int GetCurSel() const;
	virtual bool SelectItem(int nIndex);

	virtual int GetChildCount() const;
	virtual bool InsertChild(CDUIControlBase *pChild, int nPos = -1);
	virtual bool InsertChild(CMMString strText, int nPos = -1);
	virtual CDUIControlBase * GetChildAt(int nIndex);
	virtual bool Remove(CDUIControlBase *pControl);
	virtual bool RemoveAt(int nIndex);
	virtual void RemoveAll();

	virtual bool Active();
	virtual bool UnActive();
	virtual bool IsActive() const;

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//paint
protected:
	void PaintText(HDC hDC) override;

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	LRESULT OnDuiKeyDown(const DuiMessage &Msg) override;

	//help
protected:
	void SetInternVisible(bool bVisible = true) override;
	CDUIControlBase * FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags) override;
	void ReapControl() override;
	CDUIAttributeColorSwitch * GetAttributeStatusColor() override;
	CDUIAttriImageSection * GetAttributeStatusImage() override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif