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

	//override
protected:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	CMMString GetDescribe() const;
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj);

	//method
public:
	//refresh
	void RefreshView();

	void Close();
	IWebBrowser2 * GetWebBrowser(); // 获取原生接口

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
	//bool OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//bool OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg) override;
	//bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg) override;
	//bool OnDuiMouseHover(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	//void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
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
	void ForwardMessageToIE(UINT uMsg, const CDUIPoint &pt, const DuiMessage &Msg);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif