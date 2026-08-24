// stdafx.cpp : source file that includes just the standard includes
// DUIThink.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
tagDuiMenuCmd							g_DuiMenuCmd;
CDUIMenuWnd *							g_pDuiMenuWndRoot = NULL;

tagDuiCalendarCmd						g_DuiCalendarCmd;
CDUICalendarWnd *						g_pDuiCalendarWnd = NULL;

//////////////////////////////////////////////////////////////////////////
IDuiWndNotify *							g_pIDuiWndNotify = NULL;

//////////////////////////////////////////////////////////////////////////
#ifdef DuiPlatform_SDL
bool SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (false == IsWindow(hWnd)) return false;

	CDUIWnd *pWnd = CDUIGlobal::GetInstance()->GetWndByHandle(hWnd);
	if (NULL == pWnd) return false;

	return 0 != pWnd->SendMessage(Msg, wParam, lParam);
}

bool PostMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (false == IsWindow(hWnd)) return false;

	CDUIWnd *pWnd = CDUIGlobal::GetInstance()->GetWndByHandle(hWnd);
	if (NULL == pWnd) return false;

	return 0 != pWnd->PostMessage(Msg, wParam, lParam);
}

HWND FindWindow(LPCTSTR lpszClassName, LPCTSTR lpszTitle)
{
	// Match registered DUI windows (class = GetClass(), title = SDL window title).
	// NULL class/title means "any", same as Win32 FindWindow.
	MapWnd mapWnd = CDUIGlobal::GetInstance()->GetWndAll();
	for (auto &Item : mapWnd)
	{
		CDUIWnd *pWnd = Item.first;
		if (NULL == pWnd) continue;

		HWND hWnd = pWnd->GetWndHandle();
		if (false == IsWindow(hWnd)) continue;

		if (lpszClassName && *lpszClassName)
		{
			LPCTSTR pszClass = pWnd->GetClass();
			if (NULL == pszClass) continue;

			if (0 != lstrcmp(pszClass, lpszClassName)) continue;
		}

		if (lpszTitle)
		{
			const char *pszUtf8 = SDL_GetWindowTitle((SDL_Window *)hWnd);
			CMMString strTitle = CA2CT(pszUtf8, CP_UTF8);

			if (0 != lstrcmp(strTitle, lpszTitle)) continue;
		}

		return hWnd;
	}

	return NULL;
}

#endif
