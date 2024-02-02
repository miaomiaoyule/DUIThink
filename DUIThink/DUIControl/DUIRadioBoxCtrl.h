#ifndef __DUIRADIOCTRL_H__
#define __DUIRADIOCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUITabLayoutCtrl;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIRadioBoxCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIRadioBoxCtrl = { 0xFD007C41,0xDC4E,0x4298,0xB0,0x16,0x1B,0xAF,0x5F,0x20,0x8B,0x0C };
class DUITHINK_API CDUIRadioBoxCtrl
	: public CDUICheckBoxCtrl
{
	friend class CDUIWndManager;

	DuiDeclare_CreateControl(CDUIRadioBoxCtrl)
	MMDeclare_ClassName(CDUIRadioBoxCtrl)

public:
	CDUIRadioBoxCtrl(void);
	~CDUIRadioBoxCtrl(void);

	//attribute
protected:
	//radio
	CDUIAttributeGroup					m_AttributeGroupRadio;
	CDUIAttributeText					m_AttributeGroupName;
	CDUIAttriTabSelect					m_AttributeBindTabCtrlID;
	CDUIAttributeLong					m_AttributeBindTabCtrlIndex;

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
	void Select(bool bSelect = true, bool bNotify = true) override;

	//group
	virtual CMMString GetGroupName();
	virtual void SetGroupName(LPCTSTR lpszGroup);

	//Tab
	virtual CDUITabLayoutCtrl * GetBindTabCtrl();
	virtual void SetBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl);
	virtual UINT GetBindTabCtrlID();
	virtual void SetBindTabCtrlID(UINT uCtrlID);
	virtual long GetBindTabCtrlIndex();
	virtual void SetBindTabCtrlIndex(int nIndex);

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;

	//message
protected:
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiWndManagerAttach() override;
	void OnDuiWndManagerDetach() override;

	//help
protected:
	//active tab
	virtual void PerformActiveBindTabPage();
	//unsel other
	virtual void PerformUnSelOther();
};

typedef std::vector<CDUIRadioBoxCtrl*> VecDuiRadioBoxCtrl;
typedef std::unordered_map<CMMString, VecDuiRadioBoxCtrl> MapDuiRadioBoxGroup;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif