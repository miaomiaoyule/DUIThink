#include "stdafx.h"
#include "DUIWndVirtual.h"

//////////////////////////////////////////////////////////////////////////
CDUIWndVirtual::CDUIWndVirtual(LPCTSTR lpszDuiName, HWND hWndParent)
	: CDUIWnd(lpszDuiName, hWndParent)
{
}

CDUIWndVirtual::~CDUIWndVirtual()
{
	DetachHost();
	MMUnregisterWnd(m_hWnd);
	m_hWnd = NULL;
	m_uWndID = 0;

	return;
}

LPVOID CDUIWndVirtual::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWndVirtual, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWndVirtual::GetDescribe() const
{
	return Dui_WindowVirtual;
}

HWND CDUIWndVirtual::Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	m_hWndParent = hWndParent;
	m_ptCreate = { x, y };
	m_hWnd = (HWND)static_cast<IMMWndInterface *>(this);
	m_uWndID = 0;
	MMRegisterWnd(m_hWnd, this);

	//create
	OnWndMessage(WM_CREATE, 0, 0);

	//pos
	AdjustWndSize();
	if (CW_USEDEFAULT != x && CW_USEDEFAULT != y)
	{
		UINT uFlags = SWP_NOZORDER | SWP_NOACTIVATE;
		if (cx <= 0 || cy <= 0 || CW_USEDEFAULT == cx || CW_USEDEFAULT == cy)
		{
			uFlags |= SWP_NOSIZE;
		}

		::SetWindowPos(m_hWnd, NULL, x, y, cx, cy, uFlags);
	}

	//show
	ShowWindow(dwStyle & WS_VISIBLE, false);

	//refesh
	RefreshLayout();

	//attach
	AttachHost();

	return m_hWnd;
}

void CDUIWndVirtual::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= true*/)
{
	if (NULL == m_hWnd) return;

	if (m_pRootCtrl)
	{
		m_pRootCtrl->SetVisible(bShow);
	}

	return;
}

void CDUIWndVirtual::ShowWindow(int nCmdShow)
{
	switch (nCmdShow)
	{
		case SW_SHOW:
		case SW_SHOWNORMAL:
		case SW_SHOWDEFAULT:
		{
			ShowWindow(true);
			break;
		}
		default:
		{
			ShowWindow(false);
			break;
		}
	}

	return;
}

void CDUIWndVirtual::Close(UINT nRet)
{
	if (IsWindow(m_hWnd))
	{
		SendMessage(WM_CLOSE, (WPARAM)nRet, 0);
	}

	return;
}

void CDUIWndVirtual::CenterWindow()
{
	return;
}

void CDUIWndVirtual::SetIcon(UINT nRes)
{
	(void)nRes;
	return;
}

LRESULT CDUIWndVirtual::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (false == IsWindow(m_hWnd))
	{
		ASSERT(false);
		return 0;
	}

	return OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDUIWndVirtual::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (false == IsWindow(m_hWnd))
	{
		ASSERT(false);
		return 0;
	}

	return ::PostMessage(m_hWnd, uMsg, wParam, lParam) ? 1 : 0;
}

void CDUIWndVirtual::SetFocusControl(CDUIControlBase *pFocusCtrl)
{
	CDUIWndBase::SetFocusControl(pFocusCtrl);

	return;
}

bool CDUIWndVirtual::IsCaptured()
{
	return false;
}

CDUIContainerCtrl * CDUIWndVirtual::DetachRootCtrl()
{
	DetachHost();

	return __super::DetachRootCtrl();
}

HDC CDUIWndVirtual::GetWndDC()
{
	IMMWndInterface *pWndParent = MMFindWnd(m_hWndParent);
	if (pWndParent)
	{
		return pWndParent->GetWndDC();
	}
	
	return NULL;
}

void CDUIWndVirtual::ResizeWnd(int cx /*= -1*/, int cy /*= -1*/)
{
	if (false == IsWindow(m_hWnd) || cx <= 0 || cy <= 0) return;

	::SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	return;
}

void CDUIWndVirtual::AdjustWndPos()
{
	return;
}

bool CDUIWndVirtual::IsVirtualWnd()
{
	return true;
}

bool CDUIWndVirtual::IsWindowVisible()
{
	return m_pRootCtrl && m_pRootCtrl->IsVisible();
}

CMMRect CDUIWndVirtual::GetWindowRect()
{
	if (NULL == m_pRootCtrl) return {};

	CDUIWndBase *pWndHost = GetHostWnd();
	if (NULL == pWndHost) return {};

	CDUIRect rcWndHost;
	::GetWindowRect(pWndHost->GetWndHandle(), &rcWndHost);

	CDUIRect rcCtrl = m_pRootCtrl->GetAbsoluteRect();
	rcCtrl.Offset(rcWndHost.left, rcWndHost.top);

	return rcCtrl;
}

CMMRect CDUIWndVirtual::GetClientRect()
{
	if (NULL == m_pRootCtrl) return {};

	CDUIRect rcCtrl = m_pRootCtrl->GetAbsoluteRect();
	rcCtrl.Offset(-rcCtrl.left, -rcCtrl.top);

	return rcCtrl;
}

bool CDUIWndVirtual::IsMaximized()
{
	return false;
}

bool CDUIWndVirtual::IsMinimized()
{
	return false;
}

void CDUIWndVirtual::SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (NULL == m_pRootCtrl || NULL == m_hWnd)
	{
		return;
	}

	CDUIRect rcCtrl = m_pRootCtrl->GetAbsoluteRect();
	if (0 == (uFlags & SWP_NOMOVE))
	{
		rcCtrl.Offset(X - rcCtrl.left, Y - rcCtrl.top);
	}
	if (0 == (uFlags & SWP_NOSIZE))
	{
		rcCtrl.right = rcCtrl.left + cx;
		rcCtrl.bottom = rcCtrl.top + cy;
	}

	m_pRootCtrl->SetAbsoluteRect(rcCtrl);
	
	return;
}

void CDUIWndVirtual::RefreshLayout()
{
	CDUIWndBase *pWndHost = GetHostWnd();
	if (NULL == pWndHost) return __super::RefreshLayout();

	pWndHost->RefreshLayout();

	return;
}

void CDUIWndVirtual::Invalidate()
{
	CDUIWndBase *pWndHost = GetHostWnd();
	if (NULL == pWndHost) return;

	pWndHost->Invalidate();

	return;
}

LRESULT CDUIWndVirtual::OnClose(WPARAM wParam, LPARAM lParam)
{
	m_uCtrlIDClose = (UINT)wParam;
	DetachHost();
	MMUnregisterWnd(m_hWnd);
	m_hWnd = NULL;
	m_uWndID = 0;

	OnFinalMessage();

	return 0;
}

LRESULT CDUIWndVirtual::OnPaint(CDUIRect rcPaint)
{
	(void)rcPaint;
	return 0;
}

CDUIWndBase * CDUIWndVirtual::GetHostWnd() const
{
	//parent wnd
	HWND hParent = m_hWndParent;
	while (hParent)
	{
		IMMWndInterface *pParent = MMFindWnd(hParent);
		if (NULL == pParent || false == pParent->IsVirtualWnd())
		{
			break;
		}

		hParent = pParent->GetParent();
	}
	if (NULL == hParent)
	{
		return NULL;
	}

	//wnd
	CDUIWnd *pWnd = CDUIGlobal::GetInstance()->GetWndByHandle(hParent);
	if (pWnd)
	{
		return pWnd;
	}

	return NULL;
}

void CDUIWndVirtual::AttachHost()
{
	//detach
	DetachHost();

	//root ctrl
	if (NULL == m_pRootCtrl)
	{
		return;
	}

	CDUIWndBase *pWndHost = GetHostWnd();
	CDUIContainerCtrl *pHostRootCtrl = pWndHost ? pWndHost->GetRootCtrl() : NULL;
	if (NULL == pHostRootCtrl)
	{
		return;
	}

	//attach
	m_pRootCtrl->SetFloat(true);
	pHostRootCtrl->InsertChild(m_pRootCtrl);
	pWndHost->AddINotify(this);

	return;
}

void CDUIWndVirtual::DetachHost()
{
	CDUIWndBase *pWndHost = GetHostWnd();
	if (NULL == m_pRootCtrl || NULL == pWndHost)
	{
		return;
	}

	CDUIContainerCtrl *pParent = m_pRootCtrl->GetParent();
	if (pParent)
	{
		pParent->DetachChild(m_pRootCtrl);
	}

	pWndHost->RemoveINotify(this);

	return;
}

//////////////////////////////////////////////////////////////////////////
