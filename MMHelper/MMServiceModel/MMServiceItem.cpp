#include "stdafx.h"
#include "MMServiceItem.h"

//////////////////////////////////////////////////////////////////////////
CMMServiceItem::CMMServiceItem(CMMThreadPool *pOwnerPool)
{
	m_pOwnerPool = pOwnerPool;

	return;
}

CMMServiceItem::~CMMServiceItem()
{
	return;
}

bool CMMServiceItem::Init()
{
	if (NULL == m_pOwnerPool) return false;

	KillTimerAll();
	while (false == Lock())
	{
		m_pOwnerPool->Clear(this);
	}
	UnLock();

	{
		std::lock_guard<std::recursive_mutex> Lock(m_UnInitedLock);

		m_bUnInited = false;
	}

	return true;
}

bool CMMServiceItem::UnInit()
{
	if (NULL == m_pOwnerPool) return false;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_UnInitedLock);

		m_bUnInited = true;
	}

	KillTimerAll();
	m_pOwnerPool->Clear(this);
	while (false == Lock())
	{
		KillTimerAll();
		m_pOwnerPool->Clear(this);
	}
	UnLock();

	return true;
}

bool CMMServiceItem::IsUnInited()
{
	if (NULL == m_pOwnerPool) return true;

	std::lock_guard<std::recursive_mutex> Lock(m_UnInitedLock);

	return m_bUnInited;
}

bool CMMServiceItem::Send(CMMServiceItem *pDest, PtrMMServiceMsg pMessage)
{
	if (NULL == pDest || NULL == m_pOwnerPool) return false;
	if (IsUnInited() || pDest->IsUnInited()) return false;

	pMessage->pSrc = this;
	pMessage->pDest = pDest;

	return m_pOwnerPool->Push(pMessage);
}

bool CMMServiceItem::PushTask(std::function<void()> pFunc)
{
	auto pMessage = std::make_shared<M_ServiceItem_Task>();
	pMessage->pFunc = pFunc;

	return Send(this, pMessage);
}

bool CMMServiceItem::Lock()
{
	std::lock_guard<std::recursive_mutex> Lock(m_ExcuteLock);

	if (m_bExcuteLocked) return false;

	m_bExcuteLocked = true;

	return true;
}

bool CMMServiceItem::UnLock()
{
	std::lock_guard<std::recursive_mutex> Lock(m_ExcuteLock);

	if (false == m_bExcuteLocked) return false;

	m_bExcuteLocked = false;

	return true;
}

bool CMMServiceItem::Execute(PtrMMServiceMsg pMessage)
{
	if (IsUnInited()) return true;

	try
	{
		if (typeid(*pMessage) == typeid(M_ServiceItem_Timer))
		{
			std::lock_guard<std::recursive_mutex> DataLock(m_TimerLock);

			auto &pTimeMsg = std::dynamic_pointer_cast<M_ServiceItem_Timer>(pMessage);
			auto pFuncIt = m_mapTimeFunc.find(pTimeMsg->uTimerID);
			if (pFuncIt != m_mapTimeFunc.end())
			{
				FuncTimer pFuncTimer = pFuncIt->second;
				pFuncTimer();
			}
		}
		else if (typeid(*pMessage) == typeid(M_ServiceItem_Task))
		{
			auto &pTaskMsg = std::dynamic_pointer_cast<M_ServiceItem_Task>(pMessage);
			if (pTaskMsg && pTaskMsg->pFunc)
			{
				(pTaskMsg->pFunc)();
			}
		}
		else
		{
			OnMessage(pMessage);
		}
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
	}

	return true;
}

void CMMServiceItem::SetTimer(uint32_t uTimerID, uint32_t uElapse, FuncTimer pFuncTimer)
{
	if (IsUnInited()) return;

	//add
	{
		std::lock_guard<std::recursive_mutex> lock(m_TimerLock);

		m_mapTimeFunc[uTimerID] = pFuncTimer;
	}

	CMMTimerPower::GetInstance()->SetTimer(uTimerID, uElapse, [uTimerID, this]()
	{
		auto pMsg = std::make_shared<M_ServiceItem_Timer>();
		pMsg->uTimerID = uTimerID;
		Send(this, pMsg);

	}, this);

	return;
}

void CMMServiceItem::KillTimer(uint32_t uTimerID)
{
	//remove
	{
		std::lock_guard<std::recursive_mutex> lock(m_TimerLock);

		auto FintIt = m_mapTimeFunc.find(uTimerID);
		if (FintIt != m_mapTimeFunc.end())
		{
			m_mapTimeFunc.erase(FintIt);
		}
	}

	CMMTimerPower::GetInstance()->KillTimer(uTimerID, this);

	return;
}

void CMMServiceItem::KillTimerAll()
{
	//remove
	{
		std::lock_guard<std::recursive_mutex> lock(m_TimerLock);

		m_mapTimeFunc.clear();
	}

	CMMTimerPower::GetInstance()->KillTimerAll(this);

	return;
}


