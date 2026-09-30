#pragma once

//////////////////////////////////////////////////////////////////////////
class CComboxView : public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CComboxView();
	~CComboxView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pComboxViewCtrl = NULL;

	CDUIComboxCtrl *					m_pCombox1Ctrl = NULL;
	CDUIComboxCtrl *					m_pCombox2Ctrl = NULL;
	CDUIComboxCtrl *					m_pCombox3Ctrl = NULL;
	CDUIComboxCtrl *					m_pCombox4Ctrl = NULL;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiItemSelectedCombox1(const DuiNotify &Notify);
	void OnDuiItemSelectedCombox2(const DuiNotify &Notify);
	void OnDuiItemSelectedCombox3(const DuiNotify &Notify);
	void OnDuiItemSelectedCombox4(const DuiNotify &Notify);
	void OnDuiClickBtnRemoveCombox2(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////

