#include "stdafx.h"
#include "MMProcess.h"

//////////////////////////////////////////////////////////////////////////
bool CMMProcess::CreateFileLink(CMMString strFileFull, CMMString strPathTo, CMMString strNameLink)
{
	if (strFileFull.empty() || strPathTo.empty()) return false;

	IShellLink *pisl = NULL;
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pisl);
	if (false == SUCCEEDED(hr)) return false;

	CMMString strPathFrom;
	CMMFile::ParseFilePathName(strFileFull, strPathFrom, CMMString());

	//这里是我们要创建快捷方式的原始文件地址
	IPersistFile* pIPF = NULL;
	pisl->SetPath(strFileFull);
	pisl->SetWorkingDirectory(strPathFrom);
	hr = pisl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
	if (false == SUCCEEDED(hr))
	{
		pisl->Release();
		return false;
	}
	
	//这里是我们要创建快捷方式的目标地址
	TCHAR szPath[MAX_PATH] = { 0 };
	lstrcpyn(szPath, strPathTo, MMCountString(szPath));
	PathAddBackslash(szPath);
	CMMString strLink = szPath + strNameLink + _T(".lnk");

	LPCOLESTR lpOleStr = W2COLE(strLink);
	pIPF->Save(lpOleStr, FALSE);
	pIPF->Release();
	pisl->Release();

	return true;
}

HANDLE CMMProcess::FindProcessParent(HANDLE hProcess)
{
	if (INVALID_HANDLE_VALUE == hProcess || NULL == hProcess) return NULL;
	
	DWORD dwProcessID = ::GetProcessId(hProcess);

	PROCESSENTRY32 Pe32 = {};
	Pe32.dwSize = sizeof(Pe32);

	//给系统内的所有进程拍一个快照--改函数用于获取系统指定进程的快照，也可以传入不同参数获取被这些进程使用的堆、模块和线程的快照
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
	{
		printf("CreateToolhelp32Snapshot 调用失败！\n");
		return INVALID_HANDLE_VALUE;
	}

	BOOL bMore = ::Process32First(hProcessSnap, &Pe32);
	while (bMore)
	{
		if (dwProcessID == Pe32.th32ProcessID)
		{
			::CloseHandle(hProcessSnap);

			return FindProcessByID(Pe32.th32ParentProcessID);
		}
		
		bMore = ::Process32Next(hProcessSnap, &Pe32);
	}

	::CloseHandle(hProcessSnap);

	return INVALID_HANDLE_VALUE;
}

HANDLE CMMProcess::FindProcessByID(DWORD dwProcessID)
{
	PROCESSENTRY32 Pe32 = {};
	Pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return INVALID_HANDLE_VALUE;
	}

	BOOL bMore = ::Process32First(hSnapshot, &Pe32);
	while (bMore)
	{
		if (dwProcessID == Pe32.th32ProcessID)
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pe32.th32ProcessID);
			::CloseHandle(hSnapshot);
			return hProcess;
		}

		bMore = ::Process32Next(hSnapshot, &Pe32);
	}

	::CloseHandle(hSnapshot);

	return INVALID_HANDLE_VALUE;
}

HANDLE CMMProcess::FindProcessByName(CMMString strName)
{
	PROCESSENTRY32 Pe32 = {};
	Pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return INVALID_HANDLE_VALUE;
	}

	BOOL bMore = ::Process32First(hSnapshot, &Pe32);
	while (bMore)
	{
		if (0 == strName.CompareNoCase(Pe32.szExeFile))
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pe32.th32ProcessID);
			::CloseHandle(hSnapshot);
			return hProcess;
		}

		bMore = ::Process32Next(hSnapshot, &Pe32);
	}

	::CloseHandle(hSnapshot);

	return INVALID_HANDLE_VALUE;
}

bool CMMProcess::IsProcessExist(CMMString strName)
{
	HANDLE hProcess = FindProcessByName(strName);
	if (hProcess && INVALID_HANDLE_VALUE != hProcess)
	{
		CloseHandle(hProcess);

		return true;
	}
	
	return false;
}

bool CMMProcess::IsWow64Process(HANDLE hProcess)
{
	typedef BOOL (WINAPI *LPIsWow64Process)(HANDLE, PBOOL);

	HMODULE hModule = GetModuleHandle(_T("kernel32"));
	if (NULL == hProcess || NULL == hModule) return false;

	LPIsWow64Process pFuncIsWow64Process = (LPIsWow64Process)GetProcAddress(hModule, "IsWow64Process");
	if (NULL == pFuncIsWow64Process) return false;
	
	BOOL bWow64Process = false;
	if (FALSE == pFuncIsWow64Process(hProcess, &bWow64Process)) return false;

	return bWow64Process;
}

CMMString CMMProcess::GetProcessName(DWORD dwProcessID)
{
	PROCESSENTRY32 Pe32 = {};
	Pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return _T("");
	}

	BOOL bMore = ::Process32First(hSnapshot, &Pe32);
	while (bMore)
	{
		if (dwProcessID == Pe32.th32ProcessID)
		{
			::CloseHandle(hSnapshot);

			return Pe32.szExeFile;
		}

		bMore = ::Process32Next(hSnapshot, &Pe32);
	}

	::CloseHandle(hSnapshot);

	return _T("");
}

bool CMMProcess::GetProcessCmdline(HANDLE hProcess, CMMString &strCmdLine)
{
	typedef ULONG KPRIORITY;
	typedef struct _PROCESS_BASIC_INFORMATION {
		NTSTATUS ExitStatus;
		PPEB PebBaseAddress;
		ULONG_PTR AffinityMask;
		KPRIORITY BasePriority;
		ULONG_PTR UniqueProcessId;
		ULONG_PTR InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION;

	typedef __kernel_entry NTSTATUS
	(NTAPI * PNtQueryInformationProcess)(
		IN HANDLE ProcessHandle,
		IN PROCESSINFOCLASS ProcessInformationClass,
		OUT PVOID ProcessInformation,
		IN ULONG ProcessInformationLength,
		OUT PULONG ReturnLength OPTIONAL
		);

	PROCESS_BASIC_INFORMATION pbi = { 0 };
	PEB                       peb = {};
	//PROCESS_PARAMETERS        ProcParam;
	SIZE_T                    dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	RTL_USER_PROCESS_PARAMETERS para;

	//获取信息
	HMODULE hModule = LoadLibrary(_T("ntdll.dll"));
	if (NULL == hModule) return false;

	PNtQueryInformationProcess pNtQueryInformationProcess = (PNtQueryInformationProcess)GetProcAddress(hModule, "NtQueryInformationProcess");
	if (0 != pNtQueryInformationProcess(hProcess, ProcessBasicInformation, (PVOID)&pbi, sizeof(pbi), NULL))
	{
		int nError = GetLastError();
		FreeLibrary(hModule);
		return false;
	}
	if (pbi.PebBaseAddress == nullptr)
	{
		//do somthing 
	}
	if (FALSE == ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), &dwDummy))
	{
		int nError = GetLastError();
		FreeLibrary(hModule);
		return false;
	}
	if (FALSE == ReadProcessMemory(hProcess, peb.ProcessParameters, &para, sizeof(para), &dwDummy))
	{
		int nError = GetLastError();
		FreeLibrary(hModule);
		return false;
	}

	lpAddress = para.CommandLine.Buffer;
	dwSize = para.CommandLine.Length;
	std::vector<TCHAR> vecCmdLine;
	vecCmdLine.resize(dwSize + 1);
	if (FALSE == ReadProcessMemory(hProcess, lpAddress, (LPVOID)vecCmdLine.data(), dwSize, &dwDummy))
	{
		int nError = GetLastError();
		FreeLibrary(hModule);
		return false;
	}

	strCmdLine = vecCmdLine.data();
	FreeLibrary(hModule);

	return true;
}
