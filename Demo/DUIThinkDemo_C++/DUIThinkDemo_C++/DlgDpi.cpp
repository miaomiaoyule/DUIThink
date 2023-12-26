#include "StdAfx.h"
#include "DlgDpi.h"

//////////////////////////////////////////////////////////////////////////
#define Timer_VerifyParentWnd_ID		(1989)
#define Timer_VerifyParentWnd_Elapse	(500)

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDlgDpi)

DuiBegin_Message_Map(CDlgDpi, CDUIWnd)
	Dui_On_Click_Ctrl(IDC_BtnSwitchRes, OnDuiClickBtnSwitchSkin)
DuiEnd_Message_Map()

CDlgDpi::CDlgDpi(LPCTSTR lpszDuiName, HWND hWndParent, bool bNeedSwitchRes)
	: CDUIWnd(lpszDuiName)
	, m_hWndParent(hWndParent)
	, m_bNeedSwitchRes(bNeedSwitchRes)
{
	return;
}

CDlgDpi::~CDlgDpi()
{
	return;
}

LRESULT CDlgDpi::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDlgDpi::OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (uMsg)
	{
		case WM_DEMO_DPISCALE:
		{
			bHandled = true;

			OnWMDemoDpiScale(wParam, lParam);

			break;
		}
		default:
			break;
	}

	return __super::OnWndCustomMessage(uMsg, wParam, lParam, bHandled);
}

void CDlgDpi::OnInitDialog()
{
	if (NULL == m_pWndManager || NULL == m_pWndManager->GetRootCtrl()) return;

	//control
	m_pBtnSwitchResCtrl = MMInterfaceHelper(CDUIButtonCtrl, m_pWndManager->FindControl(IDC_BtnSwitchRes));

	//init
	m_pBtnSwitchResCtrl->SetVisible(m_bNeedSwitchRes);
	m_pWndManager->SetTimer(m_pWndManager->GetRootCtrl(), Timer_VerifyParentWnd_ID, Timer_VerifyParentWnd_Elapse);

	::PostMessage(m_hWndParent, WM_DEMO_DPIWND, (WPARAM)m_hWnd, NULL);

	CenterWindow();

	return;
}

void CDlgDpi::OnDuiTimer(const DuiNotify &Notify)
{
	if (Timer_VerifyParentWnd_ID == Notify.wParam)
	{
		if (m_hWndParent && false == IsWindow(m_hWndParent))
		{
			PostQuitMessage(0);
		}

		return;
	}

	return;
}

void CDlgDpi::OnDuiClickBtnSwitchSkin(const DuiNotify &Notify)
{
	int nSkin = (CDUIGlobal::GetInstance()->GetSwitchResIndex() + 1) % 2;
	CDUIGlobal::GetInstance()->PerformSwitchRes(nSkin);

	return;
}

void CDlgDpi::OnWMDemoDpiScale(WPARAM wParam, LPARAM lParam)
{
	CDUIGlobal::GetInstance()->SetScale((int)wParam);

	return;
}