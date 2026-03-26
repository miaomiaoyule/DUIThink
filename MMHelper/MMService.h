#ifndef __MM_SERVICE_H__
#define __MM_SERVICE_H__

#pragma once

#include "winioctl.h"
#include "Iprtrmib.h"
#include "Iptypes.h"
#include "Iphlpapi.h"

//////////////////////////////////////////////////////////////////////////////////
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMService
{
	//system
public:
	//clipboard
	static bool SetClipboardString(HWND hWnd, LPCTSTR lpszString);
	static bool SetClipboardFiles(HWND hWndOwner, std::vector<CMMString> vecFiles, bool bCopy = true);
	static CMMString GetClipboardString();
	static std::vector<CMMString> GetClipboardFile();
	static DROPEFFECT GetClipboardDropEffect();

	//compile date
	static int GetCompileDate();
	
	//IP to str
	static CMMString IPAddrToString(DWORD dwIPAddr);
	static DWORD StringToIPAddr(CMMString strIPAddr);

	//machine
	static bool GetMachineID(TCHAR szMachineID[Len_Machine_ID]);
	static CMMString GetMachineID();
	static bool GetMACAddress(TCHAR szMACAddress[Len_Network_ID]);
	static BOOL GetMacAddr(BYTE* szSystemInfo, UINT uSystemInfoLen);
	static BOOL GetHdiskSerial(BYTE* szSystemInfo, UINT uSystemInfoLen);
	static void GetCpuID(BYTE* szSystemInfo, UINT uSystemInfoLen) /* CPU ID */;	
	static BOOL DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned );

	//hotkey
	static bool RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey);
	static bool UnRegisterHotKey(HWND hWnd, UINT uKeyID);

	//work dir
	//************************************
	// Description:	get work directory
	// Parameter: 	LPCTSTR szWorkDirectory Àý£ºC:\user\desktop
	// Parameter: 	WORD wBufferCount /* size of szWorkDirectory on word*/
	//************************************
	static bool GetWorkDirectory(OUT TCHAR szWorkDirectory[], IN WORD wBufferCount);
	//************************************
	// Description:	get work directory
	// Return: 		CMMString Àý£ºC:\windows
	//************************************
	static CMMString GetWorkDirectory();
	//µ±Ç°Ä¿Â¼
	static CMMString GetCurrentPath();
	//localappdata path + folder
	//Return: 		CMMString Àý£º"C:\Users\XXX\AppData\Local\strFolderName\"
	static CMMString GetUserDataPath(CMMString strFolderName);
	static CMMString GetLocalTempPath();
	static CMMString GetAppName();
	static CMMString GetAppFile();

	//GUID
	static CMMString ProductGUID();

	//rand
	static int RandValue(int nMin, int nMax);
	static double RandValue(double lfMin, double lfMax);
	static bool IsZero(DWORD dwNum);

	//encrypt
	static std::string EncryptBase64(unsigned char const* bytes_to_encode, unsigned int in_len);

	//help
private:
	//Ó²ÅÌÐòÁÐºÅ
	static BOOL WinNTHDSerialNumAsScsiRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen);
	//Ó²ÅÌÂë
	static BOOL WinNTHDSerialNumAsPhysicalRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen);
	//»úÆ÷±êÊ¶
	static bool GetImportIDEx(TCHAR szMachineID[Len_Machine_ID], TCHAR szCPUID[20] = NULL);
};

//////////////////////////////////////////////////////////////////////////////////

#endif