// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_DEPRECATE

// Remove pointless warning messages
#ifdef _MSC_VER
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#endif // _MSC_VER
#ifdef __BORLANDC__
#pragma option -w-8027		   // function not expanded inline
#endif

// Required for VS 2008 (fails on XP and Win2000 without this fix)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP//0x0500
#endif

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

// TODO: 在此处引用程序需要的其他头文件
//////////////////////////////////////////////////////////////////////////
#include "DUITHinkHead.h"

#define CLAMP(x,a,b) (min(b, max(a, x)))

#ifndef __FILET__
#ifdef _UNICODE
#define __FILET__						((LPCTSTR)CA2CT(__FILE__))
#define __FUNCTIONT__					((LPCTSTR)CA2CT(__FUNCTION__))
#else
#define __FILET__						__FILE__
#define __FUNCTIONT__					__FUNCTION__
#endif
#endif

#ifdef _DEBUG
	#ifdef _DLL
	#ifdef _WIN64
		#pragma comment(lib, "../lib/tinyxml264D.lib")
	#else
		#pragma comment(lib, "../lib/tinyxml2D.lib")
	#endif
	#else
	#ifdef _WIN64
		#pragma comment(lib, "../lib/tinyxml264_MT_D.lib")
	#else
		#pragma comment(lib, "../lib/tinyxml2_MT_D.lib")
	#endif
	#endif	
#else
	#ifdef _DLL
	#ifdef _WIN64
		#pragma comment(lib, "../lib/tinyxml264.lib")
	#else
		#pragma comment(lib, "../lib/tinyxml2.lib")
	#endif
	#else
	#ifdef _WIN64	
		#pragma comment(lib, "../lib/tinyxml264_MT.lib")
	#else	
		#pragma comment(lib, "../lib/tinyxml2_MT.lib")
	#endif
	#endif
#endif

//////////////////////////////////////////////////////////////////////////
extern tagDuiMenuCmd				g_DuiMenuCmd;
extern CDUIMenuWnd *				g_pDuiMenuWndRoot;

extern tagDuiCalendarCmd			g_DuiCalendarCmd;
extern CDUICalendarWnd *			g_pDuiCalendarWnd;
#ifdef DUITHINKWKE
extern std::map<wkeWebView, CDUIWkeBrowserCtrl*> g_mapWkeBrowserCtrl;
#endif

//////////////////////////////////////////////////////////////////////////
//global ptr
extern IDUIWndNotify *				g_pIDuiWndNotify;
