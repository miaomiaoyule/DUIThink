#if defined DuiPlatform_SDL

#pragma once

class IDuiPaintScene;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWndSDL INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWndSDL = { 0xfb0000bc, 0xedd4, 0x4b04, 0x85, 0x42, 0xed, 0x2d, 0xe3, 0xb3, 0x42, 0x8 };
class DUITHINK_API CDUIWndSDL : public CDUIWndBase
{
public:
	CDUIWndSDL(LPCTSTR lpszDuiName = NULL, HWND hWndParent = NULL);
	virtual ~CDUIWndSDL();

	//variant
protected:
	IDuiPaintScene *							m_pPaintScene = NULL;
	bool										m_bWndMoving = false;	// skip EXPOSED paint while dragging/moving

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	HDC GetWndDC() override;
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

	//message
protected:
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnClose(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnPaint(CDUIRect rcPaint) override;

	//help
protected:
	void PerformCalcWndMinMaxInfo();
	void UpdateImeCompositionPos() override;
	void EnsurePaintScene() override;
	void ReleasePaintScene() override;

	//wnd
private:
	void OnSdlWindowEvent(const SDL_Event& e);
	void OnSdlMouseEvent(const SDL_Event& e);
	void OnSdlKeyEvent(const SDL_Event& e);
	static Uint32 GetSdlUserEventType();
	static UINT SdlKeycodeToVK(SDL_Keycode key);
	static SDL_HitTestResult SDLCALL SDLEnableHitTest(SDL_Window *win, const SDL_Point *pt, void *userdata);
	static bool SDLCALL SDLEventWatch(void *userdata, SDL_Event *e);

	//static
public:
	static void ForegroundWindow(HWND hWnd);
};

//////////////////////////////////////////////////////////////////////////
#endif