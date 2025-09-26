// Written by 城南花已开 QQ:284168136 QQGroup:885314879
// Copyright (c) 城南花已开
//
// This code may be used in compiled form in any way you desire. These
// source files may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Acknowledgements :
// 城南花已开 QQ:284168136 QQGroup:885314879
//
// Beware of bugs.
//

#ifndef __MM_DEFINE_H__
#define __MM_DEFINE_H__

//////////////////////////////////////////////////////////////////////////////////
//无效数值
#define INVALID_BYTE					((BYTE)(0xFF))						//无效数值
#define INVALID_WORD					((WORD)(0xFFFF))					//无效数值
#define INVALID_DWORD					((DWORD)(0xFFFFFFFF))				//无效数值

#ifndef PI
#define PI								(3.141592653589793f)
#endif

#define Len_MD5							(33)								//加密密码
#define Len_Machine_ID					(33)								//序列长度
#define Len_Network_ID					(13)								//网卡长度

//////////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define CountStringBuffer CountStringBufferW
#else
#define CountStringBuffer CountStringBufferA
#endif

//存储长度
#define CountStringBufferA(String)		((UINT)((lstrlenA(String) + 1) * sizeof(CHAR)))
#define CountStringBufferW(String)		((UINT)((lstrlenW(String) + 1) * sizeof(WCHAR)))

//////////////////////////////////////////////////////////////////////////////////
#define MMCountArray(Array)				(sizeof(Array) / sizeof(Array[0]))
#define MMCountString(Array)			(sizeof(Array) / sizeof(Array[0]) - 1)
#define MMSafeRelease(pPointer)			{ if (pPointer) { pPointer->Release(); pPointer = NULL; } }
#define MMSafeDelete(pPointer)			{ try { if (pPointer) {delete pPointer;} } catch (...) { assert(false); } pPointer = NULL; } 
#define MMSafeCloseHandle(hHandle)		{ if (hHandle) { ::CloseHandle(hHandle); hHandle = NULL; } }
#define MMSafeDeleteObject(hObject)		{ if (hObject) { ::DeleteObject(hObject); hObject = NULL; } }
#define MMSafeDeleteDC(hDC)				{ if (hDC) { ::DeleteDC(hDC); hDC = NULL; } }
#define MMSafeDeleteArray(pData)		{ try { delete []pData; } catch (...) { assert(false); } pData = NULL; } 
#define MMSafeDeletePCIDL(pPCIDL)		{ try { if (pPCIDL) { CoTaskMemFree((LPVOID)pPCIDL); } } catch (...) { assert(false); } pPCIDL = NULL; } 
#define MMStaticPtr(Type, pPointer)		(static_cast<Type*>(pPointer))
#define MMDynamicPtr(Type, pPointer)	(dynamic_cast<Type*>(pPointer))
#define MMInvalidString(Str)			(NULL == Str || _T('\0') == Str[0])
#define MMInvalidIPAddress(IPAddress)	((IPAddress == 0L) || (IPAddress == INADDR_NONE))

//////////////////////////////////////////////////////////////////////////
#define MMDeclare_ClassName(class_name)\
public:\
	virtual LPCTSTR GetClass();

#define MMImplement_ClassName(class_name)\
	LPCTSTR class_name::GetClass()\
	{ return _T(#class_name); }

#define MMScreenToClient(DUIRect, hWnd) \
	{ POINT pt = { DUIRect.left, DUIRect.top };\
	ScreenToClient(hWnd, &pt);\
	DUIRect.Offset(pt.x - DUIRect.left, pt.y - DUIRect.top); }

#define MMClientToScreen(DUIRect, hWnd) \
	{ POINT pt = { DUIRect.left, DUIRect.top };\
	ClientToScreen(hWnd, &pt);\
	DUIRect.Offset(pt.x - DUIRect.left, pt.y - DUIRect.top); }

#define MMEmojiExtract(String, nIndex) \
	int nEmoji = CDUIGlobal::IsEmoji(String[nIndex]); \
	CMMString strTextSub = CMMString(String.c_str() + nIndex, max(1, nEmoji)); \
	nEmoji > 0 ? nIndex += (nEmoji - 1) : 0;

//////////////////////////////////////////////////////////////////////////
typedef DWORD DROPEFFECT;

//////////////////////////////////////////////////////////////////////////

#endif