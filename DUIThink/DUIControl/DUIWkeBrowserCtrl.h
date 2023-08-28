#ifndef __DUIWKEBROWSERCTRL_H__
#define __DUIWKEBROWSERCTRL_H__

#pragma once
#pragma pack(1)

#ifdef DUITHINKWKE

//////////////////////////////////////////////////////////////////////////
typedef struct _rend_data
{
	CDUIRect							rcRender;
	BYTE *								pBitmapBits = NULL;
	HDC									hDC = ::CreateCompatibleDC(0);
	HBITMAP								hBitmap = NULL;
	HBITMAP								hBmpOld = NULL;

	_rend_data()
	{
	}
	~_rend_data()
	{
		Reset();

		MMSafeDeleteDC(hDC);

		return;
	}

	void Reset()
	{
		if (hDC)
		{
			SelectObject(hDC, hBmpOld);
		}

		MMSafeDeleteObject(hBitmap);
		pBitmapBits = NULL;

		return;
	}

}REND_DATA, *PRENDDATA;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWkeBrowserCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWkeBrowserCtrl = { 0x209FAAD5,0x84F4,0x4774,0xB9,0x49,0x33,0xD9,0x78,0xD9,0x67,0x46 };
class DUITHINK_API CDUIWkeBrowserCtrl
	: public CDUIControlBase
{
	DuiDeclare_CreateControl(CDUIWkeBrowserCtrl)
	MMDeclare_ClassName(CDUIWkeBrowserCtrl)

public:
	CDUIWkeBrowserCtrl(void);
	~CDUIWkeBrowserCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupWke;
	CDUIAttributeText					m_AttributeUrlHome;
	CDUIAttributeText					m_AttributeUrlError;

	//variant
private:
	REND_DATA							m_RendData;
	wkeWebView							m_pWkeWebView = wkeCreateWebView();
	CMMString							m_strUrlCur;

private:
	void *								m_pListenObj = NULL;					//监听对象
	IDUIWkeCallback *					m_pWkeCallback = NULL;					//回调接口

	//override
protected:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	CMMString GetDescribe() const;
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj);

	//method
public:
	//refresh
	void RefreshView();

	//method wke
public:
	//wkeview
	wkeWebView GetWebView();
	void Close();
	void * GetListenObj();
	void SetListenObj(void *pListenObj);
	IDUIWkeCallback * GetWkeCallBack();
	void SetWkeCallback(IDUIWkeCallback *pWkeCallback);

	//load url
	void Navigate(LPCTSTR lpszUrl);
	void NavigateHomePage();
	void LoadHtml(LPCTSTR lpszHtml);

	//url
	void Back();
	void Forward();
	void Stop();
	void Reload();
	CMMString GetUrlHome();
	void SetUrlHome(LPCTSTR lpszUrlHome);
	CMMString GetUrlError();
	void SetUrlError(LPCTSTR lpszUrlError);
	CMMString GetUrlCur();
	void SetPageFocus();

	//js
	void ExecuteJS(LPCTSTR lpszJS);

	//message 
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiSetFocus() override;
	bool OnDuiKillFocus() override;

	void OnDuiWndManagerAttach() override;
	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;

	LRESULT OnDuiKeyDown(const DuiMessage &Msg) override;
	LRESULT OnDuiKeyUp(const DuiMessage &Msg) override;
	LRESULT OnDuiChar(const DuiMessage &Msg) override;
	LRESULT OnDuiContextMenu(const DuiMessage &Msg) override;

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//paint
protected:
	void PaintBkImage(HDC hDC) override;

	//help
protected:
	UINT ConstructWkeFlag(WPARAM wParam);

	//static
private:
	static void WKE_CALL OnWkeTitleChanged(wkeWebView webView, void *param, wkeString title);
	static void WKE_CALL OnWkeURLChanged(wkeWebView webView, void *param, wkeString url);
	static void WKE_CALL OnWkeAlertBox(wkeWebView webView, void *param, wkeString msg);
	static bool WKE_CALL OnWkeNavigation(wkeWebView webView, void *param, wkeNavigationType navigationType, wkeString url);
	static wkeWebView WKE_CALL OnWkeCreateView(wkeWebView webView, void *param, const wkeNewViewInfo* info);
	static void WKE_CALL OnWkeDocumentReady(wkeWebView webView, void *param, const wkeDocumentReadyInfo* info);
	static void WKE_CALL OnWkeLoadingFinish(wkeWebView webView, void *param, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
	static void WKE_CALL OnWkePaintUpdated(wkeWebView webView, void *param, const void *hdc, int x, int y, int cx, int cy);
};

//////////////////////////////////////////////////////////////////////////
#endif
#pragma pack()
#endif