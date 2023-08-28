#ifndef __DUITABLAYOUTCTRL_H__
#define __DUITABLAYOUTCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUITabLayoutCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUITabLayoutCtrl = { 0xF48C83D4,0xF08B,0x4DBB,0xA5,0x5F,0xB4,0x47,0x15,0x7C,0x5E,0xC4 };
class DUITHINK_API CDUITabLayoutCtrl
	: public CDUIContainerCtrl
	, public CDUIAnimation
{
	DuiDeclare_CreateControl(CDUITabLayoutCtrl)
	MMDeclare_ClassName(CDUITabLayoutCtrl)

public:
	CDUITabLayoutCtrl(void);
	~CDUITabLayoutCtrl(void);

	//attribute
protected:
	CDUIAttributeCombox					m_AttributeAnimateSelSwitchType;

	CDUIAttributeGroup					m_AttributeGroupSelect;
	CDUIAttributeLong					m_AttributeSelectIndex;

	//variant
protected:
	int									m_nOldSel;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	void OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame) override;
	void OnAnimationStop(int nAnimationID) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//basic
	bool SetTimer(UINT uTimerID, UINT nElapse) override;
	bool KillTimer(UINT uTimerID) override;
	bool KillTimer() override;

	//refresh
	void RefreshView() override;

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIControlBase * DetachChild(int nIndex) override;
	bool SetChildIndex(CDUIControlBase *pControl, int nIndex) override;
	bool SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView = true) override;
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;

	//select
	virtual bool Select(int nIndex);
	virtual bool Select(CDUIControlBase *pChild);
	virtual int GetCurSel();
	virtual enDuiAnimateSelSwithType GetAnimateSelSwitchType();
	virtual void SetAnimateSelSwitchType(enDuiAnimateSelSwithType AnimateSelSwitchType);

	//attribute
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//help
protected:
	void SelectItem(int nIndex);
	void SetCurSel(int nIndex);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif