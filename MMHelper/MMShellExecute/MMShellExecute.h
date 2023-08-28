#ifndef __MM_SHELLEXECUTE_H__
#define __MM_SHELLEXECUTE_H__

#pragma once

#include "Define.h"

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMShellExecute : public CMMServiceItem
{
private:
	CMMShellExecute();
	~CMMShellExecute();

	//variant
protected:
	CMMThreadPool						m_ThreadPool;

	//method
public:
	static CMMShellExecute * GetInstance();
	bool Init();
	bool UnInit();

	bool PerformOpen(HWND hWnd, LPCTSTR lpszOperation, LPCTSTR lpszFile, LPCTSTR lpszParameters, LPCTSTR lpszDirectory, INT nShowCmd, bool bSync = false);
	bool PerformOpenEx(SHELLEXECUTEINFO ShellInfo, bool bSync = false);
	void PerformOpenPos(CMMString strFile);
	void PerformOpenMode(CMMString strFile);
	void PerformOpenAttribute(HWND hWnd, CMMString strFile);

	//override
protected:
	void OnMessage(PtrMMServiceMsg pMessage);

	bool OnSubOpen(PtrMsgMMShellExecuteOpen pRecvData);
	bool OnSubOpenEx(PtrMsgMMShellExecuteOpenEx pRecvData);
	void OnSubOpenPos(PtrMsgMMShellExecuteOpenPos pRecvData);
	void OnSubOpenMode(PtrMsgMMShellExecuteOpenMode pRecvData);
};

//////////////////////////////////////////////////////////////////////////

#endif

