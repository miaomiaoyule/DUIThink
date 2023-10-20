#ifndef __MM_FILEMONITOR_H__
#define __MM_FILEMONITOR_H__

#pragma once

#include "Define.h"

//////////////////////////////////////////////////////////////////////////
class CMMFileMonitor : public CMMAsyncObject
{
private:
	CMMFileMonitor();
	virtual ~CMMFileMonitor();

protected:
	std::recursive_mutex				m_MonitorLock;
	UINT								m_uMsgRegister = 0;
	VecMonitorTask						m_vecMonitorTask;
	VecMMMonitorFile					m_vecMonitorFile;

public:
	static CMMFileMonitor * GetInstance();
	bool Init();
	bool UnInit();
	
	bool RegisterShellChange(LPCTSTR lpszPath, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink);
	bool RegisterShellChange(PCIDLIST_ABSOLUTE pPCIDL, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink);
	void UnRegisterShellChange(LPCTSTR lpszPath, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink);
	void UnRegisterShellChange(PCIDLIST_ABSOLUTE pPCIDL, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink);
	void UnRegisterShellChange(IMMFileMonitorAttemperSink *pIAttemperSink);

	//msg
protected:
	LRESULT OnWMMonitorTask(WPARAM wParam, LPARAM lParam);
	LRESULT OnWMShellChange(UINT uMsg, WPARAM wParam, LPARAM lParam);

	//helep
protected:
	bool IsItemNotificationEvent(long lEvent);
	void MonitorRegister(std::shared_ptr<tagMonitorTask> pRecvData);
	void MonitorUnRegister(std::shared_ptr<tagMonitorTask> pRecvData);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
};

//////////////////////////////////////////////////////////////////////////

#endif
