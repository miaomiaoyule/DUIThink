#ifndef __MM_DEFINESHELLEXECUTE_H__
#define __MM_DEFINESHELLEXECUTE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
struct M_MMShellExecute_Open : public tagMMServiceMsg
{
	HWND							hWnd = NULL;
	ULONG							uMask = 0;
	CMMString						strOperation;
	CMMString						strFile;
	CMMString						strParameters;
	CMMString						strDirectory;
	INT								nShowCmd = 0;
};

struct M_MMShellExecute_OpenEx : public tagMMServiceMsg
{
	HWND							hWnd = NULL;
	ULONG							uMask = 0;
	CMMString						strVerb;
	CMMString						strFile;
	CMMString						strParameters;
	CMMString						strDirectory;
	INT								nShowCmd = 0;
	HINSTANCE						hInstApp = NULL;
	void *							lpIDList = NULL;
	CMMString						strClass;
	HKEY							hkeyClass = NULL;
	DWORD							dwHotKey = NULL;
};

struct M_MMShellExecute_OpenPos : public tagMMServiceMsg
{
	CMMString						strFile;
};

struct M_MMShellExecute_OpenMode : public tagMMServiceMsg
{
	CMMString						strFile;
};

typedef std::shared_ptr<M_MMShellExecute_Open> PtrMsgMMShellExecuteOpen;
typedef std::shared_ptr<M_MMShellExecute_OpenEx> PtrMsgMMShellExecuteOpenEx;
typedef std::shared_ptr<M_MMShellExecute_OpenPos> PtrMsgMMShellExecuteOpenPos;
typedef std::shared_ptr<M_MMShellExecute_OpenMode> PtrMsgMMShellExecuteOpenMode;

//////////////////////////////////////////////////////////////////////////

#endif