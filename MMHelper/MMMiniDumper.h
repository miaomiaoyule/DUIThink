﻿#ifndef __MM_MINIDUMPER_H__
#define __MM_MINIDUMPER_H__

#pragma once

class MMHELPER_API MiniDumper
{
public:
	MiniDumper(bool headless);

protected:
	static MiniDumper *g_pDumper;
	static LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpTopLevelExceptionFilter);
	virtual void VSetDumpFileName(void);
	virtual MINIDUMP_USER_STREAM_INFORMATION *VGetUserStreamArray() { return NULL; }
	virtual const TCHAR *VGetUserMessage() { return _T(""); }

	_EXCEPTION_POINTERS *				m_pExceptionInfo;

	TCHAR								m_szDumpPath[MAX_PATH];
	TCHAR								m_szAppPath[MAX_PATH];
	TCHAR								m_szAppBaseName[MAX_PATH];
	LONG								WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo);
	bool								m_bHeadless;
};   

#endif