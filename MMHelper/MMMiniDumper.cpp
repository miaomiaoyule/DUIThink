﻿#include "stdafx.h"
#include "MMMiniDumper.h"

// based on dbghelp.h
typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

MiniDumper* MiniDumper::g_pDumper = NULL;

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if (pOrgEntry == NULL) return FALSE;
	unsigned char newJump[100];
	DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
	dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
	DWORD dwNewEntryAddr = (DWORD)pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

	newJump[0] = 0xE9;  // JMP absolute
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(), pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);

	return bRet;
}

// EXCEPTION_EXECUTE_HANDLER equ 1 表示我已经处理了异常, 可以优雅地结束了
// EXCEPTION_CONTINUE_SEARCH equ 0 表示我不处理, 其他人来吧, 于是windows调用默认的处理程序显示一个错误框, 并结束
// EXCEPTION_CONTINUE_EXECUTION equ - 1 表示错误已经被修复, 请从异常发生处继续执行
LONG WINAPI MiniDumper::MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpTopLevelExceptionFilter)
{
	if (!MiniDumper::g_pDumper)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return g_pDumper->WriteMiniDump(lpTopLevelExceptionFilter);
}

//
// MiniDumper::MiniDumper			
//
MiniDumper::MiniDumper(bool headless)
{
	// Detect if there is more than one MiniDumper.
	//assert(!gpDumper);

	if (!g_pDumper)
	{
		::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		BOOL bRet = PreventSetUnhandledExceptionFilter();
		g_pDumper = this;
		m_bHeadless = headless;						// doesn't throw up a dialog, just writes the file.
	}
}

//
// MiniDumper::WriteMiniDump			
//
LONG MiniDumper::WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo)
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	m_pExceptionInfo = pExceptionInfo;

	// You have to find the right dbghelp.dll. 
	// Look next to the EXE first since the one in System32 might be old (Win2k)

	HMODULE hDll = NULL;
	TCHAR szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName(NULL, m_szAppPath, _MAX_PATH))
	{
		TCHAR *pSlash = _tcsrchr(m_szAppPath, '\\');

		if (pSlash)
		{
			_tcscpy(m_szAppBaseName, pSlash + 1);
			*(pSlash + 1) = 0;
		}

		_tcscpy(szDbgHelpPath, m_szAppPath);
		_tcscat(szDbgHelpPath, _T("DBGHELP.DLL"));
		hDll = ::LoadLibrary(szDbgHelpPath);
	}

	if (hDll == NULL)
	{
		// If we haven't found it yet - try one more time.
		hDll = ::LoadLibrary(_T("DBGHELP.DLL"));
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");

		if (pMiniDumpWriteDump)
		{
			TCHAR szScratch[1024];

			VSetDumpFileName();

			// ask the user if they want to save a dump file
			//_tcssprintf(szScratch, _T("There was an unexpected error:\n\n%s\nWould you like to save a diagnostic file?\n\nFilename: %s"), VGetUserMessage(), m_szDumpPath);
			wsprintf(szScratch, _T("There was an unexpected error:\n\n%s\nWould you like to save a diagnostic file?\n\nFilename: %s"), VGetUserMessage(), m_szDumpPath);
			if (m_bHeadless || (::MessageBox(NULL, szScratch, NULL, MB_YESNO) == IDYES))
			{
				// create the file
				HANDLE hFile = ::CreateFile(m_szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile != INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, VGetUserStreamArray(), NULL);
					if (bOK)
					{
						szResult = NULL;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						//_tcssprintf( szScratch, _T("Failed to save dump file to '%s' (error %d)"), m_szDumpPath, GetLastError() );
						wsprintf(szScratch, _T("Failed to save dump file to '%s' (error %d)"), m_szDumpPath, GetLastError());
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					//_tcssprintf( szScratch, _T("Failed to create dump file '%s' (error %d)"), m_szDumpPath, GetLastError() );
					wsprintf(szScratch, _T("Failed to create dump file '%s' (error %d)"), m_szDumpPath, GetLastError());
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = _T("DBGHELP.DLL too old");
		}
	}
	else
	{
		szResult = _T("DBGHELP.DLL not found");
	}

	if (szResult && !m_bHeadless)
		::MessageBox(NULL, szResult, NULL, MB_OK);

	TerminateProcess(GetCurrentProcess(), 0);

	// MLM Note: ExitThread will work, and it allows the MiniDumper to kill a crashed thread
	// without affecting the rest of the application. The question of the day:
	//   Is That A Good Idea??? Answer: ABSOLUTELY NOT!!!!!!!
	//
	//ExitThread(0);

	return retval;
}

//
// MiniDumper::VSetDumpFileName				
//
void MiniDumper::VSetDumpFileName(void)
{
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	wsprintf(m_szDumpPath, _T("%s%s.%d-%02d-%02d %02d-%02d-%02d.dmp"), m_szAppPath, m_szAppBaseName,
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}  