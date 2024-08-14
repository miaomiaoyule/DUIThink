#include "StdAfx.h"
#include "DlgDemo.h"

//////////////////////////////////////////////////////////////////////////
#define Time_TrayIcon_ID				(1989)
#define Time_TrayIcon_Elapse			(450)

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CDlgDemo, CDUIWnd)
	Dui_On_Notify_Ctrl(DuiNotify_Timer, IDC_TabViewControls, OnTimerTabViewControls)
DuiEnd_Message_Map()

CDlgDemo::CDlgDemo(LPCTSTR lpszDuiName)
	: CDUIWnd(lpszDuiName)
{
	return;
}

CDlgDemo::~CDlgDemo()
{
	return;
}

LRESULT CDlgDemo::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDlgDemo::OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (uMsg)
	{
		case WM_DEMO_DPIWND:
		{
			bHandled = true;

			OnWMDemoDpiWnd(wParam, lParam);

			break;
		}
		case WM_DEMO_TRAYICON:
		{
			bHandled = true;

			OnWMDemoTrayIcon(wParam, lParam);

			break;
		}
		default:
			break;
	}

	return __super::OnWndCustomMessage(uMsg, wParam, lParam, bHandled);
}

void CDlgDemo::OnFindControl()
{
	__super::OnFindControl();

	Dui_DDX_Control(CDUIContainerCtrl, m_pTabViewControls, IDC_TabViewControls)

	return;
}

void CDlgDemo::OnInitDialog()
{
	if (NULL == m_pWndManager) return;

	//control
	{
		m_ProgressView.Attach(MMInterfaceHelper(CDUIContainerCtrl, m_pTabViewControls->FindSubControl(IDC_HorizProgressView)));
		m_CheckBoxView.Attach(MMInterfaceHelper(CDUIContainerCtrl, m_pTabViewControls->FindSubControl(IDC_VertCheckBoxView)));
		m_RadioBoxView.Attach(MMInterfaceHelper(CDUIContainerCtrl, m_pTabViewControls->FindSubControl(IDC_VertRadioBoxView)));
		m_ComboxView.Attach(MMInterfaceHelper(CDUIContainerCtrl, m_pTabViewControls->FindSubControl(IDC_VertComboxView)));
	}

	//usage
	{
		MMInterfaceHelper(CDUIContainerCtrl, m_pWndManager->FindControl(IDC_TabViewDisplay), pTabUsage);
		if (NULL == pTabUsage) return;

		m_DpiDlgView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabUsage->FindSubControl(IDC_VertDpiDlgView)));
		m_SvgImageView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabUsage->FindSubControl(IDC_VertSvgImageView)));
		m_SwitchSkinView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabUsage->FindSubControl(IDC_VertSwitchSkinView)));
		m_AnimateDlgView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabUsage->FindSubControl(IDC_VertAnimateDlgView)));

		m_pWndManager->AddPreMessagePtr(&m_SvgImageView);
	}

	//qq
	{
		MMInterfaceHelper(CDUIContainerCtrl, m_pWndManager->FindControl(IDC_TabViewMain), pTabMain);
		if (NULL == pTabMain) return;

		m_QQView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabMain->FindSubControl(IDC_VertQQView)));
	}

	//tray
	{
		m_TrayIcon.CreateTrayIcon(m_hWnd, CDUIGlobal::GetInstance()->GetInstanceHandle(), IDI_DUITHINKDEMOC, _T("DUIThinkDemo"), WM_DEMO_TRAYICON);
		m_pTabViewControls->SetTimer(Time_TrayIcon_ID, Time_TrayIcon_Elapse);
	}

	CenterWindow();

	return;
}

void CDlgDemo::OnTimerTabViewControls(const DuiNotify &Notify)
{
	switch (Notify.wParam)
	{
		case Time_TrayIcon_ID:
		{
			m_bShowTrayIcon = !m_bShowTrayIcon;
			m_TrayIcon.ShowIcon(m_bShowTrayIcon);

			break;
		}
	}

	return;
}

void CDlgDemo::OnWMDemoDpiWnd(WPARAM wParam, LPARAM lParam)
{
	HWND hWndDpiDlg = (HWND)wParam;
	m_DpiDlgView.SetDpiDlgHandle(hWndDpiDlg);

	return;
}

void CDlgDemo::OnWMDemoTrayIcon(WPARAM wParam, LPARAM lParam)
{
	if (WM_LBUTTONDOWN == lParam)
	{

	}
	else if (WM_LBUTTONUP == lParam)
	{
	}
	else if (WM_MOUSEMOVE == lParam)
	{
		//icon pos
		CDUIRect rcTrayIcon = CMMTrayIcon::GetTrayIconPos(m_hWnd);

		//win11
		bool bWin11 = false;
		if (rcTrayIcon.Empty())
		{
			bWin11 = true;

			CDUIPoint ptMouse;
			GetCursorPos(&ptMouse);

			HWND hWndShellTray = FindWindow(_T("Shell_TrayWnd"), NULL);
			GetWindowRect(hWndShellTray, &rcTrayIcon);
			rcTrayIcon.left = ptMouse.x - 1;
			rcTrayIcon.right = ptMouse.x + 1;
		}

		//verify
		if (m_pDlgChatTip && IsWindow(*m_pDlgChatTip))
		{
			if (bWin11)
			{
				m_pDlgChatTip->PerformRefreshMonitorPos(rcTrayIcon);
			}

			return;
		}

		//dlg chat tip
		MMSafeDelete(m_pDlgChatTip);
		m_pDlgChatTip = new CDlgChatTip(rcTrayIcon);
		m_pDlgChatTip->Create(m_hWnd, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG | WS_EX_TOPMOST);
	}

	return;
}