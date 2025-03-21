#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_SERVICE_H__
#define __MM_SERVICE_H__

#pragma once

#include "winioctl.h"
#include "Iprtrmib.h"
#include "Iptypes.h"
#include "Iphlpapi.h"

//////////////////////////////////////////////////////////////////////////
#define FILE_DEVICE_SCSI				0x0000001b
#define IOCTL_SCSI_MINIPORT_IDENTIFY	( ( FILE_DEVICE_SCSI << 16 ) + 0x0501 )
	    
#define IOCTL_SCSI_MINIPORT				0x0004D008  //  see NTDDSCSI.H for definition
	    
#define IDENTIFY_BUFFER_SIZE			512
#define SENDIDLENGTH					( sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE )
	    
#define IDE_ATAPI_IDENTIFY				0xA1  //  Returns ID sector for ATAPI.
#define IDE_ATA_IDENTIFY				0xEC  //  Returns ID sector for ATA.
#define DFP_RECEIVE_DRIVE_DATA			0x0007c088

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
//
//typedef struct _DRIVERSTATUS
//{
//	BYTE  bDriverError;  //  Error code from driver, or 0 if no error.
//	BYTE  bIDEStatus;    //  Contents of IDE Error register.
//	//  Only valid when bDriverError is SMART_IDE_ERROR.
//	BYTE  bReserved[2];  //  Reserved for future expansion.
//	DWORD  dwReserved[2];  //  Reserved for future expansion.
//} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;
//
//typedef struct _SENDCMDOUTPARAMS
//{
//	DWORD         cBufferSize;   //  Size of bBuffer in bytes
//	DRIVERSTATUS  DriverStatus;  //  Driver status structure.
//	BYTE          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
//} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;
//
typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;
//
//typedef struct _IDEREGS
//{
//	BYTE bFeaturesReg;       // Used for specifying SMART "commands".
//	BYTE bSectorCountReg;    // IDE sector count register
//	BYTE bSectorNumberReg;   // IDE sector number register
//	BYTE bCylLowReg;         // IDE low order cylinder value
//	BYTE bCylHighReg;        // IDE high order cylinder value
//	BYTE bDriveHeadReg;      // IDE drive/head register
//	BYTE bCommandReg;        // Actual IDE command.
//	BYTE bReserved;          // reserved for future use.  Must be zero.
//} IDEREGS, *PIDEREGS, *LPIDEREGS;
//
//typedef struct _SENDCMDINPARAMS
//{
//	DWORD     cBufferSize;   //  Buffer size in bytes
//	IDEREGS   irDriveRegs;   //  Structure with drive register values.
//	BYTE bDriveNumber;       //  Physical drive number to send 
//	//  command to (0,1,2,3).
//	BYTE bReserved[3];       //  Reserved for future expansion.
//	DWORD     dwReserved[4]; //  For future use.
//	BYTE      bBuffer[1];    //  Input buffer.
//} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;
//
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
private:
	CMMService();

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

	//Ini file
	static UINT ReadFileValue(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
	static VOID ReadFileString(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount);
	static VOID WriteFileValue(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue);
	static VOID WriteFileString(LPCTSTR lpszFileName, LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString);

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
#endif