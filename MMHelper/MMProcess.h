#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_PROCESS_H__
#define __MM_PROCESS_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMProcess
{
public:
	//************************************
	// Description:	CreateLink On strPathTo For strFileFull£¬you need init com interface
	// Parameter: 	CMMString strFileFull Àý£ºc:\aa.exe
	// Parameter: 	CMMString strPathTo Àý£ºd:\
	// Parameter: 	CMMString lpszStr Àý£ºlnkName
	//************************************
	static bool CreateFileLink(CMMString strFileFull, CMMString strPathTo, CMMString strNameLink);

	//************************************
	// Description:	Find ParentProcess of the Process
	// Parameter: 	HANDLE hProcess
	//************************************
	static HANDLE FindProcessParent(HANDLE hProcess);

	//************************************
	// Description:	Find process of the processID
	// Parameter: 	DWORD dwProcessID
	//************************************
	static HANDLE FindProcessByID(DWORD dwProcessID);

	//************************************
	// Description:	Find process of the process name
	// Parameter: 	CMMString strName
	//************************************
	static HANDLE FindProcessByName(CMMString strName);

	//************************************
	// Description:	Find process of the process name
	// Parameter: 	CMMString strName
	//************************************
	static bool IsProcessExist(CMMString strName);

	//************************************
	// Description:	is 64wow process
	// Parameter: 	HANDLE hProcess
	//************************************
	static bool IsWow64Process(HANDLE hProcess);

	static CMMString GetProcessName(DWORD dwProcessID);
	static bool GetProcessCmdline(HANDLE hProcess, CMMString &strCmdLine);
	static void LaunchAsExplorerFromSystem(CString strFile, CString strCmdLine);
	static void LaunchAsExplorerFromAdmin(CString strFile, CString strCmdLine);
};

#endif
#endif