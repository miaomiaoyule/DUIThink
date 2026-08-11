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

bool DuiIsWindow(HWND hWnd)
{
#if defined(DuiPlatform_SDL)
	return (NULL != hWnd && SDL_GetWindowFromID(SDL_GetWindowID((SDL_Window *)hWnd)) == (SDL_Window *)hWnd);
#else
	return ::IsWindow(hWnd) == TRUE;
#endif
}