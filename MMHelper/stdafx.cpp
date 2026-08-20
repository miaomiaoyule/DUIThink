// stdafx.cpp : 只包括标准包含文件的源文件
// MMHelper.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

//////////////////////////////////////////////////////////////////////////
// v141_xp / older uuid.lib often lack IID_IAgileObject; SDL3 static refs it.
#if defined(DuiPlatform_SDL) && (defined(_WIN32) || defined(_WIN64))
extern "C" const GUID IID_IAgileObject =
{ 0x94ea2b94, 0xe9cc, 0x49e0, { 0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90 } };
#endif

//////////////////////////////////////////////////////////////////////////
void MMHELPER_API MMTrace(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
	CMMString strMsg;
	va_list Args;

	va_start(Args, pstrFormat);
	strMsg.Format(pstrFormat, Args);
	va_end(Args);

	strMsg += _T("\n");
	OutputDebugString(strMsg.GetBuffer(0));

#endif
}

#if defined(DuiPlatform_SDL)

bool IsWindow(HWND hWnd)
{
	return (NULL != hWnd && SDL_GetWindowFromID(SDL_GetWindowID((SDL_Window *)hWnd)) == (SDL_Window *)hWnd);
}

bool PathFileExists(LPCTSTR lpszFile)
{
	if (SDL_GetPathInfo(CT2CA(lpszFile), NULL)) return true;

	return false;
}

bool PathIsDirectory(LPCTSTR lpszFile)
{
	SDL_PathInfo info;
	if (SDL_GetPathInfo(CT2CA(lpszFile), &info) && info.type == SDL_PATHTYPE_DIRECTORY) 
	{
		return true;
	}

	return false;
}

LPTSTR PathAddBackslash(LPTSTR lpszPath)
{
	CMMString strPath = lpszPath;
	if (strPath.Right(1) != _T('\\') && strPath.Right(1) != _T('/'))
	{
		int nLen = lstrlen(lpszPath);
		lpszPath[nLen] = _T('\\');
		lpszPath[nLen + 1] = _T('0');
	}

	return lpszPath;
}

bool DeleteFile(LPCTSTR lpszFile)
{
	if (false == SDL_RemovePath(CT2CA(lpszFile)))
	{
		SDL_Log("delete failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

BOOL MoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName)
{
	if (false == SDL_RenamePath(CT2CA(lpExistingFileName), CT2CA(lpNewFileName))) 
	{
		SDL_Log("move failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool IsWindowVisible(HWND hWnd)
{
	return IsWindow(hWnd) && 0 == (SDL_GetWindowFlags((SDL_Window *)hWnd) & SDL_WINDOW_HIDDEN);
}

bool IsIconic(HWND hWnd)
{
	return IsWindow(hWnd) && 0 != (SDL_GetWindowFlags((SDL_Window *)hWnd) & SDL_WINDOW_MINIMIZED);
}

bool IsZoomed(HWND hWnd)
{
	return IsWindow(hWnd) && 0 != (SDL_GetWindowFlags((SDL_Window *)hWnd) & SDL_WINDOW_MAXIMIZED);
}

void InvalidateRect(HWND hWnd, LPCRECT lpRect, bool bErase)
{
	if (false == IsWindow(hWnd)) return;

	SDL_Rect sdlRect = {};
	if (NULL != lpRect)
	{
		sdlRect.x = lpRect->left;
		sdlRect.y = lpRect->top;
		sdlRect.w = lpRect->right - lpRect->left;
		sdlRect.h = lpRect->bottom - lpRect->top;
	}

	SDL_UpdateWindowSurfaceRects(hWnd, NULL == lpRect ? NULL : &sdlRect, NULL == lpRect ? 0 : 1);

	return;
}

BOOL ScreenToClient(HWND hWnd, LPPOINT lpPoint)
{
	if (false == IsWindow(hWnd) || NULL == lpPoint) return 0;

	SDL_Window *pWindow = (SDL_Window *)hWnd;
	int x, y;
	SDL_GetWindowPosition(pWindow, &x, &y);
	lpPoint->x -= x;
	lpPoint->y -= y;

	return 1;
}

BOOL ClientToScreen(HWND hWnd, LPPOINT lpPoint)
{
	if (false == IsWindow(hWnd) || NULL == lpPoint) return 0;

	SDL_Window *pWindow = (SDL_Window *)hWnd;
	int x, y;
	SDL_GetWindowPosition(pWindow, &x, &y);
	lpPoint->x += x;
	lpPoint->y += y;

	return 1;
}

void GetCursorPos(LPPOINT lpPoint)
{
	if (NULL == lpPoint) return;

	float xGlobal = 0.0f;
	float yGlobal = 0.0f;
	SDL_GetGlobalMouseState(&xGlobal, &yGlobal);
	lpPoint->x = (int)xGlobal;
	lpPoint->y = (int)yGlobal;

	return;
}

void SetCursorPos(int X, int Y)
{
	SDL_WarpMouseGlobal(X, Y);

	return;
}

void GetCaretPos(LPPOINT lpPoint)
{
	if (NULL == lpPoint) return;

	float xGlobal = 0.0f;
	float yGlobal = 0.0f;
	SDL_GetGlobalMouseState(&xGlobal, &yGlobal);
	lpPoint->x = (int)xGlobal;
	lpPoint->y = (int)yGlobal;

	return;
}

HWND GetParent(HWND hWnd)
{
	if (false == IsWindow(hWnd)) return NULL;
	return SDL_GetWindowParent((SDL_Window *)hWnd);
}

HWND GetFocus()
{
	return SDL_GetKeyboardFocus();
}

void GetWindowRect(HWND hWnd, LPRECT lpRect)
{
	if (false == IsWindow(hWnd) || NULL == lpRect) return;
	SDL_Window *pWindow = (SDL_Window *)hWnd;
	int x, y, w, h;
	SDL_GetWindowPosition(pWindow, &x, &y);
	SDL_GetWindowSize(pWindow, &w, &h);
	lpRect->left = x;
	lpRect->top = y;
	lpRect->right = x + w;
	lpRect->bottom = y + h;

	return;
}

void GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
{
	if (NULL == hMonitor || NULL == lpMonitorInfo) return;

	SDL_DisplayID displayID = (SDL_DisplayID)hMonitor;
	SDL_Rect rect;
	if (SDL_GetDisplayBounds(displayID, &rect) != 0) return;
	lpMonitorInfo->rcMonitor.left = rect.x;
	lpMonitorInfo->rcMonitor.top = rect.y;
	lpMonitorInfo->rcMonitor.right = rect.x + rect.w;
	lpMonitorInfo->rcMonitor.bottom = rect.y + rect.h;

	SDL_Rect workRect;
	if (SDL_GetDisplayUsableBounds(displayID, &workRect) != 0) return;
	lpMonitorInfo->rcWork.left = workRect.x;
	lpMonitorInfo->rcWork.top = workRect.y;
	lpMonitorInfo->rcWork.right = workRect.x + workRect.w;
	lpMonitorInfo->rcWork.bottom = workRect.y + workRect.h;
	lpMonitorInfo->dwFlags = 0; // SDL does not provide monitor flags

	return;
}

short GetKeyState(int vKey)
{
	// 需要 SDL 初始化（至少 VIDEO / INPUT），否则返回 0
	if (SDL_WasInit(0) == 0)
	{
		// 仍允许调用 SDL_GetKeyboardState，即使未显式初始化也可能工作，但这里保守返回 0
		// 若希望即使未调用 SDL_Init 也能尽量工作，可以移除此分支。
		return 0;
	}

	// 修饰符（Caps/Num/Scroll）使用 SDL 的 ModState
	SDL_Keymod mods = SDL_GetModState();

	// 处理切换键（返回低位）
	if (vKey == VK_CAPITAL)
	{
		return (mods & SDL_KMOD_CAPS) ? 1 : 0;
	}
	if (vKey == VK_NUMLOCK)
	{
		return (mods & SDL_KMOD_NUM) ? 1 : 0;
	}
	if (vKey == VK_SCROLL)
	{
		return (mods & SDL_KMOD_SCROLL) ? 1 : 0;
	}

	// 获取当前键盘扫描码数组
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (!state) return 0;

	// 常用组合键：左右区分
	if (vKey == VK_SHIFT || vKey == VK_LSHIFT || vKey == VK_RSHIFT)
	{
		return (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) ? static_cast<short>(0x8000) : 0;
	}
	if (vKey == VK_CONTROL || vKey == VK_LCONTROL || vKey == VK_RCONTROL)
	{
		return (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) ? static_cast<short>(0x8000) : 0;
	}
	if (vKey == VK_MENU || vKey == VK_LMENU || vKey == VK_RMENU) // Alt
	{
		return (state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT]) ? static_cast<short>(0x8000) : 0;
	}

	// 字母和数字：尝试把 vKey (ASCII/虚拟键) 映射到 SDL_Keycode（对字母转小写）
	if ((vKey >= 0x30 && vKey <= 0x39) || (vKey >= 0x41 && vKey <= 0x5A))
	{
		// 对 A-Z 使用小写 keycode
		SDL_Keycode keycode = (vKey >= 0x41 && vKey <= 0x5A) ? static_cast<SDL_Keycode>(std::tolower(vKey)) : static_cast<SDL_Keycode>(vKey);
		SDL_Scancode sc = SDL_GetScancodeFromKey(keycode);
		if (sc != SDL_SCANCODE_UNKNOWN)
			return state[sc] ? static_cast<short>(0x8000) : 0;
	}

	// 试探性通用映射：直接把 vKey 当作 SDL_Keycode 传入（对一些键可能有效）
	{
		SDL_Scancode sc = SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(vKey));
		if (sc != SDL_SCANCODE_UNKNOWN)
			return state[sc] ? static_cast<short>(0x8000) : 0;
	}

	// 未识别：返回 0
	return 0;
}

void SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (0 == (uFlags & SWP_NOMOVE))
	{
		SDL_SetWindowPosition(hWnd, X, Y);
	}
	if (0 == (uFlags & SWP_NOSIZE))
	{
		SDL_SetWindowSize(hWnd, cx, cy);
	}

	return;
}

void MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint)
{
	if (false == IsWindow(hWnd)) return;

	SDL_SetWindowPosition(hWnd, X, Y);
	SDL_SetWindowSize(hWnd, nWidth, nHeight);

	return;
}

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	if (false == IsWindow(hWnd)) return 0;

	// SDL does not have a built-in message box, so we can use SDL_ShowSimpleMessageBox
	SDL_MessageBoxFlags flags = 0;
	if (uType & MB_ICONERROR) flags |= SDL_MESSAGEBOX_ERROR;
	else if (uType & MB_ICONWARNING) flags |= SDL_MESSAGEBOX_WARNING;
	else if (uType & MB_ICONINFORMATION) flags |= SDL_MESSAGEBOX_INFORMATION;
	return SDL_ShowSimpleMessageBox(flags, MMStringToUtf8(lpCaption).c_str(), MMStringToUtf8(lpText).c_str(), (SDL_Window *)hWnd);
}

void SetForegroundWindow(HWND hWnd)
{
	if (false == IsWindow(hWnd)) return;

	SDL_ShowWindow(hWnd);
	SDL_RaiseWindow((SDL_Window *)hWnd);

	return;
}

void SetFocus(HWND hWnd)
{
	if (false == IsWindow(hWnd)) return;

	SDL_ShowWindow(hWnd);
	SDL_RaiseWindow((SDL_Window *)hWnd);

	return;
}

void ShowWindow(HWND hWnd, int nCmdShow)
{
	if (false == IsWindow(hWnd)) return;

	switch (nCmdShow)
	{
		case SW_HIDE:
			SDL_HideWindow(hWnd);
			break;
		case SW_SHOW:
		case SW_SHOWNORMAL:
		case SW_SHOWDEFAULT:
			SDL_ShowWindow(hWnd);
			break;
		case SW_MINIMIZE:
			SDL_MinimizeWindow(hWnd);
			break;
		case SW_MAXIMIZE:
			SDL_MaximizeWindow(hWnd);
			break;
		case SW_RESTORE:
			SDL_RestoreWindow(hWnd);
			break;
		default:
			break;
	}

	return;
}

bool SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return false;
	/*if (false == IsWindow(hWnd)) return false;

	CDUIWnd *pWnd = CDUIGlobal::GetInstance()->GetWndByHandle(hWnd);
	if (NULL == pWnd) return false;

	return pWnd->SendMessage(Msg, wParam, lParam);*/
}

HMONITOR MonitorFromWindow(HWND hWnd, DWORD dwFlags)
{
	if (false == IsWindow(hWnd)) return NULL;

	return SDL_GetDisplayForWindow((SDL_Window *)hWnd);
}

void UpdateWindow(HWND hWnd)
{
	if (false == IsWindow(hWnd)) return;

	SDL_UpdateWindowSurface((SDL_Window *)hWnd);

	return;
}

#endif