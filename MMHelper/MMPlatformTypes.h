// Written by 城南花已开 QQ:284168136 QQGroup:885314879
// Copyright (c) 城南花已开
//
// Win32-shaped types for DuiPlatform_SDL.
// - Windows: keep GDI/Win32 types from windows.h, remap HWND -> SDL_Window*
// - Non-Windows: provide portable POD / message constants with same names

#ifndef __MM_PLATFORM_TYPES_H__
#define __MM_PLATFORM_TYPES_H__

#if defined(DuiPlatform_SDL)

#include "../ThirdDepend/SDL3/SDL.h"

//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)

// Remap HWND while including windows.h so GDI types remain available for paint.
#define HWND NativeWinHWND_DontUse
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#undef HWND
typedef SDL_Window *HWND;
typedef SDL_DisplayID HMONITOR;

#else

//////////////////////////////////////////////////////////////////////////
// basic integer types
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
#ifdef __LP64__
typedef long long INT_PTR;
typedef unsigned long long UINT_PTR;
typedef unsigned long long ULONG_PTR;
typedef long long LONG_PTR;
#else
typedef int INT_PTR;
typedef unsigned int UINT_PTR;
typedef unsigned long ULONG_PTR;
typedef long LONG_PTR;
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
typedef void *HZIPDT;

//////////////////////////////////////////////////////////////////////////
// string types (also defined in MMHelperHead; keep consistent)
#ifndef CP_ACP
#define CP_ACP 0
#define CP_UTF8 1
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

typedef SDL_Window *HWND;
typedef SDL_DisplayID HMONITOR;

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

#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_MBUTTON 0x04
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
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
#define VK_DELETE 0x2E
#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_F1 0x70
#define VK_F12 0x7B

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
#define WM_PAINT 0x000F
#define WM_CLOSE 0x0010
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_SHOWWINDOW 0x0018
#define WM_SETCURSOR 0x0020
#define WM_MOUSEACTIVATE 0x0021
#define WM_GETMINMAXINFO 0x0024
#define WM_WINDOWPOSCHANGING 0x0046
#define WM_WINDOWPOSCHANGED 0x0047
#define WM_CONTEXTMENU 0x007B
#define WM_NCCREATE 0x0081
#define WM_NCDESTROY 0x0082
#define WM_NCCALCSIZE 0x0083
#define WM_NCHITTEST 0x0084
#define WM_NCPAINT 0x0085
#define WM_NCACTIVATE 0x0086
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_CHAR 0x0102
#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105
#define WM_SYSCHAR 0x0106
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
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
#define WM_SIZING 0x0214
#define WM_MOVING 0x0216
#define WM_ENTERSIZEMOVE 0x0231
#define WM_EXITSIZEMOVE 0x0232
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
#define WS_EX_WINDOWEDGE 0x00000100L
#define WS_EX_TOOLWINDOW 0x00000080L
#define WS_EX_DLGMODALFRAME 0x00000001L
#define WS_EX_LAYERED 0x00080000L

#define SRCCOPY 0x00CC0020
#define DIB_RGB_COLORS 0
#define BI_RGB 0L

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
#ifndef FreeLibrary
inline BOOL FreeLibrary(HMODULE) { return TRUE; }
#endif
inline BOOL IntersectRect(LPRECT lprcDst, LPCRECT lprcSrc1, LPCRECT lprcSrc2)
{
	if (NULL == lprcDst || NULL == lprcSrc1 || NULL == lprcSrc2) return FALSE;
	lprcDst->left = (lprcSrc1->left > lprcSrc2->left) ? lprcSrc1->left : lprcSrc2->left;
	lprcDst->top = (lprcSrc1->top > lprcSrc2->top) ? lprcSrc1->top : lprcSrc2->top;
	lprcDst->right = (lprcSrc1->right < lprcSrc2->right) ? lprcSrc1->right : lprcSrc2->right;
	lprcDst->bottom = (lprcSrc1->bottom < lprcSrc2->bottom) ? lprcSrc1->bottom : lprcSrc2->bottom;
	return (lprcDst->left < lprcDst->right && lprcDst->top < lprcDst->bottom) ? TRUE : FALSE;
}
#ifndef ZeroMemory
#define ZeroMemory(Destination, Length) memset((Destination), 0, (Length))
#endif
#ifndef CopyMemory
#define CopyMemory(Destination, Source, Length) memcpy((Destination), (Source), (Length))
#endif
inline int lstrlenA(const char *lpString) { return lpString ? (int)strlen(lpString) : 0; }
inline int lstrlenW(const wchar_t *lpString) { return lpString ? (int)wcslen(lpString) : 0; }
#ifdef UNICODE
#define lstrlen lstrlenW
#else
#define lstrlen lstrlenA
#endif

#endif // _WIN32

#endif // DuiPlatform_SDL

#endif // __MM_PLATFORM_TYPES_H__
