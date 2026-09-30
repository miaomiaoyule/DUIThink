#pragma once

//////////////////////////////////////////////////////////////////////////
class CSwitchSkinView
	: public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CSwitchSkinView();
	~CSwitchSkinView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pSwitchSkinViewCtrl = NULL;

	CDUIButtonCtrl *					m_pBtnSwitchSkin1Ctrl = NULL;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiClickBtnSwitchSkin1(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////

