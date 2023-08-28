#pragma once

//////////////////////////////////////////////////////////////////////////
class CRadioBoxView : public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CRadioBoxView();
	~CRadioBoxView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pRadioBoxViewCtrl = NULL;

	CDUIRadioBoxCtrl *					m_pRadioBox1Ctrl = NULL;
	CDUIRadioBoxCtrl *					m_pRadioBox2Ctrl = NULL;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////

