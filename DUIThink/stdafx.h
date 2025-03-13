// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
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

//////////////////////////////////////////////////////////////////////////
extern IDUIWndNotify *					g_pIDuiWndNotify;
