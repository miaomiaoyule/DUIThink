// Written by 城南花已开 QQ:284168136 QQGroup:885314879
// Copyright (c) 城南花已开
//
// Win32-shaped types for DuiPlatform_SDL.
// - Windows: keep GDI/Win32 types from windows.h, remap HWND -> SDL_Window*
// - Non-Windows: provide portable POD / message constants with same names

#ifndef __MM_PLATFORM_TYPES_H__
#define __MM_PLATFORM_TYPES_H__

#include <wchar.h>
#include "../ThirdDepend/SDL3/SDL.h"

// SDL3 must live in exactly ONE module. Linking SDL3-static into MMHelper + DUIThink
// (+ Demo) creates separate SDL states; Win32 then fails CreateWindowEx with
// ERROR_INVALID_PARAMETER ("参数错误") because the SDL window class was registered
// by another HINSTANCE. Only the MMHelper build embeds SDL; other modules import
// SDL_* from MMHelper.dll (see SDL3_exports.def).
#if defined(_WIN32) || defined(_WIN64)
#if defined(MMHELPER_DLL) || defined(MMHELPLIB)
#ifdef _DEBUG
#pragma comment(lib, "../lib/SDL3-staticd.lib")
#else
#pragma comment(lib, "../lib/SDL3-static.lib")
#endif
#pragma comment(lib, "winmm.lib")      // timeBeginPeriod / timeEndPeriod
#pragma comment(lib, "setupapi.lib")   // SetupDi*
#pragma comment(lib, "version.lib")    // GetFileVersionInfo* / VerQueryValue
#pragma comment(lib, "imm32.lib")      // Imm*
#pragma comment(lib, "ole32.lib")
// uuid.lib on v141_xp may miss IID_IAgileObject; see IID_IAgileObject in this header
#pragma comment(lib, "uuid.lib")
#endif
#endif

typedef SDL_Window *HWND;
typedef SDL_DisplayID HMONITOR;

//////////////////////////////////////////////////////////////////////////
typedef char CHAR;
typedef CHAR *LPSTR;
typedef const CHAR *LPCSTR;
typedef wchar_t WCHAR;
typedef WCHAR *LPWSTR;
typedef const WCHAR *LPCWSTR;
#ifdef UNICODE
typedef WCHAR TCHAR;
#else
typedef CHAR TCHAR;
#endif
typedef TCHAR *LPTSTR;
typedef const TCHAR *LPCTSTR;

typedef short SHORT;
typedef int INT;
typedef float REAL;
typedef long LONG;
typedef long long LONGLONG;
typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef BYTE* LPBYTE;
typedef DWORD ARGB;
typedef DWORD COLORREF;
#ifdef __LP64__
typedef long long INT_PTR;
typedef unsigned long long UINT_PTR;
typedef unsigned long long ULONG_PTR;
typedef long long LONG_PTR;
typedef unsigned long long SIZE_T;
#else
typedef int INT_PTR;
typedef unsigned int UINT_PTR;
typedef unsigned long ULONG_PTR;
typedef long LONG_PTR;
typedef unsigned long SIZE_T;
#endif
typedef ULONG_PTR DWORD_PTR;
typedef LONG_PTR LRESULT;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef int BOOL;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
typedef float FLOAT;

//////////////////////////////////////////////////////////////////////////
// handle-like types
typedef void *HDC;
typedef void *HBITMAP;
typedef void *HFONT;
typedef void *HRGN;
typedef void *HICON;
typedef void *HCURSOR;
typedef void *HBRUSH;
typedef void *HINSTANCE;
typedef void *HMODULE;
typedef void *HMENU;
typedef void *HGDIOBJ;
typedef void *HANDLE;
typedef void *LPVOID;
typedef void *PVOID;
typedef void VOID;

//////////////////////////////////////////////////////////////////////////
/*
* DrawText() Format Flags
*/
#define DT_TOP                      0x00000000
#define DT_LEFT                     0x00000000
#define DT_CENTER                   0x00000001
#define DT_RIGHT                    0x00000002
#define DT_VCENTER                  0x00000004
#define DT_BOTTOM                   0x00000008
#define DT_WORDBREAK                0x00000010
#define DT_SINGLELINE               0x00000020
#define DT_NOCLIP                   0x00000100
#define DT_CALCRECT                 0x00000400
#define DT_NOPREFIX                 0x00000800
#define DT_EDITCONTROL              0x00002000
#define DT_PATH_ELLIPSIS            0x00004000
#define DT_END_ELLIPSIS             0x00008000
#define DT_WORD_ELLIPSIS            0x00040000

//////////////////////////////////////////////////////////////////////////
#ifdef _MAC
#define CALLBACK    PASCAL
#define WINAPI      CDECL
#define WINAPIV     CDECL
#define APIENTRY    WINAPI
#define APIPRIVATE  CDECL
#ifdef _68K_
#define PASCAL      __pascal
#else
#define PASCAL
#endif
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
#define APIENTRY    WINAPI
#define APIPRIVATE  __stdcall
#define PASCAL      __stdcall
#else
#define CALLBACK
#define WINAPI
#define WINAPIV
#define APIENTRY    WINAPI
#define APIPRIVATE
#define PASCAL      pascal
#endif

#ifndef DLL_PROCESS_ATTACH
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3
#define DLL_PROCESS_DETACH 0
#endif

/*
* Dialog Box Command IDs
*/
#define IDOK						1
#define IDCANCEL					2
#define IDABORT						3
#define IDRETRY						4
#define IDIGNORE					5
#define IDYES						6
#define IDNO						7
#define IDCLOSE						8
#define IDHELP						9

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

//////////////////////////////////////////////////////////////////////////
// string types (also defined in MMHelperHead; keep consistent)
// basic integer types
#ifndef _T
#ifdef UNICODE
#define _T(x) L##x
#else
#define _T(x) x
#endif
#endif
#ifndef TEXT
#define TEXT _T
#endif

#ifndef CP_ACP
#define CP_ACP 0
#define CP_UTF8 1
#endif

#undef FAR
#undef  NEAR
#define FAR							far
#define NEAR						near

//////////////////////////////////////////////////////////////////////////
// COM-style GUID (NOT SDL_GUID — SDL_GUID is joystick/device id: Uint8[16])
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID;
#endif
typedef GUID IID;
typedef GUID CLSID;

#ifdef __cplusplus
#define REFGUID const GUID &
#define REFIID const IID &
#define REFCLSID const CLSID &
inline bool operator==(const GUID &a, const GUID &b)
{
	return a.Data1 == b.Data1
		&& a.Data2 == b.Data2
		&& a.Data3 == b.Data3
		&& 0 == memcmp(a.Data4, b.Data4, 8);
}
inline bool operator!=(const GUID &a, const GUID &b)
{
	return !(a == b);
}
#else
#define REFGUID const GUID *
#define REFIID const IID *
#define REFCLSID const CLSID *
#endif

#ifndef InlineIsEqualGUID
#define InlineIsEqualGUID(rguid1, rguid2) (*(rguid1) == *(rguid2))
#endif
#ifndef IsEqualGUID
#define IsEqualGUID(rguid1, rguid2) InlineIsEqualGUID(rguid1, rguid2)
#endif
#ifndef IsEqualIID
#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)
#endif

// v141_xp uuid.lib often lacks IID_IAgileObject; SDL3-static references it.
// Put the definition in this header with selectany/weak so every module that
// includes MMPlatformTypes.h gets the symbol — no per-module copy in stdafx.cpp.
#if defined(_WIN32) || defined(_WIN64)
#if defined(_MSC_VER)
extern "C" __declspec(selectany) const GUID IID_IAgileObject =
{ 0x94ea2b94, 0xe9cc, 0x49e0, { 0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90 } };
#else
extern "C" __attribute__((weak)) const GUID IID_IAgileObject =
{ 0x94ea2b94, 0xe9cc, 0x49e0, { 0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90 } };
#endif
#endif

// MSVC extension used across the framework
#ifndef interface
#define interface struct
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef DECLARE_HANDLE
#define DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name
#endif

#define LOBYTE(w) ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w) ((BYTE)(((DWORD_PTR)(w) >> 8) & 0xff))
#define LOWORD(l) ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l) ((WORD)(((DWORD_PTR)(l) >> 16) & 0xffff))
#define MAKELONG(a, b) ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define MAKEWORD(a, b) ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELPARAM(l, h) ((LPARAM)(DWORD)MAKELONG(l, h))
#define MAKEWPARAM(l, h) ((WPARAM)(DWORD)MAKELONG(l, h))
#define RGB(r, g, b) ((DWORD)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))

#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_SHOWMINIMIZED 2
#define SW_SHOWMAXIMIZED 3
#define SW_MAXIMIZE 3
#define SW_SHOWNOACTIVATE 4
#define SW_SHOW 5
#define SW_MINIMIZE 6
#define SW_SHOWMINNOACTIVE 7
#define SW_SHOWNA 8
#define SW_RESTORE 9
#define SW_SHOWDEFAULT 10

#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004
#define SWP_NOREDRAW 0x0008
#define SWP_NOACTIVATE 0x0010
#define SWP_FRAMECHANGED 0x0020
#define SWP_SHOWWINDOW 0x0040
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOOWNERZORDER 0x0200
#define HWND_TOP ((HWND)0)
#define HWND_BOTTOM ((HWND)1)
#define HWND_TOPMOST ((HWND)-1)
#define HWND_NOTOPMOST ((HWND)-2)

#define MB_OK 0x00000000L
#define MB_ICONERROR 0x00000010L
#define MB_ICONWARNING 0x00000030L
#define MB_ICONINFORMATION 0x00000040L

#define MK_LBUTTON 0x0001
#define MK_RBUTTON 0x0002
#define MK_SHIFT 0x0004
#define MK_CONTROL 0x0008
#define MK_MBUTTON 0x0010
#define MK_ALT 0x1000

#ifndef MAKEINTRESOURCE
#ifdef UNICODE
#define MAKEINTRESOURCE(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#else
#define MAKEINTRESOURCE(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#endif
#endif

// Win32 system cursor IDs (match enDuiCursor)
#ifndef IDC_ARROW
#define IDC_ARROW       MAKEINTRESOURCE(32512)
#define IDC_IBEAM       MAKEINTRESOURCE(32513)
#define IDC_WAIT        MAKEINTRESOURCE(32514)
#define IDC_CROSS       MAKEINTRESOURCE(32515)
#define IDC_UPARROW     MAKEINTRESOURCE(32516)
#define IDC_SIZENWSE    MAKEINTRESOURCE(32642)
#define IDC_SIZENESW    MAKEINTRESOURCE(32643)
#define IDC_SIZEWE      MAKEINTRESOURCE(32644)
#define IDC_SIZENS      MAKEINTRESOURCE(32645)
#define IDC_SIZEALL     MAKEINTRESOURCE(32646)
#define IDC_NO          MAKEINTRESOURCE(32648)
#define IDC_HAND        MAKEINTRESOURCE(32649)
#endif

#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_MBUTTON 0x04
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_CAPITAL 0x14
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_F1 0x70
#define VK_F4 0x73
#define VK_F12 0x7B
#define VK_NUMLOCK 0x90
#define VK_SCROLL  0x91
#define VK_LSHIFT  0xA0
#define VK_RSHIFT  0xA1
#define VK_LCONTROL 0xA2
#define VK_RCONTROL 0xA3
#define VK_LMENU   0xA4
#define VK_RMENU   0xA5

#define WM_NULL 0x0000
#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_ACTIVATE 0x0006
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_ENABLE 0x000A
#define WM_SETTEXT 0x000C
#define WM_GETTEXT 0x000D
#define WM_GETTEXTLENGTH 0x000E
#define WM_PAINT 0x000F
#define WM_CLOSE 0x0010
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_SHOWWINDOW 0x0018
#define WM_ACTIVATEAPP 0x001C
#define WM_SETCURSOR 0x0020
#define WM_MOUSEACTIVATE 0x0021
#define WM_GETMINMAXINFO 0x0024
#define WM_DRAWITEM 0x002B
#define WM_MEASUREITEM 0x002C
#define WM_WINDOWPOSCHANGING 0x0046
#define WM_WINDOWPOSCHANGED 0x0047
#define WM_NOTIFY 0x004E
#define WM_CONTEXTMENU 0x007B
#define WM_GETICON 0x007F
#define WM_SETICON 0x0080
#define WM_NCCREATE 0x0081
#define WM_NCDESTROY 0x0082
#define WM_NCCALCSIZE 0x0083
#define WM_NCHITTEST 0x0084
#define WM_NCPAINT 0x0085
#define WM_NCACTIVATE 0x0086
#define WM_NCMOUSEMOVE 0x00A0
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_CHAR 0x0102
#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105
#define WM_SYSCHAR 0x0106
#define WM_IME_COMPOSITION 0x010F
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
#define WM_HSCROLL 0x0114
#define WM_VSCROLL 0x0115
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_LBUTTONDBLCLK 0x0203
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_RBUTTONDBLCLK 0x0206
#define WM_MBUTTONDOWN 0x0207
#define WM_MBUTTONUP 0x0208
#define WM_MOUSEWHEEL 0x020A
#define WM_PARENTNOTIFY 0x0210
#define WM_SIZING 0x0214
#define WM_CAPTURECHANGED 0x0215
#define WM_MOVING 0x0216
#define WM_ENTERSIZEMOVE 0x0231
#define WM_EXITSIZEMOVE 0x0232
#define WM_DROPFILES 0x0233
#define WM_MOUSEHOVER 0x02A1
#define WM_MOUSELEAVE 0x02A3
#define WM_USER 0x0400
#define WM_APP 0x8000

#define SIZE_RESTORED 0
#define SIZE_MINIMIZED 1
#define SIZE_MAXIMIZED 2
#define CW_USEDEFAULT ((int)0x80000000)
#define WHEEL_DELTA 120
#define TTF_IDISHWND 0x0001

#ifndef SC_SIZE
#define SC_SIZE 0xF000
#define SC_MOVE 0xF010
#define SC_MINIMIZE 0xF020
#define SC_MAXIMIZE 0xF030
#define SC_CLOSE 0xF060
#define SC_RESTORE 0xF120
#endif

#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002
#define CS_DBLCLKS 0x0008
#define CS_SAVEBITS 0x0800

#define WS_OVERLAPPED 0x00000000L
#define WS_POPUP 0x80000000L
#define WS_CHILD 0x40000000L
#define WS_MINIMIZE 0x20000000L
#define WS_VISIBLE 0x10000000L
#define WS_DISABLED 0x08000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_CLIPCHILDREN 0x02000000L
#define WS_CAPTION 0x00C00000L
#define WS_BORDER 0x00800000L
#define WS_DLGFRAME 0x00400000L
#define WS_SYSMENU 0x00080000L
#define WS_THICKFRAME 0x00040000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_MAXIMIZEBOX 0x00010000L
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_EX_TOPMOST 0x00000008L
#define WS_EX_WINDOWEDGE 0x00000100L
#define WS_EX_TOOLWINDOW 0x00000080L
#define WS_EX_DLGMODALFRAME 0x00000001L
#define WS_EX_LAYERED 0x00080000L
#define WS_EX_NOACTIVATE 0x08000000L

#define SRCCOPY 0x00CC0020
#define DIB_RGB_COLORS 0
#define BI_RGB 0L

#ifndef INADDR_NONE
#define INADDR_NONE ((DWORD)0xffffffff)
#endif
#ifndef INADDR_ANY
#define INADDR_ANY ((DWORD)0x00000000)
#endif

#ifndef OUT
#define OUT
#endif
#ifndef IN
#define IN
#endif
#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifdef UNICODE
#define _ttoi _wtoi
#define _ttol _wtol
#define _tcstod wcstod
#define _tcstol wcstol
#define _tcstoul wcstoul
#define _tcscmp wcscmp
#define _tcsncpy wcsncpy
#define _tcslen wcslen
#define lstrlen wcslen
#define lstrcmp wcscmp
#define lstrcpyn wcsncpy
#else
#define _ttoi atoi
#define _ttol atol
#define _tcstod strtod
#define _tcstol strtol
#define _tcstoul strtoul
#define _tcscmp strcmp
#define _tcsncpy strncpy
#define _tcslen strlen
#define lstrlen strlen
#define lstrcmp strcmp
#define lstrcpyn strncpy
#endif

#ifndef _ASSERTE
#define _ASSERTE(expr) assert(expr)
#endif

#ifndef OutputDebugString
inline void OutputDebugStringA(const char *) {}
inline void OutputDebugStringW(const wchar_t *) {}
#ifdef UNICODE
#define OutputDebugString OutputDebugStringW
#else
#define OutputDebugString OutputDebugStringA
#endif
#endif

#ifdef UNICODE
#ifndef LoadLibrary
#define LoadLibrary LoadLibraryW
#endif
#ifndef GetModuleHandle
#define GetModuleHandle GetModuleHandleW
#endif
#else
#ifndef LoadLibrary
#define LoadLibrary LoadLibraryA
#endif
#ifndef GetModuleHandle
#define GetModuleHandle GetModuleHandleA
#endif
#endif
#ifndef MinMax
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#ifndef ZeroMemory
#define ZeroMemory(Destination, Length) memset((Destination), 0, (Length))
#endif
#ifndef CopyMemory
#define CopyMemory(Destination, Source, Length) memcpy((Destination), (Source), (Length))
#endif

//////////////////////////////////////////////////////////////////////////
// geometry
typedef struct tagRECT
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT, *PRECT, *LPRECT;
typedef const RECT *LPCRECT;

typedef struct tagPOINT
{
	LONG x;
	LONG y;
} POINT, *PPOINT, *LPPOINT;

typedef struct tagSIZE
{
	LONG cx;
	LONG cy;
} SIZE, *PSIZE, *LPSIZE;

typedef struct tagMONITORINFO
{
	DWORD cbSize;
	RECT rcMonitor;
	RECT rcWork;
	DWORD dwFlags;
} MONITORINFO, *LPMONITORINFO;

typedef struct tagMSG
{
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG, *PMSG, *LPMSG;

typedef struct tagTOOLINFO
{
	UINT cbSize;
	UINT uFlags;
	HWND hwnd;
	UINT_PTR uId;
	RECT rect;
	HINSTANCE hinst;
	void *lpszText;
} TOOLINFO, *PTOOLINFO, *LPTOOLINFO;

#ifndef FILETIME_DEFINED
#define FILETIME_DEFINED
typedef struct _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif

#ifndef _SYSTEMTIME_
#define _SYSTEMTIME_
typedef struct _SYSTEMTIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} 	SYSTEMTIME;
typedef struct _SYSTEMTIME *PSYSTEMTIME;
typedef struct _SYSTEMTIME *LPSYSTEMTIME;
#endif // !_SYSTEMTIME

// dynamic library — use SDL, do not call Win32 LoadLibrary under DuiPlatform_SDL
#ifndef FARPROC
typedef void (*FARPROC)();
#endif
inline HMODULE LoadLibraryA(const char *lpszFile)
{
	if (NULL == lpszFile || 0 == lpszFile[0]) return NULL;
	return (HMODULE)SDL_LoadObject(lpszFile);
}
inline FARPROC GetProcAddress(HMODULE hModule, const char *lpszProc)
{
	if (NULL == hModule || NULL == lpszProc) return NULL;
	return (FARPROC)SDL_LoadFunction((SDL_SharedObject*)hModule, lpszProc);
}
inline BOOL FreeLibrary(HMODULE hModule)
{
	if (NULL == hModule) return FALSE;
	SDL_UnloadObject((SDL_SharedObject*)hModule);
	return TRUE;
}
inline HMODULE GetModuleHandle(const char * /*lpszModule*/)
{
	// SDL has no exact GetModuleHandle(NULL); return NULL = "current image" fallback
	return NULL;
}
inline HMODULE LoadLibraryW(const wchar_t *lpszFile)
{
	if (NULL == lpszFile) return NULL;
	char szUtf8[MAX_PATH * 4] = {};
	int nOut = 0;
	for (int i = 0; lpszFile[i] && nOut + 4 < (int)sizeof(szUtf8); ++i)
	{
		unsigned int ch = (unsigned int)lpszFile[i];
		if (ch < 0x80)
		{
			szUtf8[nOut++] = (char)ch;
		}
		else if (ch < 0x800)
		{
			szUtf8[nOut++] = (char)(0xC0 | (ch >> 6));
			szUtf8[nOut++] = (char)(0x80 | (ch & 0x3F));
		}
		else
		{
			szUtf8[nOut++] = (char)(0xE0 | (ch >> 12));
			szUtf8[nOut++] = (char)(0x80 | ((ch >> 6) & 0x3F));
			szUtf8[nOut++] = (char)(0x80 | (ch & 0x3F));
		}
	}
	return LoadLibraryA(szUtf8);
}

inline BOOL SetRectEmpty(LPRECT lprc)
{
	if (NULL == lprc) return FALSE;
	lprc->left = lprc->top = lprc->right = lprc->bottom = 0;
	return TRUE;
}

inline BOOL IsRectEmpty(LPCRECT lprc)
{
	if (NULL == lprc) return TRUE;
	return (lprc->left >= lprc->right || lprc->top >= lprc->bottom) ? TRUE : FALSE;
}

inline BOOL EqualRect(LPCRECT lprc1, LPCRECT lprc2)
{
	if (NULL == lprc1 || NULL == lprc2) return FALSE;
	return (lprc1->left == lprc2->left
		&& lprc1->top == lprc2->top
		&& lprc1->right == lprc2->right
		&& lprc1->bottom == lprc2->bottom) ? TRUE : FALSE;
}

inline BOOL CopyRect(LPRECT lprcDst, LPCRECT lprcSrc)
{
	if (NULL == lprcDst || NULL == lprcSrc) return FALSE;
	*lprcDst = *lprcSrc;
	return TRUE;
}

inline BOOL OffsetRect(LPRECT lprc, int dx, int dy)
{
	if (NULL == lprc) return FALSE;
	lprc->left += dx;
	lprc->top += dy;
	lprc->right += dx;
	lprc->bottom += dy;
	return TRUE;
}

inline BOOL InflateRect(LPRECT lprc, int dx, int dy)
{
	if (NULL == lprc) return FALSE;
	lprc->left -= dx;
	lprc->top -= dy;
	lprc->right += dx;
	lprc->bottom += dy;
	return TRUE;
}

// Win32 semantics: left/top inclusive, right/bottom exclusive
inline BOOL PtInRect(LPCRECT lprc, POINT pt)
{
	if (NULL == lprc) return FALSE;
	return (pt.x >= lprc->left && pt.x < lprc->right
		&& pt.y >= lprc->top && pt.y < lprc->bottom) ? TRUE : FALSE;
}

inline BOOL IntersectRect(LPRECT lprcDst, LPCRECT lprcSrc1, LPCRECT lprcSrc2)
{
	if (NULL == lprcDst || NULL == lprcSrc1 || NULL == lprcSrc2) return FALSE;
	lprcDst->left = (lprcSrc1->left > lprcSrc2->left) ? lprcSrc1->left : lprcSrc2->left;
	lprcDst->top = (lprcSrc1->top > lprcSrc2->top) ? lprcSrc1->top : lprcSrc2->top;
	lprcDst->right = (lprcSrc1->right < lprcSrc2->right) ? lprcSrc1->right : lprcSrc2->right;
	lprcDst->bottom = (lprcSrc1->bottom < lprcSrc2->bottom) ? lprcSrc1->bottom : lprcSrc2->bottom;
	if (lprcDst->left >= lprcDst->right || lprcDst->top >= lprcDst->bottom)
	{
		SetRectEmpty(lprcDst);
		return FALSE;
	}
	return TRUE;
}

inline BOOL UnionRect(LPRECT lprcDst, LPCRECT lprcSrc1, LPCRECT lprcSrc2)
{
	if (NULL == lprcDst || NULL == lprcSrc1 || NULL == lprcSrc2) return FALSE;

	const BOOL bEmpty1 = IsRectEmpty(lprcSrc1);
	const BOOL bEmpty2 = IsRectEmpty(lprcSrc2);
	if (bEmpty1 && bEmpty2)
	{
		SetRectEmpty(lprcDst);
		return FALSE;
	}
	if (bEmpty1)
	{
		*lprcDst = *lprcSrc2;
		return TRUE;
	}
	if (bEmpty2)
	{
		*lprcDst = *lprcSrc1;
		return TRUE;
	}

	lprcDst->left = min(lprcSrc1->left, lprcSrc2->left);
	lprcDst->top = min(lprcSrc1->top, lprcSrc2->top);
	lprcDst->right = max(lprcSrc1->right, lprcSrc2->right);
	lprcDst->bottom = max(lprcSrc1->bottom, lprcSrc2->bottom);
	return TRUE;
}

inline int MulDiv(int nNumber, int nNumerator, int nDenominator)
{
	using int64 = long long;

	int64 prod = static_cast<int64>(nNumber) * static_cast<int64>(nNumerator);
	int64 denom = static_cast<int64>(nDenominator);

	if (denom == 0)
	{
		return (prod >= 0) ? INT_MAX : INT_MIN;
	}

	int64 q = prod / denom;
	int64 r = prod % denom;

	int64 abs_r = (r >= 0) ? r : -r;
	int64 abs_d = (denom >= 0) ? denom : -denom;

	if (abs_r * 2 >= abs_d)
	{
		if ((prod ^ denom) >= 0) q += 1;
		else q -= 1;
	}

	if (q > INT_MAX) return INT_MAX;
	if (q < INT_MIN) return INT_MIN;

	return static_cast<int>(q);
}

static const std::chrono::steady_clock::time_point g_tick_start = std::chrono::steady_clock::now();
inline DWORD GetTickCount() noexcept
{
	using namespace std::chrono;
	auto ms = duration_cast<milliseconds>(steady_clock::now() - g_tick_start).count();
	return static_cast<DWORD>(ms & 0xFFFFFFFFu);
}

inline ULONGLONG GetTickCount64() noexcept
{
	using namespace std::chrono;
	auto ms = duration_cast<milliseconds>(steady_clock::now() - g_tick_start).count();
	return static_cast<ULONGLONG>(ms);
}

inline void safe_localtime(const std::time_t *tt, std::tm *outTm)
{
#if defined(_MSC_VER)
	localtime_s(outTm, tt);
#elif defined(__STDC_LIB_EXT1__)
	localtime_s(outTm, tt);
#else
	localtime_r(tt, outTm);
#endif
}

inline void GetLocalTime(SYSTEMTIME *pst)
{
	if (pst == nullptr) return;

	using namespace std::chrono;
	auto now = system_clock::now();
	auto now_ms = duration_cast<milliseconds>(now.time_since_epoch());
	std::time_t t = system_clock::to_time_t(now);

	std::tm local_tm;
	safe_localtime(&t, &local_tm);

	//wDayOfWeek: Windows: 0 = Sunday, tm_wday follows same convention
	pst->wYear = static_cast<WORD>(local_tm.tm_year + 1900);
	pst->wMonth = static_cast<WORD>(local_tm.tm_mon + 1);
	pst->wDayOfWeek = static_cast<WORD>(local_tm.tm_wday);
	pst->wDay = static_cast<WORD>(local_tm.tm_mday);
	pst->wHour = static_cast<WORD>(local_tm.tm_hour);
	pst->wMinute = static_cast<WORD>(local_tm.tm_min);
	pst->wSecond = static_cast<WORD>(local_tm.tm_sec);

	//high-resolution system_clock 的残余量
	pst->wMilliseconds = static_cast<WORD>(now_ms.count() % 1000ULL);

	return;
}

MMHELPER_API inline bool IsWindow(HWND hWnd);
MMHELPER_API inline bool PathFileExists(LPCTSTR lpszFile);
MMHELPER_API inline bool PathIsDirectory(LPCTSTR lpszFile);
MMHELPER_API inline LPTSTR PathAddBackslash(LPTSTR lpszPath);
MMHELPER_API inline bool DeleteFile(LPCTSTR lpszFile);
MMHELPER_API inline BOOL MoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName);
MMHELPER_API inline bool IsWindowVisible(HWND hWnd);
MMHELPER_API inline bool IsIconic(HWND hWnd);
MMHELPER_API inline bool IsZoomed(HWND hWnd);
MMHELPER_API inline void InvalidateRect(HWND hWnd, LPCRECT lpRect, bool bErase);
MMHELPER_API inline BOOL GetUpdateRect(HWND hWnd, LPRECT lpRect, BOOL bErase);
MMHELPER_API inline BOOL ScreenToClient(HWND hWnd, LPPOINT lpPoint);
MMHELPER_API inline BOOL ClientToScreen(HWND hWnd, LPPOINT lpPoint);
MMHELPER_API inline void GetCursorPos(LPPOINT lpPoint);
MMHELPER_API inline void SetCursorPos(int X, int Y);
MMHELPER_API inline HCURSOR LoadCursor(HINSTANCE hInstance, LPCTSTR lpCursorName);
MMHELPER_API inline HCURSOR SetCursor(HCURSOR hCursor);
MMHELPER_API inline void GetCaretPos(LPPOINT lpPoint);
MMHELPER_API inline HWND GetParent(HWND hWnd);
MMHELPER_API inline HWND GetFocus();
MMHELPER_API inline void GetWindowRect(HWND hWnd, LPRECT lpRect);
MMHELPER_API inline void GetClientRect(HWND hWnd, LPRECT lpRect);
MMHELPER_API inline void GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo);
MMHELPER_API inline short GetKeyState(int vKey);
MMHELPER_API inline void SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
MMHELPER_API inline void MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);
MMHELPER_API inline int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
MMHELPER_API inline void SetForegroundWindow(HWND hWnd);
MMHELPER_API inline void SetFocus(HWND hWnd);
MMHELPER_API inline void ShowWindow(HWND hWnd, int nCmdShow);
MMHELPER_API inline HMONITOR MonitorFromWindow(HWND hWnd, DWORD dwFlags);
MMHELPER_API inline void UpdateWindow(HWND hWnd);

//////////////////////////////////////////////////////////////////////////
/* Logical Font */
/* Font Weights */
#define FW_DONTCARE					0
#define FW_THIN						100
#define FW_EXTRALIGHT				200
#define FW_LIGHT					300
#define FW_NORMAL					400
#define FW_MEDIUM					500
#define FW_SEMIBOLD					600
#define FW_BOLD						700
#define FW_EXTRABOLD				800
#define FW_HEAVY					900

#define ANSI_CHARSET				0
#define DEFAULT_CHARSET				1
#define SYMBOL_CHARSET				2
#define SHIFTJIS_CHARSET			128
#define HANGEUL_CHARSET				129
#define HANGUL_CHARSET				129
#define GB2312_CHARSET				134
#define CHINESEBIG5_CHARSET			136
#define OEM_CHARSET					255
#define JOHAB_CHARSET				130
#define HEBREW_CHARSET				177
#define ARABIC_CHARSET				178
#define GREEK_CHARSET				161
#define TURKISH_CHARSET				162
#define VIETNAMESE_CHARSET			163
#define THAI_CHARSET				222
#define EASTEUROPE_CHARSET			238
#define RUSSIAN_CHARSET				204
#define MAC_CHARSET					77
#define BALTIC_CHARSET				186

#define CLEARTYPE_QUALITY			5
#define LF_FACESIZE					32
typedef struct tagLOGFONTA
{
	LONG      lfHeight;
	LONG      lfWidth;
	LONG      lfEscapement;
	LONG      lfOrientation;
	LONG      lfWeight;
	BYTE      lfItalic;
	BYTE      lfUnderline;
	BYTE      lfStrikeOut;
	BYTE      lfCharSet;
	BYTE      lfOutPrecision;
	BYTE      lfClipPrecision;
	BYTE      lfQuality;
	BYTE      lfPitchAndFamily;
	CHAR      lfFaceName[LF_FACESIZE];
} LOGFONTA;
typedef struct tagLOGFONTW
{
	LONG      lfHeight;
	LONG      lfWidth;
	LONG      lfEscapement;
	LONG      lfOrientation;
	LONG      lfWeight;
	BYTE      lfItalic;
	BYTE      lfUnderline;
	BYTE      lfStrikeOut;
	BYTE      lfCharSet;
	BYTE      lfOutPrecision;
	BYTE      lfClipPrecision;
	BYTE      lfQuality;
	BYTE      lfPitchAndFamily;
	WCHAR     lfFaceName[LF_FACESIZE];
} LOGFONTW;
#ifdef UNICODE
typedef LOGFONTW LOGFONT;
#else
typedef LOGFONTA LOGFONT;
#endif // UNICODE

//////////////////////////////////////////////////////////////////////////
// GDI object shims (implemented in DUIThink raster backend)
typedef struct tagBITMAP
{
	LONG   bmType;
	LONG   bmWidth;
	LONG   bmHeight;
	LONG   bmWidthBytes;
	WORD   bmPlanes;
	WORD   bmBitsPixel;
	LPVOID bmBits;
} BITMAP, *PBITMAP, *LPBITMAP;

typedef struct _BLENDFUNCTION
{
	BYTE BlendOp;
	BYTE BlendFlags;
	BYTE SourceConstantAlpha;
	BYTE AlphaFormat;
} BLENDFUNCTION, *PBLENDFUNCTION;

#define AC_SRC_OVER					0x00
#define AC_SRC_ALPHA				0x01
#define ULW_ALPHA					0x00000002
#define OBJ_PEN						1
#define OBJ_BRUSH					2
#define OBJ_DC						3
#define OBJ_FONT					6
#define OBJ_BITMAP					7
#define OBJ_REGION					8
#define OBJ_MEMDC					10
#define LOGPIXELSX					88
#define LOGPIXELSY					90
#define DEFAULT_GUI_FONT			17
#define HOLLOW_BRUSH				5
#define NULL_BRUSH					5
#define TRANSPARENT					1
#define DT_NOPREFIX					0x00000800
#define PS_SOLID					0
#define PS_INSIDEFRAME				0x00000040
#define RGN_AND						1
#define DIB_RGB_COLORS				0
#define BI_RGB						0L

#ifndef GetRValue
#define GetRValue(rgb) ((BYTE)((rgb) & 0xff))
#define GetGValue(rgb) ((BYTE)(((rgb) >> 8) & 0xff))
#define GetBValue(rgb) ((BYTE)(((rgb) >> 16) & 0xff))
#endif

typedef void *HPEN;
// HRGN already typedef'd above as void*

typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize;
	LONG  biWidth;
	LONG  biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG  biXPelsPerMeter;
	LONG  biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO;

#ifndef HRESULT
typedef long HRESULT;
#endif
#ifndef E_FAIL
#define E_FAIL ((HRESULT)0x80004005L)
#endif
#ifndef HGLOBAL
typedef void *HGLOBAL;
#endif
#ifndef GMEM_FIXED
#define GMEM_FIXED 0x0000
#endif

struct IStream
{
	virtual ~IStream() {}
	virtual ULONG Release() { delete this; return 0; }
};

HGLOBAL GlobalAlloc(UINT uFlags, SIZE_T dwBytes);
LPVOID GlobalLock(HGLOBAL hMem);
BOOL GlobalUnlock(HGLOBAL hMem);
HRESULT CreateStreamOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease, IStream **ppstm);

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API IMMWndSDL
{
public:
	virtual ~IMMWndSDL() {}

	// Called by MMSdlDispatchEvent for queued async / EXPOSED events.
	virtual void OnWndMessage(SDL_Event &e) = 0;
};

struct tagMMSdlAsyncMsg
{
	IMMWndSDL *						pWnd = NULL;
	UINT								uMsg = 0;
	WPARAM								wParam = 0;
	LPARAM								lParam = 0;
};

// One shared user-event type for all PostMessage-style async posts.
MMHELPER_API Uint32 MMSdlGetAsyncEventType();

// Register / unregister for EXPOSED lookup by SDL_WindowID.
MMHELPER_API void MMSdlRegisterWnd(SDL_WindowID uWndID, IMMWndSDL *pWnd);
MMHELPER_API void MMSdlUnregisterWnd(SDL_WindowID uWndID);

// Single dispatch entry (like DispatchMessage): call after WaitEvent / PollEvent.
MMHELPER_API void MMSdlDispatchEvent(SDL_Event &e);

//////////////////////////////////////////////////////////////////////////
#endif // __MM_PLATFORM_TYPES_H__
