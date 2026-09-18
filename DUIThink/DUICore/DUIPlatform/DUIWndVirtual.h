#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWndVirtual INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWndVirtual = { 0x7c3a91e2, 0x4b5d, 0x4e18, 0x9a, 0x2c, 0x1d, 0x8f, 0x6b, 0x4e, 0x2a, 0x11 };
class DUITHINK_API CDUIWndVirtual
	: public CDUIWnd
{
	MMDeclare_Super(CDUIWnd)

public:
	CDUIWndVirtual(LPCTSTR lpszDuiName = NULL, HWND hWndParent = NULL);
	virtual ~CDUIWndVirtual();

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//create
	HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT) override;
	void ShowWindow(bool bShow = true, bool bTakeFocus = true) override;
	void ShowWindow(int nCmdShow) override;
	void Close(UINT nRet = IDOK) override;
	void CenterWindow() override;
	void SetIcon(UINT nRes) override;
	LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L) override;
	LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L) override;

	void SetFocusControl(CDUIControlBase *pFocusCtrl) override;
	bool IsCaptured() override;

	bool AttachRootCtrl(CDUIContainerCtrl *pControl) override;
	CDUIContainerCtrl * DetachRootCtrl() override;

	HDC GetWndDC() override;
	void ResizeWnd(int cx = -1, int cy = -1) override;
	void AdjustWndPos() override;
	bool IsVirtualWnd() override;
	bool IsWindowVisible() override;
	CMMRect GetWindowRect() override;
	CMMRect GetClientRect() override;
	bool IsMaximized() override;
	bool IsMinimized() override;
	void SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) override;

	void RefreshLayout() override;
	void Invalidate() override;

	//message
protected:
	LRESULT OnClose(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnPaint(CDUIRect rcPaint) override;

	//help
protected:
	CDUIWndBase * GetHostWnd() const;
	bool IsAttachHost();
	void AttachHost();
	void DetachHost();
};

#if defined(__ANDROID__)
typedef CDUIWndVirtual CDUIPopupWnd;
#endif

//////////////////////////////////////////////////////////////////////////