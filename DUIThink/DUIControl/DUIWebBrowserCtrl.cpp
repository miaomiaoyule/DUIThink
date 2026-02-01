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
}

// 辅助函数：查找 IE 内核的渲染窗口句柄
static HWND FindIEServerWnd(HWND hHost)
{
	if (!hHost) return NULL;
	HWND hServer = ::FindWindowEx(hHost, NULL, _T("Internet Explorer_Server"), NULL);
	if (hServer) return hServer;

	// 如果不是直接子窗口，尝试遍历查找 (通常层级: Shell Embedding -> Shell DocObject View -> Internet Explorer_Server)
	struct FindContext { HWND hFound; } ctx = { NULL };
	::EnumChildWindows(hHost, [](HWND hWnd, LPARAM lParam) -> BOOL {
		TCHAR szClassName[64];
		if (::GetClassName(hWnd, szClassName, 64) && _tcsicmp(szClassName, _T("Internet Explorer_Server")) == 0) {
			((FindContext*)lParam)->hFound = hWnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&ctx);
	return ctx.hFound;
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

	if (m_hWndIE && ::IsWindow(m_hWndIE))
	{
		CDUIRect rc = GetAbsoluteRect();

		// [方案修正]
		// 不要使用 SetWindowRgn(..., 0,0,0,0) —— 这会导致黑屏且无法响应消息。
		// 应该使用"离屏渲染"策略：将窗口移动到屏幕外（例如右侧 3000 像素处），
		// 这里虽然看不见，但窗口仍处于"可见(Visible)"状态，IE 会继续渲染和响应消息。

		// 获取主屏幕信息
		CMMRectF rcPrimary;
		CMMDisplayer::GetScreenInfo(CMMRectF(), CMMRectF(), rcPrimary, std::vector<MONITORINFOEX>());

		// 移动窗口到屏幕可视区域之外 (TopMost 确保不被意外覆盖优化)
		// 注意：这里的坐标偏移量要足够大，确保用户看不见
		::SetWindowPos(m_hWndIE, NULL, 
			rcPrimary.left, rcPrimary.top, 
			rc.GetWidth(), rc.GetHeight(), 
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);

		// 移除之前的 SetWindowRgn 限制 (如果之前设置过)
		::SetWindowRgn(m_hWndIE, NULL, TRUE);
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

	if (m_hWndIE && ::IsWindow(m_hWndIE))
	{
		::DestroyWindow(m_hWndIE);
		m_hWndIE = NULL;
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
	if (m_hWndIE && ::IsWindow(m_hWndIE))
	{
		::SetFocus(m_hWndIE);
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

	ForwardMessageToIE(WM_LBUTTONDOWN, Msg.wParam, Msg.lParam);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonUp(pt, Msg)) return false;

	ForwardMessageToIE(WM_LBUTTONUP, Msg.wParam, Msg.lParam);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseMove(pt, Msg)) return false;

	ForwardMessageToIE(WM_MOUSEMOVE, Msg.wParam, Msg.lParam);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseWheel(pt, Msg);

	ForwardMessageToIE(WM_MOUSEWHEEL, Msg.wParam, Msg.lParam);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonDown(pt, Msg)) return false;

	ForwardMessageToIE(WM_RBUTTONDOWN, Msg.wParam, Msg.lParam);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonUp(pt, Msg)) return false;

	ForwardMessageToIE(WM_RBUTTONUP, Msg.wParam, Msg.lParam);

	return true;
}

bool CDUIWebBrowserCtrl::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDlk(pt, Msg)) return false;

	ForwardMessageToIE(WM_LBUTTONDBLCLK, Msg.wParam, Msg.lParam);

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
	if (GetWndHandle() && false == IsWindow(m_hWndIE))
	{
		HWND hParentWnd = GetWndHandle();
		CDUIRect rc = GetAbsoluteRect();

		// 修正：使用 CAxWindow 辅助类创建窗口
		// CAxWindow 会自动处理正确的类名("AtlAxWin" 或 "AtlAxWinLic") 和模块句柄，避免 1407 错误
		CAxWindow wndIE;
		RECT rcWin = { rc.left, rc.top, rc.right, rc.bottom };
		m_hWndIE = wndIE.Create(hParentWnd, &rcWin, _T("Shell.Explorer.2"),
			DUI_WNDSTYLE_DIALOG & ~WS_CAPTION);

		if (m_hWndIE)
		{
			// 从宿主窗口获取 IWebBrowser2 接口指针
			IUnknown* pUnk = NULL;
			if (SUCCEEDED(AtlAxGetControl(m_hWndIE, &pUnk)) && pUnk)
			{
				pUnk->QueryInterface(IID_IWebBrowser2, (void**)&m_pWebBrowser);
				pUnk->Release();

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
	// 只有当 IE 窗口无效时才绘制背景（例如设计模式或加载前）
	if (!m_hWndIE || !::IsWindowVisible(m_hWndIE))
	{
		__super::PaintBkImage(hDC);
	}

	{
		HWND hWnd = m_hWndIE;
		TCHAR szClassName[MAX_PATH] = { 0 };

		while (hWnd)
		{
			// 查找第一个子窗口
			HWND hChild = ::GetWindow(hWnd, GW_CHILD);
			if (!hChild) break;

			// [优化] 只有 PrintWindow 才会响应重绘请求，InvalidateRect 对离屏窗口意义不大
			// InvalidateRect(hChild, NULL, TRUE); 
			hWnd = hChild;

			// 可选：验证类名是否为 Internet Explorer_Server
			::GetClassName(hWnd, szClassName, MAX_PATH);
			if (_tcsicmp(szClassName, _T("Internet Explorer_Server")) == 0)
			{
				CDUIRect rcCtrl = GetBorderRect();

				// ---------------------------------------------------------
				// [修正] 离屏/移出屏幕后的正确截图方式
				// ---------------------------------------------------------

				// 1. 创建兼容内存 DC
				// 注意：GetDC(hWnd) 获取的是屏幕 DC，如果 CreateCompatibleBitmap 传入 hDC，
				// 在某些显卡驱动下，离屏窗口可能创建出单色位图。建议传入 hDC (DUI的绘制DC) 以确保色彩兼容。
				HDC hMemDC = ::CreateCompatibleDC(hDC);

				// 2. 创建与 IE 窗口大小一致的空位图
				HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rcCtrl.GetWidth(), rcCtrl.GetHeight());
				HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

				// 预填充白色背景（防止 IE 未完全渲染时出现黑底）
				HBRUSH hBrWhite = (HBRUSH)::GetStockObject(WHITE_BRUSH);
				::FillRect(hMemDC, &rcCtrl, hBrWhite);

				// 3. [关键步骤] 使用 PrintWindow 替代 BitBlt
				// BitBlt 只能烤屏(屏幕必须可见)，PrintWindow 能强迫窗口画到 hMemDC 上
				// 0 是默认标志，也可以尝试 PW_RENDERFULLCONTENT (需 Win8.1+)
				BOOL bPrinted = ::PrintWindow(hWnd, hMemDC, 0);

				// 如果 PrintWindow 失败（IE 有时会拒绝绘制离屏窗口），可以尝试发送消息
				if (!bPrinted)
				{
					::SendMessage(hWnd, WM_PRINT, (WPARAM)hMemDC, PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_OWNED);
				}

				// 4. 将获取到的画面 绘制回 DUI 主 DC 用于显示
				::BitBlt(hDC, rcCtrl.left, rcCtrl.top, rcCtrl.GetWidth(), rcCtrl.GetHeight(), hMemDC, 0, 0, SRCCOPY);

				// 5. 保存截图
				CDUIRenderEngine::SaveImage(hBitmap, _T("IE_Server_Capture.png"));

				// 6. 清理
				::SelectObject(hMemDC, hOldBmp);
				::DeleteObject(hBitmap);
				::DeleteDC(hMemDC);

				// 既然找到了渲染层并处理完毕，可以退出了
				// ReleaseDC(hWnd, hDCIE); // 不再需要 GetDC(hWnd)
				return; 
			}
		}
		// 如果循环没找到子窗口，会继续执行下面的 IViewObject 逻辑作为 fallback
	}

	if (!m_pWebBrowser) return;

	// ... (余下的 IViewObject 逻辑)
	// ...

	// 2. 尝试从【文档对象(Document)】获取 IViewObject 接口进行绘制
	//    关键修正：直接从 m_pWebBrowser (IE控件) 获取 IViewObject 在离屏/隐藏模式下经常导致黑屏。
	//    必须从 get_Document 获取到的 Document 对象中 Query 接口，它是负责实际渲染 HTML 的引擎。
	IDispatch* pDispDoc = NULL;
	if (SUCCEEDED(m_pWebBrowser->get_Document(&pDispDoc)) && pDispDoc)
	{
		IViewObject* pViewObject = NULL;
		if (SUCCEEDED(pDispDoc->QueryInterface(IID_IViewObject, (void**)&pViewObject)))
		{
			CDUIRect rcControl = GetBorderRect();

			// 转换为 RECTL 结构
			RECTL rcDest = { rcControl.left, rcControl.top, rcControl.right, rcControl.bottom };

			// 调用 Draw 方法绘制内容到 hDC
			pViewObject->Draw(DVASPECT_CONTENT, 1, NULL, NULL, NULL, hDC, &rcDest, NULL, NULL, 0);
			pViewObject->Release();
		}

		pDispDoc->Release();
	}

	return;
}

void CDUIWebBrowserCtrl::ForwardMessageToIE(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!m_hWndIE) return;

	// 找到负责处理输入的 IE Server 窗口
	HWND hServer = FindIEServerWnd(m_hWndIE);
	if (hServer && ::IsWindow(hServer))
	{
		CDUIRect rcCtrl = GetAbsoluteRect();
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		pt.x -= rcCtrl.left;
		pt.y -= rcCtrl.top;

		// 重新构造 lParam
		LPARAM lNewParam = MAKELPARAM(pt.x, pt.y);
		::PostMessage(hServer, uMsg, wParam, lNewParam);
	}
}
//////////////////////////////////////////////////////////////////////////