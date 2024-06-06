#ifndef __DUIWND_H__
#define __DUIWND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDUIWndManager;

/////////////////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUINotifyPump : public IDUINotify
{
	DuiDeclare_Message_Map()

public:
	void OnNotify(const DuiNotify &Notify) override;
};

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIWnd
	: public CDUINotifyPump
	, public IDUIPreMessage
{
	MMDeclare_ClassName(CDUIWnd)
	DuiDeclare_Message_Map()

public:
	CDUIWnd(LPCTSTR lpszDuiName = NULL, HWND hWndParent = NULL);
	virtual ~CDUIWnd();

protected:
	CMMString							m_strDuiName;
	HWND								m_hWndParent = NULL;
	HWND								m_hWnd = NULL;
	WNDPROC								m_OldWndProc = NULL;
	bool								m_bSubWindow = false;
	CDUIWndManager *  					m_pWndManager = NULL;
	CDUIPoint							m_ptCreate = {};
	int									m_nCtrlIDClose = 0;

public:
	virtual HWND GetHWND() const;
	virtual operator HWND() const;
	virtual CDUIWndManager * GetWndManager();

	virtual CMMString GetDuiName() const;
	virtual UINT GetClassStyle() const;

	virtual HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
	virtual HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
	virtual HWND SubWindow(HWND hWnd);
	virtual void UnSubWindow();
	virtual void ShowWindow(bool bShow = true, bool bTakeFocus = true);
	virtual UINT DoModal();
	virtual UINT DoBlock();
	virtual void Close(UINT nRet = IDOK);
	virtual void CenterWindow();
	virtual void SetIcon(UINT nRes);

	virtual LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual void ResizeWnd(int cx = -1, int cy = -1);
	virtual void AdjustWnd();
	virtual LPCTSTR GetSuperClassName() const;

protected:	virtual void OnFindControl() {}	virtual void OnInitDialog() {}
	virtual void OnFinalMessage();
	virtual void OnDuiTimer(const DuiNotify &Notify);
	virtual void OnDuiClick(const DuiNotify &Notify);
	virtual void OnDuiWndAnimateFinish(const DuiNotify &Notify);

protected:
	virtual LRESULT OnClose(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnDestroy(WPARAM wParam, LPARAM lParam, bool &bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
	virtual LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnNcCalcSize(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam, bool &bHandled);
#endif
	virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam, bool &bHandled);
	virtual LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);

protected:
	virtual LRESULT OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	LRESULT OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool RegisterSuperclass();
	bool RegisterWindowClass();

	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	static void ForegroundWindow(HWND hWnd);
};

//////////////////////////////////////////////////////////////////////////

#endif 
