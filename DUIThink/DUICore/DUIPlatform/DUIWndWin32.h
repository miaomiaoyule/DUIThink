#ifndef DuiPlatform_SDL
#pragma once
//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWndWin32 INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWndWin32 = { 0x66b5aa83, 0x3756, 0x4495, 0xad, 0xa0, 0xbd, 0x82, 0xa1, 0xdc, 0x27, 0x82 };
class CDUIWndWin32 
	: public CDUIWndBase
	, public CMMDragDrop
{
public:
	CDUIWndWin32(LPCTSTR lpszDuiName = NULL, HWND hWndParent = NULL);
	virtual ~CDUIWndWin32();
	//variant
protected:
	WNDPROC										m_OldWndProc = NULL;

	//override
protected:
#if (NTDDI_VERSION >= NTDDI_VISTA)
protected:
	void OnWinDragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect) override;
	void OnWinDragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect) override;
	void OnWinDragLeave() override;
	void OnWinDrop(IDataObject *pIDataObject, POINTL pt, DWORD *pdwEffect) override;
#endif

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	UINT MapKeyState() override;

	//create
	HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT) override;
	HWND SubWindow(HWND hWnd) override;
	void UnSubWindow() override;
	void ShowWindow(bool bShow = true, bool bTakeFocus = true) override;
	UINT DoModal() override;
	UINT DoBlock() override;
	void CenterWindow() override;
	void SetIcon(UINT nRes) override;
	LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L) override;
	LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L) override;

	//capture
	void SetCapture() override;
	void ReleaseCapture() override;
	bool IsCaptured() override;

	//focus
	void SetFocusControl(CDUIControlBase *pFocusCtrl) override;

	//root
	CDUIContainerCtrl * DetachRootCtrl() override;

	//tips
	void RefreshToolTip(CMMString strToolTip) override;

	//window
	void ResizeWnd(int cx = -1, int cy = -1) override;
	void AdjustWndPos() override;
	bool IsMaximized() override;
	bool IsMinimized() override;
	CDUIRect GetClientRect() const override;
	CDUIRect GetWindowRect() override;

	//refresh
	void Invalidate() override;

	//caret
	bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight) override;
	void ShowCaret(bool bShow) override;
	void SetCaretPos(CDUIPoint pt) override;

	//window dragdrop
	virtual bool BeginDragDrop(CDUIControlBase *pControl, WPARAM wParam, LPARAM lParam, int nFlag = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
	virtual void EndDragDrop();
#if (NTDDI_VERSION >= NTDDI_VISTA)
	virtual void SetDropDescription(DROPIMAGETYPE dwDropType, PCWSTR pszMsg, PCWSTR pszInsert);
#endif
	virtual void ClearDescription();
	virtual const tagDuiDropData * GetDropData();
	virtual HBITMAP ProductBitmap(IN std::vector<CDUIControlBase*> vecCtrl, IN CDUIControlBase* pFromCtrl, OUT CDUIRect &rcBitmap, OUT CDUIPoint &ptFromOffset);

	//message
protected:
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnClose(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnPaint(CDUIRect rcPaint) override;
	LRESULT OnImeComPosition(WPARAM wParam, LPARAM lParam) override;

	//notify
protected:
	void OnFinalMessage() override;

	//help
protected:
	void UpdateImeCompositionPos() override;
	void ReleasePaintScene() override;

	//wnd
private:
	bool RegisterSuperclass();
	bool RegisterWindowClass();
	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//static
public:
	static void ForegroundWindow(HWND hWnd);
};
#endif