#include "StdAfx.h"
#include "DUIWebBrowserCtrl.h"
#include <atlbase.h>
#include <atlhost.h>

class CDUIThinkWebModule : public ATL::CAtlDllModuleT<CDUIThinkWebModule> {};
CDUIThinkWebModule _DuiWebModule;

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIWebBrowserCtrl)
MMImplement_ClassName(CDUIWebBrowserCtrl)

CDUIWebBrowserCtrl::CDUIWebBrowserCtrl(void)
{
	// 初始化 ATL 控件宿主类支持
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

	// 这是一个 HWND 控件，需要跟随 DUI 控件的位置移动
	if (m_hWndIE && ::IsWindow(m_hWndIE))
	{
		CDUIRect rc = GetAbsoluteRect();
		// 移动子窗口
		::MoveWindow(m_hWndIE, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), TRUE);
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
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

		if (m_hWndIE)
		{
			// 从宿主窗口获取 IWebBrowser2 接口指针
			IUnknown* pUnk = NULL;
			if (SUCCEEDED(AtlAxGetControl(m_hWndIE, &pUnk)) && pUnk)
			{
				pUnk->QueryInterface(IID_IWebBrowser2, (void**)&m_pWebBrowser);
				pUnk->Release();
			}

			// [修改] 设置静默模式，屏蔽脚本错误弹窗
			if (m_pWebBrowser)
			{
				// VARIANT_TRUE 为 -1 (0xFFFF)，VARIANT_FALSE 为 0
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
	// 只有当 IE 窗口无效时才绘制背景（例如设计模式或加载前）
	if (!m_hWndIE || !::IsWindowVisible(m_hWndIE))
	{
		__super::PaintBkImage(hDC);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////