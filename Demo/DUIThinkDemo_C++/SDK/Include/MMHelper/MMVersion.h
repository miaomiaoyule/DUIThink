#ifndef __MM_VERSION_H__
#define __MM_VERSION_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMVersion
{
public:
	static bool GetOSVersionNum(DWORD &dwOsMajorVer, DWORD &dwOsMinorVer, DWORD &dwOsBuild);
	static bool GetOSVersionInfo(PRTL_OSVERSIONINFOEXW lpVersionInformation);
	static bool IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor);
	static bool IsWow64();

	static bool IsWindowsXPOrGreater();
	static bool IsWindowsXPSP1OrGreater();
	static bool IsWindowsXPSP2OrGreater();
	static bool IsWindowsXPSP3OrGreater();
	static bool IsWindowsVistaOrGreater();
	static bool IsWindowsVistaSP1OrGreater();
	static bool IsWindowsVistaSP2OrGreater();
	static bool IsWindows7OrGreater();
	static bool IsWindows7SP1OrGreater();
	static bool IsWindows8OrGreater();
	static bool IsWindows8Point1OrGreater();				//8.1(6.3)ртио╟Ф╠╬
	static bool IsWindowsThresholdOrGreater();
	static bool IsWindows10OrGreater();
	static bool IsWindows11OrGreater();
	static bool IsWindowsServer();
};

//////////////////////////////////////////////////////////////////////////

#endif