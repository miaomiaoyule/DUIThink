#include "StdAfx.h"
#include "DUIWebBrowserCtrl.h"
#include <atlbase.h>
#include <atlhost.h>

class CDUIThinkWebModule : public ATL::CAtlDllModuleT<CDUIThinkWebModule> {};
CDUIThinkWebModule _DuiWebModule;

// --------------------------------------------------------------------------
// [兼容性修复] 设置 IE 控件使用 IE11 内核模式
// --------------------------------------------------------------------------
static void DuiSetWebBrowserEmulation()
{
	TCHAR szExeName[MAX_PATH] = { 0 };
	if (::GetModuleFileName(NULL, szExeName, MAX_PATH))
	{
		// 获取当前进程名 (如 YourApp.exe)
		TCHAR* pName = _tcsrchr(szExeName, _T('\\'));
		if (pName) pName++;
		else pName = szExeName;

		HKEY hKey = NULL;
		// 修正：使用 REG_OPTION_NON_VOLATILE 确保设置持久有效
		// IE 内核读取此配置时依赖于注册表实际存在的值
		if (ERROR_SUCCESS == ::RegCreateKeyEx(HKEY_CURRENT_USER,
			_T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"),
			0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL))
		{
			// 11001 (0x2AF9) = IE11 Edge Mode (强制使用 IE11 标准渲染)
			DWORD dwVal = 11001; 
			::RegSetValueEx(hKey, pName, 0, REG_DWORD, (const BYTE*)&dwVal, sizeof(dwVal));
			::RegCloseKey(hKey);
		}
	}

	return;
}

// 辅助函数：查找 IE 内核的渲染窗口句柄
static HWND FindIEServerWnd(HWND hHost)
{
	if (NULL == hHost) return NULL;

	HWND hServer = ::FindWindowEx(hHost, NULL, _T("Internet Explorer_Server"), NULL);
	if (hServer) return hServer;

	// 如果不是直接子窗口，尝试遍历查找 (通常层级: Shell Embedding -> Shell DocObject View -> Internet Explorer_Server)
	struct tagFindContext { HWND hFound; } FindContext = { NULL };
	::EnumChildWindows(hHost, [](HWND hWnd, LPARAM lParam) -> BOOL 
	{
		TCHAR szClassName[64];
		if (::GetClassName(hWnd, szClassName, 64) && _tcsicmp(szClassName, _T("Internet Explorer_Server")) == 0) 
		{
			((tagFindContext*)lParam)->hFound = hWnd;
			
			return FALSE;
		}

		return TRUE;
	}, (LPARAM)&FindContext);
	
	return FindContext.hFound;
}

// 定义用于存储数据的属性名
static const LPCTSTR kPropOldProc = _T("DuiIE_OldProc");
static const LPCTSTR kPropCtrl = _T("DuiIE_Ctrl");
static LRESULT CALLBACK DuiIEHOokWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldProc = (WNDPROC)::GetProp(hWnd, kPropOldProc);
	CDUIWebBrowserCtrl *pCtrl = (CDUIWebBrowserCtrl*)::GetProp(hWnd, kPropCtrl);

	// 当 IE 拥有捕获时，系统会直接发送 WM_MOUSEMOVE 给它，且坐标是基于 IE 真实位置（屏幕外）
	// 我们需要拦截这个消息，将其坐标修正为视觉上的位置（即相对于 DUI 控件的位置）
	if (uMsg == WM_MOUSEMOVE && pCtrl && ::GetCapture() == hWnd)
	{
		POINT ptScreen;
		::GetCursorPos(&ptScreen);

		HWND hMainWnd = pCtrl->GetWndHandle();
		if (hMainWnd && ::IsWindow(hMainWnd))
		{
			// 1. 将屏幕坐标转为主窗口客户区坐标
			POINT ptInMain = ptScreen;
			::ScreenToClient(hMainWnd, &ptInMain);

			// 2. 获取控件在主窗口中的位置
			CDUIRect rcCtrl = pCtrl->GetAbsoluteRect();

			// 3. 计算相对于控件左上角的坐标 (即 IE 应该看到的正确坐标)
			int x = ptInMain.x - rcCtrl.left;
			int y = ptInMain.y - rcCtrl.top;

			// 4. 重写 LPARAM
			lParam = MAKELPARAM(x, y);
		}
	}
	else if (uMsg == WM_NCDESTROY)
	{
		// 清理属性
		::RemoveProp(hWnd, kPropOldProc);
		::RemoveProp(hWnd, kPropCtrl);
	}

	if (oldProc)
		return ::CallWindowProc(oldProc, hWnd, uMsg, wParam, lParam);

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIWebBrowserCtrl)
MMImplement_ClassName(CDUIWebBrowserCtrl)

CDUIWebBrowserCtrl::CDUIWebBrowserCtrl(void)
{
	// 1. 先设置注册表，确保控件创建时能读取到 IE11 模式
	DuiSetWebBrowserEmulation();

	// 2. 初始化 ATL 控件宿主类支持
	AtlAxWinInit();

	return;
}

CDUIWebBrowserCtrl::~CDUIWebBrowserCtrl(void)
{
	Close();

	return;
}

LPVOID CDUIWebBrowserCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWebBrowserCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWebBrowserCtrl::GetDescribe() const
{
	return Dui_Ctrl_WebBrowser;
}

bool CDUIWebBrowserCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeUrlHome)
	{
		NavigateHomePage();

		return true;
	}

	return false;
}

void CDUIWebBrowserCtrl::RefreshView()
{
	__super::RefreshView();

	if (m_hWndIEOwner && ::IsWindow(m_hWndIEOwner))
	{
		CDUIRect rcCtrl = GetAbsoluteRect();
		::SetWindowPos(m_hWndIEOwner, NULL, 19999, 0, rcCtrl.GetWidth(), rcCtrl.GetHeight(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	return;
}

void CDUIWebBrowserCtrl::Close()
{
	if (m_pWebBrowser)
	{
		m_pWebBrowser->Release();
		m_pWebBrowser = NULL;
	}

	if (m_hWndIEOwner && ::IsWindow(m_hWndIEOwner))
	{
		::DestroyWindow(m_hWndIEOwner);
		m_hWndIEOwner = NULL;
	}

	return;
}

IWebBrowser2 * CDUIWebBrowserCtrl::GetWebBrowser()
{
	return m_pWebBrowser;
}

void CDUIWebBrowserCtrl::Navigate(LPCTSTR lpszUrl)
{
	if (NULL == m_pWebBrowser) return;

	CComBSTR bstrUrl(lpszUrl);
	CComVariant vEmpty;
	m_pWebBrowser->Navigate(bstrUrl, &vEmpty, &vEmpty, &vEmpty, &vEmpty);

	m_strUrlCur = lpszUrl;

	return;
}

void CDUIWebBrowserCtrl::NavigateHomePage()
{
	Navigate(GetUrlHome());

	return;
}

void CDUIWebBrowserCtrl::LoadHtml(LPCTSTR lpszHtml)
{
	if (NULL == m_pWebBrowser) return;

	// 获取文档对象并写入 HTML
	IDispatch* pDisp = NULL;
	if (SUCCEEDED(m_pWebBrowser->get_Document(&pDisp)) && pDisp)
	{
		IHTMLDocument2* pDoc = NULL;
		if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc)) && pDoc)
		{
			SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
			if (psa)
			{
				VARIANT* param;
				HRESULT hr = SafeArrayAccessData(psa, (LPVOID*)&param);
				if (SUCCEEDED(hr))
				{
					param->vt = VT_BSTR;
					param->bstrVal = SysAllocString(lpszHtml);
					SafeArrayUnaccessData(psa);

					pDoc->write(psa);
					pDoc->close();
				}

				SafeArrayDestroy(psa);
			}

			pDoc->Release();
		}

		pDisp->Release();
	}

	return;
}

void CDUIWebBrowserCtrl::Back()
{
	if (m_pWebBrowser) m_pWebBrowser->GoBack();

	return;
}

void CDUIWebBrowserCtrl::Forward()
{
	if (m_pWebBrowser) m_pWebBrowser->GoForward();

	return;
}

void CDUIWebBrowserCtrl::Stop()
{
	if (m_pWebBrowser) m_pWebBrowser->Stop();

	return;
}

void CDUIWebBrowserCtrl::Reload()
{
	if (m_pWebBrowser) m_pWebBrowser->Refresh();

	return;
}

CMMString CDUIWebBrowserCtrl::GetUrlHome()
{
	return m_AttributeUrlHome.GetValue();
}

void CDUIWebBrowserCtrl::SetUrlHome(LPCTSTR lpszUrlHome)
{
	if (lpszUrlHome == GetUrlHome()) return;

	m_AttributeUrlHome.SetValue(lpszUrlHome);
	Navigate(GetUrlHome());

	return;
}

CMMString CDUIWebBrowserCtrl::GetUrlError()
{
	return m_AttributeUrlError.GetValue();
}

void CDUIWebBrowserCtrl::SetUrlError(LPCTSTR lpszUrlError)
{
	if (lpszUrlError == GetUrlError()) return;
	m_AttributeUrlError.SetValue(lpszUrlError);
	return;
}

CMMString CDUIWebBrowserCtrl::GetUrlCur()
{
	return m_strUrlCur;
}

void CDUIWebBrowserCtrl::SetPageFocus()
{
	if (m_hWndIEOwner && ::IsWindow(m_hWndIEOwner))
	{
		::SetFocus(m_hWndIEOwner);
	}

	return;
}

void CDUIWebBrowserCtrl::ExecuteJS(LPCTSTR lpszJS)
{
	if (NULL == m_pWebBrowser) return;

	IDispatch* pDisp = NULL;
	if (SUCCEEDED(m_pWebBrowser->get_Document(&pDisp)) && pDisp)
	{
		IHTMLDocument2* pDoc = NULL;
		if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc)) && pDoc)
		{
			IHTMLWindow2* pWin = NULL;
			if (SUCCEEDED(pDoc->get_parentWindow(&pWin)) && pWin)
			{
				CComBSTR bstrJS(lpszJS);
				CComBSTR bstrLang(L"JavaScript");
				CComVariant vRet;
				pWin->execScript(bstrJS, bstrLang, &vRet);
				pWin->Release();
			}

			pDoc->Release();
		}

		pDisp->Release();
	}

	return;
}

bool CDUIWebBrowserCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDown(pt, Msg)) return false;

	ForwardMessageToIE(WM_LBUTTONDOWN, pt, Msg);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonUp(pt, Msg)) return false;

	ForwardMessageToIE(WM_LBUTTONUP, pt, Msg);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseMove(pt, Msg)) return false;

	ForwardMessageToIE(WM_MOUSEMOVE, pt, Msg);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseWheel(pt, Msg);

	ForwardMessageToIE(WM_MOUSEWHEEL, pt, Msg);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonDown(pt, Msg)) return false;

	ForwardMessageToIE(WM_RBUTTONDOWN, pt, Msg);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonUp(pt, Msg)) return false;

	ForwardMessageToIE(WM_RBUTTONUP, pt, Msg);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDlk(pt, Msg)) return false;

	ForwardMessageToIE(WM_LBUTTONDBLCLK, pt, Msg);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiSetFocus()
{
	if (false == __super::OnDuiSetFocus()) return false;

	// 当 DUI 控件获得焦点时，将焦点传递给 IE 原生窗口
	SetPageFocus();

	return true;
}

void CDUIWebBrowserCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	// 创建 ActiveX 宿主窗口
	if (GetWndHandle() && false == IsWindow(m_hWndIEOwner))
	{
		HWND hWndParent = GetWndHandle();
		CDUIRect rcCtrl = GetAbsoluteRect();
		rcCtrl.Offset(19999 - rcCtrl.left, 0 - rcCtrl.top);

		// 修正：使用 CAxWindow 辅助类创建窗口
		// CAxWindow 会自动处理正确的类名("AtlAxWin" 或 "AtlAxWinLic") 和模块句柄，避免 1407 错误
		CAxWindow wndIE;
		m_hWndIEOwner = wndIE.Create(hWndParent, &rcCtrl, _T("Shell.Explorer.2"),
			DUI_WNDSTYLE_DIALOG & ~WS_CAPTION);

		if (m_hWndIEOwner)
		{
			// 从宿主窗口获取 IWebBrowser2 接口指针
			IUnknown* pUnk = NULL;
			if (SUCCEEDED(AtlAxGetControl(m_hWndIEOwner, &pUnk)) && pUnk)
			{
				pUnk->QueryInterface(IID_IWebBrowser2, (void**)&m_pWebBrowser);
				pUnk->Release();
			}

			// [重要] 设置静默模式
			// 由于 WebBrowser(IE11) 内核无法解析现代网页中的 ES6+ 语法(如箭头函数)，
			// 会必然导致 "Syntax Error" (语法错误)。
			// 这种错误无法修复(引擎不支持)，如果不开启 Silent，就会弹出报错框打断用户。
			// 开启 Silent 是处理此类兼容性问题的唯一标准做法。
			if (m_pWebBrowser)
			{
				// VARIANT_TRUE = -1
				m_pWebBrowser->put_Silent(VARIANT_TRUE);
			}
		}
	}

	NavigateHomePage();

	return;
}

void CDUIWebBrowserCtrl::OnDuiWndManagerDetach()
{
	__super::OnDuiWndManagerDetach();

	Close();

	return;
}

void CDUIWebBrowserCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupWeb, _T("Web"));
	DuiCreateAttribute(m_AttributeUrlHome, _T("UrlHome"), _T(""), m_AttributeGroupWeb);
	DuiCreateAttribute(m_AttributeUrlError, _T("UrlError"), _T(""), m_AttributeGroupWeb);

	return;
}

void CDUIWebBrowserCtrl::InitComplete()
{
	__super::InitComplete();

	return;
}

void CDUIWebBrowserCtrl::PaintBkImage(HDC hDC)
{
	__super::PaintBkImage(hDC);

	if (false == IsWindow(m_hWndIEServer))
	{
		m_hWndIEServer = FindIEServerWnd(m_hWndIEOwner);
	}
	if (false == IsWindow(m_hWndIEServer))
	{
		return;
	}

	// [新增] 检查并安装钩子 (只需安装一次)
	if (NULL == ::GetProp(m_hWndIEServer, kPropOldProc))
	{
		WNDPROC oldProc = (WNDPROC)::SetWindowLongPtr(m_hWndIEServer, GWLP_WNDPROC, (LONG_PTR)DuiIEHOokWndProc);
		::SetProp(m_hWndIEServer, kPropOldProc, (HANDLE)oldProc);
		::SetProp(m_hWndIEServer, kPropCtrl, (HANDLE)this);
	}

	CDUIRect rcCtrl = GetBorderRect();
	CDUIMemDC MemDC(hDC, rcCtrl, false);

	// [关键步骤] 使用 PrintWindow 替代 BitBlt
	// BitBlt 只能烤屏(屏幕必须可见)，PrintWindow 能强迫窗口画到 hMemDC 上
	// 0 是默认标志，也可以尝试 PW_RENDERFULLCONTENT (需 Win8.1+)
	BOOL bPrinted = ::PrintWindow(m_hWndIEServer, MemDC, 0);

	// 如果 PrintWindow 失败（IE 有时会拒绝绘制离屏窗口），可以尝试发送消息
	if (false == bPrinted)
	{
		::SendMessage(m_hWndIEServer, WM_PRINT, (WPARAM)MemDC.GetMemHDC(), PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_OWNED);
	}

	return; 
}

void CDUIWebBrowserCtrl::ForwardMessageToIE(UINT uMsg, const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == IsWindow(m_hWndIEServer)) return;

	CDUIRect rcCtrl = GetAbsoluteRect();
	CDUIPoint ptMouse = pt;
	ptMouse.x -= rcCtrl.left;
	ptMouse.y -= rcCtrl.top;

	// 重新构造 lParam
	LPARAM lNewParam = MAKELPARAM(ptMouse.x, ptMouse.y);
	::PostMessage(m_hWndIEServer, uMsg, Msg.wParam, lNewParam);

	return;
}
//////////////////////////////////////////////////////////////////////////