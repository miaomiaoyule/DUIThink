#include "stdafx.h"
#include "MMRegMonitor.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
//////////////////////////////////////////////////////////////////////////
CMMRegMonitor::CMMRegMonitor(HKEY hKeyMain, CMMString strKeySub)
	: CMMServiceItem(&m_ThreadPool)
	, m_hKeyMain(hKeyMain)
	, m_strKeySub(strKeySub)
{
	auto lRes = RegOpenKeyEx(m_hKeyMain, m_strKeySub, 0, KEY_NOTIFY, &m_hKeyMonitor);
	if (ERROR_SUCCESS != lRes || NULL == m_hKeyMonitor)
	{
		assert(false);
		//ERRORLOG("MonitorReg Error {}-{}-Res:{}", (int)m_hKeyMain, (LPCSTR)CT2CA(m_strKeySub), lRes);
	}

	return;
}

CMMRegMonitor::~CMMRegMonitor()
{

}

bool CMMRegMonitor::Init()
{
	if (false == __super::Init()) return false;

	auto pMessage = std::make_shared<M_MMRegMonitor_Start>();
	Send(this, pMessage);

	m_ThreadPool.Run(1);

	return true;
}

bool CMMRegMonitor::UnInit()
{
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		m_bStop = true;

		if (m_hKeyMonitor)
		{
			::RegCloseKey(m_hKeyMonitor);
			m_hKeyMonitor = NULL;
		}

		m_vecRegNotify.clear();
	}

	__super::UnInit();

	m_ThreadPool.Stop();

	return true;
}

void CMMRegMonitor::RegisterNotify(IMMRegNotify *pIMMRegNotify)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find(m_vecRegNotify.begin(), m_vecRegNotify.end(), pIMMRegNotify);
	if (FindIt != m_vecRegNotify.end()) return;

	m_vecRegNotify.push_back(pIMMRegNotify);

	return;
}

void CMMRegMonitor::UnRegisterNotify(IMMRegNotify *pIMMRegNotify)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find(m_vecRegNotify.begin(), m_vecRegNotify.end(), pIMMRegNotify);
	if (FindIt == m_vecRegNotify.end()) return;

	m_vecRegNotify.erase(FindIt);

	return;
}

void CMMRegMonitor::OnMessage(PtrMMServiceMsg pMessage)
{
	if (typeid(*pMessage) == typeid(M_MMRegMonitor_Start))
	{
		return OnSubStart(std::static_pointer_cast<M_MMRegMonitor_Start>(pMessage));
	}

	return;
}

void CMMRegMonitor::OnSubStart(std::shared_ptr<M_MMRegMonitor_Start> pRecvData)
{
	while (false == IsStop() && m_hKeyMonitor)
	{
		if (RegNotifyChangeKeyValue(m_hKeyMonitor, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, NULL, FALSE) == ERROR_SUCCESS)
		{
			if (IsStop()) return;

			{
				std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

				for (auto pIRegNotify : m_vecRegNotify)
				{
					pIRegNotify->OnRegChange(m_hKeyMain, m_strKeySub);
				}
			}
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
#endif