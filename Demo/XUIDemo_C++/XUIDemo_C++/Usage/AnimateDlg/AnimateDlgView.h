#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgDpi;

//////////////////////////////////////////////////////////////////////////
class CAnimateDlgView
	: public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CAnimateDlgView();
	~CAnimateDlgView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pAnimateDlgViewCtrl = NULL;

	CDUIListViewCtrl *					m_pListViewAnimateDlgCtrl = NULL;

	//variant
protected:
	CDlgDpi *							m_pDlgAnimate = NULL;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiItemClickListViewAnimateDlg(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////


