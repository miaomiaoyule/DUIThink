#pragma once

//////////////////////////////////////////////////////////////////////////
class CCheckBoxView : public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CCheckBoxView();
	~CCheckBoxView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pCheckBoxViewCtrl = NULL;

	CDUICheckBoxCtrl *					m_pCheckBox1Ctrl = NULL;
	CDUICheckBoxCtrl *					m_pCheckBox2Ctrl = NULL;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////
