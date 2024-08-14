#include "StdAfx.h"
#include "DlgChatTip.h"

//////////////////////////////////////////////////////////////////////////
#define Time_VerifyMousePos_ID			(1989)
#define Time_VerifyMousePos_Elapse		(300)

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDlgChatTip)
DuiBegin_Message_Map(CDlgChatTip, CDUIWnd)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify(DuiNotify_WndLayout, OnDuiWndLayout)
	Dui_On_Notify_Ctrl(DuiNotify_Timer, IDC_ListViewChatTip, OnDuiTimer_IDC_ListViewChatTip)
DuiEnd_Message_Map()

CDlgChatTip::CDlgChatTip(CDUIRect rcMonitor)
	: CDUIWnd(_T("DlgChatTip"))
	, m_rcMonitor(rcMonitor)
{
	return;
}

CDlgChatTip::~CDlgChatTip()
{
	return;
}

void CDlgChatTip::PerformRefreshMonitorPos(CDUIRect rcMonitor)
{
	if (NULL == m_pListViewChatTipCtrl) return;

	m_rcMonitor = rcMonitor;

	m_pListViewChatTipCtrl->SetTimer(Time_VerifyMousePos_ID, Time_VerifyMousePos_Elapse);

	return;
}

void CDlgChatTip::OnFindControl()
{
	Dui_DDX_Control(CDUIListViewCtrl, m_pListViewChatTipCtrl, IDC_ListViewChatTip);

	return;
}

void CDlgChatTip::OnInitDialog()
{
	//wnd pos
	CDUIRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);

	CDUIPoint ptWnd = { m_rcMonitor.left + m_rcMonitor.GetWidth() / 2 - rcWnd.GetWidth() / 2, m_rcMonitor.top - rcWnd.GetHeight() };
	SetWindowPos(m_hWnd, NULL, ptWnd.x, ptWnd.y, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);

	//verify mouse
	m_pListViewChatTipCtrl->SetTimer(Time_VerifyMousePos_ID, Time_VerifyMousePos_Elapse);

	return;
}

LRESULT CDlgChatTip::OnMouseMove(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	return __super::OnMouseMove(wParam, lParam, bHandled);
}

void CDlgChatTip::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

void CDlgChatTip::OnDuiWndLayout(const DuiNotify &Notify)
{
	return;
}

void CDlgChatTip::OnDuiTimer_IDC_ListViewChatTip(const DuiNotify &Notify)
{
	if (Time_VerifyMousePos_ID == Notify.wParam)
	{
		CDUIRect rcWnd;
		GetWindowRect(m_hWnd, &rcWnd);

		CDUIPoint ptMouse;
		GetCursorPos(&ptMouse);

		if (false == rcWnd.PtInRect(ptMouse) && false == m_rcMonitor.PtInRect(ptMouse))
		{
			Close();
		}

		return;
	}

	return;
}
