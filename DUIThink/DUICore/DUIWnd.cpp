#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////////
DuiBaseBegin_Message_Map(CDUINotifyPump)
DuiEnd_Message_Map()

static const Dui_MsgMapEntry * DuiFindMessageEntry(const Dui_MsgMapEntry *lpEntry, const DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl) return NULL;

	UINT uCtrlID = Notify.pNotifyCtrl->GetCtrlID();
	const Dui_MsgMapEntry *pNotifyEntry = NULL;
	while (lpEntry->nSig != DuiSign_end)
	{
		if (lpEntry->Notify == Notify.NotifyType)
		{
			if (0 != lpEntry->uCtrlID && lpEntry->uCtrlID == uCtrlID)
			{
				return lpEntry;
			}
			else if (0 == lpEntry->uCtrlID)
			{
				pNotifyEntry = lpEntry;
			}
		}

		lpEntry++;
	}

	return pNotifyEntry;
}

void CDUINotifyPump::OnNotify(const DuiNotify &Notify)
{
	const Dui_MsgMapEntry *lpEntry = NULL;
	const Dui_MsgMap *pMessageMap = NULL;

#ifndef DUI_LIB
	for (pMessageMap = GetDUIMessageMap(); pMessageMap != NULL; pMessageMap = (*pMessageMap->pfnGetBaseMap)())
#else
	for (pMessageMap = GetDUIMessageMap(); pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMap)
#endif
	{
#ifndef DUI_LIB
		ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
#else
		ASSERT(pMessageMap != pMessageMap->pBaseMap);
#endif
		if ((lpEntry = DuiFindMessageEntry(pMessageMap->lpEntries, Notify)) != NULL)
		{
			goto Dispatch;
		}
	}
	return;

Dispatch:
	union DuiMessageMapFunctions mmf;
	mmf.pfn = lpEntry->pfn;

	bool bRes = false;
	switch (lpEntry->nSig)
	{
		case DuiSign_l_w_l:
		{
			(this->*mmf.pfn_Notify_l_w_l)(Notify.wParam, Notify.lParam);
			bRes = true;
			break;
		}
		case DuiSign_v_n:
		{
			(this->*mmf.pfn_Notify_v_n)(Notify);
			bRes = true;
			break;
		}
		default:
		{
			ASSERT(false);
			break;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIWnd)

DuiBegin_Message_Map(CDUIWnd, CDUINotifyPump)
	Dui_On_Timer()
	Dui_On_Notify(DuiNotify_Click, OnDuiClick)
	Dui_On_Notify(DuiNotify_WndAnimateFinish, OnDuiWndAnimateFinish)
DuiEnd_Message_Map()

CDUIWnd::CDUIWnd(LPCTSTR lpszDuiName, HWND hWndParent)
	: m_strDuiName(lpszDuiName)
	, m_hWndParent(hWndParent)
	, m_hWnd(NULL)
	, m_OldWndProc(::DefWindowProc)
	, m_bSubWindow(false)
{
	CDUIWndManager *pWndManager = new CDUIWndManager();
	pWndManager->Init();

	m_pWndManager = pWndManager;

	return;
}

CDUIWnd::~CDUIWnd()
{
	if (m_hWnd)
	{
		Close(Dui_CtrlIDInner_BtnCancel);
		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
		UnSubWindow();
		OnFinalMessage();
	}

	MMSafeDelete(m_pWndManager);

	return;
}

HWND CDUIWnd::GetHWND() const
{
	return m_hWnd;
}

CDUIWnd::operator HWND() const
{
	return m_hWnd;
}

CDUIWndManager * CDUIWnd::GetWndManager()
{
	return m_pWndManager;
}

CMMString CDUIWnd::GetDuiName() const
{
	return m_strDuiName;
}

UINT CDUIWnd::GetClassStyle() const
{
	return CS_DBLCLKS;
}

HWND CDUIWnd::Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
	return Create(hWndParent, lpszName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND CDUIWnd::Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
	if (GetSuperClassName() != NULL && !RegisterSuperclass()) return NULL;
	if (GetSuperClassName() == NULL && !RegisterWindowClass()) return NULL;
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();
	m_hWndParent = hWndParent;
	m_ptCreate = { x, y };
	m_hWnd = ::CreateWindowEx(dwExStyle, GetClass(), lpszName, dwStyle, x, y, cx, cy, hWndParent, hMenu, hInstance, this);
	ASSERT(m_hWnd != NULL);
	return m_hWnd;
}

HWND CDUIWnd::SubWindow(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	ASSERT(m_hWnd == NULL);
	m_OldWndProc = SubclassWindow(hWnd, __WndProc);
	if (m_OldWndProc == NULL) return NULL;
	m_bSubWindow = true;
	m_hWnd = hWnd;
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
	return m_hWnd;
}

void CDUIWnd::UnSubWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	if (!::IsWindow(m_hWnd)) return;
	if (!m_bSubWindow) return;
	SubclassWindow(m_hWnd, m_OldWndProc);
	m_OldWndProc = ::DefWindowProc;
	m_bSubWindow = false;
	return;
}

void CDUIWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
	ASSERT(::IsWindow(m_hWnd));
	if (!::IsWindow(m_hWnd)) return;
	::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOW : SW_SHOWNOACTIVATE) : SW_HIDE);

	return;
}

UINT CDUIWnd::DoModal()
{
	//create
	if (false == IsWindow(m_hWnd))
	{
		Create(m_hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	}

	//message
	UINT nRet = 0;
	HWND hWndParent = GetWindowOwner(m_hWnd);
	bool bWndParentEnable = IsWindowEnabled(hWndParent);
	::EnableWindow(hWndParent, false);
	
	BOOL bRet = 0;
	MSG Msg = {};
	while (::IsWindow(m_hWnd) && (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0)
	{
		// handle the error and possibly exit
		if (-1 == bRet)
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);

			break;
		}
		if (Msg.message == WM_CLOSE && Msg.hwnd == m_hWnd)
		{
			nRet = Msg.wParam;
			::EnableWindow(hWndParent, bWndParentEnable);
			::SetFocus(hWndParent);
		}
		if (false == CDUIGlobal::TranslateMessage(&Msg))
		{
			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}
		if (Msg.message == WM_QUIT) break;
	}

	::EnableWindow(hWndParent, bWndParentEnable);
	::SetFocus(hWndParent);
	if (Msg.message == WM_QUIT) ::PostQuitMessage(Msg.wParam);

	return nRet;
}

UINT CDUIWnd::DoBlock()
{
	//create
	if (false == IsWindow(m_hWnd))
	{
		Create(m_hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	}

	//message
	UINT nRet = 0;
	BOOL bRet = 0;
	MSG Msg = {};
	while (::IsWindow(m_hWnd) && (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0)
	{
		// handle the error and possibly exit
		if (-1 == bRet)
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);

			break;
		}
		if (Msg.message == WM_CLOSE && Msg.hwnd == m_hWnd)
		{
			nRet = Msg.wParam;
		}
		if (false == CDUIGlobal::TranslateMessage(&Msg))
		{
			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}
		if (Msg.message == WM_QUIT) break;
	}
	if (Msg.message == WM_QUIT) ::PostQuitMessage(Msg.wParam);
	
	return nRet;
}

void CDUIWnd::Close(UINT nRet)
{
	if (false == ::IsWindow(m_hWnd)) return;
	
	PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);

	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndClose(dynamic_cast<CDUIWndManager*>(m_pWndManager));
	}

	return;
}

void CDUIWnd::CenterWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD) == 0);
	RECT rcDlg = {};
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = {};
	RECT rcCenter = {};
	HWND hWnd = *this;
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);
	if (hWndCenter != NULL)
		hWnd = hWndCenter;

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if (hWndCenter == NULL)
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if (xLeft < rcArea.left) xLeft = rcArea.left;
	else if (xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
	if (yTop < rcArea.top) yTop = rcArea.top;
	else if (yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

	return;
}

void CDUIWnd::SetIcon(UINT nRes)
{
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();
	HICON hIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// 防止高DPI下图标模糊
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);

	hIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// 防止高DPI下图标模糊
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM)false, (LPARAM)hIcon);

	return;
}

LRESULT CDUIWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CDUIWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void CDUIWnd::ResizeWnd(int cx /*= -1*/, int cy /*= -1*/)
{
	ASSERT(::IsWindow(m_hWnd));
	
	CDUIRect rcWnd;
	if (!::GetWindowRect(m_hWnd, &rcWnd)) return;
	if (cx != -1) rcWnd.right = rcWnd.left + cx;
	if (cy != -1) rcWnd.bottom = rcWnd.top + cy;
	if (!::AdjustWindowRectEx(&rcWnd, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd))) return;
	::SetWindowPos(m_hWnd, NULL, 0, 0, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	return;
}

void CDUIWnd::AdjustWnd()
{
	if (false == IsWindow(m_hWnd)) return;

	CDUIRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(GetHWND(), MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;
	if (rcWnd.left < rcWork.left)
	{
		rcWnd.Offset(rcWork.left - rcWnd.left, 0);
	}
	if (rcWnd.right > rcWork.right)
	{
		rcWnd.Offset(rcWork.right - rcWnd.right, 0);
	}
	if (rcWnd.top < rcWork.top)
	{
		rcWnd.Offset(0, rcWork.top - rcWnd.top);
	}
	if (rcWnd.bottom > rcWork.bottom)
	{
		rcWnd.Offset(0, rcWork.bottom - rcWnd.bottom);
	}

	MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), TRUE);

	return;
}

LPCTSTR CDUIWnd::GetSuperClassName() const
{
	return NULL;
}

void CDUIWnd::OnFinalMessage()
{
	if (m_pWndManager)
	{
		m_pWndManager->RemovePreMessagePtr(this);
		m_pWndManager->RemoveINotify(this);

		if (m_pWndManager->GetRootCtrl())
		{
			m_pWndManager->GetRootCtrl()->ReapControl();
		}
	}

	m_hWnd = NULL;
	m_OldWndProc = NULL;
	m_bSubWindow = false;

	return;
}

void CDUIWnd::OnDuiTimer(const DuiNotify &Notify)
{
	return;
}

void CDUIWnd::OnDuiClick(const DuiNotify &Notify)
{
	if (NULL == m_pWndManager) return;

	UINT uCtrlID = Notify.pNotifyCtrl->GetCtrlID();
	
	if (Dui_CtrlIDInner_BtnClose == uCtrlID
		|| Dui_CtrlIDInner_BtnCancel == uCtrlID
		|| Dui_CtrlIDInner_BtnOk == uCtrlID)
	{
		m_nCtrlIDClose = uCtrlID;

		if (AnimateWnd_None != m_pWndManager->GetAnimateWndType())
		{
			m_pWndManager->StartAnimationWnd(false);

			return;
		}

		Close(Dui_CtrlIDInner_BtnOk == uCtrlID ? uCtrlID : Dui_CtrlIDInner_BtnCancel);
		
		return;
	}
	else if (Dui_CtrlIDInner_BtnMin == uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		
		return;
	}
	else if (Dui_CtrlIDInner_BtnMax == uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		
		return;
	}
	else if (Dui_CtrlIDInner_BtnRestore == uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		
		return;
	}

	return;
}

void CDUIWnd::OnDuiWndAnimateFinish(const DuiNotify &Notify)
{
	if (false == Notify.DUINotifyExtend.WndAnimate.bShow)
	{
		ShowWindow(false);

		Close(Dui_CtrlIDInner_BtnOk == m_nCtrlIDClose ? m_nCtrlIDClose : Dui_CtrlIDInner_BtnCancel);
	}

	return;
}

LRESULT CDUIWnd::OnClose(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;

	return 0;
}

LRESULT CDUIWnd::OnDestroy(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;

	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndClose(dynamic_cast<CDUIWndManager*>(m_pWndManager));
	}

	return 0;
}

#if defined(WIN32) && !defined(UNDER_CE)
LRESULT CDUIWnd::OnNcActivate(WPARAM wParam, LPARAM /*lParam*/, bool &bHandled)
{	
	if (false == ::IsIconic(m_hWnd)) bHandled = TRUE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CDUIWnd::OnNcCalcSize(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = TRUE;
	LPRECT pRect = NULL;

	if (wParam == TRUE)
	{
		LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
		pRect = &pParam->rgrc[0];
	}
	else
	{
		pRect = (LPRECT)lParam;
	}

	if (::IsZoomed(m_hWnd))
	{	// 最大化时，计算当前显示器最适合宽高度
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
		CDUIRect rcWork = oMonitor.rcWork;
		CDUIRect rcMonitor = oMonitor.rcMonitor;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		pRect->left = pRect->top = 0;
		pRect->right = pRect->left + rcWork.GetWidth();
		pRect->bottom = pRect->top + rcWork.GetHeight();
		return WVR_REDRAW;
	}

	return 0;
}

LRESULT CDUIWnd::OnNcPaint(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = TRUE;
	return 0;
}

LRESULT CDUIWnd::OnNcHitTest(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = TRUE;
	POINT pt = {};
	GetCursorPos(&pt);
	::ScreenToClient(m_hWnd, &pt);

	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	if (!::IsZoomed(m_hWnd))
	{
		RECT rcSizeBox = m_pWndManager->GetResizeTrack();
		if (pt.y < rcClient.top + rcSizeBox.top)
		{
			if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
			return HTTOP;
		}
		else if (pt.y > rcClient.bottom - rcSizeBox.bottom)
		{
			if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if (pt.x < rcClient.left + rcSizeBox.left) return HTLEFT;
		if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
	}

	CDUIRect rcCaption = { 0, 0, rcClient.right, m_pWndManager->GetCaptionHeight() };
	if (rcCaption.PtInRect(pt))
	{
		CDUIControlBase *pControl = m_pWndManager->FindControl(pt);
		if (NULL == pControl 
			|| (NULL == MMInterfaceHelper(CDUIButtonCtrl, pControl) && NULL == MMInterfaceHelper(CDUIProgressCtrl, pControl)))
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CDUIWnd::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = true;
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;
	CDUIRect rcMonitor = oMonitor.rcMonitor;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	// 计算最大化时，正确的原点坐标
	lpMMI->ptMaxPosition.x = rcWork.left;
	lpMMI->ptMaxPosition.y = rcWork.top;

	lpMMI->ptMaxSize.x = rcWork.GetWidth();
	lpMMI->ptMaxSize.y = rcWork.GetHeight();

	lpMMI->ptMaxTrackSize.x = m_pWndManager->GetWndMaxSize().cx == 0 ? rcWork.GetWidth() : m_pWndManager->GetWndMaxSize().cx;
	lpMMI->ptMaxTrackSize.y = m_pWndManager->GetWndMaxSize().cy == 0 ? rcWork.GetHeight() : m_pWndManager->GetWndMaxSize().cy;

	lpMMI->ptMinTrackSize.x = m_pWndManager->GetWndMinSize().cx;
	lpMMI->ptMinTrackSize.y = m_pWndManager->GetWndMinSize().cy;

	return 0;
}

LRESULT CDUIWnd::OnMouseWheel(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnMouseHover(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnExitSizeMove(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = false;

	return 0;
}
#endif

LRESULT CDUIWnd::OnSize(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnChar(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnSysCommand(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = TRUE;

	if (wParam == SC_CLOSE)
	{	
		SendMessage(WM_CLOSE);
		return 0;
	}

#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = OnOldWndProc(WM_SYSCOMMAND, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed && false == IsIconic(m_hWnd))
	{
		CDUIControlBase *pBtnMax = m_pWndManager->FindControl(Dui_CtrlIDInner_BtnMax);
		CDUIControlBase *pBtnRestore = m_pWndManager->FindControl(Dui_CtrlIDInner_BtnRestore);

		if (pBtnMax && pBtnRestore)
		{
			pBtnMax->SetVisible(false == ::IsZoomed(m_hWnd));
			pBtnRestore->SetVisible(::IsZoomed(m_hWnd));
		}
	}
#else
	LRESULT lRes = OnOldWndProc(uMsg, wParam, lParam);
#endif

	return lRes;
}

LRESULT CDUIWnd::OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = TRUE;
	LRESULT lRes = OnOldWndProc(WM_CREATE, wParam, lParam);

	//modify
	LONG lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle &= ~WS_CAPTION;
	//lStyle &= ~WS_SYSMENU;
	::SetWindowLong(m_hWnd, GWL_STYLE, lStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	//root
	CDUIContainerCtrl *pRootCtrl = dynamic_cast<CDUIContainerCtrl*>(CDUIGlobal::GetInstance()->LoadDui(GetDuiName(), m_pWndManager));

	//init
	m_pWndManager->AttachDialog(m_hWnd);
	m_pWndManager->AddPreMessagePtr(this);
	m_pWndManager->AddINotify(this);
	m_pWndManager->AttachRootCtrl(pRootCtrl);

	//center wnd
	if (CW_USEDEFAULT == m_ptCreate.x 
		&& CW_USEDEFAULT == m_ptCreate.y
		&& 0 == (lStyle & WS_CHILD))
	{
		CenterWindow();
	}

	//max restore
	CDUIControlBase *pBtnMax = m_pWndManager->FindControl(Dui_CtrlIDInner_BtnMax);
	CDUIControlBase *pBtnRestore = m_pWndManager->FindControl(Dui_CtrlIDInner_BtnRestore);
	if (pBtnMax && pBtnRestore)
	{
		pBtnMax->SetVisible(false == ::IsZoomed(m_hWnd));
		pBtnRestore->SetVisible(::IsZoomed(m_hWnd));
	}

	//init func
	OnFindControl();
	OnInitDialog();

	//call func
	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndCreate(m_pWndManager);
	}

	return lRes;
}

LRESULT CDUIWnd::OnKeyDown(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnKillFocus(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnSetFocus(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnLButtonDown(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnLButtonUp(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnMouseMove(WPARAM /*wParam*/, LPARAM /*lParam*/, bool &bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT CDUIWnd::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = false;

	switch (uMsg)
	{
		case WM_CREATE:			lRes = OnCreate(wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose(wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy(wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)
		case WM_NCACTIVATE:		lRes = OnNcActivate(wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize(wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint(wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest(wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(wParam, lParam, bHandled); break;
		case WM_MOUSEWHEEL:		lRes = OnMouseWheel(wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:			lRes = OnSize(wParam, lParam, bHandled); break;
		case WM_TIMER:			lRes = OnTimer(wParam, lParam, bHandled); break;
		case WM_CHAR:			lRes = OnChar(wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:		lRes = OnSysCommand(wParam, lParam, bHandled); break;
		case WM_KEYDOWN:		lRes = OnKeyDown(wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:		lRes = OnKillFocus(wParam, lParam, bHandled); break;
		case WM_SETFOCUS:		lRes = OnSetFocus(wParam, lParam, bHandled); break;
		case WM_LBUTTONUP:		lRes = OnLButtonUp(wParam, lParam, bHandled); break;
		case WM_LBUTTONDOWN:	lRes = OnLButtonDown(wParam, lParam, bHandled); break;
		case WM_MOUSEMOVE:		lRes = OnMouseMove(wParam, lParam, bHandled); break;
		case WM_MOUSEHOVER:		lRes = OnMouseHover(wParam, lParam, bHandled); break; 
		case WM_EXITSIZEMOVE:	lRes = OnExitSizeMove(wParam, lParam, bHandled); break;
	}
	if (bHandled) return lRes;

	lRes = OnWndCustomMessage(uMsg, wParam, lParam, bHandled);
	if (bHandled) return lRes;

	if (m_pWndManager->OnWndMessage(uMsg, wParam, lParam, lRes)) return lRes;

	return OnOldWndProc(uMsg, wParam, lParam);
}

LRESULT CDUIWnd::OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	bHandled = false;

	return 0;
}

LRESULT CDUIWnd::OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (WM_KEYDOWN == uMsg)
	{
		switch (wParam)
		{
			case VK_ESCAPE:
			{
				bHandled = TRUE;
				
				Close(Dui_CtrlIDInner_BtnCancel);
				return 1;			
			}

			default:
				break;
		}
	}

	return 0;
}

LRESULT CDUIWnd::OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_OldWndProc) return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);

	return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

bool CDUIWnd::RegisterSuperclass()
{
	// Get the class information from an existing
	// window so we can subclass it later on...
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	if (!::GetClassInfoEx(NULL, GetSuperClassName(), &wc))
	{
		if (!::GetClassInfoEx(hInstance, GetSuperClassName(), &wc))
		{
			ASSERT(!"Unable to locate window class");
			return NULL;
		}
	}

	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = CDUIWnd::__ControlProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = GetClass();
	ATOM ret = ::RegisterClassEx(&wc);
	ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool CDUIWnd::RegisterWindowClass()
{
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();

	WNDCLASS wc = {};
	wc.style = GetClassStyle();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = CDUIWnd::__WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GetClass();
	ATOM ret = ::RegisterClass(&wc);
	ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CDUIWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDUIWnd* pThis = NULL;
	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CDUIWnd*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CDUIWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (uMsg == WM_NCDESTROY && pThis != NULL)
		{
			LRESULT lRes = pThis->OnOldWndProc(uMsg, wParam, lParam);
			::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			if (pThis->m_bSubWindow) pThis->UnSubWindow();
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage();
			return lRes;
		}
	}

	if (pThis != NULL)
	{
		return pThis->OnWndMessage(uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CDUIWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDUIWnd* pThis = NULL;
	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CDUIWnd*>(lpcs->lpCreateParams);
		::SetProp(hWnd, _T("WndX"), (HANDLE)pThis);
		pThis->m_hWnd = hWnd;
	}
	else
	{
		pThis = reinterpret_cast<CDUIWnd*>(::GetProp(hWnd, _T("WndX")));
		if (uMsg == WM_NCDESTROY && pThis != NULL)
		{
			LRESULT lRes = pThis->OnOldWndProc(uMsg, wParam, lParam);
			if (pThis->m_bSubWindow) pThis->UnSubWindow();
			::SetProp(hWnd, _T("WndX"), NULL);
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage();
			return lRes;
		}
	}

	if (pThis != NULL)
	{
		return pThis->OnWndMessage(uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
