// stdafx.cpp : 只包括标准包含文件的源文件
// DUIThink.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
//////////////////////////////////////////////////////////////////////////
#if defined(DuiPlatform_SDL) && (defined(_WIN32) || defined(_WIN64))
// v141_xp uuid.lib lacks IID_IAgileObject; SDL3 static refs it when linked into this DLL.
extern "C" const GUID IID_IAgileObject =
{ 0x94ea2b94, 0xe9cc, 0x49e0, { 0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90 } };
#endif

//////////////////////////////////////////////////////////////////////////
tagDuiMenuCmd							g_DuiMenuCmd;
CDUIMenuWnd *							g_pDuiMenuWndRoot = NULL;

tagDuiCalendarCmd						g_DuiCalendarCmd;
CDUICalendarWnd *						g_pDuiCalendarWnd = NULL;

//////////////////////////////////////////////////////////////////////////
IDuiWndNotify *							g_pIDuiWndNotify = NULL;

//////////////////////////////////////////////////////////////////////////
