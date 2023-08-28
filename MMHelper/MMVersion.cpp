#include "StdAfx.h"
#include "MMVersion.h"

//////////////////////////////////////////////////////////////////////////
#define _WIN32_WINNT_NT4                    0x0400
#define _WIN32_WINNT_WIN2K                  0x0500
#define _WIN32_WINNT_WINXP                  0x0501
#define _WIN32_WINNT_WS03                   0x0502
#define _WIN32_WINNT_WIN6                   0x0600
#define _WIN32_WINNT_VISTA                  0x0600
#define _WIN32_WINNT_WS08                   0x0600
#define _WIN32_WINNT_LONGHORN               0x0600
#define _WIN32_WINNT_WIN7                   0x0601
#define _WIN32_WINNT_WIN8                   0x0602
#define _WIN32_WINNT_WINBLUE                0x0603
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 /* ABRACADABRA_THRESHOLD*/
#define _WIN32_WINNT_WIN10                  0x0A00 /* ABRACADABRA_THRESHOLD*/

//////////////////////////////////////////////////////////////////////////
bool CMMVersion::GetOSVersionNum(DWORD &dwOsMajorVer, DWORD &dwOsMinorVer, DWORD &dwOsBuild)
{
	HMODULE hModule = NULL;
	bool bRet = false;
	if (hModule = ::LoadLibrary(_T("ntdll.dll")))
	{
		typedef void (WINAPI *getver)(DWORD*, DWORD*, DWORD*);
		getver gv = (getver)GetProcAddress(hModule, "RtlGetNtVersionNumbers");
		if (gv)
		{
			gv(&dwOsMajorVer, &dwOsMinorVer, &dwOsBuild);
			
			bRet = true;
		}

		::FreeLibrary(hModule);
	}

	return bRet;
}

bool CMMVersion::GetOSVersionInfo(PRTL_OSVERSIONINFOEXW lpVersionInformation)
{
	HMODULE hModule = NULL;
	bool bRet = false;
	if (hModule = ::LoadLibrary(_T("ntdll.dll")))
	{
		typedef void (WINAPI *getver)(PRTL_OSVERSIONINFOEXW);
		getver gv = (getver)GetProcAddress(hModule, "RtlGetVersion");
		if (gv)
		{
			gv(lpVersionInformation);

			bRet = true;
		}

		::FreeLibrary(hModule);
	}

	return bRet;
}

bool CMMVersion::IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0 };
	DWORDLONG const dwlConditionMask = VerSetConditionMask(VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL), VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;
	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != false;
}

bool CMMVersion::IsWow64()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
		|| si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		return true;

	return false;
}

bool CMMVersion::IsWindowsXPOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
}

bool CMMVersion::IsWindowsXPSP1OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1);
}

bool CMMVersion::IsWindowsXPSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2);
}

bool CMMVersion::IsWindowsXPSP3OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
}

bool CMMVersion::IsWindowsVistaOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

bool CMMVersion::IsWindowsVistaSP1OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1);
}

bool CMMVersion::IsWindowsVistaSP2OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2);
}

bool CMMVersion::IsWindows7OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}

bool CMMVersion::IsWindows7SP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
}

bool CMMVersion::IsWindows8OrGreater() 
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

//8.1(6.3)ртио╟Ф╠╬
bool CMMVersion::IsWindows8Point1OrGreater()
{
	DWORD dwOsMajorVer = 0, dwOsMinorVer = 0, dwOsBuild = 0;
	if (GetOSVersionNum(dwOsMajorVer, dwOsMinorVer, dwOsBuild))
	{
		if (dwOsMajorVer > HIBYTE(_WIN32_WINNT_WINBLUE))
		{
			return true;
		}
		if (dwOsMajorVer == HIBYTE(_WIN32_WINNT_WINBLUE))
		{
			return dwOsMinorVer >= LOBYTE(_WIN32_WINNT_WINBLUE);
		}
	}

	return false;
}

bool CMMVersion::IsWindowsThresholdOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINTHRESHOLD), LOBYTE(_WIN32_WINNT_WINTHRESHOLD), 0);
}

bool CMMVersion::IsWindows10OrGreater()
{
	static bool bInit = false;
	static bool bWin10 = false;
	if (bInit)
		return bWin10;

	DWORD osMajorVer = 0, osMinorVer = 0, osBuild = 0;
	if (GetOSVersionNum(osMajorVer, osMinorVer, osBuild))
	{
		bInit = true;
		return bWin10 = (osMajorVer >= HIBYTE(_WIN32_WINNT_WINTHRESHOLD) && osMinorVer >= LOBYTE(_WIN32_WINNT_WINTHRESHOLD));
	}

	return false;
}

bool CMMVersion::IsWindows11OrGreater()
{
	static bool bInit = false;
	static bool bWin11 = false;
	if (bInit)
		return bWin11;

	OSVERSIONINFOEXW info = { 0 };
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	if (GetOSVersionInfo(&info))
	{
		bInit = true;
		DWORD dwOsMajorVer = info.dwMajorVersion;
		DWORD dwOsMinorVer = info.dwMinorVersion;
		DWORD dwOsBuild = info.dwBuildNumber;

		return bWin11 = (dwOsMajorVer > 6 && dwOsBuild >= 22000);
	}

	return false;
}

bool CMMVersion::IsWindowsServer() 
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0, 0, VER_NT_WORKSTATION };
	DWORDLONG const dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);
	
	return false == VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
}