#pragma once

//////////////////////////////////////////////////////////////////////////
class CQQView : public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CQQView();
	~CQQView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pQQViewCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnQQChatCtrl = NULL;

	//variant
protected:

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiClickBtnQQChat(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////

