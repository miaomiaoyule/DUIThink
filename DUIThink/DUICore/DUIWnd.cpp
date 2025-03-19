#include "StdAfx.h"
#include "DUIWnd.h"

//////////////////////////////////////////////////////////////////////////
#define GetPIDLFolder(pida)				(LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define GetPIDLItem(pida, i)			(LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])
#define Dui_ControlDrag					_T("DUIThinkControlDrag")

////////////////////////////////////////////////////////////////////
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
	__super::Init();

	SetDpi(CDUIGlobal::GetInstance()->GetDpi());

	CDUIGlobal::GetInstance()->AddWnd(this);

	return;
}

CDUIWnd::~CDUIWnd()
{
	OnDuiDelayDelete();

	CDUIAnimationWnd::UnInit();
	CDUIGlobal::GetInstance()->RemoveWnd(this);

	ReleaseRootCtrl();
	ReleasePaintScene();

	//wnd dc
	if (IsWindow(m_hWnd))
	{
		if (m_hDCPaint)
		{
			::ReleaseDC(m_hWnd, m_hDCPaint);
			m_hDCPaint = NULL;
		}

		Close(Dui_CtrlIDInner_BtnCancel);
		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
		UnSubWindow();
		OnFinalMessage();
	}

	return;
}

bool CDUIWnd::RegisterControlListen(IDuiInterface *pIControlListen)
{
	if (false == __super::RegisterControlListen(pIControlListen)) return false;

	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return true;

	return pRootCtrl->RegisterControlListen(pIControlListen);
}

bool CDUIWnd::UnRegisterControlListen(IDuiInterface *pIControlListen)
{
	if (false == __super::UnRegisterControlListen(pIControlListen)) return false;

	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return false;

	return pRootCtrl->UnRegisterControlListen(pIControlListen);
}

bool CDUIWnd::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (NULL == pAttributeObj) return true;

	__super::OnAttributeChange(pAttributeObj);

	if (m_hWnd && m_pRootCtrl)
	{
		AdjustWndSize();

		CDUIRect rcClient(0, 0, GetClientSize().cx, GetClientSize().cy);
		m_pRootCtrl->OnDuiSize(rcClient);

		Invalidate();
	}

	return true;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
void CDUIWnd::OnWinDragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_DropData.vecDropFiles.clear();
	m_DropData.vecFileNameMap.clear();
	m_DropData.bHasGUID = false;
	for (auto &it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	m_DropData.vecPIDL.clear();

	// parse IDataObject
	CDUIPoint ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWnd, &ptMouse);
	m_ptMousePosLast = ptMouse;
	m_DropData.ptMouse = ptMouse;
	m_DropData.pIDataObject = pIDataObject;
	m_DropData.dwKeyState = dwKeyState;

	FORMATETC ftetc = { 0 };
	ftetc.lindex = -1;
	ftetc.dwAspect = DVASPECT_CONTENT;

	//CFSTR_PREFERREDDROPEFFECT
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(CFSTR_PREFERREDDROPEFFECT);
	ftetc.tymed = TYMED_HGLOBAL;

	//parse dropfiles
	ftetc.cfFormat = CF_HDROP;
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM medium = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop)
		{
			UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
			if (cFiles > 0)
			{
				for (int i = 0; i < cFiles; i++)
				{
					TCHAR szFileName[MAX_PATH] = { 0 };
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					m_DropData.vecDropFiles.push_back(szFileName);
				}
			}
		}

		GlobalUnlock(medium.hGlobal);
		ReleaseStgMedium(&medium);
	}

	//parse FileNameMap
	ftetc.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILENAMEMAP);
	ftetc.tymed = TYMED_HGLOBAL;
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop != NULL)
		{
			LPCWSTR s = reinterpret_cast<LPCWSTR>(hDrop);
			while (*s)
			{
				CMMString strFileName = CMMStrHelp::Format(_T("%ls"), s);
				m_DropData.vecFileNameMap.push_back(strFileName);
				s = 1 + wcschr(s, L'\0');
			}

			GlobalUnlock(medium.hGlobal);
			ReleaseStgMedium(&medium);

			//检测CFSTR_FILENAMEMAP与CF_HDROP文件数量保持一致,如不一致清空filenamemap
			if (m_DropData.vecFileNameMap.size() > 0)
			{
				if (m_DropData.vecFileNameMap.size() != m_DropData.vecDropFiles.size())
				{
					m_DropData.vecFileNameMap.clear();
				}
			}
		}
	}

	// parse pidl
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(CFSTR_SHELLIDLIST);
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM mediumPIDL = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &mediumPIDL))
	{
		LPIDA pida = (LPIDA)GlobalLock(mediumPIDL.hGlobal);
		PCIDLIST_ABSOLUTE pidl_parent = GetPIDLFolder(pida);
		for (int i = 0; i < pida->cidl; i++)
		{
			PCIDLIST_ABSOLUTE pcid = ILCombine(pidl_parent, GetPIDLItem(pida, i));
			m_DropData.vecPIDL.push_back(pcid);

			TCHAR szPath[MAX_PATH] = { 0 };
			SHGetPathFromIDList(pcid, szPath);
			if (lstrlen(szPath) == 0)
			{
				SHDESCRIPTIONID shdescid = { 0 };
				CComPtr<IShellFolder> psfParent = nullptr;
				PCUITEMID_CHILD lpidRelative = nullptr;
				HRESULT hr = SHBindToParent(pcid, IID_IShellFolder, (void**)&psfParent, &lpidRelative);
				if (hr != S_OK)
					continue;

				hr = SHGetDataFromIDList(psfParent, lpidRelative, SHGDFIL_DESCRIPTIONID, &shdescid, sizeof(SHDESCRIPTIONID));

				if (hr != S_OK)
					continue;

				LPOLESTR pszStr = NULL;
				hr = ::StringFromCLSID(shdescid.clsid, &pszStr);
				if (hr != S_OK)
					continue;

				CMMString strPath = pszStr;
				auto itor = std::find_if(m_DropData.vecDropFiles.begin(), m_DropData.vecDropFiles.end(), [&](CMMString& strTemp) {
					return strTemp.CompareNoCase(strPath) == 0;
				});
				if (!m_DropData.bHasGUID)
				{
					m_DropData.bHasGUID = true;
				}
				if (itor == m_DropData.vecDropFiles.end())
					m_DropData.vecDropFiles.push_back(strPath);
			}
		}

		GlobalUnlock(mediumPIDL.hGlobal);
		ReleaseStgMedium(&mediumPIDL);
	}

	// parse text
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(Dui_ControlDrag);
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM mediumText = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &mediumText))
	{
		PWSTR pzStr = (PWSTR)GlobalLock(mediumText.hGlobal);

		GlobalUnlock(mediumText.hGlobal);
		ReleaseStgMedium(&mediumText);
	}

	//notify
	m_pWinDragEnterCtrl = FindControlByDrop(ptMouse);
	if (m_pWinDragEnterCtrl)
	{
		m_pWinDragEnterCtrl->OnWinDragEnter(&m_DropData, &m_DropData.dwEffect);
	}

	return;
}

void CDUIWnd::OnWinDragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (NULL == m_pRootCtrl) return;

	POINT ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWnd, &ptMouse);
	if (m_DropData.ptMouse == ptMouse
		&& m_DropData.dwKeyState == dwKeyState)
	{
		*pdwEffect = m_DropData.dwEffect;

		return;
	}

	m_DropData.dwKeyState = dwKeyState;
	m_DropData.ptMouse = ptMouse;
	m_ptMousePosLast = ptMouse;

	CDUIControlBase *pOverControl = FindControlByDrop(ptMouse);

	//leave
	if (pOverControl != m_pWinDragEnterCtrl)
	{
		if (m_pWinDragEnterCtrl
			&& m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
		{
			m_pWinDragEnterCtrl->OnWinDragLeave(&m_DropData);
		}

		m_pWinDragEnterCtrl = NULL;
	}
	//enter
	if (pOverControl != m_pWinDragEnterCtrl && pOverControl)
	{
		m_pWinDragEnterCtrl = pOverControl;
		pOverControl->OnWinDragEnter(&m_DropData, &m_DropData.dwEffect);
	}
	//over
	if (m_pWinDragEnterCtrl)
	{
		m_pWinDragEnterCtrl->OnWinDragOver(&m_DropData, m_DropData.dwEffect);
	}

	*pdwEffect = m_DropData.dwEffect;

	return;
}

void CDUIWnd::OnWinDragLeave()
{
	if (NULL == m_pRootCtrl) return;

	m_DropData.ptMouse = m_ptMousePosLast;
	if (m_pWinDragEnterCtrl && m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
	{
		m_pWinDragEnterCtrl->OnWinDragLeave(&m_DropData);
	}
	for (auto& it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	m_pWinDragEnterCtrl = NULL;
	m_DropData = {};

	return;
}

void CDUIWnd::OnWinDrop(IDataObject *pIDataObject, POINTL pt, DWORD *pdwEffect)
{
	if (NULL == m_pRootCtrl) return;

	POINT ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWnd, &ptMouse);
	m_ptMousePosLast = ptMouse;
	m_DropData.pIDataObject = pIDataObject;
	m_DropData.ptMouse = ptMouse;

	//从外部拖拽时查询数据对象是否支持异步操作
	if (m_DropData.pDragDropFrom != this)
	{
		//0x602 win7 
#if (_WIN32_WINNT >= 0x602)
		IDataObjectAsyncCapability *pac = NULL;
#else
		IAsyncOperation *pac = NULL;
#endif

		BOOL bAsyncSupported = FALSE;
		/* Does the drop source support asynchronous copy */
		HRESULT hr = pIDataObject->QueryInterface(IID_PPV_ARGS(&pac));
		if (hr == S_OK && pac != NULL)
		{
			pac->GetAsyncMode(&bAsyncSupported);
			pac->Release();
			pac = NULL;
			m_DropData.bAsyncMode = (bAsyncSupported == TRUE);
		}
		else
		{
			m_DropData.bAsyncMode = false;
		}
	}

	//兼容从压缩包中直接拖拽文件，再parse下 
	m_DropData.vecDropFiles.clear();
	FORMATETC ftetc = { 0 };
	ftetc.lindex = -1;
	ftetc.dwAspect = DVASPECT_CONTENT;
	ftetc.cfFormat = CF_HDROP;
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM medium = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop)
		{
			UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
			if (cFiles > 0)
			{
				for (int i = 0; i < cFiles; i++)
				{
					TCHAR szFileName[MAX_PATH] = { 0 };
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					m_DropData.vecDropFiles.push_back(szFileName);
				}
			}
		}

		GlobalUnlock(medium.hGlobal);
		ReleaseStgMedium(&medium);
	}

	//notify
	if (m_pWinDragEnterCtrl && m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
	{
		m_pWinDragEnterCtrl->OnWinDrop(&m_DropData, pdwEffect);
	}

	m_pWinDragEnterCtrl = NULL;

	return;
}
#endif

bool CDUIWnd::IsDesigned()
{
	return m_bDesigned;
}

void CDUIWnd::SetDesigned(bool bDesigned)
{
	if (bDesigned == IsDesigned()) return;

	m_bDesigned = bDesigned;

	return;
}

LPVOID CDUIWnd::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWnd, Guid, dwQueryVer);

	return CDUIPropertyObject::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWnd::GetDescribe() const
{
	return Dui_Window;
}

HWND CDUIWnd::GetWndHandle() const
{
	return m_hWnd;
}

HDC CDUIWnd::GetWndDC()
{
	return m_hDCPaint;
}

CDUIWnd::operator HWND() const
{
	return m_hWnd;
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
	m_hWnd = ::CreateWindowEx(dwExStyle, GetClass(), lpszName, dwStyle, x, y, cx, cy, hWndParent, hMenu, hInstance, MMStaticPtr(CDUIWnd, this));
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

	//disable parent
	UINT nRet = 0;
	HWND hWndParent = GetWindowOwner(m_hWnd);
	bool bWndParentEnable = IsWindowEnabled(hWndParent);
	::EnableWindow(hWndParent, false);

	//show
	ShowWindow(true, false);

	//message
	BOOL bRet = 0;
	MSG Msg = {};
	while (::IsWindowVisible(m_hWnd) && (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0)
	{
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

	//show
	ShowWindow(true, false);

	//message
	UINT nRet = 0;
	BOOL bRet = 0;
	MSG Msg = {};
	while (::IsWindowVisible(m_hWnd) && (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0)
	{
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
		g_pIDuiWndNotify->OnDuiWndClose(this);
	}

	return;
}

void CDUIWnd::CenterWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT((GetWindowStyle(m_hWnd) & WS_CHILD) == 0);
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

void CDUIWnd::AdjustWndPos()
{
	if (false == IsWindow(m_hWnd)) return;

	CDUIRect rcWnd = GetWindowRect();
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
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

void CDUIWnd::SetCapture()
{
	::SetCapture(m_hWnd);
}

void CDUIWnd::ReleaseCapture()
{
	::ReleaseCapture();
}

bool CDUIWnd::IsCaptured()
{
	return ::GetCapture() == m_hWnd;
}

CDUIControlBase* CDUIWnd::GetCaptureControl()
{
	return m_pCaptureCtrl;
}

CDUIControlBase* CDUIWnd::GetFocusControl()
{
	return m_pFocusCtrl;
}

void CDUIWnd::SetFocusControl(CDUIControlBase *pFocusCtrl)
{
	if (m_pFocusCtrl == pFocusCtrl) return;

	//kill focus
	if (m_pFocusCtrl)
	{
		CDUIControlBase *pControl = m_pFocusCtrl;
		m_pFocusCtrl = NULL;
		pControl->OnDuiKillFocus();
	}

	//set focus
	m_pFocusCtrl = pFocusCtrl;
	if (m_pFocusCtrl)
	{
		m_pFocusCtrl->OnDuiSetFocus();
	}

	//focus child wnd
	MMInterfaceHelper(CDUIEditCtrl, m_pFocusCtrl, pEditFocus);
	MMInterfaceHelper(CDUIHotKeyCtrl, m_pFocusCtrl, pHotKeyFocus);

	if (m_pFocusCtrl
		&& GetFocus() != m_hWnd
		&& NULL == pEditFocus
		&& NULL == pHotKeyFocus)
	{
		::SetFocus(m_hWnd);
	}

	return;
}

CDUIControlBase * CDUIWnd::GetHoverCtrl()
{
	return m_pHoverCtrl;
}

bool CDUIWnd::AttachRootCtrl(CDUIContainerCtrl* pControl)
{
	if (NULL == pControl) return false;

	// Reset any previous attachment
	ReleaseRootCtrl();

	// Set the dialog root element
	m_pRootCtrl = pControl;

	//m_shadow.Create(this);
	m_mapControl.clear();

	// Initiate all control
	InitControls(pControl);

	//active tabpag
	for (auto GroupItem : m_mapRadioBoxGroup)
	{
		auto &vecRadioBox = GroupItem.second;

		for (int i = 0; i < vecRadioBox.size(); i++)
		{
			CDUIRadioBoxCtrl *pRadioBoxCtrl = vecRadioBox[i];
			if (NULL == pRadioBoxCtrl) continue;

			if (pRadioBoxCtrl->IsSelected())
			{
				pRadioBoxCtrl->PerformUnSelOther(false);
				pRadioBoxCtrl->PerformActiveBindTabPage();

				break;
			}
		}
	}

	CDUIGlobal::PerformNotifyChildAdd(this, m_pRootCtrl);

	NeedRefreshView();
	Invalidate();

	return true;
}

bool CDUIWnd::ReleaseRootCtrl()
{
	CDUIContainerCtrl *pRootCtrl = DetachRootCtrl();

	MMSafeDelete(pRootCtrl);

	return true;
}

CDUIContainerCtrl * CDUIWnd::GetRootCtrl()
{
	return m_pRootCtrl;
}

CDUIContainerCtrl * CDUIWnd::DetachRootCtrl()
{
	CDUIGlobal::PerformNotifyChildRemove(this, m_pRootCtrl);

	CDUIContainerCtrl *pRootCtrl = m_pRootCtrl;
	if (pRootCtrl)
	{
		pRootCtrl->SetWndOwner(NULL);
	}

	m_pRootCtrl = NULL;
	m_pCaptureCtrl = NULL;
	m_pFocusCtrl = NULL;
	m_pHoverCtrl = NULL;
	m_pEventCtrl = NULL;
	m_pWinDragCtrl = NULL;
	m_pWinDragEnterCtrl = NULL;

	m_ptMousePosLast = {};

	m_mapControl.clear();
	m_vecAsynNotify.clear();
	RemoveAllRadioBoxGroup();
	RemoveAllTimer();

	if (IsWindow(m_hWndTooltip)) DestroyWindow(m_hWndTooltip);

	m_hWndTooltip = NULL;

	return pRootCtrl;
}

bool CDUIWnd::InitControls(CDUIControlBase *pControl)
{
	ASSERT(pControl);
	if (NULL == pControl) return false;
	pControl->SetWndOwner(this);
	pControl->FindControl(__FindControlFromIDHash, this, DuiFind_All);

	return true;
}

void CDUIWnd::InitControlIDHash(CDUIControlBase *pControl)
{
	__FindControlFromIDHash(pControl, this);

	return;
}

void CDUIWnd::UnInitControlIDHash(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	m_mapControl.erase(pControl->GetCtrlID());

	return;
}

void CDUIWnd::ReapControl(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;
	if (pControl == m_pCaptureCtrl) m_pCaptureCtrl = NULL;
	if (pControl == m_pFocusCtrl) m_pFocusCtrl = NULL;
	if (pControl == m_pHoverCtrl) m_pHoverCtrl = NULL;
	if (pControl == m_pEventCtrl) m_pEventCtrl = NULL;
	if (pControl == m_pWinDragCtrl) m_pWinDragCtrl = NULL;
	if (pControl == m_pWinDragEnterCtrl) m_pWinDragEnterCtrl = NULL;

	m_mapControl.erase(pControl->GetCtrlID());

	KillTimer(pControl);

	for (int i = m_vecAsynNotify.size() - 1; i >= 0; i--)
	{
		DuiNotify &Notify = m_vecAsynNotify[i];
		if (Notify.pNotifyCtrl == pControl
			|| Notify.DuiNotifyExtend.TreeView.pTreeNode == pControl
			|| Notify.DuiNotifyExtend.TreeView.pRootNode == pControl
			|| Notify.DuiNotifyExtend.TreeView.pRootView == pControl)
		{
			m_vecAsynNotify.erase(m_vecAsynNotify.begin() + i);
		}
	}

	return;
}

bool CDUIWnd::VerifyControl(CDUIControlBase *pControl)
{
	if (NULL == pControl || NULL == m_pRootCtrl) return false;

	if (pControl == m_pRootCtrl) return true;

	return m_pRootCtrl->VerifyChild(pControl);
}

UINT_PTR CDUIWnd::GetTag()
{
	return m_AttributeUserTag.GetValue();
}

void CDUIWnd::SetTag(UINT_PTR pTag)
{
	if (pTag == m_AttributeUserTag.GetValue()) return;

	m_AttributeUserTag.SetValue(pTag);

	return;
}

bool CDUIWnd::SetTimer(CDUIPropertyObject *pPropObj, UINT uTimerID, UINT uElapse)
{
	ASSERT(pPropObj != NULL);
	ASSERT(uElapse > 0);
	if (NULL == pPropObj || 0 == uElapse) return false;

	for (auto &TimerInfo : m_vecTimers)
	{
		if (TimerInfo.pPropObj == pPropObj
			&& TimerInfo.hWnd == m_hWnd
			&& TimerInfo.nLocalID == uTimerID)
		{
			if (TimerInfo.bKilled == true)
			{
				if (::SetTimer(m_hWnd, TimerInfo.uWinTimer, uElapse, NULL))
				{
					TimerInfo.bKilled = false;
					return true;
				}

				return false;
			}

			return false;
		}
	}

	m_uTimerID = (++m_uTimerID) % 0xF0; //0xf1-0xfe特殊用途
	if (false == ::SetTimer(m_hWnd, m_uTimerID, uElapse, NULL)) return false;

	DuiTimerInfo TimerInfo;
	TimerInfo.hWnd = m_hWnd;
	TimerInfo.pPropObj = pPropObj;
	TimerInfo.pControl = MMInterfaceHelper(CDUIControlBase, pPropObj);
	TimerInfo.nLocalID = uTimerID;
	TimerInfo.uWinTimer = m_uTimerID;
	TimerInfo.bKilled = false;
	m_vecTimers.push_back(TimerInfo);

	return true;
}

bool CDUIWnd::FindTimer(CDUIPropertyObject *pPropObj, UINT uTimerID)
{
	ASSERT(pPropObj != NULL);
	if (NULL == pPropObj) return false;

	auto FindIt = find_if(m_vecTimers.begin(), m_vecTimers.end(), [&](DuiTimerInfo &TimerInfo)
	{
		return TimerInfo.pPropObj == pPropObj
			&& TimerInfo.hWnd == m_hWnd
			&& TimerInfo.nLocalID == uTimerID
			&& false == TimerInfo.bKilled;
	});

	return FindIt != m_vecTimers.end();
}

bool CDUIWnd::KillTimer(CDUIPropertyObject *pPropObj, UINT uTimerID)
{
	ASSERT(pPropObj != NULL);
	if (NULL == pPropObj) return false;

	for (auto &TimerInfo : m_vecTimers)
	{
		if (TimerInfo.pPropObj == pPropObj
			&& TimerInfo.hWnd == m_hWnd
			&& TimerInfo.nLocalID == uTimerID)
		{
			if (TimerInfo.bKilled == false)
			{
				if (::IsWindow(m_hWnd)) ::KillTimer(TimerInfo.hWnd, TimerInfo.uWinTimer);

				TimerInfo.bKilled = true;

				return true;
			}
		}
	}

	return false;
}

bool CDUIWnd::KillTimer(CDUIPropertyObject *pPropObj)
{
	ASSERT(pPropObj != NULL);
	if (NULL == pPropObj) return false;

	for (int i = m_vecTimers.size() - 1; i >= 0; i--)
	{
		DuiTimerInfo TimerInfo = (m_vecTimers[i]);

		if (TimerInfo.pPropObj == pPropObj)
		{
			if (false == TimerInfo.bKilled) ::KillTimer(TimerInfo.hWnd, TimerInfo.uWinTimer);

			m_vecTimers.erase(m_vecTimers.begin() + i);
		}
	}

	return false;
}

bool CDUIWnd::RemoveAllTimer()
{
	for (auto &TimerInfo : m_vecTimers)
	{
		if (TimerInfo.bKilled == false)
		{
			if (::IsWindow(m_hWnd)) ::KillTimer(m_hWnd, TimerInfo.uWinTimer);
		}
	}

	m_vecTimers.clear();

	return true;
}

bool CDUIWnd::AddRadioBoxToGroup(CDUIRadioBoxCtrl *pControl)
{
	if (pControl == NULL) return false;

	auto &vecRadioBox = m_mapRadioBoxGroup[pControl->GetGroupName()];
	for (int i = 0; i < vecRadioBox.size(); i++)
	{
		if (vecRadioBox[i] == pControl)
		{
			return false;
		}
	}

	vecRadioBox.push_back(pControl);

	return true;
}

VecDuiRadioBoxCtrl CDUIWnd::GetRadioBoxGroup(LPCTSTR lpszGroup)
{
	auto FindIt = m_mapRadioBoxGroup.find(lpszGroup);

	return FindIt != m_mapRadioBoxGroup.end() ? FindIt->second : VecDuiRadioBoxCtrl();
}

bool CDUIWnd::RemoveRadioBoxFromGroup(CDUIRadioBoxCtrl *pControl)
{
	auto FindIt = m_mapRadioBoxGroup.find(pControl->GetGroupName());
	if (FindIt == m_mapRadioBoxGroup.end()) return false;

	auto &vecRadioBox = FindIt->second;

	for (int i = 0; i < vecRadioBox.size(); i++)
	{
		if (vecRadioBox[i] == pControl)
		{
			vecRadioBox.erase(vecRadioBox.begin() + i);

			return true;
		}
	}

	return false;
}

void CDUIWnd::RemoveAllRadioBoxGroup()
{
	m_mapRadioBoxGroup.clear();

	return;
}

CDUIRadioBoxCtrl* CDUIWnd::GetRadioBoxOfBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl, long lBindIndex)
{
	if (pTabCtrl == NULL || lBindIndex < 0) return NULL;

	for (auto GroupItem : m_mapRadioBoxGroup)
	{
		VecDuiRadioBoxCtrl &vecRadioBox = GroupItem.second;
		if (vecRadioBox.empty()) continue;

		CDUIRadioBoxCtrl *pRadioBoxCtrl = vecRadioBox[0];
		if (NULL == pRadioBoxCtrl) continue;

		if (pRadioBoxCtrl->GetBindTabCtrl() != pTabCtrl) continue;

		for (int i = 0; i < vecRadioBox.size(); i++)
		{
			pRadioBoxCtrl = vecRadioBox[i];
			if (NULL == pRadioBoxCtrl) continue;

			if (pRadioBoxCtrl->GetBindTabCtrlIndex() == lBindIndex) return pRadioBoxCtrl;
		}
	}

	return NULL;
}

HWND CDUIWnd::GetToolTipWindow() const
{
	return m_hWndTooltip;
}

int CDUIWnd::GetToolTipHoverTime() const
{
	return m_nToolTipHoverTime;
}

void CDUIWnd::SetToolTipHoverTime(int nTime)
{
	m_nToolTipHoverTime = nTime;
}

void CDUIWnd::Invalidate()
{
	if (NULL == m_hWnd) return;

	InvalidateRect(m_hWnd, NULL, TRUE);

	return;
}

bool CDUIWnd::IsRefreshViewNeeded() const
{
	return m_bRefreshViewNeeded;
}

void CDUIWnd::NeedRefreshView()
{
	m_bRefreshViewNeeded = true;

	return;
}

POINT CDUIWnd::GetMousePosLast() const
{
	return m_ptMousePosLast;
}

POINT CDUIWnd::GetMousePosDown() const
{
	return m_ptMousePosDown;
}

SIZE CDUIWnd::GetClientSize() const
{
	RECT rcClient = {};
	::GetClientRect(m_hWnd, &rcClient);

	return CDUISize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

CDUIRect CDUIWnd::GetWindowRect()
{
	CDUIRect rcWnd;
	::GetWindowRect(m_hWnd, &rcWnd);

	return rcWnd;
}

SIZE CDUIWnd::GetWndInitSize()
{
	return GetDpiObj().Scale(m_AttributeWndInitSize.GetValue());
}

void CDUIWnd::SetWndInitSize(int cx, int cy)
{
	if (GetWndInitSize() == DuiDpiScaleWnd(CDUISize(cx, cy))) return;

	m_AttributeWndInitSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

SIZE CDUIWnd::GetWndMinSize()
{
	return GetDpiObj().Scale(m_AttributeWndMinSize.GetValue());
}

void CDUIWnd::SetWndMinSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	if (GetWndMinSize() == DuiDpiScaleWnd(CDUISize(cx, cy))) return;

	m_AttributeWndMinSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

SIZE CDUIWnd::GetWndMaxSize()
{
	return GetDpiObj().Scale(m_AttributeWndMaxSize.GetValue());
}

void CDUIWnd::SetWndMaxSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	if (GetWndMaxSize() == DuiDpiScaleWnd(CDUISize(cx, cy))) return;

	m_AttributeWndMaxSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

RECT CDUIWnd::GetResizeTrack()
{
	return m_AttributeWndResizeTrack.GetValue();
}

void CDUIWnd::SetResizeTrack(RECT &rcResizeTrack)
{
	if (GetResizeTrack() == rcResizeTrack) return;

	m_AttributeWndResizeTrack.SetValue(rcResizeTrack);

	return;
}

int CDUIWnd::GetCaptionHeight()
{
	return GetDpiObj().Scale(m_AttributeWndCaptionHeight.GetValue());
}

void CDUIWnd::SetCaptionHeight(int nHeight)
{
	if (DuiDpiScaleWnd(nHeight) == GetCaptionHeight()) return;

	m_AttributeWndCaptionHeight.SetValue(nHeight);

	return;
}

const CMMDpi & CDUIWnd::GetDpiObj()
{
	return m_DpiInfo;
}

int CDUIWnd::GetDpi()
{
	return GetDpiObj().GetDpi();
}

bool CDUIWnd::SetDpi(int nDpi)
{
	if (nDpi == GetDpi()) return true;

 	m_DpiInfo.SetDpi(nDpi);

	AdjustWndSize();
	AdjustWndPos();

	return true;
}

int CDUIWnd::GetScale()
{
	return GetDpiObj().GetScale();
}

bool CDUIWnd::SetScale(int nScale)
{
	if (nScale < 100) return false;

	SetDpi(MulDiv(nScale, 96, 100));

	return true;
}

bool CDUIWnd::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
{
	if (false == IsWindow(m_hWnd)) return false;
	if (false == ::CreateCaret(m_hWnd, hBmp, nWidth, nHeight)) return false;

	m_bCaretActive = false;
	m_rcCaret.right = m_rcCaret.left + nWidth;
	m_rcCaret.bottom = m_rcCaret.top + nHeight;

	return true;
}

bool CDUIWnd::IsShowCaret()
{
	return m_bCaretActive;
}

void CDUIWnd::ShowCaret(bool bShow)
{
	if (bShow == IsShowCaret()) return;

	m_bCaretActive = bShow;

	if (IsShowCaret())
	{
		::ShowCaret(GetWndHandle());

		Invalidate();

		return;
	}

	::HideCaret(GetWndHandle());

	Invalidate();

	return;
}

void CDUIWnd::SetCaretPos(CDUIPoint pt)
{
	if (false == ::SetCaretPos(pt.x, pt.y)) return;

	m_rcCaret.Offset(pt.x - m_rcCaret.left, pt.y - m_rcCaret.top);

	return;
}

BYTE CDUIWnd::GetWndAlpha()
{
	return m_AttributeWndAlpha.GetValue();
}

void CDUIWnd::SetWndAlpha(BYTE cbWndAlpha)
{
	if (cbWndAlpha == GetWndAlpha()) return;

	cbWndAlpha = min(255, max(0, cbWndAlpha));
	m_AttributeWndAlpha.SetValue(cbWndAlpha);

	SetWndLayered(true);

	Invalidate();

	return;
}

bool CDUIWnd::IsWndLayered()
{
	return m_AttributeWndLayered.GetValue();
}

void CDUIWnd::SetWndLayered(bool bLayered)
{
	m_AttributeWndLayered.SetValue(bLayered);

	return;
}

bool CDUIWnd::IsGdiplusRenderText()
{
	return m_AttributeGdiplusRenderText.GetValue();
}

void CDUIWnd::SetGdiplusRenderText(bool bGdiplus)
{
	if (bGdiplus == IsGdiplusRenderText()) return;

	m_AttributeGdiplusRenderText.SetValue(bGdiplus);

	Invalidate();

	return;
}

Gdiplus::TextRenderingHint CDUIWnd::GetGdiplusRenderTextType()
{
	return (Gdiplus::TextRenderingHint)m_AttributeGdiplusRenderType.GetCurSelItem();
}

void CDUIWnd::SetGdiplusRenderTextType(Gdiplus::TextRenderingHint RenderType)
{
	if (RenderType == GetGdiplusRenderTextType()) return;

	m_AttributeGdiplusRenderType.SelectItem(RenderType);

	Invalidate();

	return;
}

bool CDUIWnd::IsGdiplusRenderImage()
{
	return m_AttributeGdiplusRenderImage.GetValue();
}

void CDUIWnd::SetGdiplusRenderImage(bool bGdiplus)
{
	if (bGdiplus == IsGdiplusRenderImage()) return;

	m_AttributeGdiplusRenderImage.SetValue(bGdiplus);

	Invalidate();

	return;
}

enDuiAnimateWndType CDUIWnd::GetAnimateWndType()
{
	return (enDuiAnimateWndType)m_AttributeAnimationType.GetCurSelItem();
}

void CDUIWnd::SetAnimateWndType(enDuiAnimateWndType AnimateWndType)
{
	if (AnimateWndType == GetAnimateWndType()) return;

	m_AttributeAnimationType.SelectItem(AnimateWndType);

	return;
}

int CDUIWnd::GetAnimateWndFrame()
{
	return m_AttributeAnimationFrame.GetValue();
}

void CDUIWnd::SetAnimateWndFrame(int nFrame)
{
	if (nFrame == GetAnimateWndFrame()) return;

	m_AttributeAnimationFrame.SetValue(nFrame);

	return;
}

int CDUIWnd::GetAnimateWndElapse()
{
	return m_AttributeAnimationElapse.GetValue();
}

void CDUIWnd::SetAnimateWndElapse(int nElapse)
{
	if (nElapse == GetAnimateWndElapse()) return;

	m_AttributeAnimationElapse.SetValue(nElapse);

	return;
}

bool CDUIWnd::AddINotify(IDuiNotify *pINotify)
{
	if (NULL == pINotify) return false;
	if (find(m_vecINotify.begin(), m_vecINotify.end(), pINotify) != m_vecINotify.end()) return true;

	m_vecINotify.push_back(pINotify);

	return true;
}

bool CDUIWnd::RemoveINotify(IDuiNotify *pINotify)
{
	m_vecINotify.erase(std::remove(m_vecINotify.begin(), m_vecINotify.end(), pINotify), m_vecINotify.end());

	return true;
}

void CDUIWnd::SendNotify(DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl) return;

	CDUIWnd *pWnd = Notify.pNotifyCtrl->GetWndOwner();
	if (NULL == pWnd) return;

	Notify.uCtrlID = Notify.pNotifyCtrl->GetCtrlID();
	Notify.ptMouse = m_ptMousePosLast;
	Notify.wKeyState = MapKeyState();
	Notify.dwTimestamp = ::GetTickCount();

	int nCount = Notify.pNotifyCtrl->GetControlCallBackCount();
	for (int n = 0; n < nCount; n++)
	{
		if (false == pWnd->VerifyControl(Notify.pNotifyCtrl)) return;

		IDuiControlCallBack *pICallBack = Notify.pNotifyCtrl->GetControlCallBack(n);
		if (NULL == pICallBack) continue;

		pICallBack->OnNotify(Notify.pNotifyCtrl, Notify);
	}
	for (int n = 0; n < m_vecINotify.size(); n++)
	{
		IDuiNotify *pINotify = m_vecINotify[n];
		if (NULL == pINotify) continue;

		pINotify->OnNotify(Notify);
	}

	return;
}

void CDUIWnd::SendNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (NULL == pControl) return;

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = pControl;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	SendNotify(Notify);

	return;
}

void CDUIWnd::PostNotify(DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl || this != Notify.pNotifyCtrl->GetWndOwner()) return;

	Notify.ptMouse = m_ptMousePosLast;
	Notify.wKeyState = MapKeyState();
	Notify.dwTimestamp = ::GetTickCount();
	m_vecAsynNotify.push_back(Notify);

	PostAppMsg();

	return;
}

void CDUIWnd::PostNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam/* = 0*/, LPARAM lParam/* = 0*/)
{
	if (NULL == pControl) return;

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = pControl;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	PostNotify(Notify);

	return;
}

CDUIControlBase * CDUIWnd::FindControl(POINT pt) const
{
	if (NULL == m_pRootCtrl) return NULL;

	return m_pRootCtrl->FindControl(__FindControlFromPoint, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWnd::FindControl(UINT uCtrlID) const
{
	auto FindIt = m_mapControl.find(uCtrlID);
	return FindIt != m_mapControl.end() ? static_cast<CDUIControlBase*>(FindIt->second) : NULL;
}

CDUIControlBase * CDUIWnd::FindControlByShortCut(TCHAR chChar) const
{
	if (NULL == m_pRootCtrl) return NULL;

	DuiFindShortCut FindShortCut = {};
	FindShortCut.chChar = toupper((int)chChar);

	UINT uFlag = DuiFind_Enabled | DuiFind_MeFirst | DuiFind_TopFirst | DuiFind_Visible;
	return m_pRootCtrl->FindControl(__FindControlFromShortcut, &FindShortCut, uFlag);
}

CDUIControlBase * CDUIWnd::FindControlByDrop(POINT pt) const
{
	if (NULL == m_pRootCtrl) return NULL;

	return m_pRootCtrl->FindControl(__FindControlFromDrop, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWnd::FindSubControlByPoint(CDUIContainerCtrl *pParent, POINT pt)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return NULL;

	return pParent->FindControl(__FindControlFromPoint, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWnd::FindSubControlByID(CDUIContainerCtrl *pParent, UINT uCtrlID)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return NULL;

	return pParent->FindControl(__FindControlFromID, (LPVOID)uCtrlID, DuiFind_All);
}

VecDuiControlBase CDUIWnd::FindSubControlsByClass(CDUIContainerCtrl *pParent, LPCTSTR pstrClass)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return {};

	m_vecFoundControls.clear();
	pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, DuiFind_All);

	return m_vecFoundControls;
}

bool CDUIWnd::AddPreMessagePtr(IDuiPreMessage *pInterface)
{
	if (find(m_vecPreMessage.begin(), m_vecPreMessage.end(), pInterface) != m_vecPreMessage.end()) return true;

	m_vecPreMessage.push_back(pInterface);

	return true;
}

bool CDUIWnd::RemovePreMessagePtr(IDuiPreMessage *pInterface)
{
	m_vecPreMessage.erase(std::remove(m_vecPreMessage.begin(), m_vecPreMessage.end(), pInterface), m_vecPreMessage.end());

	return true;
}

bool CDUIWnd::BeginDragDrop(CDUIControlBase *pControl, WPARAM wParam, LPARAM lParam, int nFlag)
{
	if (NULL == pControl) return false;

	m_pWinDragCtrl = pControl;
	m_DropData.wParam = wParam;
	m_DropData.lParam = lParam;
	m_DropData.pDragDropFrom = this;

	CDUIRect rcCtrl = m_pWinDragCtrl->GetAbsoluteRect();

	//1、先获取DataObject
	CComPtr<IDataObject> pIDataObject = nullptr;
	CComPtr<IDropSource> pIDropSource = nullptr;
	SendNotify(m_pWinDragCtrl, DuiNotify_WinDragCreateObj, (WPARAM)&pIDataObject);
	if (NULL == pIDataObject)
	{
		CMMDropSource *pDropSource = new CMMDropSource;
		if (NULL == pDropSource) return false;

		pIDataObject = new CMMDataObject((CMMDropSource*)pDropSource);
		if (NULL == pIDataObject) return false;

		pIDropSource = pDropSource;
	}

	//bitmap
	HBITMAP hBitmapDrag = NULL;
	CDUIPoint ptOffset = {};
	SendNotify(m_pWinDragCtrl, DuiNotify_WinDragCreateBmp, (WPARAM)&hBitmapDrag, (LPARAM)&ptOffset);
	if (NULL == hBitmapDrag)
	{
		hBitmapDrag = CDUIRenderEngine::GenerateBitmap(this, m_pWinDragCtrl, rcCtrl);
		ptOffset = { rcCtrl.GetWidth() / 2, rcCtrl.GetHeight() / 2 };
	}

	CComPtr<IDragSourceHelper2> pIDragSourceHelper2 = nullptr;
	if (SUCCEEDED(GetDragDropHelper(IID_PPV_ARGS(&pIDragSourceHelper2))))
	{
		BITMAP bmap;
		GetObject(hBitmapDrag, sizeof(BITMAP), &bmap);

		SHDRAGIMAGE di = {};
		di.hbmpDragImage = hBitmapDrag;
		di.sizeDragImage.cx = bmap.bmWidth;
		di.sizeDragImage.cy = bmap.bmHeight;
		di.ptOffset.x = ptOffset.x;
		di.ptOffset.y = ptOffset.y;
		pIDragSourceHelper2->SetFlags(DSH_ALLOWDROPDESCRIPTIONTEXT);
		pIDragSourceHelper2->InitializeFromBitmap(&di, pIDataObject);
	}

	DWORD dwEffectResult;
	HRESULT hRes = ::SHDoDragDrop(NULL, pIDataObject, NULL, nFlag, &dwEffectResult);
	EndDragDrop();

	MMSafeDeleteObject(hBitmapDrag);

	return true;
}

void CDUIWnd::EndDragDrop()
{
	SendNotify(m_pRootCtrl, DuiNotify_WinDragFinish);

	for (auto &it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	::ReleaseCapture();

	m_DropData = {};
	m_pWinDragCtrl = NULL;
	m_pCaptureCtrl = NULL;

	return;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
void CDUIWnd::SetDropDescription(DROPIMAGETYPE dwDropType, PCWSTR pszMsg, PCWSTR pszInsert)
{
	SetDropTip(m_DropData.pIDataObject, dwDropType, pszMsg, pszInsert);
}
#endif

void CDUIWnd::ClearDescription()
{
	SetDropTip(m_DropData.pIDataObject, DROPIMAGE_INVALID, L"", NULL);
}

const tagDuiDropData * CDUIWnd::GetDropData()
{
	return &m_DropData;
}

HBITMAP CDUIWnd::ProductBitmap(IN std::vector<CDUIControlBase*> vecCtrl, IN CDUIControlBase* pFromCtrl, OUT CDUIRect &rcBitmap, OUT CDUIPoint &ptFromOffset)
{
	for (auto pCtrl : vecCtrl)
	{
		if (NULL == pCtrl) continue;

		CDUIRect rcItem = pCtrl->GetAbsoluteRect();
		::UnionRect(&rcBitmap, &rcItem, &rcBitmap);
	}

	HDC hPaintDC = ::CreateCompatibleDC(GetWndDC());
	HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(GetWndDC(), rcBitmap.GetWidth(), rcBitmap.GetHeight());
	ASSERT(hPaintDC);
	ASSERT(hPaintBitmap);
	HBITMAP hPaintBitmapOld = (HBITMAP)::SelectObject(hPaintDC, hPaintBitmap);

	for (auto pCtrl : vecCtrl)
	{
		if (NULL == pCtrl) continue;

		HBITMAP hBitmap = CDUIRenderEngine::GenerateBitmap(this, pCtrl, pCtrl->GetAbsoluteRect());
		if (NULL == hBitmap) continue;

		CDUIRect rcPos = pCtrl->GetAbsoluteRect();
		rcPos.Offset(-rcBitmap.left, -rcBitmap.top);

		Bitmap *pBmp = CDUIRenderEngine::GetAlphaBitmap(hBitmap);
		CDUIRenderEngine::DrawImage(hPaintDC, pBmp, rcPos);
		DeleteObject(hBitmap);
		MMSafeDelete(pBmp);

		if (pCtrl == pFromCtrl)
		{
			ptFromOffset = { rcPos.left + rcPos.GetWidth() / 2, rcPos.top + rcPos.GetHeight() / 2 };
		}
	}

	SelectObject(hPaintDC, hPaintBitmapOld);
	DeleteDC(hPaintDC);

	return hPaintBitmap;
}

void CDUIWnd::InitProperty()
{
	DuiCreateGroupAttribute(m_AttributeGroupWndInfo, _T("WndInfo"));
	DuiCreateAttribute(m_AttributeWndCaptionHeight, _T("WndCaptionHeight"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndInitSize, _T("WndInitSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndMinSize, _T("WndMinSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndMaxSize, _T("WndMaxSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndResizeTrack, _T("WndResizeTrack"), _T(""), m_AttributeGroupWndInfo);

	DuiCreateGroupAttribute(m_AttributeGroupLayered, _T("WndLayered"));
	DuiCreateAttribute(m_AttributeWndLayered, _T("WndLayered"), _T(""), m_AttributeGroupLayered);
	DuiCreateAttribute(m_AttributeWndAlpha, _T("WndAlpha"), _T(""), m_AttributeGroupLayered);

	DuiCreateGroupAttribute(m_AttributeGroupRenderText, _T("RenderText"));
	DuiCreateAttribute(m_AttributeGdiplusRenderText, _T("GdiplusRenderText"), _T(""), m_AttributeGroupRenderText);
	DuiCreateAttribute(m_AttributeGdiplusRenderType, _T("GdiplusRenderType"), _T(""), m_AttributeGroupRenderText);

	DuiCreateGroupAttribute(m_AttributeGroupRenderImage, _T("RenderImage"));
	DuiCreateAttribute(m_AttributeGdiplusRenderImage, _T("GdiplusRenderImage"), _T(""), m_AttributeGroupRenderImage);

	DuiCreateGroupAttribute(m_AttributeGroupAnimation, _T("Animation"));
	DuiCreateAttribute(m_AttributeAnimationType, _T("AnimationType"), _T("please set window layered style, or the animation is not perfect"), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeAnimationFrame, _T("AnimationFrame"), _T("Window Animation TotalFrame"), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeAnimationElapse, _T("AnimationElapse"), _T("Window Animation Elapse Of One Frame"), m_AttributeGroupAnimation);

	DuiCreateGroupAttribute(m_AttributeGroupCustom, _T("CustomData"));
	DuiCreateAttribute(m_AttributeUserTag, _T("UserTag"), _T(""), m_AttributeGroupCustom);

	//value
	if (false == m_AttributeGdiplusRenderType.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ TextRenderingHintSystemDefault, _T("HintSystemDefault") });
		AttriCombox.vecItem.push_back({ TextRenderingHintSingleBitPerPixelGridFit, _T("HintSingleBitPerPixelGridFit") });
		AttriCombox.vecItem.push_back({ TextRenderingHintSingleBitPerPixel, _T("HintSingleBitPerPixel") });
		AttriCombox.vecItem.push_back({ TextRenderingHintAntiAliasGridFit, _T("HintAntiAliasGridFit") });
		AttriCombox.vecItem.push_back({ TextRenderingHintAntiAlias, _T("HintAntiAlias") });
		AttriCombox.vecItem.push_back({ TextRenderingHintClearTypeGridFit, _T("HintClearTypeGridFit") });
		m_AttributeGdiplusRenderType.SetCombox(AttriCombox);
		m_AttributeGdiplusRenderType.SelectItem(TextRenderingHintSystemDefault);
	}
	if (false == m_AttributeAnimationType.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ AnimateWnd_None, _T("None") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Left, _T("Left") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftDiffuse, _T("LeftDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Right, _T("Right") });
		AttriCombox.vecItem.push_back({ AnimateWnd_RightDiffuse, _T("RightDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Top, _T("Top") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopDiffuse, _T("TopDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Bottom, _T("Bottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_BottomDiffuse, _T("BottomDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Size, _T("Size") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_SizeSpring, _T("SizeSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_SizeDiffuse, _T("SizeDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_CenterDiffuse, _T("CenterDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightDiffuse, _T("LeftRightDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightExpand, _T("LeftRightExpand") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_LeftRightExpandSpring, _T("LeftRightExpandSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightCombin, _T("LeftRightCombin") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomDiffuse, _T("TopBottomDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomExpand, _T("TopBottomExpand") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_TopBottomExpandSpring, _T("TopBottomExpandSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomCombin, _T("TopBottomCombin") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateLeftRight, _T("3DRotateLeftRight") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateTopBottom, _T("3DRotateTopBottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundLeft, _T("3DRoundLeft") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundRight, _T("3DRoundRight") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundTop, _T("3DRoundTop") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundBottom, _T("3DRoundBottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateReplaseLR, _T("3DRotateReplaseLR") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateReplaseTB, _T("3DRotateReplaseTB") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundReplaseLR, _T("3DRoundReplaseLR") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundReplaseTB, _T("3DRoundReplaseTB") });
		m_AttributeAnimationType.SetCombox(AttriCombox);
		m_AttributeAnimationType.SelectItem(AnimateWnd_None);
	}

	return;
}

void CDUIWnd::InitComplete()
{
	__super::InitComplete();

	return;
}

bool CDUIWnd::SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild)
{
	if (NULL == pNode || NULL == pNode->GetDocument()) return false;

	if (false == __super::SaveAttribute(pNode)) return false;

	//控件
	if (m_pRootCtrl)
	{
		tinyxml2::XMLElement *pNodeCtrl = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(m_pRootCtrl->GetClass()));
		if (!pNodeCtrl) return true;

		m_pRootCtrl->SaveAttribute(pNodeCtrl);

		pNode->LinkEndChild(pNodeCtrl);
	}

	return true;
}

bool CDUIWnd::SetTimer(UINT uTimerID, UINT nElapse)
{
	return SetTimer(this, uTimerID, nElapse);
}

bool CDUIWnd::KillTimer(UINT uTimerID)
{
	return KillTimer(this, uTimerID);
}

bool CDUIWnd::KillTimer()
{
	return KillTimer(this);
}

LRESULT CDUIWnd::OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	//pre message
	bHandled = false;
	for (auto pPreMessage : m_vecPreMessage)
	{
		if (NULL == pPreMessage)
		{
			assert(false);
			break;
		}

		pPreMessage->OnPreWndMessage(uMsg, wParam, lParam, bHandled);

		if (bHandled)
		{
			return 0;
		}
	}

	//message
	switch (uMsg)
	{
		case WM_SYSCHAR:
		{
			// Handle ALT-shortcut key-combinations
			CDUIControlBase *pControl = FindControlByShortCut(wParam);
			if (pControl)
			{
				pControl->SetFocus();
				pControl->Active();
			}

			break;
		}
		case WM_KEYDOWN:
		{
			if (VK_ESCAPE == wParam)
			{
				bHandled = TRUE;

				Close(Dui_CtrlIDInner_BtnCancel);
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

LRESULT CDUIWnd::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//notify
	DispatchNotifyAsyn();

	//delaydelete
	OnDuiDelayDelete();

	LRESULT lRes = 0;
	switch (uMsg)
	{
		case WM_DUIAPP:
		{
			m_bPostedAppMsg = false;

			return 0;
		}
		case WM_CREATE:
		{
			return OnCreate(wParam, lParam);
		}
		case WM_CLOSE:
		{
			return OnClose(wParam, lParam);
		}
		case WM_DESTROY:
		{
			return OnDestroy(wParam, lParam);
		}
#if defined(WIN32) && !defined(UNDER_CE)
		case WM_NCACTIVATE:
		{
			return OnNcActivate(wParam, lParam);
		}
		case WM_NCCALCSIZE:
		{
			return OnNcCalcSize(wParam, lParam);
		}
		case WM_NCPAINT:
		{
			return OnNcPaint(wParam, lParam);
		}
		case WM_NCHITTEST:
		{
			return OnNcHitTest(wParam, lParam);
		}
		case WM_GETMINMAXINFO:
		{
			return OnGetMinMaxInfo(wParam, lParam);
		}
#endif
		case WM_EXITSIZEMOVE:
		{
			return OnExitSizeMove(wParam, lParam);
		}
		case WM_SYSCOMMAND:
		{
			return OnSysCommand(wParam, lParam);
		}
		case WM_LBUTTONDOWN:
		{
			return OnLButtonDown(wParam, lParam);
		}
		case WM_LBUTTONUP:
		{
			return OnLButtonUp(wParam, lParam);
		}
		case WM_LBUTTONDBLCLK:
		{
			return OnLButtonDlk(wParam, lParam);
		}
		case WM_RBUTTONDOWN:
		{
			return OnRButtonDown(wParam, lParam);
		}
		case WM_RBUTTONUP:
		{
			return OnRButtonUp(wParam, lParam);
		}
		case WM_RBUTTONDBLCLK:
		{
			return OnRButtonDlk(wParam, lParam);
		}
		case WM_MOUSEMOVE:
		{
			return OnMouseMove(wParam, lParam);
		}
		case WM_MOUSEHOVER:
		{
			return OnMouseHover(wParam, lParam);
		}
		case WM_MOUSELEAVE:
		{
			return OnMouseLeave(wParam, lParam);
		}
		case WM_MOUSEWHEEL:
		{
			return OnMouseWheel(wParam, lParam);
		}
		case WM_TIMER:
		{
			return OnTimer(wParam, lParam);
		}
		case WM_SIZE:
		{
			return OnSize(wParam, lParam);
		}
		case WM_KEYDOWN:
		{
			return OnKeyDown(wParam, lParam);
		}
		case WM_KEYUP:
		{
			return OnKeyUp(wParam, lParam);
		}
		case WM_CHAR:
		{
			return OnChar(wParam, lParam);
		}
		case WM_SYSKEYDOWN:
		{
			return OnSysKeyDown(wParam, lParam);
		}
		case WM_SYSKEYUP:
		{
			return OnSysKeyUp(wParam, lParam);
		}
		case WM_SYSCHAR:
		{
			return OnSysChar(wParam, lParam);
		}
		case WM_PAINT:
		{
			CDUIRect rcPaint;
			bool bUpdate = ::GetUpdateRect(m_hWnd, &rcPaint, FALSE);

#ifdef DUI_DESIGN
			bUpdate = true;
			if (rcPaint.Empty())
			{
				GetClientRect(m_hWnd, &rcPaint);
			}
#endif

			if (bUpdate)
			{
				PAINTSTRUCT ps = {};
				::BeginPaint(m_hWnd, &ps);

				lRes = OnPaint(rcPaint);

				::EndPaint(m_hWnd, &ps);
			}

			//design refresh
			if (g_pIDuiWndNotify)
			{
				g_pIDuiWndNotify->OnDuiWndPaint(this);
			}

			return 0;
		}
		case WM_ERASEBKGND:
		{
			// We'll do the painting here...
			return 1;
		}
		case WM_CONTEXTMENU:
		{
			return OnContextMenu(wParam, lParam);
		}
		case WM_SETFOCUS:
		{
			return OnSetFocus(wParam, lParam);
		}
		case WM_KILLFOCUS:
		{
			return OnKillFocus(wParam, lParam);
		}
		case WM_SETCURSOR:
		{
			return OnSetCursor(wParam, lParam);
		}
		case WM_DPICHANGED:
		{
			return OnDpiChanged(wParam, lParam);
		}
		case WM_NOTIFY:
		{
			LPNMHDR lpNMHDR = (LPNMHDR)lParam;
			if (lpNMHDR != NULL) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
			
			return lRes;
		}
		case WM_COMMAND:
		{
			if (lParam == 0)
			{
				return OnCommand(wParam, lParam);
			}

			HWND hWndChild = (HWND)lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			
			return lRes;
		}
		case WM_IME_COMPOSITION:
		{
			return OnImeComPosition(wParam, lParam);
		}
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
			if (lParam == 0) break;

			HWND hWndChild = (HWND)lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			
			return lRes;
		}
	}

	return OnOldWndProc(uMsg, wParam, lParam);
}

LRESULT CDUIWnd::OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_OldWndProc) return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);

	return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CDUIWnd::OnCreate(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = OnOldWndProc(WM_CREATE, wParam, lParam);

	//modify
	LONG lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle &= ~WS_CAPTION;
	//lStyle &= ~WS_SYSMENU;
	::SetWindowLong(m_hWnd, GWL_STYLE, lStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	//root
	CDUIContainerCtrl *pRootCtrl = dynamic_cast<CDUIContainerCtrl*>(CDUIGlobal::GetInstance()->LoadDui(GetDuiName(), this));
	if (NULL == pRootCtrl)
	{
		OutputDebugString(CDUIGlobal::GetInstance()->GetDuiLastError());
	}

	//init
	CMMDragDrop::UnRegister();
	m_hDCPaint = ::GetDC(m_hWnd);
	m_uTimerID = 0x1000;
	m_bMouseTracking = false;
	m_bRefreshToolTip = false;
	m_bRefreshViewNeeded = false;
	m_bPostedAppMsg = false;
	m_bFirstLayout = true;

	AdjustWndSize();
	AddINotify(this);
	AttachRootCtrl(pRootCtrl);

	//center wnd
	if (CW_USEDEFAULT == m_ptCreate.x
		&& CW_USEDEFAULT == m_ptCreate.y
		&& 0 == (lStyle & WS_CHILD))
	{
		CenterWindow();
	}

	//max restore
	CDUIControlBase *pBtnMax = FindControl(Dui_CtrlIDInner_BtnMax);
	CDUIControlBase *pBtnRestore = FindControl(Dui_CtrlIDInner_BtnRestore);
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
		g_pIDuiWndNotify->OnDuiWndCreate(this);
	}

	return lRes;
}

LRESULT CDUIWnd::OnClose(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_CLOSE, wParam, lParam);
}

LRESULT CDUIWnd::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndClose(this);
	}

	return OnOldWndProc(WM_DESTROY, wParam, lParam);
}

LRESULT CDUIWnd::OnNcActivate(WPARAM wParam, LPARAM lParam)
{
	if (false == ::IsIconic(m_hWnd)) return (wParam == 0) ? TRUE : FALSE;

	return OnOldWndProc(WM_NCACTIVATE, wParam, lParam);
}

LRESULT CDUIWnd::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{
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
	{	
		// 最大化时，计算当前显示器最适合宽高度
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

LRESULT CDUIWnd::OnNcPaint(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWnd::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {};
	GetCursorPos(&pt);
	::ScreenToClient(m_hWnd, &pt);

	CDUIRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	if (false == ::IsZoomed(m_hWnd) && rcClient.PtInRect(pt))
	{
		RECT rcSizeBox = GetResizeTrack();
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

	CDUIRect rcCaption = { 0, 0, rcClient.right, GetCaptionHeight() };
	if (rcCaption.PtInRect(pt))
	{
		CDUIControlBase *pControl = FindControl(pt);
		if (NULL == pControl
			|| (NULL == MMInterfaceHelper(CDUIButtonCtrl, pControl) && NULL == MMInterfaceHelper(CDUIProgressCtrl, pControl)))
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CDUIWnd::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
{
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

	lpMMI->ptMaxTrackSize.x = GetWndMaxSize().cx == 0 ? rcWork.GetWidth() : GetWndMaxSize().cx;
	lpMMI->ptMaxTrackSize.y = GetWndMaxSize().cy == 0 ? rcWork.GetHeight() : GetWndMaxSize().cy;

	lpMMI->ptMinTrackSize.x = GetWndMinSize().cx;
	lpMMI->ptMinTrackSize.y = GetWndMinSize().cy;

	return 0;
}

LRESULT CDUIWnd::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_EXITSIZEMOVE, wParam, lParam);
}

LRESULT CDUIWnd::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
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
		CDUIControlBase *pBtnMax = FindControl(Dui_CtrlIDInner_BtnMax);
		CDUIControlBase *pBtnRestore = FindControl(Dui_CtrlIDInner_BtnRestore);

		if (pBtnMax && pBtnRestore)
		{
			pBtnMax->SetVisible(false == ::IsZoomed(m_hWnd));
			pBtnRestore->SetVisible(::IsZoomed(m_hWnd));
		}
	}
#else
	LRESULT lRes = OnOldWndProc(WM_SYSCOMMAND, wParam, lParam);
#endif

	return lRes;
}

LRESULT CDUIWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;
	m_ptMousePosDown = m_ptMousePosLast;

	::SetCapture(m_hWnd);

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonDown(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiLButtonDown, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_LBUTTONDOWN, wParam, lParam);
}

LRESULT CDUIWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	::ReleaseCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	m_pCaptureCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonUp(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiLButtonUp, pt, DuiMsg, true);
	}

	m_pCaptureCtrl = NULL;

	return OnOldWndProc(WM_LBUTTONUP, wParam, lParam);
}

LRESULT CDUIWnd::OnLButtonDlk(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	::SetCapture(m_hWnd);

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonDlk(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiLButtonDlk, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_LBUTTONDBLCLK, wParam, lParam);
}

LRESULT CDUIWnd::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;
	m_ptMousePosDown = m_ptMousePosLast;

	::SetCapture(m_hWnd);

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonDown(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiRButtonDown, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_RBUTTONDOWN, wParam, lParam);
}

LRESULT CDUIWnd::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	::ReleaseCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	m_pCaptureCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonUp(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiRButtonUp, pt, DuiMsg, true);
	}

	m_pCaptureCtrl = NULL;

	return OnOldWndProc(WM_RBUTTONUP, wParam, lParam);
}

LRESULT CDUIWnd::OnRButtonDlk(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//查找
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonDlk(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiRButtonDlk, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_RBUTTONDBLCLK, wParam, lParam);
}

LRESULT CDUIWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	do
	{
		CDUIPoint pt(lParam);
		m_ptMousePosLast.x = pt.x;
		m_ptMousePosLast.y = pt.y;

		//msg
		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.ptMouse = pt;

		//capture
		if (m_pCaptureCtrl)
		{
			DuiMsg.pMsgCtrl = m_pCaptureCtrl;
			m_pCaptureCtrl->OnDuiMouseMove(pt, DuiMsg);

			//model
			Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiMouseMove, pt, DuiMsg, false);

			if (NULL == m_pWinDragCtrl
				&& (abs(m_ptMousePosLast.x - m_ptMousePosDown.x) >= 4 || abs(m_ptMousePosLast.y - m_ptMousePosDown.y) >= 4))
			{
				if (m_pCaptureCtrl->IsWinDragEnabled())
				{
					m_pCaptureCtrl->OnWinDragStart(pt, DuiMsg);

					//model
					Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnWinDragStart, pt, DuiMsg, false);
				}
				else if ((NULL == MMInterfaceHelper(CDUIThinkEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
					&& (NULL == MMInterfaceHelper(CDUIEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
					&& (NULL == MMInterfaceHelper(CDUIRichEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
					&& m_pCaptureCtrl->GetOwnerModelCtrl()
					&& m_pCaptureCtrl->GetOwnerModelCtrl()->IsWinDragEnabled())
				{
					m_pCaptureCtrl->GetOwnerModelCtrl()->OnWinDragStart(pt, DuiMsg);
				}
			}

			break;
		}

		//tooltip
		if (false == m_bMouseTracking)
		{
			TRACKMOUSEEVENT tme = {};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			tme.dwHoverTime = NULL == m_hWndTooltip ? m_nToolTipHoverTime : (DWORD)::SendMessage(m_hWndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
			BOOL bRes = _TrackMouseEvent(&tme);
			m_bMouseTracking = true;
		}

		CDUIControlBase *pControl = FindSubControlByPoint(m_pRootCtrl, pt);

		//hover
		if (pControl == m_pHoverCtrl && m_pHoverCtrl)
		{
			DuiMsg.pMsgCtrl = m_pHoverCtrl;
			DuiMsg.pMsgCtrl->OnDuiMouseMove(pt, DuiMsg);

			//model
			Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseMove, pt, DuiMsg, false);

			break;
		}

		//leave
		if (m_pHoverCtrl)
		{
			DuiMessage msgLeave = {};
			msgLeave.wParam = DuiMsg.wParam;
			msgLeave.lParam = DuiMsg.lParam;
			msgLeave.pMsgCtrl = m_pHoverCtrl;
			m_pHoverCtrl->OnDuiMouseLeave(pt, msgLeave);
			Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseLeave, pt, msgLeave, false);

			if (m_hWndTooltip)
			{
				::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, false, (LPARAM)&m_ToolTip);
			}
		}

		//hover
		m_pHoverCtrl = pControl;
		m_bRefreshToolTip = true;

		if (m_pHoverCtrl)
		{
			//enter
			DuiMsg.pMsgCtrl = pControl;
			m_pHoverCtrl->OnDuiMouseEnter(pt, DuiMsg);
			Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseEnter, pt, DuiMsg, false);

			//move
			DuiMsg.pMsgCtrl = pControl;
			m_pHoverCtrl->OnDuiMouseMove(pt, DuiMsg);
			Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseMove, pt, DuiMsg, false);
		}

	} while (false);

	return OnOldWndProc(WM_MOUSEMOVE, wParam, lParam);
}

LRESULT CDUIWnd::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	CDUIPoint pt(lParam);
	m_bMouseTracking = false;

	if (NULL == m_pHoverCtrl) return lRes;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	DuiMsg.pMsgCtrl = m_pHoverCtrl;
	m_pHoverCtrl->OnDuiMouseHover(pt, DuiMsg);

	//track modify
	CMMString strToolTip = m_pHoverCtrl->GetToolTip();
	if (strToolTip.empty()) return lRes;
	if (false == m_bRefreshToolTip && m_strToolTip == strToolTip) return lRes;

	m_bRefreshToolTip = false;
	m_strToolTip = strToolTip;

	HINSTANCE hInst = CDUIGlobal::GetInstance()->GetInstanceHandle();
	::ZeroMemory(&m_ToolTip, sizeof(m_ToolTip));
	m_ToolTip.cbSize = sizeof(m_ToolTip);
	m_ToolTip.uFlags = TTF_IDISHWND;
	m_ToolTip.hwnd = m_hWnd;
	m_ToolTip.uId = (UINT_PTR)m_hWnd;
	m_ToolTip.hinst = hInst;
	m_ToolTip.lpszText = const_cast<LPTSTR>((LPCTSTR)strToolTip);
	m_ToolTip.rect = m_pHoverCtrl->GetAbsoluteRect();
	if (NULL == m_hWndTooltip)
	{
		m_hWndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWnd, NULL, hInst, NULL);
		::SendMessage(m_hWndTooltip, TTM_ADDTOOL, 0, (LPARAM)&m_ToolTip);
	}
	::SendMessage(m_hWndTooltip, TTM_SETMAXTIPWIDTH, 0, m_pHoverCtrl->GetToolTipWidth());
	::SendMessage(m_hWndTooltip, TTM_SETTIPBKCOLOR, RGB(DUIARGBGetR(m_pHoverCtrl->GetToolTipBkColor()), DUIARGBGetG(m_pHoverCtrl->GetToolTipBkColor()), DUIARGBGetB(m_pHoverCtrl->GetToolTipBkColor())), 0);
	::SendMessage(m_hWndTooltip, TTM_SETTIPTEXTCOLOR, RGB(DUIARGBGetR(m_pHoverCtrl->GetToolTipTextColor()), DUIARGBGetG(m_pHoverCtrl->GetToolTipTextColor()), DUIARGBGetB(m_pHoverCtrl->GetToolTipTextColor())), 0);
	::SendMessage(m_hWndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTip);
	::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTip);

	return lRes;
}

LRESULT CDUIWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	DuiMessage msgLeave = {};
	msgLeave.wParam = wParam;
	msgLeave.lParam = MAKELPARAM(-1, -1);

	if (m_pCaptureCtrl)
	{
		msgLeave.pMsgCtrl = m_pCaptureCtrl;
		m_pCaptureCtrl->OnDuiMouseLeave(pt, msgLeave);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiMouseLeave, pt, msgLeave, false);
	}
	if (m_pHoverCtrl)
	{
		msgLeave.pMsgCtrl = m_pHoverCtrl;
		msgLeave.pMsgCtrl->OnDuiMouseLeave(pt, msgLeave);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseLeave, pt, msgLeave, false);
	}

	m_pHoverCtrl = NULL;
	m_bRefreshToolTip = true;

	//提示窗体
	if (m_hWndTooltip) ::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, false, (LPARAM)&m_ToolTip);
	if (m_bMouseTracking && m_hWnd) ::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM)-1);
	m_bMouseTracking = false;

	return OnOldWndProc(WM_MOUSELEAVE, wParam, lParam);
}

LRESULT CDUIWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	//variant
	POINT pt = {};
	pt.x = (INT)((SHORT)(LOWORD(lParam)));
	pt.y = (INT)((SHORT)(HIWORD(lParam)));
	::ScreenToClient(m_hWnd, &pt);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;
	DuiMsg.wParam = wParam;

	//find
	CDUIControlBase *pControl = m_pCaptureCtrl;
	NULL == pControl ? pControl = FindControl(pt) : NULL;

	if (pControl)
	{
		m_pEventCtrl = pControl;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		bool bRes = DuiMsg.pMsgCtrl->OnDuiMouseWheel(pt, DuiMsg);

		if (false == bRes)
		{
			while (m_pEventCtrl = m_pEventCtrl->GetParent())
			{
				if (m_pEventCtrl
					&& m_pEventCtrl->IsEnabled()
					&& false == m_pEventCtrl->IsMouseThrough()
					&& ((m_pEventCtrl->GetHorizScrollBar() && m_pEventCtrl->GetHorizScrollBar()->IsVisible())
						|| (m_pEventCtrl->GetVertScrollBar() && m_pEventCtrl->GetVertScrollBar()->IsVisible())))
				{
					DuiMsg.pMsgCtrl = m_pEventCtrl;
					DuiMsg.pMsgCtrl->OnDuiMouseWheel(pt, DuiMsg);

					break;
				}
			}
		}
	}

	// Let's make sure that the scroll item below the cursor is the same as before...
	::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(m_ptMousePosLast.x, m_ptMousePosLast.y));

	return OnOldWndProc(WM_MOUSEWHEEL, wParam, lParam);
}

LRESULT CDUIWnd::OnTimer(WPARAM wParam, LPARAM lParam)
{
	DuiMessage DuiMsg = {};

	for (auto &TimerInfo : m_vecTimers)
	{
		if (NULL == TimerInfo.pPropObj) continue;

		if (TimerInfo.hWnd == m_hWnd
			&& TimerInfo.uWinTimer == wParam
			&& TimerInfo.bKilled == false)
		{
			DuiMsg.pMsgCtrl = TimerInfo.pControl;
			DuiMsg.wParam = TimerInfo.nLocalID;
			DuiMsg.ptMouse = m_ptMousePosLast;
			if (DuiMsg.pMsgCtrl)
			{
				DuiMsg.pMsgCtrl->OnDuiTimer(TimerInfo.nLocalID, DuiMsg);
			}
			else
			{
				__super::OnAnimationElapse(TimerInfo.nLocalID);
			}

			break;
		}
	}

	return OnOldWndProc(WM_TIMER, wParam, lParam);
}

LRESULT CDUIWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	ReleasePaintScene();

	if (m_pRootCtrl)
	{
		m_pRootCtrl->NeedRefreshView();
	}

	//design refresh
	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndSize(this);
	}

	return 0;
}

LRESULT CDUIWnd::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiKeyDown(DuiMsg);
		}

		//model
		Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiKeyDown, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_KEYDOWN, wParam, lParam);
}

LRESULT CDUIWnd::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiKeyUp(DuiMsg);
		}

		//model
		Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiKeyUp, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_KEYUP, wParam, lParam);
}

LRESULT CDUIWnd::OnChar(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiChar(DuiMsg);
		}

		//model
		Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiChar, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_CHAR, wParam, lParam);
}

LRESULT CDUIWnd::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiSysKeyDown(DuiMsg);
		}

		//model
		Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiSysKeyDown, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_SYSKEYDOWN, wParam, lParam);
}

LRESULT CDUIWnd::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiSysKeyUp(DuiMsg);
		}

		//model
		Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiSysKeyUp, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_SYSKEYUP, wParam, lParam);
}

LRESULT CDUIWnd::OnSysChar(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiSysChar(DuiMsg);
		}

		//model
		Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiSysChar, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_SYSCHAR, wParam, lParam);
}

LRESULT CDUIWnd::OnPaint(CDUIRect rcPaint)
{
	LRESULT lRes = 0;

	//layered
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	DWORD dwNewStyle = IsWndLayered() ? (dwStyle | WS_EX_LAYERED) : (dwStyle & ~WS_EX_LAYERED);
	if (dwStyle != dwNewStyle) ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
	dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	//layout
	RefreshLayout();

	//verify
	if (NULL == m_pRootCtrl) return lRes;

	//animation wnd
	if (IsAnimatingWnd()) return lRes;

	//Should we paint?
	CDUIRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	if (false == ::IntersectRect(&rcPaint, &rcPaint, &rcClient)) return lRes;
	if (NULL == m_pRootCtrl || rcPaint.Empty()) return lRes;

	//object
	if (NULL == m_hBmpBackground || NULL == m_hMemDcBackground)
	{
		//release
		ReleasePaintScene();

		//create
		(dwStyle & WS_EX_LAYERED) ? rcPaint = rcClient : rcPaint;
		m_hMemDcBackground = ::CreateCompatibleDC(m_hDCPaint);
		m_hBmpBackground = CDUIRenderEngine::CreateARGB32Bitmap(m_hDCPaint, rcClient.GetWidth(), rcClient.GetHeight(), &m_pBmpBackgroundBits);

		if (m_hMemDcBackground && m_hBmpBackground && m_pBmpBackgroundBits)
		{
			m_hBmpBackgroundOld = (HBITMAP)SelectObject(m_hMemDcBackground, m_hBmpBackground);
		}
	}
	if (NULL == m_hMemDcBackground || NULL == m_hBmpBackground || NULL == m_pBmpBackgroundBits)
	{
		Invalidate();

		return lRes;
	}

	//clear pixel
	CDUIRenderEngine::ClearPixel(m_pBmpBackgroundBits, rcClient.GetWidth(), rcPaint);

	//paint
	int iSaveDC = ::SaveDC(m_hMemDcBackground);
	m_pRootCtrl->OnDraw(m_hMemDcBackground, rcPaint);
	::RestoreDC(m_hMemDcBackground, iSaveDC);

	//update
	if (dwStyle & WS_EX_LAYERED)
	{
		CDUIRect rcWnd = {};
		::GetWindowRect(m_hWnd, &rcWnd);
		POINT pt = { rcWnd.left, rcWnd.top };
		SIZE szWindow = { rcWnd.GetWidth(), rcWnd.GetHeight() };
		POINT ptSrc = { 0, 0 };
		BLENDFUNCTION Blend = { AC_SRC_OVER, 0, GetWndAlpha(), AC_SRC_ALPHA };
		::UpdateLayeredWindow(m_hWnd, m_hDCPaint, &pt, &szWindow, m_hMemDcBackground,
			&ptSrc, 0, &Blend, ULW_ALPHA);

		return lRes;
	}

	::BitBlt(m_hDCPaint, rcPaint.left, rcPaint.top, rcPaint.GetWidth(), rcPaint.GetHeight(),
		m_hMemDcBackground, rcPaint.left, rcPaint.top, SRCCOPY);

	return lRes;
}

LRESULT CDUIWnd::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	do
	{
		//pt
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		::ScreenToClient(m_hWnd, &pt);

		//menu ctrl
		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;
		while (m_pEventCtrl && false == m_pEventCtrl->IsContextMenu())
		{
			m_pEventCtrl = m_pEventCtrl->GetParent();
		}

		if (NULL == m_pEventCtrl) break;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.ptMouse = pt;
		DuiMsg.wKeyState = (WORD)DuiMsg.wParam;
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsContextMenu())
		{
			DuiMsg.pMsgCtrl->OnDuiContextMenu(DuiMsg);
		}

		//model
		Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiContextMenu, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_CONTEXTMENU, wParam, lParam);
}

LRESULT CDUIWnd::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_SETFOCUS, wParam, lParam);
}

LRESULT CDUIWnd::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	do
	{
		HWND hWndFocus = GetFocus();
		if (m_hWnd == hWndFocus
			|| (m_hWnd == GetParent(hWndFocus) && (GetWindowLong(hWndFocus, GWL_STYLE) & WS_CHILD))
			|| (m_hWnd == ::GetWindowOwner(hWndFocus) && (GetWindowLong(hWndFocus, GWL_STYLE) & WS_CHILD))) break;

		SetFocusControl(NULL);

	} while (false);

	return OnOldWndProc(WM_KILLFOCUS, wParam, lParam);
}

LRESULT CDUIWnd::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (LOWORD(lParam) != HTCLIENT) break;
		if (m_pCaptureCtrl) return 1;

		POINT pt = { 0 };
		::GetCursorPos(&pt);
		::ScreenToClient(m_hWnd, &pt);
		CDUIControlBase *pControl = FindControl(pt);
		if (NULL == pControl) break;
		if ((pControl->GetControlFlags() & DUIFLAG_SETCURSOR) == 0) break;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();
		DuiMsg.ptMouse = pt;
		pControl->OnDuiSetCursor(pt, DuiMsg);

		return 1;

	} while (false);
	
	return OnOldWndProc(WM_SETCURSOR, wParam, lParam);
}

LRESULT CDUIWnd::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_DPICHANGED, wParam, lParam);
}

LRESULT CDUIWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		::ScreenToClient(m_hWnd, &pt);

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.ptMouse = pt;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		DuiMsg.pMsgCtrl->OnDuiCommand(DuiMsg);

		//model
		Dui_Dispatch_ModelEvent(m_pEventCtrl, OnDuiCommand, DuiMsg);

	} while (false);
	
	return OnOldWndProc(WM_COMMAND, wParam, lParam);
}

LRESULT CDUIWnd::OnImeComPosition(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		POINT pt = {};
		GetCursorPos(&pt);
		::ScreenToClient(m_hWnd, &pt);

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;
		DuiMsg.ptMouse = pt;
		DuiMsg.wKeyState = MapKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();
		LRESULT lRes = DuiMsg.pMsgCtrl->OnDuiImeComPosition(DuiMsg);

		if (lRes > 0)
		{
			return lRes;
		}

	} while (false);
	
	return OnOldWndProc(WM_IME_COMPOSITION, wParam, lParam);
}

void CDUIWnd::OnFinalMessage()
{
	RemoveINotify(this);

	if (GetRootCtrl())
	{
		GetRootCtrl()->ReapControl();
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
	if (Dui_CtrlIDInner_BtnClose == Notify.uCtrlID
		|| Dui_CtrlIDInner_BtnCancel == Notify.uCtrlID
		|| Dui_CtrlIDInner_BtnOk == Notify.uCtrlID)
	{
		m_uCtrlIDClose = Notify.uCtrlID;

		if (AnimateWnd_None != GetAnimateWndType())
		{
			StartAnimationWnd(false);

			return;
		}

		Close(Dui_CtrlIDInner_BtnOk == Notify.uCtrlID ? Notify.uCtrlID : Dui_CtrlIDInner_BtnCancel);

		return;
	}
	else if (Dui_CtrlIDInner_BtnMin == Notify.uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);

		return;
	}
	else if (Dui_CtrlIDInner_BtnMax == Notify.uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

		return;
	}
	else if (Dui_CtrlIDInner_BtnRestore == Notify.uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);

		return;
	}

	return;
}

void CDUIWnd::OnDuiWndAnimateFinish(const DuiNotify &Notify)
{
	if (false == Notify.DuiNotifyExtend.WndAnimate.bShow)
	{
		ShowWindow(false);

		Close(Dui_CtrlIDInner_BtnOk == m_uCtrlIDClose ? m_uCtrlIDClose : Dui_CtrlIDInner_BtnCancel);
	}

	return;
}

CDUIWnd * CDUIWnd::GetWndOwner()
{
	return this;
}

void CDUIWnd::OnDuiDelayDelete()
{
	while (false == m_queDelayDelete.empty())
	{
		auto pControl = m_queDelayDelete.front();
		m_queDelayDelete.pop_front();

		MMSafeDelete(pControl);
	}

	return;
}

void CDUIWnd::OnDpiChanged(int nScalePre)
{
	nScalePre = max(100, nScalePre);
	SetDpi(CDUIGlobal::GetInstance()->GetDpi());

	//wndsize
	if (false == ::IsZoomed(GetWndHandle()))
	{
		CDUIRect rcWnd = GetWindowRect();
		rcWnd.right = rcWnd.left + (rcWnd.GetWidth()) * (GetScale() * 1.0f / nScalePre);
		rcWnd.bottom = rcWnd.top + (rcWnd.GetHeight()) * (GetScale() * 1.0f / nScalePre);
		SetWindowPos(GetWndHandle(), NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
	}

	//refresh
	if (GetRootCtrl())
	{
		GetRootCtrl()->OnDpiChanged(nScalePre);
		GetRootCtrl()->NeedRefreshView();
	}

	SendNotify(GetRootCtrl(), DuiNotify_DpiChanged);

	return;
}

void CDUIWnd::AdjustWndSize()
{
	if (false == IsWindow(m_hWnd)) return;

	CDUISize szWndInit = GetWndInitSize();
	if (szWndInit.cx > 0 && szWndInit.cy > 0)
	{
		CDUISize szWndMax = GetWndMaxSize();
		CDUISize szWndMin = GetWndMinSize();
		szWndInit.cx = szWndMax.cx > 0 ? min(szWndInit.cx, szWndMax.cx) : szWndInit.cx;
		szWndInit.cy = szWndMax.cy > 0 ? min(szWndInit.cy, szWndMax.cy) : szWndInit.cy;
		szWndInit.cx = max(szWndInit.cx, szWndMin.cx);
		szWndInit.cy = max(szWndInit.cy, szWndMin.cy);
		::SetWindowPos(m_hWnd, NULL, 0, 0, szWndInit.cx, szWndInit.cy, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	return;
}

void CDUIWnd::AdjustImagesHSL()
{
	/*TImageInfo* data;
	for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++)
	{
		faw::String key = m_ResInfo.m_ImageHash.GetAt(i)->Key;
		if (!key.empty())
		{
			data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.find(key.str_view()));
			if (data && data->bUseHSL)
			{
				CDUIRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
	}*/

	Invalidate();

	return;
}

void CDUIWnd::RefreshLayout()
{
	CDUIRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	//layout
	bool bNeedLayoutMsg = false;
	if (false == IsRefreshViewNeeded()) return;
	if (NULL == m_pRootCtrl || rcClient.Empty() || ::IsIconic(m_hWnd)) return;

	m_bRefreshViewNeeded = false;

	if (m_pRootCtrl && m_pRootCtrl->IsRefreshViewNeeded())
	{
		m_pRootCtrl->OnDuiSize(rcClient);
		bNeedLayoutMsg = true;
	}
	else
	{
		m_vecFoundControls.clear();
		m_pRootCtrl->FindControl(__FindControlsFromUpdate, nullptr, DuiFind_Visible | DuiFind_MeFirst | DuiFind_UpdateTest);
		for (auto pControl : m_vecFoundControls)
		{
			pControl->OnDuiSize(pControl->GetModalParentRect());
		}
	}

	if (m_bFirstLayout)
	{
		SendNotify(m_pRootCtrl, DuiNotify_WndInited, 0, 0);
	}
	if (bNeedLayoutMsg)
	{
		SendNotify(m_pRootCtrl, DuiNotify_WndLayout, 0, 0);
	}
	if (m_bFirstLayout)
	{
		//animation wnd
#ifndef DUI_DESIGN
		if (AnimateWnd_None != GetAnimateWndType())
		{
			StartAnimationWnd();
		}
#endif
	}

	m_bFirstLayout = false;

	return;
}

void CDUIWnd::DelayDelete(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	pControl->ReapControl();
	pControl->SetWndOwner(NULL);
	pControl->SetParent(NULL);
	m_queDelayDelete.push_back(pControl);
	PostAppMsg();

	return;
}

void CDUIWnd::DispatchNotifyAsyn()
{
	VecDuiNotify vecAsynNotify = m_vecAsynNotify;
	m_vecAsynNotify.clear();

	for (auto &Notify : vecAsynNotify)
	{
		if (NULL == Notify.pNotifyCtrl) continue;

		CDUIWnd *pWnd = Notify.pNotifyCtrl->GetWndOwner();
		if (NULL == pWnd) continue;

		int nCount = Notify.pNotifyCtrl->GetControlCallBackCount();
		for (int n = 0; n < nCount; n++)
		{
			if (false == pWnd->VerifyControl(Notify.pNotifyCtrl)) break;

			IDuiControlCallBack *pICallBack = Notify.pNotifyCtrl->GetControlCallBack(n);
			if (NULL == pICallBack) continue;

			pICallBack->OnNotify(Notify.pNotifyCtrl, Notify);
		}
		for (int n = 0; n < m_vecINotify.size(); n++)
		{
			IDuiNotify *pINotify = m_vecINotify[n];
			if (NULL == pINotify) continue;

			pINotify->OnNotify(Notify);
		}
	}

	return;
}

void CDUIWnd::PostAppMsg()
{
	if (false == m_bPostedAppMsg)
	{
		::PostMessage(m_hWnd, WM_DUIAPP, 0, 0);

		m_bPostedAppMsg = true;
	}

	return;
}

void CDUIWnd::ReleasePaintScene()
{
	if (m_hMemDcBackground)
	{
		SelectObject(m_hMemDcBackground, m_hBmpBackgroundOld);
	}

	MMSafeDeleteObject(m_hBmpBackground);
	MMSafeDeleteDC(m_hMemDcBackground);
	m_pBmpBackgroundBits = NULL;
	m_hBmpBackgroundOld = NULL;

	return;
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

VecDuiControlBase & CDUIWnd::GetFoundControls()
{
	return m_vecFoundControls;
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlFromIDHash(CDUIControlBase *pThis, LPVOID pData)
{
	CDUIWnd *pManager = static_cast<CDUIWnd*>(pData);
	UINT uCtrlID = pThis->GetCtrlID();
	if (0 == uCtrlID) return NULL;
	// Add this control to the hash list
	pManager->m_mapControl[uCtrlID] = pThis;
	return NULL; // Attempt to add all controls
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlFromCount(CDUIControlBase* /*pThis*/, LPVOID pData)
{
	int* pnCount = static_cast<int*>(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlFromPoint(CDUIControlBase *pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->GetAbsoluteRect(), *pPoint) ? pThis : NULL;
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlFromShortcut(CDUIControlBase *pThis, LPVOID pData)
{
	if (false == pThis->IsVisible()) return NULL;

	DuiFindShortCut *pFindShortCut = static_cast<DuiFindShortCut*>(pData);
	if (pFindShortCut->chChar == toupper(pThis->GetShortcut())) return pThis;

	return NULL;
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlFromDrop(CDUIControlBase *pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->GetAbsoluteRect(), *pPoint) && pThis->IsWinDropEnabled() ? pThis : nullptr;
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlFromID(CDUIControlBase *pThis, LPVOID pData)
{
	UINT uFindID = *(static_cast<UINT*>(pData));
	UINT uCtrlID = pThis->GetCtrlID();

	return (uFindID == uCtrlID) ? pThis : NULL;
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlsFromClass(CDUIControlBase *pThis, LPVOID pData)
{
	LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
	LPCTSTR pType = pThis->GetClass();
	if (_tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0)
	{
		if (pThis->GetWndOwner())
		{
			pThis->GetWndOwner()->GetFoundControls().push_back(pThis);
		}
	}

	return NULL;
}

CDUIControlBase * CALLBACK CDUIWnd::__FindControlsFromUpdate(CDUIControlBase *pThis, LPVOID pData)
{
	if (NULL == pThis) return NULL;

	if (pThis->IsRefreshViewNeeded() && pThis->GetWndOwner())
	{
		pThis->GetWndOwner()->GetFoundControls().push_back(pThis);

		return pThis;
	}

	return nullptr;
}

void CDUIWnd::ForegroundWindow(HWND hWnd)
{
	HWND hWndForground = GetForegroundWindow();
	DWORD dwThreadIDForground = ::GetWindowThreadProcessId(hWndForground, NULL);
	DWORD dwThreadIDCur = ::GetCurrentThreadId();
	if (dwThreadIDForground == dwThreadIDCur || hWndForground == hWnd) return;

	::AttachThreadInput(dwThreadIDCur, dwThreadIDForground, TRUE);
	::SetActiveWindow(hWnd);
	::BringWindowToTop(hWnd);
	::SetForegroundWindow(hWnd);
	::AttachThreadInput(dwThreadIDCur, dwThreadIDForground, FALSE);

	return;
}

UINT CDUIWnd::MapKeyState()
{
	UINT uState = 0;
	if (::GetKeyState(VK_CONTROL) < 0) uState |= MK_CONTROL;
	if (::GetKeyState(VK_RBUTTON) < 0) uState |= MK_RBUTTON;
	if (::GetKeyState(VK_LBUTTON) < 0) uState |= MK_LBUTTON;
	if (::GetKeyState(VK_SHIFT) < 0) uState |= MK_SHIFT;
	if (::GetKeyState(VK_MENU) < 0) uState |= MK_ALT;
	return uState;
}