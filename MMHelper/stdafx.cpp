// stdafx.cpp : 只包括标准包含文件的源文件
// MMHelper.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

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

namespace
{
	CMMStringA MMEnsureSdlPath(LPCTSTR lpszPath)
	{
		CMMString strPath = lpszPath ? lpszPath : _T("");
#if !defined(_WIN32)
		strPath.Replace(_T('\\'), _T('/'));
#endif

		return CT2CA(strPath);
	}

	std::unordered_map<SDL_Window *, CMMRect> g_mapWndUpdate;
	std::unordered_map<SDL_WindowID, IMMWndSDL *> g_mapSdlWnd;
	std::mutex g_csSdlWnd;
}

bool IsWindow(HWND hWnd)
{
	return (NULL != hWnd && SDL_GetWindowFromID(SDL_GetWindowID((SDL_Window *)hWnd)) == (SDL_Window *)hWnd);
}

bool PathFileExists(LPCTSTR lpszFile)
{
	if (SDL_GetPathInfo(MMEnsureSdlPath(lpszFile).c_str(), NULL)) return true;

	return false;
}

bool PathIsDirectory(LPCTSTR lpszFile)
{
	SDL_PathInfo info;
	if (SDL_GetPathInfo(MMEnsureSdlPath(lpszFile).c_str(), &info) && info.type == SDL_PATHTYPE_DIRECTORY) 
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
	if (false == SDL_RemovePath(MMEnsureSdlPath(lpszFile).c_str()))
	{
		SDL_Log("delete failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

BOOL MoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName)
{
	if (false == SDL_RenamePath(MMEnsureSdlPath(lpExistingFileName).c_str(), MMEnsureSdlPath(lpNewFileName).c_str())) 
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

	//cur update
	CMMRect rcUpdate;
	GetUpdateRect(hWnd, &rcUpdate, false);

	//combin rect
	CMMRect rcClient, rcInvalidate;
	GetClientRect(hWnd, &rcClient);
	if (lpRect)
	{
		if (false == IntersectRect(&rcInvalidate, lpRect, &rcClient))
		{
			return;
		}
	}
	else
	{
		rcInvalidate = rcClient;
	}
	if (IsRectEmpty(&rcInvalidate))
	{
		return;
	}
	
	UnionRect(&rcInvalidate, &rcInvalidate, &rcUpdate);
	g_mapWndUpdate[hWnd] = rcInvalidate;

	if (false == IsRectEmpty(&rcUpdate)) return;

	//paint message
	SDL_Event e = {};
	e.type = SDL_EVENT_WINDOW_EXPOSED;
	e.window.timestamp = SDL_GetTicksNS();
	e.window.windowID = SDL_GetWindowID(hWnd);
	e.window.data1 = 0;
	e.window.data2 = 0;
	SDL_PeepEvents(&e, 1, SDL_ADDEVENT, 0, 0);

	return;
}

BOOL GetUpdateRect(HWND hWnd, LPRECT lpRect, BOOL bErase)
{
	if (false == IsWindow(hWnd)) return FALSE;

	SDL_Window *pWindow = (SDL_Window *)hWnd;
	auto it = g_mapWndUpdate.find(pWindow);
	if (it == g_mapWndUpdate.end() || IsRectEmpty(&it->second))
	{
		if (lpRect)
		{
			SetRectEmpty(lpRect);
		}
		if (bErase)
		{
			g_mapWndUpdate.erase(hWnd);
		}

		return FALSE;
	}

	if (lpRect)
	{
		*lpRect = it->second;
	}
	if (bErase)
	{
		g_mapWndUpdate.erase(hWnd);
	}

	return TRUE;
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

static SDL_SystemCursor DuiIdcToSdlSystemCursor(int idc)
{
	switch (idc)
	{
	case 32512: return SDL_SYSTEM_CURSOR_DEFAULT;      // IDC_ARROW
	case 32513: return SDL_SYSTEM_CURSOR_TEXT;         // IDC_IBEAM
	case 32514: return SDL_SYSTEM_CURSOR_WAIT;         // IDC_WAIT
	case 32515: return SDL_SYSTEM_CURSOR_CROSSHAIR;    // IDC_CROSS
	case 32516: return SDL_SYSTEM_CURSOR_N_RESIZE;     // IDC_UPARROW (closest)
	case 32642: return SDL_SYSTEM_CURSOR_NWSE_RESIZE;  // IDC_SIZENWSE
	case 32643: return SDL_SYSTEM_CURSOR_NESW_RESIZE;  // IDC_SIZENESW
	case 32644: return SDL_SYSTEM_CURSOR_EW_RESIZE;    // IDC_SIZEWE
	case 32645: return SDL_SYSTEM_CURSOR_NS_RESIZE;    // IDC_SIZENS
	case 32646: return SDL_SYSTEM_CURSOR_MOVE;         // IDC_SIZEALL
	case 32648: return SDL_SYSTEM_CURSOR_NOT_ALLOWED;  // IDC_NO
	case 32649: return SDL_SYSTEM_CURSOR_POINTER;      // IDC_HAND
	default:    return SDL_SYSTEM_CURSOR_DEFAULT;
	}
}

static SDL_Cursor *DuiGetCachedSystemCursor(SDL_SystemCursor id)
{
	static SDL_Cursor *s_cursors[SDL_SYSTEM_CURSOR_COUNT] = {};
	if (id < 0 || id >= SDL_SYSTEM_CURSOR_COUNT)
		return NULL;
	if (NULL == s_cursors[id])
		s_cursors[id] = SDL_CreateSystemCursor(id);
	return s_cursors[id];
}

HCURSOR LoadCursor(HINSTANCE /*hInstance*/, LPCTSTR lpCursorName)
{
	if (NULL == lpCursorName)
		return NULL;

	// System cursors use MAKEINTRESOURCE(id): pointer value holds the ID in the low word.
	const ULONG_PTR idValue = (ULONG_PTR)lpCursorName;
	if (idValue > 0xFFFF)
	{
		// Named / module resource cursors are not supported under SDL yet.
		return (HCURSOR)DuiGetCachedSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	}

	return (HCURSOR)DuiGetCachedSystemCursor(DuiIdcToSdlSystemCursor((int)idValue));
}

HCURSOR SetCursor(HCURSOR hCursor)
{
	HCURSOR hPrev = (HCURSOR)SDL_GetCursor();
	SDL_SetCursor((SDL_Cursor *)hCursor);
	return hPrev;
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

void GetClientRect(HWND hWnd, LPRECT lpRect)
{
	if (false == IsWindow(hWnd) || NULL == lpRect) return;

	int nWidth = 0;
	int nHeight = 0;
	SDL_GetWindowSize(hWnd, &nWidth, &nHeight);
	lpRect->left = 0;
	lpRect->top = 0;
	lpRect->right = nWidth;
	lpRect->bottom = nHeight;

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
	// Win32 semantics: bit15 = currently down, bit0 = toggle on (Caps/Num/Scroll)
	auto downBit = [](bool down) -> short { return down ? (short)0x8000 : (short)0; };

	// Mouse buttons
	if (vKey == VK_LBUTTON || vKey == VK_RBUTTON || vKey == VK_MBUTTON)
	{
		const SDL_MouseButtonFlags buttons = SDL_GetMouseState(NULL, NULL);
		if (vKey == VK_LBUTTON) return downBit((buttons & SDL_BUTTON_LMASK) != 0);
		if (vKey == VK_RBUTTON) return downBit((buttons & SDL_BUTTON_RMASK) != 0);
		return downBit((buttons & SDL_BUTTON_MMASK) != 0);
	}

	int numkeys = 0;
	const bool *state = SDL_GetKeyboardState(&numkeys);
	const SDL_Keymod mods = SDL_GetModState();

	auto scDown = [state, numkeys](SDL_Scancode sc) -> bool
	{
		return state && (int)sc >= 0 && (int)sc < numkeys && state[sc];
	};

	// Toggle keys: low bit = locked, high bit = currently held
	if (vKey == VK_CAPITAL)
	{
		short r = (mods & SDL_KMOD_CAPS) ? (short)1 : (short)0;
		if (scDown(SDL_SCANCODE_CAPSLOCK)) r |= (short)0x8000;
		return r;
	}
	if (vKey == VK_NUMLOCK)
	{
		short r = (mods & SDL_KMOD_NUM) ? (short)1 : (short)0;
		if (scDown(SDL_SCANCODE_NUMLOCKCLEAR)) r |= (short)0x8000;
		return r;
	}
	if (vKey == VK_SCROLL)
	{
		short r = (mods & SDL_KMOD_SCROLL) ? (short)1 : (short)0;
		if (scDown(SDL_SCANCODE_SCROLLLOCK)) r |= (short)0x8000;
		return r;
	}

	// Modifiers (generic vs left/right)
	if (vKey == VK_SHIFT)
		return downBit(scDown(SDL_SCANCODE_LSHIFT) || scDown(SDL_SCANCODE_RSHIFT));
	if (vKey == VK_LSHIFT) return downBit(scDown(SDL_SCANCODE_LSHIFT));
	if (vKey == VK_RSHIFT) return downBit(scDown(SDL_SCANCODE_RSHIFT));

	if (vKey == VK_CONTROL)
		return downBit(scDown(SDL_SCANCODE_LCTRL) || scDown(SDL_SCANCODE_RCTRL));
	if (vKey == VK_LCONTROL) return downBit(scDown(SDL_SCANCODE_LCTRL));
	if (vKey == VK_RCONTROL) return downBit(scDown(SDL_SCANCODE_RCTRL));

	if (vKey == VK_MENU) // Alt
		return downBit(scDown(SDL_SCANCODE_LALT) || scDown(SDL_SCANCODE_RALT));
	if (vKey == VK_LMENU) return downBit(scDown(SDL_SCANCODE_LALT));
	if (vKey == VK_RMENU) return downBit(scDown(SDL_SCANCODE_RALT));

	SDL_Scancode sc = SDL_SCANCODE_UNKNOWN;
	switch (vKey)
	{
	case VK_BACK:   sc = SDL_SCANCODE_BACKSPACE; break;
	case VK_TAB:    sc = SDL_SCANCODE_TAB; break;
	case VK_RETURN: sc = SDL_SCANCODE_RETURN; break;
	case VK_ESCAPE: sc = SDL_SCANCODE_ESCAPE; break;
	case VK_SPACE:  sc = SDL_SCANCODE_SPACE; break;
	case VK_PRIOR:  sc = SDL_SCANCODE_PAGEUP; break;
	case VK_NEXT:   sc = SDL_SCANCODE_PAGEDOWN; break;
	case VK_END:    sc = SDL_SCANCODE_END; break;
	case VK_HOME:   sc = SDL_SCANCODE_HOME; break;
	case VK_LEFT:   sc = SDL_SCANCODE_LEFT; break;
	case VK_UP:     sc = SDL_SCANCODE_UP; break;
	case VK_RIGHT:  sc = SDL_SCANCODE_RIGHT; break;
	case VK_DOWN:   sc = SDL_SCANCODE_DOWN; break;
	case VK_INSERT: sc = SDL_SCANCODE_INSERT; break;
	case VK_DELETE: sc = SDL_SCANCODE_DELETE; break;
	case VK_LWIN:   sc = SDL_SCANCODE_LGUI; break;
	case VK_RWIN:   sc = SDL_SCANCODE_RGUI; break;
	default:
		if (vKey >= '0' && vKey <= '9')
			sc = (SDL_Scancode)(SDL_SCANCODE_0 + (vKey - '0'));
		else if (vKey >= 'A' && vKey <= 'Z')
			sc = (SDL_Scancode)(SDL_SCANCODE_A + (vKey - 'A'));
		else if (vKey >= VK_F1 && vKey <= VK_F12)
			sc = (SDL_Scancode)(SDL_SCANCODE_F1 + (vKey - VK_F1));
		else
		{
			// Fallback: treat vKey as SDL_Keycode (works for overlapping ASCII values)
			SDL_Keycode key = (SDL_Keycode)vKey;
			if (vKey >= 'A' && vKey <= 'Z')
				key = (SDL_Keycode)(vKey - 'A' + 'a');
			sc = SDL_GetScancodeFromKey(key, NULL);
		}
		break;
	}

	if (sc == SDL_SCANCODE_UNKNOWN)
		return 0;
	return downBit(scDown(sc));
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
	const std::string strCaption = (NULL == lpCaption) ? std::string() : MMStringToUtf8(lpCaption);
	const std::string strText = (NULL == lpText) ? std::string() : MMStringToUtf8(lpText);
	fprintf(stderr, "[MessageBox] %s%s%s\n", strCaption.c_str(), strCaption.empty() ? "" : ": ", strText.c_str());
	fflush(stderr);

	SDL_MessageBoxFlags flags = 0;
	if (uType & MB_ICONERROR) flags |= SDL_MESSAGEBOX_ERROR;
	else if (uType & MB_ICONWARNING) flags |= SDL_MESSAGEBOX_WARNING;
	else if (uType & MB_ICONINFORMATION) flags |= SDL_MESSAGEBOX_INFORMATION;
	SDL_Window *pWnd = IsWindow(hWnd) ? (SDL_Window *)hWnd : NULL;
	return SDL_ShowSimpleMessageBox(flags, strCaption.c_str(), strText.c_str(), pWnd);
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

HMONITOR MonitorFromWindow(HWND hWnd, DWORD dwFlags)
{
	if (false == IsWindow(hWnd)) return NULL;

	return SDL_GetDisplayForWindow((SDL_Window *)hWnd);
}

void UpdateWindow(HWND hWnd)
{
	if (false == IsWindow(hWnd)) return;

	// Same as InvalidateRect: surface update does not drive Renderer paint.
	InvalidateRect(hWnd, NULL, false);

	return;
}

static IMMWndSDL *MMSdlFindWnd(HWND hWnd)
{
	if (false == IsWindow(hWnd)) return NULL;

	const SDL_WindowID uWndID = SDL_GetWindowID((SDL_Window *)hWnd);
	std::lock_guard<std::mutex> lock(g_csSdlWnd);
	auto it = g_mapSdlWnd.find(uWndID);
	return (it != g_mapSdlWnd.end()) ? it->second : NULL;
}

LRESULT SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	IMMWndSDL *pWnd = MMSdlFindWnd(hWnd);
	if (NULL == pWnd) return 0;

	tagMMSdlAsyncMsg *pAsyncMsg = new (std::nothrow) tagMMSdlAsyncMsg();
	if (NULL == pAsyncMsg) return 0;

	pAsyncMsg->pWnd = pWnd;
	pAsyncMsg->uMsg = Msg;
	pAsyncMsg->wParam = wParam;
	pAsyncMsg->lParam = lParam;

	SDL_Event e = {};
	e.type = MMSdlGetAsyncEventType();
	e.user.timestamp = SDL_GetTicksNS();
	e.user.windowID = SDL_GetWindowID((SDL_Window *)hWnd);
	e.user.code = 0;
	e.user.data1 = pAsyncMsg;
	e.user.data2 = nullptr;

	// Synchronous dispatch: handlers free pAsyncMsg.
	pWnd->OnWndMessage(e);
	return 0;
}

BOOL PostMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	IMMWndSDL *pWnd = MMSdlFindWnd(hWnd);
	if (NULL == pWnd) return FALSE;

	tagMMSdlAsyncMsg *pAsyncMsg = new (std::nothrow) tagMMSdlAsyncMsg();
	if (NULL == pAsyncMsg) return FALSE;

	pAsyncMsg->pWnd = pWnd;
	pAsyncMsg->uMsg = Msg;
	pAsyncMsg->wParam = wParam;
	pAsyncMsg->lParam = lParam;

	SDL_Event e = {};
	e.type = MMSdlGetAsyncEventType();
	e.user.timestamp = SDL_GetTicksNS();
	e.user.windowID = SDL_GetWindowID((SDL_Window *)hWnd);
	e.user.code = 0;
	e.user.data1 = pAsyncMsg;
	e.user.data2 = nullptr;

	if (SDL_PeepEvents(&e, 1, SDL_ADDEVENT, 0, 0) != 1)
	{
		MMSafeDelete(pAsyncMsg);
		return FALSE;
	}

	return TRUE;
}

Uint32 MMSdlGetAsyncEventType()
{
	static Uint32 s_uAsyncEvent = 0;
	if (0 == s_uAsyncEvent)
	{
		s_uAsyncEvent = SDL_RegisterEvents(1);
	}

	return s_uAsyncEvent;
}

void MMSdlRegisterWnd(SDL_WindowID uWndID, IMMWndSDL *pWnd)
{
	if (0 == uWndID || NULL == pWnd) return;

	std::lock_guard<std::mutex> lock(g_csSdlWnd);
	g_mapSdlWnd[uWndID] = pWnd;
}

void MMSdlUnregisterWnd(SDL_WindowID uWndID)
{
	if (0 == uWndID) return;

	std::lock_guard<std::mutex> lock(g_csSdlWnd);
	g_mapSdlWnd.erase(uWndID);
}

void MMSdlDispatchEvent(SDL_Event &e)
{
	IMMWndSDL *pWnd = NULL;

	{
		std::lock_guard<std::mutex> lock(g_csSdlWnd);
		auto it = g_mapSdlWnd.find(e.window.windowID);
		if (it != g_mapSdlWnd.end())
		{
			pWnd = it->second;
		}
	}
	if (NULL == pWnd && e.type == MMSdlGetAsyncEventType())
	{
		tagMMSdlAsyncMsg *pAsyncMsg = static_cast<tagMMSdlAsyncMsg *>(e.user.data1);
		if (pAsyncMsg)
		{
			pWnd = pAsyncMsg->pWnd;
		}
	}

	if (pWnd)
	{
		pWnd->OnWndMessage(e);
	}
}

#endif