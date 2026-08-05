// stdafx.cpp : 只包括标准包含文件的源文件
// DUIThink.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
//////////////////////////////////////////////////////////////////////////
tagDuiMenuCmd							g_DuiMenuCmd;
CDUIMenuWnd *							g_pDuiMenuWndRoot = NULL;

tagDuiCalendarCmd						g_DuiCalendarCmd;
CDUICalendarWnd *						g_pDuiCalendarWnd = NULL;

//////////////////////////////////////////////////////////////////////////
IDuiWndNotify *							g_pIDuiWndNotify = NULL;

//////////////////////////////////////////////////////////////////////////
bool DuiIsWindowVisible(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	return DuiIsWindow(hWnd) && 0 == (SDL_GetWindowFlags((SDL_Window *)hWnd) & SDL_WINDOW_HIDDEN);
#else
	return ::IsWindowVisible(hWnd) == TRUE;
#endif
}

bool DuiIsIconic(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	return DuiIsWindow(hWnd) && 0 != (SDL_GetWindowFlags((SDL_Window *)hWnd) & SDL_WINDOW_MINIMIZED);
#else
	return ::IsIconic(hWnd) == TRUE;
#endif
}

bool DuiIsZoomed(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	return DuiIsWindow(hWnd) && 0 != (SDL_GetWindowFlags((SDL_Window *)hWnd) & SDL_WINDOW_MAXIMIZED);
#else
	return ::IsZoomed(hWnd) == TRUE;
#endif
}

void DuiInvalidateRect(HWND hWnd, LPCRECT lpRect, bool bErase)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return;

	CDUIWnd *pWnd = CDUIGlobal::GetInstance()->GetWndByHandle(hWnd);
	if (pWnd)
	{
		pWnd->Invalidate();

		return;
	}

	SDL_Rect sdlRect = {};
	if (NULL != lpRect)
	{
		sdlRect.x = lpRect->left;
		sdlRect.y = lpRect->top;
		sdlRect.w = lpRect->right - lpRect->left;
		sdlRect.h = lpRect->bottom - lpRect->top;
	}
	SDL_UpdateWindowSurfaceRects(hWnd, NULL == lpRect ? NULL : &sdlRect, NULL == lpRect ? 0 : 1);
#else
	::InvalidateRect(hWnd, lpRect, bErase);
#endif
}

BOOL DuiScreenToClient(HWND hWnd, LPPOINT lpPoint)
{
	if (false == DuiIsWindow(hWnd) || NULL == lpPoint) return 0;

#if defined(DuiPlatform_SDL)
	SDL_Window *pWindow = (SDL_Window *)hWnd;
	int x, y;
	SDL_GetWindowPosition(pWindow, &x, &y);
	lpPoint->x -= x;
	lpPoint->y -= y;

	return 1;
#else
	return ::ScreenToClient(hWnd, lpPoint);
#endif
}

BOOL DuiClientToScreen(HWND hWnd, LPPOINT lpPoint)
{
	if (false == DuiIsWindow(hWnd) || NULL == lpPoint) return 0;

#if defined(DuiPlatform_SDL)
	SDL_Window *pWindow = (SDL_Window *)hWnd;
	int x, y;
	SDL_GetWindowPosition(pWindow, &x, &y);
	lpPoint->x += x;
	lpPoint->y += y;

	return 1;
#else
	return ::ClientToScreen(hWnd, lpPoint);
#endif
}

void DuiGetCursorPos(LPPOINT lpPoint)
{
	if (NULL == lpPoint) return;

#if defined(DuiPlatform_SDL)
	float xGlobal = 0.0f;
	float yGlobal = 0.0f;
	SDL_GetGlobalMouseState(&xGlobal, &yGlobal);
	lpPoint->x = (int)xGlobal;
	lpPoint->y = (int)yGlobal;
#else
	::GetCursorPos(lpPoint);
#endif
}

void DuiSetCursorPos(int X, int Y)
{
#if defined(DuiPlatform_SDL)
	SDL_WarpMouseGlobal(X, Y);
#else
	::SetCursorPos(X, Y);
#endif
}

void DuiGetCaretPos(LPPOINT lpPoint)
{
	if (NULL == lpPoint) return;

#if defined(DuiPlatform_SDL)
	float xGlobal = 0.0f;
	float yGlobal = 0.0f;
	SDL_GetGlobalMouseState(&xGlobal, &yGlobal);
	lpPoint->x = (int)xGlobal;
	lpPoint->y = (int)yGlobal;
#else
	::GetCaretPos(lpPoint);
#endif
}

HWND DuiGetParent(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return NULL;
	return SDL_GetWindowParent((SDL_Window *)hWnd);
#else
	return ::GetParent(hWnd);
#endif
}

HWND DuiGetFocus()
{
#if defined(DuiPlatform_SDL)
	return SDL_GetKeyboardFocus();
#else
	return ::GetFocus();
#endif
}

void DuiGetWindowRect(HWND hWnd, LPRECT lpRect)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd) || NULL == lpRect) return;
	SDL_Window *pWindow = (SDL_Window *)hWnd;
	int x, y, w, h;
	SDL_GetWindowPosition(pWindow, &x, &y);
	SDL_GetWindowSize(pWindow, &w, &h);
	lpRect->left = x;
	lpRect->top = y;
	lpRect->right = x + w;
	lpRect->bottom = y + h;
#else
	::GetWindowRect(hWnd, lpRect);
#endif
}

void DuiGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
{
#if defined(DuiPlatform_SDL)
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
#else
	::GetMonitorInfo(hMonitor, lpMonitorInfo);
#endif
}

void DuiSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
#if defined(DuiPlatform_SDL)
	if (0 == (uFlags & SWP_NOMOVE))
	{
		SDL_SetWindowPosition(hWnd, X, Y);
	}
	if (0 == (uFlags & SWP_NOSIZE))
	{
		SDL_SetWindowSize(hWnd, cx, cy);
	}
#else
	::SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
#endif
}

void DuiMoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return;

	SDL_SetWindowPosition(hWnd, X, Y);
	SDL_SetWindowSize(hWnd, nWidth, nHeight);
#else
	::MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
#endif
}

void DuiMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return;

	// SDL does not have a built-in message box, so we can use SDL_ShowSimpleMessageBox
	SDL_MessageBoxFlags flags = 0;
	if (uType & MB_ICONERROR) flags |= SDL_MESSAGEBOX_ERROR;
	else if (uType & MB_ICONWARNING) flags |= SDL_MESSAGEBOX_WARNING;
	else if (uType & MB_ICONINFORMATION) flags |= SDL_MESSAGEBOX_INFORMATION;
	SDL_ShowSimpleMessageBox(flags, WStringToUtf8(lpCaption).c_str(), WStringToUtf8(lpText).c_str(), (SDL_Window *)hWnd);
#else
	::MessageBox(hWnd, lpText, lpCaption, uType);
#endif
}

void DuiSetForegroundWindow(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return;

	SDL_ShowWindow(hWnd);
	SDL_RaiseWindow((SDL_Window *)hWnd);
#else
	::SetForegroundWindow(hWnd);
#endif
}

void DuiSetFocus(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return;

	SDL_ShowWindow(hWnd);
	SDL_RaiseWindow((SDL_Window *)hWnd);
#else
	::SetFocus(hWnd);
#endif
}

void DuiShowWindow(HWND hWnd, int nCmdShow)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return;

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
#else
	::ShowWindow(hWnd, nCmdShow);
#endif
}

bool DuiSendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return false;

	CDUIWnd *pWnd = CDUIGlobal::GetInstance()->GetWndByHandle(hWnd);
	if (NULL == pWnd) return false;

	return pWnd->SendMessage(Msg, wParam, lParam);
#else
	return ::SendMessage(hWnd, Msg, wParam, lParam) != 0;
#endif
}

HMONITOR DuiMonitorFromWindow(HWND hWnd, DWORD dwFlags)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return NULL;

	return SDL_GetDisplayForWindow((SDL_Window *)hWnd);
#else
	return ::MonitorFromWindow(hWnd, dwFlags);
#endif
}

void DuiUpdateWindow(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	if (false == DuiIsWindow(hWnd)) return;

	SDL_UpdateWindowSurface((SDL_Window *)hWnd);
#else
	::UpdateWindow(hWnd);
#endif
}