#include "StdAfx.h"
#include "MMFileMonitor.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define MSG_MONITOR_TASK				(WM_USER + 999)
#define MSG_MONITOR_BEGIN				(WM_USER + 1000)

//////////////////////////////////////////////////////////////////////////
CMMFileMonitor::CMMFileMonitor()
{
}

CMMFileMonitor::~CMMFileMonitor()
{
}

CMMFileMonitor * CMMFileMonitor::GetInstance()
{
	static CMMFileMonitor Obj;
	return &Obj;
}

bool CMMFileMonitor::Init()
{
	if (false == __super::Init()) return false;

	std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

	m_uMsgRegister = MSG_MONITOR_BEGIN;

	return true;
}

bool CMMFileMonitor::UnInit()
{
	if (false == __super::UnInit()) return false;

	std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

	for (auto &ItemIt : m_vecMonitorFile)
	{
		::CoTaskMemFree((LPVOID)ItemIt.pPCIDL);
		SHChangeNotifyDeregister(ItemIt.uHanlde);
	}

	m_vecMonitorFile.clear();

	return true;
}

bool CMMFileMonitor::RegisterShellChange(LPCTSTR lpszPath, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink)
{
	PCIDLIST_ABSOLUTE pPCIDL = CMMFile::GetPCIDLFromPath(lpszPath);
	bool bRes = RegisterShellChange(pPCIDL, bRecursive, pIAttemperSink);
	
	if (pPCIDL)
	{
		::CoTaskMemFree((LPVOID)pPCIDL);
		pPCIDL = NULL;
	}

	return bRes;
}

bool CMMFileMonitor::RegisterShellChange(PCIDLIST_ABSOLUTE pPCIDL, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink)
{
	if (NULL == pPCIDL) return false;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

		auto pTask = std::make_shared<tagMonitorTask>();
		pTask->MonitorTask = MonitorTask_Register;
		pTask->pPCIDL = ILCloneFull(pPCIDL);
		pTask->bRecursive = bRecursive;
		pTask->pIAttemperSink = pIAttemperSink;
		m_vecMonitorTask.push_back(pTask);
	}

	PostMessage(MSG_MONITOR_TASK, 0, 0);

	return true;
}

void CMMFileMonitor::UnRegisterShellChange(LPCTSTR lpszPath, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink)
{
	PCIDLIST_ABSOLUTE pPCIDL = CMMFile::GetPCIDLFromPath(lpszPath);
	UnRegisterShellChange(pPCIDL, bRecursive, pIAttemperSink);

	if (pPCIDL)
	{
		::CoTaskMemFree((LPVOID)pPCIDL);
		pPCIDL = NULL;
	}

	return;
}

void CMMFileMonitor::UnRegisterShellChange(PCIDLIST_ABSOLUTE pPCIDL, bool bRecursive, IMMFileMonitorAttemperSink *pIAttemperSink)
{
	if (NULL == pPCIDL) return;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

		auto pTask = std::make_shared<tagMonitorTask>();
		pTask->MonitorTask = MonitorTask_UnRegister;
		pTask->pPCIDL = ILCloneFull(pPCIDL);
		pTask->bRecursive = bRecursive;
		pTask->pIAttemperSink = pIAttemperSink;
		m_vecMonitorTask.push_back(pTask);
	}

	PostMessage(MSG_MONITOR_TASK, 0, 0);

	return;
}

void CMMFileMonitor::UnRegisterShellChange(IMMFileMonitorAttemperSink *pIAttemperSink)
{
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

		auto pTask = std::make_shared<tagMonitorTask>();
		pTask->MonitorTask = MonitorTask_UnRegister;
		pTask->pIAttemperSink = pIAttemperSink;
		m_vecMonitorTask.push_back(pTask);
	}

	PostMessage(MSG_MONITOR_TASK, 0, 0);

	return;
}

LRESULT CMMFileMonitor::OnWMMonitorTask(WPARAM wParam, LPARAM lParam)
{
	VecMonitorTask vecMonitorTask;

	//copy
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

		if (m_vecMonitorTask.empty()) return 1;

		vecMonitorTask = m_vecMonitorTask;
		m_vecMonitorTask.clear();
	}

	//task
	for (auto pTask : vecMonitorTask)
	{
		if (NULL == pTask) continue;

		switch (pTask->MonitorTask)
		{
			case MonitorTask_Register:
			{
				MonitorRegister(pTask);

				break;
			}
			case MonitorTask_UnRegister:
			{
				MonitorUnRegister(pTask);

				break;
			}
		}
	}

	return 1;
}

LRESULT CMMFileMonitor::OnWMShellChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//sink
	VecMMFileMonitorAttemperSink vecAttemperSink;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

		auto FindIt = find_if(m_vecMonitorFile.begin(), m_vecMonitorFile.end(), [&](tagMMMonitorFile &MonitorFile)
		{
			return MonitorFile.uMsg == uMsg;
		});
		if (FindIt == m_vecMonitorFile.end()) return 0;

		vecAttemperSink = FindIt->vecAttemperSink;
	}

	LONG lEvent;
	PIDLIST_ABSOLUTE *rgpidl;
	HANDLE hNotifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &rgpidl, &lEvent);
	PIDLIST_ABSOLUTE psid1 = nullptr, psid2 = nullptr;
	psid1 = rgpidl[0];
	psid2 = rgpidl[1];

	do 
	{
		if (NULL == psid1 && NULL == psid2) break;

		if (hNotifyLock && IsItemNotificationEvent(lEvent))
		{
			for (auto pIAttemperSink : vecAttemperSink)
			{
				std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

				auto FindIt = find_if(m_vecMonitorFile.begin(), m_vecMonitorFile.end(), [&](tagMMMonitorFile &MonitorFile)
				{
					return MonitorFile.uMsg == uMsg;
				});
				if (FindIt == m_vecMonitorFile.end()) break;
				if (find((*FindIt).vecAttemperSink.begin(), (*FindIt).vecAttemperSink.end(), pIAttemperSink) == (*FindIt).vecAttemperSink.end()) continue;

				pIAttemperSink->OnNotify(lEvent, psid1, psid2);
			}
		}

	} while (false);

	SHChangeNotification_Unlock(hNotifyLock);

	return 1;
}

bool CMMFileMonitor::IsItemNotificationEvent(long lEvent)
{
	return !(lEvent & (SHCNE_EXTENDED_EVENT | SHCNE_FREESPACE | SHCNE_DRIVEADDGUI | SHCNE_SERVERDISCONNECT));
}

void CMMFileMonitor::MonitorRegister(std::shared_ptr<tagMonitorTask> pRecvData)
{
	if (NULL == pRecvData || NULL == pRecvData->pPCIDL) return;

	//has
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

		for (auto &ItemIt : m_vecMonitorFile)
		{
			if ((ItemIt.pPCIDL == pRecvData->pPCIDL || ::ILIsEqual(ItemIt.pPCIDL, pRecvData->pPCIDL))
				&& ItemIt.bRecursive == pRecvData->bRecursive)
			{
				ItemIt.vecAttemperSink.push_back(pRecvData->pIAttemperSink);
				return;
			}
		}
	}

	SHChangeNotifyEntry enter = {};
	enter.fRecursive = pRecvData->bRecursive;
	enter.pidl = pRecvData->pPCIDL;

	int nSources = SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery;
	pRecvData->bRecursive ? nSources |= SHCNRF_RecursiveInterrupt : nSources;
	ULONG lRet = SHChangeNotifyRegister(m_hWnd, nSources, SHCNE_ALLEVENTS, ++m_uMsgRegister, 1, &enter);
	if (lRet <= 0)
	{
		assert(false);
		::CoTaskMemFree((LPVOID)pRecvData->pPCIDL);
		pRecvData->pPCIDL = NULL;
		return;
	}

	//add
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

		m_vecMonitorFile.push_back(tagMMMonitorFile());
		m_vecMonitorFile.back().pPCIDL = pRecvData->pPCIDL;
		m_vecMonitorFile.back().uHanlde = lRet;
		m_vecMonitorFile.back().uMsg = m_uMsgRegister;
		m_vecMonitorFile.back().bRecursive = pRecvData->bRecursive;
		m_vecMonitorFile.back().vecAttemperSink.push_back(pRecvData->pIAttemperSink);
	}

	return;
}

void CMMFileMonitor::MonitorUnRegister(std::shared_ptr<tagMonitorTask> pRecvData)
{
	if (NULL == pRecvData) return;

	std::lock_guard<std::recursive_mutex> Lock(m_MonitorLock);

	//pcid
	if (pRecvData->pPCIDL)
	{
		auto FindIt = std::find_if(m_vecMonitorFile.begin(), m_vecMonitorFile.end(), [&](tagMMMonitorFile &MonitorFile)
		{
			return (MonitorFile.pPCIDL == pRecvData->pPCIDL || ::ILIsEqual(MonitorFile.pPCIDL, pRecvData->pPCIDL))
				&& MonitorFile.bRecursive == pRecvData->bRecursive;
		});
		if (FindIt == m_vecMonitorFile.end()) return;

		//attempersink
		auto FindSink = std::find((*FindIt).vecAttemperSink.begin(), (*FindIt).vecAttemperSink.end(), pRecvData->pIAttemperSink);
		if (FindSink == (*FindIt).vecAttemperSink.end()) return;

		(*FindIt).vecAttemperSink.erase(FindSink);

		//empty
		if ((*FindIt).vecAttemperSink.empty())
		{
			SHChangeNotifyDeregister((*FindIt).uHanlde);
			::CoTaskMemFree((LPVOID)(*FindIt).pPCIDL);
			m_vecMonitorFile.erase(FindIt);
		}

		return;
	}

	//all
	for (int m = m_vecMonitorFile.size() - 1; m >= 0; m--)
	{
		auto &MonitorFile = m_vecMonitorFile[m];
		for (int n = MonitorFile.vecAttemperSink.size() - 1; n >= 0; n--)
		{
			if (MonitorFile.vecAttemperSink[n] == pRecvData->pIAttemperSink)
			{
				MonitorFile.vecAttemperSink.erase(MonitorFile.vecAttemperSink.begin() + n);
			}
		}

		//empty
		if (MonitorFile.vecAttemperSink.empty())
		{
			SHChangeNotifyDeregister(MonitorFile.uHanlde);
			::CoTaskMemFree((LPVOID)MonitorFile.pPCIDL);
			m_vecMonitorFile.erase(m_vecMonitorFile.begin() + m);

			continue;
		}
	}

	return;
}

LRESULT CMMFileMonitor::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (MSG_MONITOR_TASK == uMsg)
	{
		bHandled = true;
		return OnWMMonitorTask(wParam, lParam);
	}
	if (uMsg >= MSG_MONITOR_BEGIN)
	{
		bHandled = true;
		return OnWMShellChange(uMsg, wParam, lParam);
	}

	return 0;
}

#endif