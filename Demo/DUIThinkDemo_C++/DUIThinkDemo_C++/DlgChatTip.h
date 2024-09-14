#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgChatTip
	: public CDUIWnd
{
	MMDeclare_ClassName(CDlgChatTip)
	DuiDeclare_Message_Map()

public:
	CDlgChatTip(CDUIRect rcMonitor);
	virtual ~CDlgChatTip();

	//control
protected:
	CDUIListViewCtrl *					m_pListViewChatTipCtrl = NULL;

	//variant
protected:
	CDUIRect							m_rcMonitor;

	//method
public:
	void PerformRefreshMonitorPos(CDUIRect rcMonitor);

	//override
protected:
	void OnFindControl() override;
	void OnInitDialog() override;
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam) override;

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiWndLayout(const DuiNotify &Notify);
	void OnDuiTimer_IDC_ListViewChatTip(const DuiNotify &Notify);

	//help
protected:
};
