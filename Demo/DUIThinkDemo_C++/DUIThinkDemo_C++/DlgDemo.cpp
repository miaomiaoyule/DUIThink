#include "StdAfx.h"
#include "DlgDemo.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CDlgDemo, CDUIWnd)
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
		default:
			break;
	}

	return __super::OnWndCustomMessage(uMsg, wParam, lParam, bHandled);
}

void CDlgDemo::OnInitDialog()
{
	if (NULL == m_pWndManager) return;

	//control
	{
		MMInterfaceHelper(CDUIContainerCtrl, m_pWndManager->FindControl(IDC_TabViewControls), pTabControl);
		if (NULL == pTabControl) return;

		m_ProgressView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabControl->FindSubControl(IDC_HorizProgressView)));
		m_CheckBoxView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabControl->FindSubControl(IDC_VertCheckBoxView)));
		m_RadioBoxView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabControl->FindSubControl(IDC_VertRadioBoxView)));
		m_ComboxView.Attach(MMInterfaceHelper(CDUIContainerCtrl, pTabControl->FindSubControl(IDC_VertComboxView)));
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

	CenterWindow();

	return;
}

void CDlgDemo::OnWMDemoDpiWnd(WPARAM wParam, LPARAM lParam)
{
	HWND hWndDpiDlg = (HWND)wParam;
	m_DpiDlgView.SetDpiDlgHandle(hWndDpiDlg);

	return;
}