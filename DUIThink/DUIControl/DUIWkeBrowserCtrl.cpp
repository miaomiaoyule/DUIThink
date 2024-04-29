#include "StdAfx.h"
#include "DUIWkeBrowserCtrl.h"

#ifdef DUITHINKWKE
#pragma comment(lib, "../lib/wke.lib")

//////////////////////////////////////////////////////////////////////////
std::map<wkeWebView, CDUIWkeBrowserCtrl*> g_mapWkeBrowserCtrl;

//////////////////////////////////////////////////////////////////////////
LPCTSTR wkeGetStringT(wkeString str)
{
#ifdef _UNICODE
	return wkeGetStringW(str);
#else
	return wkeGetString(str);
#endif
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIWkeBrowserCtrl)
MMImplement_ClassName(CDUIWkeBrowserCtrl)

CDUIWkeBrowserCtrl::CDUIWkeBrowserCtrl(void)
{
	g_mapWkeBrowserCtrl[m_pWkeWebView] = this;

	TCHAR modulePath[MAX_PATH] = { 0 };
	TCHAR curDir[MAX_PATH] = { 0 }, drive[_MAX_DRIVE] = { 0 }, dir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	_wsplitpath(modulePath, drive, dir, NULL, NULL);
	_tcscpy(curDir, drive), _tcscat(curDir, dir);
	CMMString strUrlError = _T("file:///");
	strUrlError += curDir;
	strUrlError += _T("error.html");
	SetUrlError(strUrlError);

	return;
}

CDUIWkeBrowserCtrl::~CDUIWkeBrowserCtrl(void)
{
	g_mapWkeBrowserCtrl.erase(m_pWkeWebView);

	if (m_pWkeWebView)
	{
		wkeDestroyWebView(m_pWkeWebView);
	}

	return;
}

LPVOID CDUIWkeBrowserCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWkeBrowserCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWkeBrowserCtrl::GetDescribe() const
{
	return Dui_Ctrl_WkeBrowser;
}

bool CDUIWkeBrowserCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeUrlHome)
	{
		NavigateHomePage();

		return true;
	}

	return false;
}

void CDUIWkeBrowserCtrl::RefreshView()
{
	__super::RefreshView();

	m_RendData.Reset();
	m_RendData.rcRender = GetAbsoluteRect();
	wkeResize(m_pWkeWebView, GetWidth(), GetHeight());

	return;
}

wkeWebView CDUIWkeBrowserCtrl::GetWebView()
{
	return m_pWkeWebView;
}

void CDUIWkeBrowserCtrl::Close()
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

	wkeDestroyWebView(m_pWkeWebView);

	m_pWkeWebView = NULL;

	return;
}

void * CDUIWkeBrowserCtrl::GetListenObj()
{
	return m_pListenObj;
}

void CDUIWkeBrowserCtrl::SetListenObj(void *pListenObj)
{
	m_pListenObj = pListenObj;

	return;
}

IDUIWkeCallback * CDUIWkeBrowserCtrl::GetWkeCallBack()
{
	return m_pWkeCallback;
}

void CDUIWkeBrowserCtrl::SetWkeCallback(IDUIWkeCallback *pWkeCallback)
{
	m_pWkeCallback = pWkeCallback;

	return;
}

void CDUIWkeBrowserCtrl::Navigate(LPCTSTR lpszUrl)
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

#ifdef _UNICODE
	wkeLoadW(m_pWkeWebView, lpszUrl);
#else
	wkeLoad(m_pWkeWebView, lpszUrl);
#endif	

	return;
}

void CDUIWkeBrowserCtrl::NavigateHomePage()
{
	Navigate(GetUrlHome());

	return;
}

void CDUIWkeBrowserCtrl::LoadHtml(LPCTSTR lpszHtml)
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

#ifdef _UNICODE
	wkeLoadHTMLW(m_pWkeWebView, lpszHtml);
#else
	wkeLoadHTML(m_pWkeWebView, lpszHtml);
#endif	

	return;
}

void CDUIWkeBrowserCtrl::Back()
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

	if (wkeCanGoBack(m_pWkeWebView))
	{
		wkeGoBack(m_pWkeWebView);
	}

	return;
}

void CDUIWkeBrowserCtrl::Forward()
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

	if (wkeCanGoForward(m_pWkeWebView))
	{
		wkeGoForward(m_pWkeWebView);
	}

	return;
}

void CDUIWkeBrowserCtrl::Stop()
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

	wkeStopLoading(m_pWkeWebView);

	return;
}

void CDUIWkeBrowserCtrl::Reload()
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

	wkeReload(m_pWkeWebView);

	return;
}

CMMString CDUIWkeBrowserCtrl::GetUrlHome()
{
	return m_AttributeUrlHome.GetValue();
}

void CDUIWkeBrowserCtrl::SetUrlHome(LPCTSTR lpszUrlHome)
{
	if (lpszUrlHome == GetUrlHome()) return;

	m_AttributeUrlHome.SetValue(lpszUrlHome);

	Navigate(GetUrlHome());

	return;
}

CMMString CDUIWkeBrowserCtrl::GetUrlError()
{
	return m_AttributeUrlError.GetValue();
}

void CDUIWkeBrowserCtrl::SetUrlError(LPCTSTR lpszUrlError)
{
	if (lpszUrlError == GetUrlError()) return;

	m_AttributeUrlError.SetValue(lpszUrlError);

	return;
}

CMMString CDUIWkeBrowserCtrl::GetUrlCur()
{
	return m_strUrlCur;
}

void CDUIWkeBrowserCtrl::SetPageFocus()
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

	wkeSetFocus(m_pWkeWebView);

	return;
}

void CDUIWkeBrowserCtrl::ExecuteJS(LPCTSTR lpszJS)
{
	assert(m_pWkeWebView);
	if (NULL == m_pWkeWebView) return;

	wkeRunJS(m_pWkeWebView, CT2CA(lpszJS, CP_UTF8));

	return;
}

bool CDUIWkeBrowserCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDown(pt, Msg)) return false;

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_LBUTTONDOWN, pt.x, pt.y, Msg.wKeyState);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonUp(pt, Msg)) return false;

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_LBUTTONUP, pt.x, pt.y, Msg.wKeyState);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDlk(pt, Msg)) return false;

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_LBUTTONDBLCLK, pt.x, pt.y, Msg.wKeyState);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonDown(pt, Msg)) return false;

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_RBUTTONDOWN, pt.x, pt.y, Msg.wKeyState);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonUp(pt, Msg)) return false;

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_RBUTTONUP, pt.x, pt.y, Msg.wKeyState);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonDlk(pt, Msg)) return false;

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_RBUTTONDBLCLK, pt.x, pt.y, Msg.wKeyState);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiSetCursor(pt, Msg)) return false;

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseEnter(pt, Msg)) return false;

	UINT uFlags = ConstructWkeFlag(Msg.wParam);

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_MOUSEMOVE, pt.x, pt.y, uFlags);

	Invalidate();

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseMove(pt, Msg)) return false;

	UINT uFlags = ConstructWkeFlag(Msg.wParam);

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_MOUSEMOVE, pt.x, pt.y, uFlags);

	return true;
}

void CDUIWkeBrowserCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseMove(pt, Msg);

	UINT uFlags = ConstructWkeFlag(Msg.wParam);

	wkeFireMouseEvent(m_pWkeWebView, WKE_MSG_MOUSEMOVE, pt.x, pt.y, uFlags);

	Invalidate();

	return;
}

bool CDUIWkeBrowserCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	wkeFireMouseWheelEvent(m_pWkeWebView, pt.x, pt.y, (int)(short)HIWORD(Msg.wParam), Msg.wKeyState);

	__super::OnDuiMouseWheel(pt, Msg);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiSetFocus()
{
	if (false == __super::OnDuiSetFocus()) return false;

	wkeSetFocus(m_pWkeWebView);

	return true;
}

bool CDUIWkeBrowserCtrl::OnDuiKillFocus()
{
	if (false == __super::OnDuiKillFocus()) return false;

	wkeKillFocus(m_pWkeWebView);

	return true;
}

void CDUIWkeBrowserCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (GetWndManager())
	{
		GetWndManager()->SetTimer(this, Dui_TimerWkeRefresh_ID, Dui_TimerWkeRefresh_Elapse);
	}

	return;
}

void CDUIWkeBrowserCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	if (Dui_TimerWkeRefresh_ID == uTimerID)
	{
		Invalidate();

		return;
	}

	return __super::OnDuiTimer(uTimerID, Msg);
}

LRESULT CDUIWkeBrowserCtrl::OnDuiKeyDown(const DuiMessage &Msg)
{
	__super::OnDuiKeyDown(Msg);

	wkeFireKeyDownEvent(m_pWkeWebView, Msg.chKey, Msg.lParam, false);

	return 0;
}

LRESULT CDUIWkeBrowserCtrl::OnDuiKeyUp(const DuiMessage &Msg)
{
	__super::OnDuiKeyUp(Msg);

	wkeFireKeyUpEvent(m_pWkeWebView, Msg.chKey, Msg.lParam, false);

	return 0;
}

LRESULT CDUIWkeBrowserCtrl::OnDuiChar(const DuiMessage &Msg)
{
	__super::OnDuiChar(Msg);

	wkeFireKeyPressEvent(m_pWkeWebView, Msg.chKey, Msg.lParam, false);

	return 0;
}

LRESULT CDUIWkeBrowserCtrl::OnDuiContextMenu(const DuiMessage &Msg)
{
	__super::OnDuiContextMenu(Msg);

	if (GetWndManager())
	{
		CDUIPoint ptMouse = GetWndManager()->GetMousePosLast();
		unsigned int flags = 0;
		if (Msg.wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (Msg.wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		wkeFireContextMenuEvent(m_pWkeWebView, ptMouse.x, ptMouse.y, flags);
	}

	return 0;
}

void CDUIWkeBrowserCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupWke, _T("Wke"));
	DuiCreateAttribute(m_AttributeUrlHome, _T("UrlHome"), _T(""), m_AttributeGroupWke);
	DuiCreateAttribute(m_AttributeUrlError, _T("UrlError"), _T(""), m_AttributeGroupWke);

	return;
}

void CDUIWkeBrowserCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	return;
}

void CDUIWkeBrowserCtrl::InitComplete()
{
	__super::InitComplete();

	wkeSetUserAgent(m_pWkeWebView, "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2228.0 Safari/537.36");
	//wkeSetName(m_pWkeWebView, CT2CA(GetCtrlID()));

	//初始化后回调接口
	wkeOnTitleChanged(m_pWkeWebView, OnWkeTitleChanged, this);
	wkeOnURLChanged(m_pWkeWebView, OnWkeURLChanged, this);
	wkeOnAlertBox(m_pWkeWebView, OnWkeAlertBox, this);
	wkeOnNavigation(m_pWkeWebView, OnWkeNavigation, this);
	wkeOnCreateView(m_pWkeWebView, OnWkeCreateView, this);
	wkeOnDocumentReady(m_pWkeWebView, OnWkeDocumentReady, this);
	wkeOnLoadingFinish(m_pWkeWebView, OnWkeLoadingFinish, this);
	wkeOnPaintUpdated(m_pWkeWebView, OnWkePaintUpdated, this);

	NavigateHomePage();

	return;
}

void CDUIWkeBrowserCtrl::PaintBkImage(HDC hDC)
{
	__super::PaintBkImage(hDC);

	if (NULL == m_RendData.hBitmap)
	{
		m_RendData.hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(NULL, m_RendData.rcRender.GetWidth(), m_RendData.rcRender.GetHeight(), &m_RendData.pBitmapBits);
		m_RendData.hBmpOld = (HBITMAP)SelectObject(m_RendData.hDC, m_RendData.hBitmap);
	}

	wkePaint2(m_pWkeWebView, m_RendData.pBitmapBits, 0);
	::BitBlt(hDC, m_RendData.rcRender.left, m_RendData.rcRender.top, m_RendData.rcRender.GetWidth(), m_RendData.rcRender.GetHeight(), m_RendData.hDC, 0, 0, SRCCOPY);

	return;
}

UINT CDUIWkeBrowserCtrl::ConstructWkeFlag(WPARAM wParam)
{
	UINT uFlags = 0;
	if (wParam & MK_CONTROL)
		uFlags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		uFlags |= WKE_SHIFT;
	if (wParam & MK_LBUTTON)
		uFlags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		uFlags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		uFlags |= WKE_RBUTTON;

	return uFlags;
}

void CDUIWkeBrowserCtrl::OnWkeTitleChanged(wkeWebView webView, void *param, wkeString title)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return;

	if (pWkeUI->m_pWkeCallback)
	{
		return pWkeUI->m_pWkeCallback->OnWkeTitleChanged(pWkeUI, wkeGetStringT(title));
	}

	return;
}

void CDUIWkeBrowserCtrl::OnWkeURLChanged(wkeWebView webView, void *param, wkeString url)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return;

	if (pWkeUI->m_pWkeCallback)
	{
		return pWkeUI->m_pWkeCallback->OnWkeURLChanged(pWkeUI, wkeGetStringT(url));
	}

	return;
}

void CDUIWkeBrowserCtrl::OnWkeAlertBox(wkeWebView webView, void *param, wkeString msg)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return;

	if (pWkeUI->m_pWkeCallback)
	{
		return pWkeUI->m_pWkeCallback->OnWkeAlertBox(pWkeUI, wkeGetStringT(msg));
	}

	return;
}

bool CDUIWkeBrowserCtrl::OnWkeNavigation(wkeWebView webView, void *param, wkeNavigationType navigationType, wkeString url)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return false;

	LPCTSTR lpszUrl = wkeGetStringT(url);
	CMMString strUrl = lpszUrl;
	CMMString strExeTest = strUrl.Right(4);
	if (strExeTest == _T(".exe") || strExeTest == _T(".zip") || strExeTest == _T(".rar"))
	{
		ShellExecute(NULL, _T("OPEN"), strUrl, NULL, NULL, SW_NORMAL);

		if (pWkeUI->m_pWkeCallback)
		{
			pWkeUI->m_pWkeCallback->OnWkeNavigation(pWkeUI, navigationType, lpszUrl);
		}

		return false;
	}
	if (-1 == strUrl.find(_T("error.html")))
	{
		pWkeUI->m_strUrlCur = lpszUrl;
	}
	else
	{
		//MessageBox(NULL, _T("哎呀，网页打开失败了！"), NULL, MB_ICONERROR);
		return false;
	}

	if (pWkeUI->m_pWkeCallback)
	{
		return pWkeUI->m_pWkeCallback->OnWkeNavigation(pWkeUI, navigationType, lpszUrl);
	}

	return true;
}

wkeWebView CDUIWkeBrowserCtrl::OnWkeCreateView(wkeWebView webView, void *param, const wkeNewViewInfo* info)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return NULL;

	if (pWkeUI->m_pWkeCallback)
	{
		return pWkeUI->m_pWkeCallback->OnWkeCreateView(pWkeUI, info);
	}

	return NULL;
}

void CDUIWkeBrowserCtrl::OnWkeDocumentReady(wkeWebView webView, void *param, const wkeDocumentReadyInfo* info)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return;

	if (pWkeUI->m_pWkeCallback)
	{
		return pWkeUI->m_pWkeCallback->OnWkeDocumentReady(pWkeUI, info);
	}

	return;
}

void CDUIWkeBrowserCtrl::OnWkeLoadingFinish(wkeWebView webView, void *param, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return;

	//页面加载失败
	if (result == WKE_LOADING_FAILED)
	{
		pWkeUI->Navigate(pWkeUI->GetUrlError());
	}

	if (pWkeUI->m_pWkeCallback)
	{
		pWkeUI->m_pWkeCallback->OnWkeLoadingFinish(pWkeUI, wkeGetStringT(url), result, wkeGetStringT(failedReason));
	}

	return;
}

void CDUIWkeBrowserCtrl::OnWkePaintUpdated(wkeWebView webView, void *param, const void *hdc, int x, int y, int cx, int cy)
{
	CDUIWkeBrowserCtrl *pWkeUI = (CDUIWkeBrowserCtrl*)param;
	if (NULL == pWkeUI)	return;

	return;
}

//////////////////////////////////////////////////////////////////////////

#endif