#pragma once

//////////////////////////////////////////////////////////////////////////
class CProgressView : public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CProgressView();
	~CProgressView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pProgressViewCtrl = NULL;

	CDUIProgressCtrl *					m_pHProgress1Ctrl = NULL;
	CDUIProgressCtrl *					m_pHProgress2Ctrl = NULL;

	CDUIProgressCtrl *					m_pVProgress1Ctrl = NULL;
	CDUIProgressCtrl *					m_pVProgress2Ctrl = NULL;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiTimer(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////
