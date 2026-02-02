#ifndef __DUIWEBBROWSERCTRL_H__
#define __DUIWEBBROWSERCTRL_H__

#pragma once
#pragma pack(1)

//WebBrowser
#include <ExDisp.h>

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWebBrowserCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWebBrowserCtrl={0xF3395628,0x2460,0x40E8,0xBB,0xEA,0xE5,0xCE,0x40,0x56,0xD3,0x83};
class DUITHINK_API CDUIWebBrowserCtrl
	: public CDUIControlBase
{
	DuiDeclare_CreateControl(CDUIWebBrowserCtrl)
	MMDeclare_ClassName(CDUIWebBrowserCtrl)

public:
	CDUIWebBrowserCtrl(void);
	~CDUIWebBrowserCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupWeb;
	CDUIAttributeText					m_AttributeUrlHome;
	CDUIAttributeText					m_AttributeUrlError;

	//variant
protected:
	CMMString							m_strUrlCur;
	IWebBrowser2 *						m_pWebBrowser = NULL;	// IE COM 接口
	HWND								m_hWndIEOwner = NULL;		// ActiveX 宿主窗口
	HWND								m_hWndIEServer = NULL;
	HWND								m_hWndIEUtility = NULL;

	//override
protected:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	CMMString GetDescribe() const;
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj);

	//method
public:
	//refresh
	void RefreshView();

	//url
	void Close();
	IWebBrowser2 * GetWebBrowser();
	HWND GetIEOwnerWnd();
	HWND GetIEServerWnd();
	HWND GetIEUtilityWnd();
	void Navigate(LPCTSTR lpszUrl);
	void NavigateHomePage();
	void LoadHtml(LPCTSTR lpszHtml);
	void Back();
	void Forward();
	void Stop();
	void Reload();
	void SetPageFocus();

	//url property
	CMMString GetUrlHome();
	void SetUrlHome(LPCTSTR lpszUrlHome);
	CMMString GetUrlError();
	void SetUrlError(LPCTSTR lpszUrlError);
	CMMString GetUrlCur();

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
	bool OnDuiMouseHover(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiSetFocus() override;
	void OnDuiWndManagerAttach() override;
	void OnDuiWndManagerDetach() override;

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:
	void PaintBkImage(HDC hDC) override;

	//help
protected:
	HWND FindIEServerWnd();
	HWND FindIEUtilityWnd();
	void InstallIEHook(HWND hWnd);
	void ForwardMessageToIE(UINT uMsg, const CDUIPoint &pt, const DuiMessage &Msg);

	//static
protected:
	static LRESULT CALLBACK DuiIEHookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif