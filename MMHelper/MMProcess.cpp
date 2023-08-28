#include "stdafx.h"
#include "MMProcess.h"

//////////////////////////////////////////////////////////////////////////
bool CMMProcess::CreateFileLink(CMMString strFileFull, CMMString strPathTo, CMMString strNameLink)
{
	if (strFileFull.IsEmpty() || strPathTo.IsEmpty()) return false;

	IShellLink *pisl = NULL;
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pisl);
	if (false == SUCCEEDED(hr)) return false;

	//����������Ҫ������ݷ�ʽ��ԭʼ�ļ���ַ
	IPersistFile* pIPF = NULL;
	pisl->SetPath(strFileFull);
	hr = pisl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
	if (false == SUCCEEDED(hr))
	{
		pisl->Release();
		return false;
	}
	
	//����������Ҫ������ݷ�ʽ��Ŀ���ַ
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

	//��ϵͳ�ڵ����н�����һ������--�ĺ������ڻ�ȡϵͳָ�����̵Ŀ��գ�Ҳ���Դ��벻ͬ������ȡ����Щ����ʹ�õĶѡ�ģ����̵߳Ŀ���
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
	{
		printf("CreateToolhelp32Snapshot ����ʧ�ܣ�\n");
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