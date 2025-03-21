#include "StdAfx.h"
#include "MMShellExecute.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
//////////////////////////////////////////////////////////////////////////
CMMShellExecute::CMMShellExecute()
	: CMMServiceItem(&m_ThreadPool)
{
}

CMMShellExecute::~CMMShellExecute()
{
}

CMMShellExecute * CMMShellExecute::GetInstance()
{
	static CMMShellExecute Obj;
	return &Obj;
}

bool CMMShellExecute::Init()
{
	if (false == __super::Init()) return false;

	//run
	m_ThreadPool.Run(1);

	return true;
}

bool CMMShellExecute::UnInit()
{
	__super::UnInit();

	m_ThreadPool.Stop();

	return true;
}

bool CMMShellExecute::PerformOpen(HWND hWnd, LPCTSTR lpszOperation, LPCTSTR lpszFile, LPCTSTR lpszParameters, LPCTSTR lpszDirectory, INT nShowCmd, bool bSync)
{
	auto pMessage = std::make_shared<M_MMShellExecute_Open>();
	pMessage->hWnd = hWnd;
	pMessage->uMask = SEE_MASK_DEFAULT;

	lpszOperation = (NULL == lpszOperation ? _T("") : lpszOperation);
	lpszFile = (NULL == lpszFile ? _T("") : lpszFile);
	lpszParameters = (NULL == lpszParameters ? _T("") : lpszParameters);
	lpszDirectory = (NULL == lpszDirectory ? _T("") : lpszDirectory);

#ifdef _UNICODE
	pMessage->strOperation = lpszOperation;
	pMessage->strFile = lpszFile;
	pMessage->strParameters = lpszParameters;
	pMessage->strDirectory = lpszDirectory;
#else
	pMessage->strOperation = Encoding::GBK2Unicode(std::string(lpszOperation));
	pMessage->strFile = Encoding::GBK2Unicode(std::string(lpszFile));
	pMessage->strParameters = Encoding::GBK2Unicode(std::string(lpszParameters));
	pMessage->strDirectory = Encoding::GBK2Unicode(std::string(lpszDirectory));
#endif

	pMessage->nShowCmd = nShowCmd;
	
	if (bSync)
	{
		return OnSubOpen(pMessage);
	}

	Send(this, pMessage);

	return true;
}

bool CMMShellExecute::PerformOpenEx(SHELLEXECUTEINFO ShellInfo, bool bSync)
{
	auto pMessage = std::make_shared<M_MMShellExecute_OpenEx>();
	pMessage->hWnd = ShellInfo.hwnd;
	pMessage->uMask = ShellInfo.fMask;

	ShellInfo.lpVerb = (NULL == ShellInfo.lpVerb ? _T("") : ShellInfo.lpVerb);
	ShellInfo.lpFile = (NULL == ShellInfo.lpFile ? _T("") : ShellInfo.lpFile);
	ShellInfo.lpParameters = (NULL == ShellInfo.lpParameters ? _T("") : ShellInfo.lpParameters);
	ShellInfo.lpDirectory = (NULL == ShellInfo.lpDirectory ? _T("") : ShellInfo.lpDirectory);
	ShellInfo.lpClass = (NULL == ShellInfo.lpClass ? _T("") : ShellInfo.lpClass);

#ifdef _UNICODE
	pMessage->strVerb = ShellInfo.lpVerb;
	pMessage->strFile = ShellInfo.lpFile;
	pMessage->strParameters = ShellInfo.lpParameters;
	pMessage->strDirectory = ShellInfo.lpDirectory;
	pMessage->strClass = ShellInfo.lpClass;
#else
	pMessage->strOperation = Encoding::GBK2Unicode(std::string(ShellInfo.lpVerb));
	pMessage->strFile = Encoding::GBK2Unicode(std::string(ShellInfo.lpFile));
	pMessage->strParameters = Encoding::GBK2Unicode(std::string(ShellInfo.lpParameters));
	pMessage->strDirectory = Encoding::GBK2Unicode(std::string(ShellInfo.lpDirectory));
	pMessage->strClass = Encoding::GBK2Unicode(std::string(ShellInfo.lpClass));
#endif

	pMessage->hInstApp = ShellInfo.hInstApp;
	pMessage->lpIDList = ShellInfo.lpIDList;
	pMessage->hkeyClass = ShellInfo.hkeyClass;
	pMessage->dwHotKey = ShellInfo.dwHotKey;
	pMessage->nShowCmd = ShellInfo.nShow;
	
	if (bSync)
	{
		return OnSubOpenEx(pMessage);
	}

	Send(this, pMessage);

	return true;
}

void CMMShellExecute::PerformOpenPos(CMMString strFile)
{
	auto pMessage = std::make_shared<M_MMShellExecute_OpenPos>();
	pMessage->strFile = strFile;
	Send(this, pMessage);
	
	return;
}

void CMMShellExecute::PerformOpenMode(CMMString strFile)
{
	auto pMessage = std::make_shared<M_MMShellExecute_OpenMode>();
	pMessage->strFile = strFile;
	Send(this, pMessage);

	return;
}

void CMMShellExecute::PerformOpenAttribute(HWND hWnd, CMMString strFile)
{
	if (false == ::PathFileExists(strFile)) return;

	PCIDLIST_ABSOLUTE pPCIDL = CMMFile::GetPCIDLFromPath(strFile);
	SHELLEXECUTEINFO ShellInfo = {};
	ShellInfo.cbSize = sizeof(ShellInfo);
	ShellInfo.hwnd = hWnd;
	ShellInfo.lpVerb = _T("properties");
	ShellInfo.lpIDList = (LPVOID)pPCIDL;
	ShellInfo.nShow = SW_SHOWNORMAL;
	ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
	PerformOpenEx(ShellInfo);

	return;
}

void CMMShellExecute::OnMessage(PtrMMServiceMsg pMessage)
{
	if (typeid(*pMessage) == typeid(M_MMShellExecute_Open))
	{
		OnSubOpen(std::dynamic_pointer_cast<M_MMShellExecute_Open>(pMessage));
		
		return;
	}
	if (typeid(*pMessage) == typeid(M_MMShellExecute_OpenEx))
	{
		OnSubOpenEx(std::dynamic_pointer_cast<M_MMShellExecute_OpenEx>(pMessage));

		return;
	}
	if (typeid(*pMessage) == typeid(M_MMShellExecute_OpenPos))
	{
		return OnSubOpenPos(std::dynamic_pointer_cast<M_MMShellExecute_OpenPos>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMShellExecute_OpenMode))
	{
		return OnSubOpenMode(std::dynamic_pointer_cast<M_MMShellExecute_OpenMode>(pMessage));
	}

	return;
}

bool CMMShellExecute::OnSubOpen(PtrMsgMMShellExecuteOpen pRecvData)
{
	if (NULL == pRecvData) return false;

	CMMReDirection ReDirection;
	ReDirection.PerformDisableReDirection();

	SHELLEXECUTEINFO ShellInfo = {};
	ShellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShellInfo.fMask = pRecvData->uMask;
	ShellInfo.hwnd = pRecvData->hWnd;
	ShellInfo.lpVerb = pRecvData->strOperation;
	ShellInfo.lpFile = pRecvData->strFile;
	ShellInfo.lpParameters = pRecvData->strParameters;
	ShellInfo.lpDirectory = pRecvData->strDirectory;
	ShellInfo.nShow = pRecvData->nShowCmd;
	BOOL bRes = ShellExecuteExW(&ShellInfo);

	return bRes;
}

bool CMMShellExecute::OnSubOpenEx(PtrMsgMMShellExecuteOpenEx pRecvData)
{
	if (NULL == pRecvData) return false;

	CMMReDirection ReDirection;
	ReDirection.PerformDisableReDirection();

	SHELLEXECUTEINFO ShellInfo = { 0 };
	ShellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShellInfo.fMask = pRecvData->uMask;
	ShellInfo.hwnd = pRecvData->hWnd;
	ShellInfo.lpVerb = pRecvData->strVerb;
	ShellInfo.lpFile = pRecvData->strFile;
	ShellInfo.lpDirectory = pRecvData->strDirectory;
	ShellInfo.lpParameters = pRecvData->strParameters;
	ShellInfo.lpClass = pRecvData->strClass;
	ShellInfo.hkeyClass = pRecvData->hkeyClass;
	ShellInfo.dwHotKey = pRecvData->dwHotKey;
	ShellInfo.lpIDList = pRecvData->lpIDList;
	ShellInfo.hInstApp = pRecvData->hInstApp;
	ShellInfo.nShow = pRecvData->nShowCmd;
	BOOL bRes = ShellExecuteEx(&ShellInfo);
	int nError = GetLastError();

	if (ShellInfo.lpIDList)
	{
		::CoTaskMemFree(ShellInfo.lpIDList);
	}

	return bRes;
}

void CMMShellExecute::OnSubOpenPos(PtrMsgMMShellExecuteOpenPos pRecvData)
{
	if (NULL == pRecvData || pRecvData->strFile.empty()) return;

	PIDLIST_RELATIVE pidl;
	LPCITEMIDLIST cpidl;
	LPSHELLFOLDER pDesktopFolder;
	ULONG chEaten;
	HRESULT hr;
	WCHAR wfilePath[MAX_PATH + 1] = { 0 };

	if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
	{
		// IShellFolder::ParseDisplayName要传入宽字节
		LPWSTR lpWStr = NULL;
#ifdef _UNICODE
		_tcscpy(wfilePath, pRecvData->strFile);
		lpWStr = wfilePath;
#else
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strFilePath, -1, wfilePath, MAX_PATH);
		lpWStr = wfilePath;
#endif

		hr = pDesktopFolder->ParseDisplayName(NULL, 0, lpWStr, &chEaten, &pidl, NULL);
		if (FAILED(hr))
		{
			pDesktopFolder->Release();
			return;
		}

		cpidl = pidl;

		// SHOpenFolderAndSelectItems是非公开的API函数，需要从shell32.dll获取
		// 该函数只有XP及以上的系统才支持，Win2000和98是不支持的，考虑到Win2000
		// 和98已经基本不用了，所以就不考虑了，如果后面要支持上述老的系统，则要
				// 添加额外的处理代码
		HMODULE hShell32DLL = ::LoadLibrary(_T("shell32.dll"));
		assert(hShell32DLL != NULL);
		if (hShell32DLL != NULL)
		{
			typedef HRESULT(WINAPI *pSelFun)(LPCITEMIDLIST pidlFolder, UINT cidl, LPCITEMIDLIST  *apidl, DWORD dwFlags);
			pSelFun pFun = (pSelFun)::GetProcAddress(hShell32DLL, "SHOpenFolderAndSelectItems");
			assert(pFun != NULL);
			if (pFun != NULL)
			{
				hr = pFun(cpidl, 0, NULL, 0); // 第二个参数cidl置为0，表示是选中文件
				if (FAILED(hr))
				{
					::FreeLibrary(hShell32DLL);
					pDesktopFolder->Release();
					return;
				}
			}

			::FreeLibrary(hShell32DLL);
		}
		else
		{
			pDesktopFolder->Release();
			return;
		}

		// 释放pDesktopFolder
		pDesktopFolder->Release();
	}
	else
	{
		return;
	}

	return;
}

void CMMShellExecute::OnSubOpenMode(PtrMsgMMShellExecuteOpenMode pRecvData)
{
	if (NULL == pRecvData || pRecvData->strFile.empty()) return;

	OPENASINFO info = { 0 };
	info.pcszFile = pRecvData->strFile;
	info.pcszClass = NULL;
	info.oaifInFlags = OAIF_ALLOW_REGISTRATION | OAIF_EXEC;

	HRESULT hr = SHOpenWithDialog(NULL, &info);

	//return hr;
	return;
}

#endif