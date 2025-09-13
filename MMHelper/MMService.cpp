#include "StdAfx.h"
#include "MMService.h"

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")

//////////////////////////////////////////////////////////////////////////////////

//״̬��Ϣ
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//����״̬
	NAME_BUFFER						NameBuff[16];						//���ֻ���
};

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMMService::CMMService()
{
}

//�����ַ�
bool CMMService::SetClipboardString(HWND hWnd, LPCTSTR pszString)
{
	//��������
	HANDLE hData = NULL;
	BOOL bOpenClopboard = FALSE;

	//ִ���߼�
	__try
	{
		//�򿪿���
		bOpenClopboard = OpenClipboard(hWnd);
		if (bOpenClopboard == FALSE) __leave;

		//��տ���
		if (EmptyClipboard() == FALSE) __leave;

		//�����ڴ�
		hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, CountStringBuffer(pszString));
		if (hData == NULL) __leave;

		//��������
		lstrcpy((LPTSTR)GlobalLock(hData), pszString);
		GlobalUnlock(hData);

		//��������
#ifndef _UNICODE
		::SetClipboardData(CF_TEXT, hData);
#else
		::SetClipboardData(CF_UNICODETEXT, hData);
#endif
	}

	//��ֹ����
	__finally
	{
		//�رտ���
		if (bOpenClopboard == TRUE) CloseClipboard();

		//�����ж�
		if (AbnormalTermination() == TRUE)
		{
			assert(FALSE);
		}
	}

	return true;
}

bool CMMService::SetClipboardFiles(HWND hWndOwner, std::vector<CMMString> vecFiles, bool bCopy /*= true*/)
{
	if (vecFiles.empty()) return false;

	UINT uDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);

	//effect
	HGLOBAL hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
	DWORD *pDropEffect = (DWORD*)GlobalLock(hGblEffect);
	*pDropEffect = bCopy ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
	GlobalUnlock(hGblEffect);

	//len
	UINT uFileListLen = 0;
	for (auto &strFile : vecFiles)
	{
		if (strFile.empty()) continue;

		uFileListLen += (strFile.length() + 1) * sizeof(strFile[0]);
	}

	UINT uGblLen = sizeof(DROPFILES) + (uFileListLen + sizeof(TCHAR));
	HGLOBAL hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, uGblLen);
	BYTE *pData = (BYTE*)GlobalLock(hGblFiles);

	//drop
	DROPFILES DropFiles = {};
	DropFiles.pFiles = sizeof(DROPFILES);
	DropFiles.pt.x = 0;
	DropFiles.pt.y = 0;
	DropFiles.fNC = FALSE;
	DropFiles.fWide = TRUE;
	memcpy(pData, (LPVOID)(&DropFiles), sizeof(DROPFILES));

	//files
	BYTE *pFileList = pData + sizeof(DROPFILES);
	for (auto &strFile : vecFiles)
	{
		if (strFile.empty()) continue;

		int nSize = (strFile.length() + 1) * sizeof(strFile[0]);
		memcpy(pFileList, (BYTE*)strFile.c_str(), nSize);
		pFileList += nSize;
	}

	GlobalUnlock(hGblFiles);

	//set
	if (OpenClipboard(hWndOwner))
	{
		EmptyClipboard();

		//�������ü��а�����Ϊ�϶��ļ�
		SetClipboardData(CF_HDROP, hGblFiles);

		//�������ü��а�����Ϊ���ƻ��߼��б�ʶ
		SetClipboardData(uDropEffect, hGblEffect);

		CloseClipboard();

		return true;
	}

	::GlobalFree(hGblEffect);
	::GlobalFree(hGblFiles);

	return false;
}

CMMString CMMService::GetClipboardString()
{
	CMMString strText;

	do 
	{
		bool bText = IsClipboardFormatAvailable(CF_TEXT);
		bool bUnicodeText = IsClipboardFormatAvailable(CF_UNICODETEXT);
		if (false == bText && false == bUnicodeText) break;
		if (false == OpenClipboard(NULL)) break;

		HANDLE hData = bText ? GetClipboardData(CF_TEXT) : GetClipboardData(CF_UNICODETEXT);
		if (NULL == hData)
		{
			CloseClipboard();

			break;
		}
		if (bText)
		{
			strText = (LPCSTR)GlobalLock(hData);
		}
		else
		{
			strText = (LPCTSTR)GlobalLock(hData);
		}
		
		GlobalUnlock(hData);
		CloseClipboard();

	} while (false);

	return strText;
}

std::vector<CMMString> CMMService::GetClipboardFile()
{
	std::vector<CMMString> vecFile;

	do
	{
		bool bDrop = IsClipboardFormatAvailable(CF_HDROP);
		if (false == bDrop) break;
		if (false == OpenClipboard(NULL)) break;

		HDROP hDropInfo = (HDROP)GetClipboardData(CF_HDROP);
		if (NULL == hDropInfo)
		{
			CloseClipboard();

			break;
		}
		
		UINT nFilesCount = DragQueryFile(hDropInfo, INFINITE, NULL, 0);
		for (UINT i = 0; i < nFilesCount; i++)
		{
			TCHAR szFile[MAX_PATH + 1] = {};
			DragQueryFile(hDropInfo, i, szFile, MMCountString(szFile));
			if (false == PathFileExists(szFile)) continue;

			vecFile.push_back(szFile);
		}

		CloseClipboard();

	} while (false);

	return vecFile;
}

DROPEFFECT CMMService::GetClipboardDropEffect()
{
	DROPEFFECT DropEffect = 0;

	do
	{
		if (false == OpenClipboard(NULL)) break;

		auto ClipFormat = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
		HANDLE hData = GetClipboardData(ClipFormat);
		if (NULL == hData)
		{
			CloseClipboard();

			break;
		}

		DropEffect = *(DROPEFFECT*)GlobalLock(hData);

		GlobalUnlock(hData);
		CloseClipboard();

	} while (false);

	return DropEffect;
}

int CMMService::GetCompileDate()
{
	char sMonth[5] = "";
	int nMonth, nDay, nYear;
	char sMonthName[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char sTime[20] = __DATE__;
	sscanf(sTime, "%s %d %d", sMonth, &nDay, &nYear);

	nMonth = (strstr(sMonthName, sMonth) - sMonthName) / 3 + 1;

	return  nYear * 10000 + nMonth * 100 + nDay;
}

//IP����
CMMString CMMService::IPAddrToString(DWORD dwIPAddr)
{
	struct in_addr in;
	memcpy((char*)&in, (char*)&dwIPAddr, 4);

	return CA2T(inet_ntoa(in));
}

DWORD CMMService::StringToIPAddr(CMMString strIPAddr)
{
	DWORD dwIPAddr = inet_addr((LPCSTR)CT2CA(strIPAddr));
	if (INADDR_NONE == dwIPAddr)
	{
		LPHOSTENT lpHost = gethostbyname((LPCSTR)CT2CA(strIPAddr));
		if (NULL == lpHost) return INADDR_NONE;

		dwIPAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwIPAddr;
}

//������ʶ
bool CMMService::GetMachineID(TCHAR szMachineID[Len_Machine_ID])
{
	GetImportIDEx(szMachineID);
	if (_T('0') != szMachineID[0]) return true;

	//������ʶ
	TCHAR szMACAddress[Len_Network_ID] = {};
	GetMACAddress(szMACAddress);

	//ת����Ϣ
	CMMString strMacAddr = CMMMD5Checksum::GetMD5((BYTE*)szMACAddress, sizeof(szMACAddress));
	lstrcpyn(szMachineID, strMacAddr, MMCountString(szMachineID));

	return true;
}

CMMString CMMService::GetMachineID()
{
	TCHAR szMachineID[Len_Machine_ID] = {};
	GetMachineID(szMachineID);

	return szMachineID;
}

//������ַ
bool CMMService::GetMACAddress(TCHAR szMACAddress[Len_Network_ID])
{
	//��������
	HINSTANCE hInstance = NULL;

	//ִ���߼�
	__try
	{
		//���� DLL
		hInstance = LoadLibrary(_T("NetApi32.dll"));
		if (hInstance == NULL) __leave;

		//��ȡ����
		typedef BYTE __stdcall NetBiosProc(NCB * Ncb);
		NetBiosProc * pNetBiosProc = (NetBiosProc *)GetProcAddress(hInstance, "Netbios");
		if (pNetBiosProc == NULL) __leave;

		//��������
		NCB Ncb;
		LANA_ENUM LanaEnum;
		ZeroMemory(&Ncb, sizeof(Ncb));
		ZeroMemory(&LanaEnum, sizeof(LanaEnum));

		//ö������
		Ncb.ncb_command = NCBENUM;
		Ncb.ncb_length = sizeof(LanaEnum);
		Ncb.ncb_buffer = (BYTE *)&LanaEnum;
		if ((pNetBiosProc(&Ncb) != NRC_GOODRET) || (LanaEnum.length == 0)) __leave;

		//��ȡ��ַ
		if (LanaEnum.length > 0)
		{
			//��������
			tagAstatInfo Adapter;
			ZeroMemory(&Adapter, sizeof(Adapter));

			//��������
			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = LanaEnum.lana[0];
			if (pNetBiosProc(&Ncb) != NRC_GOODRET) __leave;

			//��ȡ״̬
			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_length = sizeof(Adapter);
			Ncb.ncb_buffer = (BYTE *)&Adapter;
			Ncb.ncb_lana_num = LanaEnum.lana[0];
			strcpy((char *)Ncb.ncb_callname, "*");
			if (pNetBiosProc(&Ncb) != NRC_GOODRET) __leave;

			//��ȡ��ַ
			for (INT i = 0; i < 6; i++)
			{
				assert((i * 2) < Len_Network_ID);
				_stprintf(&szMACAddress[i * 2], _T("%02X"), Adapter.AdapterStatus.adapter_address[i]);
			}
		}
	}

	//��������
	__finally
	{
		//�ͷ���Դ
		if (hInstance != NULL)
		{
			FreeLibrary(hInstance);
			hInstance = NULL;
		}

		//�������
		if (AbnormalTermination() == TRUE)
		{
			assert(FALSE);
		}
	}

	return true;
}

//@szSystemInfo	len=4096		// �ڳ���ִ����Ϻ󣬴˴��洢ȡ�õ�ϵͳ������
//@uSystemInfoLen= 0;			// �ڳ���ִ����Ϻ󣬴˴��洢ȡ�õ�ϵͳ������ĳ���
//���� MAC ��ַ��ע��: MAC ��ַ�ǿ�����ע������޸ĵ�
BOOL CMMService::GetMacAddr(BYTE* szSystemInfo, UINT uSystemInfoLen)
{
	UINT uErrorCode = 0;
	IP_ADAPTER_INFO iai;
	ULONG uSize = 0;
	DWORD dwResult = GetAdaptersInfo(&iai, &uSize);
	if (dwResult == ERROR_BUFFER_OVERFLOW)
	{
		IP_ADAPTER_INFO* piai = (IP_ADAPTER_INFO*)HeapAlloc(GetProcessHeap(), 0, uSize);
		if (piai != NULL)
		{
			dwResult = GetAdaptersInfo(piai, &uSize);
			if (ERROR_SUCCESS == dwResult)
			{
				IP_ADAPTER_INFO* piai2 = piai;
				while (piai2 != NULL && (uSystemInfoLen + piai2->AddressLength) < 4096U)
				{
					CopyMemory(szSystemInfo + uSystemInfoLen, piai2->Address, piai2->AddressLength);
					uSystemInfoLen += piai2->AddressLength;
					piai2 = piai2->Next;
				}
			}
			else
			{
				uErrorCode = 0xF0000000U + dwResult;
			}
			assert(HeapFree(GetProcessHeap(), 0, piai));
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		uErrorCode = 0xE0000000U + dwResult;
	}
	if (uErrorCode != 0U)
	{
		return FALSE;
	}
	return TRUE;
}

// Ӳ�����кţ�ע�⣺�е�Ӳ��û�����к�
BOOL CMMService::GetHdiskSerial(BYTE* szSystemInfo, UINT uSystemInfoLen)
{
	OSVERSIONINFOEX ovi = {};
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	CMMVersion::GetOSVersionInfo(&ovi);

	if (ovi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		// Only Windows 2000, Windows XP, Windows Server 2003...
		return FALSE;
	}
	else
	{
		if (!WinNTHDSerialNumAsPhysicalRead(szSystemInfo, &uSystemInfoLen, 1024))
		{
			WinNTHDSerialNumAsScsiRead(szSystemInfo, &uSystemInfoLen, 1024);
		}
	}
	return TRUE;
}

// CPU ID
void CMMService::GetCpuID(BYTE* szSystemInfo, UINT uSystemInfoLen)
// CPU ID
{
	BOOL bException = FALSE;
	BYTE szCpu[16] = {};
	UINT uCpuID = 0U;

	__try
	{
#ifndef _WIN64
		_asm
		{
			mov eax, 0
			cpuid
			mov dword ptr szCpu[0], ebx
			mov dword ptr szCpu[4], edx
			mov dword ptr szCpu[8], ecx
			mov eax, 1
			cpuid
			mov uCpuID, edx
		}
#endif
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		bException = TRUE;
	}

	if (!bException)
	{
		CopyMemory(szSystemInfo + uSystemInfoLen, &uCpuID, sizeof(UINT));
		uSystemInfoLen += sizeof(UINT);

		uCpuID = strlen((char*)szCpu);
		CopyMemory(szSystemInfo + uSystemInfoLen, szCpu, uCpuID);
		uSystemInfoLen += uCpuID;
	}
}

BOOL CMMService::DoIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
	PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
	PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg = 0;
	pSCIP->irDriveRegs.bSectorCountReg = 1;
	pSCIP->irDriveRegs.bSectorNumberReg = 1;
	pSCIP->irDriveRegs.bCylLowReg = 0;
	pSCIP->irDriveRegs.bCylHighReg = 0;

	// calc the drive number.
	pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

	// The command can either be IDE identify or ATAPI identify.
	pSCIP->irDriveRegs.bCommandReg = bIDCmd;
	pSCIP->bDriveNumber = bDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return DeviceIoControl(hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID)pSCIP,
		sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID)pSCOP,
		sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL);
}

bool CMMService::RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey)
{
	//��������
	BYTE cbModifiers = 0;
	if (HIBYTE(wHotKey)&HOTKEYF_ALT) cbModifiers |= MOD_ALT;
	if (HIBYTE(wHotKey)&HOTKEYF_SHIFT) cbModifiers |= MOD_SHIFT;
	if (HIBYTE(wHotKey)&HOTKEYF_CONTROL) cbModifiers |= MOD_CONTROL;

	//ע���ȼ�
	BOOL bSuccess = ::RegisterHotKey(hWnd, uKeyID, cbModifiers, LOBYTE(wHotKey));

	return (bSuccess == TRUE) ? true : false;
}

bool CMMService::UnRegisterHotKey(HWND hWnd, UINT uKeyID)
{
	//ע���ȼ�
	BOOL bSuccess = ::UnregisterHotKey(hWnd, uKeyID);

	return (bSuccess == TRUE) ? true : false;
}

//����Ŀ¼
bool CMMService::GetWorkDirectory(OUT TCHAR szWorkDirectory[], IN WORD wBufferCount)
{
	//ģ��·��
	TCHAR szModulePath[MAX_PATH] = _T("");
	GetModuleFileName(NULL, szModulePath, MMCountArray(szModulePath));

	//�����ļ�
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == _T('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	//���ý��
	assert(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

CMMString CMMService::GetWorkDirectory()
{
	TCHAR szPath[MAX_PATH] = {};
	GetWorkDirectory(szPath, MMCountString(szPath));

	return szPath;
}

CMMString CMMService::GetCurrentPath()
{
	TCHAR szModulePath[MAX_PATH + 1] = _T("");
	::GetCurrentDirectory(MAX_PATH, szModulePath);

	return szModulePath;
}

CMMString CMMService::GetUserDataPath(CMMString strFolderName)
{
	TCHAR szPath[MAX_PATH + 1] = {};
	if (false == SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, FALSE)
		&& false == SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE)
		&& false == ::GetTempPath(MAX_PATH, szPath))
	{
		return CMMString();
	}

	if (false == strFolderName.empty())
	{
		PathAppend(szPath, strFolderName);
	}

	PathAddBackslash(szPath);
	SHCreateDirectory(NULL, szPath);

	return szPath;
}

CMMString CMMService::GetLocalTempPath()
{
	TCHAR szPath[MAX_PATH + 1] = {};
	GetTempPath(MAX_PATH, szPath);
	PathAddBackslash(szPath);
	return szPath;
}

CMMString CMMService::GetAppName()
{
	int nPos = -1;
	std::wstring sName;
	TCHAR szPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);

	sName = szPath;
	nPos = sName.rfind(L"\\");
	if (nPos > 0)
		sName = sName.substr(nPos + 1, sName.length() - nPos - 1);
	else
		sName = L"";

	return sName.c_str();
}

UINT CMMService::ReadFileValue(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = _T("");
	CMMService::GetWorkDirectory(szDirectory, MMCountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = _T("");
	_sntprintf(szServerInfoPath, MMCountArray(szServerInfoPath), _T("%s\\%s"), szDirectory, lpszFileName);

	//��ȡ����
	UINT nValue = GetPrivateProfileInt(pszKeyName, pszItemName, nDefault, szServerInfoPath);

	return nValue;
}

//��ȡ����
VOID CMMService::ReadFileString(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = _T("");
	CMMService::GetWorkDirectory(szDirectory, MMCountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = _T("");
	_sntprintf(szServerInfoPath, MMCountArray(szServerInfoPath), _T("%s\\%s"), szDirectory, lpszFileName);

	//��ȡ����
	GetPrivateProfileString(pszKeyName, pszItemName, _T(""), szResult, wMaxCount, szServerInfoPath);

	return;
}

//д����ֵ
VOID CMMService::WriteFileValue(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = _T("");
	CMMService::GetWorkDirectory(szDirectory, MMCountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = _T("");
	_sntprintf(szServerInfoPath, MMCountArray(szServerInfoPath), _T("%s\\%s"), szDirectory, lpszFileName);

	//��������
	TCHAR szString[16] = _T("");
	_sntprintf(szString, MMCountArray(szString), _T("%d"), nValue);

	//д������
	WritePrivateProfileString(pszKeyName, pszItemName, szString, szServerInfoPath);

	return;
}

//д������
VOID CMMService::WriteFileString(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH] = _T("");
	CMMService::GetWorkDirectory(szDirectory, MMCountArray(szDirectory));

	//����·��
	TCHAR szServerInfoPath[MAX_PATH] = _T("");
	_sntprintf(szServerInfoPath, MMCountArray(szServerInfoPath), _T("%s\\%s"), szDirectory, lpszFileName);

	WritePrivateProfileString(pszKeyName, pszItemName, pszString, szServerInfoPath);

	return;
}

//GUID
CMMString CMMService::ProductGUID()
{
	CMMString strGUID;
	GUID guid;
	CoCreateGuid(&guid);
	strGUID.Format(_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2,
		guid.Data3, guid.Data4[0],
		guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4],
		guid.Data4[5], guid.Data4[6],
		guid.Data4[7]);

	return strGUID;
}

//���ֵ
int CMMService::RandValue(int nMin, int nMax)
{
	if (nMax <= nMin) return nMin;
	return nMin + rand() % (nMax - nMin + 1);
}

double CMMService::RandValue(double lfMin, double lfMax)
{
	if (lfMax <= lfMin) return lfMin;
	return lfMin + (lfMax - lfMin) * (rand() * 1.0 / RAND_MAX);
}

bool CMMService::IsZero(DWORD dwNum)
{
	return (abs((long long)dwNum) <= 1e-15);
}

std::string CMMService::EncryptBase64(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++) ret += base64_chars[char_array_4[i]];

			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}

BOOL CMMService::WinNTHDSerialNumAsScsiRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen)
{
	BOOL bInfoLoaded = FALSE;

	for (int iController = 0; iController < 2; ++iController)
	{
		HANDLE hScsiDriveIOCTL = 0;
		TCHAR   szDriveName[256];

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		_snwprintf(szDriveName, sizeof(szDriveName), L"\\\\.\\Scsi%d:", iController);

		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFile(szDriveName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		// if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
		//            controller, GetLastError ());

		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			int iDrive = 0;
			for (iDrive = 0; iDrive < 2; ++iDrive)
			{
				char szBuffer[sizeof(SRB_IO_CONTROL) + SENDIDLENGTH] = {};

				SRB_IO_CONTROL* p = (SRB_IO_CONTROL*)szBuffer;
				SENDCMDINPARAMS* pin = (SENDCMDINPARAMS*)(szBuffer + sizeof(SRB_IO_CONTROL));
				DWORD dwResult;

				p->HeaderLength = sizeof(SRB_IO_CONTROL);
				p->Timeout = 10000;
				p->Length = SENDIDLENGTH;
				p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				strncpy((char*)p->Signature, "SCSIDISK", 8);

				pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin->bDriveNumber = iDrive;

				if (DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT,
					szBuffer,
					sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
					szBuffer,
					sizeof(SRB_IO_CONTROL) + SENDIDLENGTH,
					&dwResult, NULL))
				{
					SENDCMDOUTPARAMS* pOut = (SENDCMDOUTPARAMS*)(szBuffer + sizeof(SRB_IO_CONTROL));
					IDSECTOR* pId = (IDSECTOR*)(pOut->bBuffer);
					if (pId->sModelNumber[0])
					{
						if (*puSerialLen + 20U <= uMaxSerialLen)
						{
							// ���к�
							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)pId) + 10, 20);

							// Cut off the trailing blanks
							UINT i = 0;
							for (i = 20; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i)
							{
							}
							*puSerialLen += i;

							// �ͺ�
							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)pId) + 27, 40);
							// Cut off the trailing blanks
							for (i = 40; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i)
							{
							}
							*puSerialLen += i;

							bInfoLoaded = TRUE;
						}
						else
						{
							::CloseHandle(hScsiDriveIOCTL);
							return bInfoLoaded;
						}
					}
				}
			}
			::CloseHandle(hScsiDriveIOCTL);
		}
	}
	return bInfoLoaded;
}


BOOL CMMService::WinNTHDSerialNumAsPhysicalRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen)
{
#define  DFP_GET_VERSION          0x00074080
	BOOL bInfoLoaded = FALSE;

	for (UINT uDrive = 0; uDrive < 4; ++uDrive)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		TCHAR szDriveName[256];
		_snwprintf(szDriveName, sizeof(szDriveName), L"\\\\.\\PhysicalDrive%d", uDrive);

		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFile(szDriveName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			GETVERSIONOUTPARAMS VersionParams = {};
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			if (DeviceIoControl(hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL,
				0,
				&VersionParams,
				sizeof(GETVERSIONOUTPARAMS),
				&cbBytesReturned, NULL))
			{
				// If there is a IDE device at number "i" issue commands
				// to the device
				if (VersionParams.bIDEDeviceMap != 0)
				{
					BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
					SENDCMDINPARAMS  scip = {};

					// Now, get the ID sector for all IDE devices in the system.
					// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
					// otherwise use the IDE_ATA_IDENTIFY command
					bIDCmd = (VersionParams.bIDEDeviceMap >> uDrive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
					BYTE IdOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1] = {};

					if (DoIdentify(hPhysicalDriveIOCTL,
						&scip,
						(PSENDCMDOUTPARAMS)&IdOutCmd,
						(BYTE)bIDCmd,
						(BYTE)uDrive,
						&cbBytesReturned))
					{
						if (*puSerialLen + 20U <= uMaxSerialLen)
						{
							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)(((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer)) + 10, 20);  // ���к�

																																// Cut off the trailing blanks
							UINT i = 0;
							for (i = 20; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i) {}
							*puSerialLen += i;

							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)(((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer)) + 27, 40); // �ͺ�

																															   // Cut off the trailing blanks
							for (i = 40; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i) {}
							*puSerialLen += i;

							bInfoLoaded = TRUE;
						}
						else
						{
							::CloseHandle(hPhysicalDriveIOCTL);
							return bInfoLoaded;
						}
					}
				}
			}
			CloseHandle(hPhysicalDriveIOCTL);
		}
	}
	return bInfoLoaded;
}

//������ʶ
bool CMMService::GetImportIDEx(TCHAR szMachineID[Len_Machine_ID], TCHAR szCPUID[20])
{
	// 1. CPU ���кţ�CPUID��
	unsigned int cpuInfo[4] = { 0 };
	__cpuid((int*)cpuInfo, 1); // eax=1 -> Processor Info
	DWORD s1 = cpuInfo[3];     // EDX
	DWORD s2 = cpuInfo[0];     // EAX

	TCHAR szCpuBuf[32] = { 0 };
	_stprintf_s(szCpuBuf, _T("%08X%08X"), s1, s2);

	if (szCPUID)
		_tcsncpy_s(szCPUID, 20, szCpuBuf, _TRUNCATE);

	// 2. Ӳ�����к�
	DWORD volumeSerial = 0;
	if (!GetVolumeInformation(_T("C:\\"), nullptr, 0, &volumeSerial, nullptr, nullptr, nullptr, 0))
		volumeSerial = 0;

	// 3. �ϲ���Ϣ
	BYTE buffer[64] = { 0 };
	memcpy(buffer, &s1, sizeof(s1));
	memcpy(buffer + 4, &s2, sizeof(s2));
	memcpy(buffer + 8, &volumeSerial, sizeof(volumeSerial));

	// 4. ���� MD5
	assert(Len_Machine_ID >= Len_MD5);
	CMMString strMacAddr = CMMMD5Checksum::GetMD5(buffer, sizeof(s1) + sizeof(s2) + sizeof(volumeSerial));
	assert(Len_Machine_ID >= (int)strMacAddr.size() + 1);
	_tcsncpy_s(szMachineID, Len_Machine_ID, strMacAddr.c_str(), _TRUNCATE);

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
