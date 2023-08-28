#include "framework.h"
#include "DlgListen.h"

//////////////////////////////////////////////////////////////////////////
#define Time_VerifyWnd_ID				(1989)
#define Time_VerifyWnd_Elapse			(1000)

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CDlgListen, CDUIWnd)
DuiEnd_Message_Map()

//////////////////////////////////////////////////////////////////////////
CDlgListen::CDlgListen(HWND hWndOwner)
	: m_hWndOwner(hWndOwner)
{

}

CDlgListen::~CDlgListen()
{

}

void CDlgListen::OnDuiTimer(const DuiNotify &Notify)
{

}

LRESULT CDlgListen::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDlgListen::OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	SetTimer(m_hWnd, Time_VerifyWnd_ID, Time_VerifyWnd_Elapse, NULL);

	return __super::OnCreate(wParam, lParam, bHandled);
}

LRESULT CDlgListen::OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (wParam)
	{
		case Time_VerifyWnd_ID:
		{
			if (m_hWndOwner && false == IsWindow(m_hWndOwner))
			{
				PostQuitMessage(0);
			}

			return 0;
		}
		default:
			break;
	}

	return __super::OnTimer(wParam, lParam, bHandled);
}
