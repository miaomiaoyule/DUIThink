#include "stdafx.h"
#include "MMServiceItem.h"
//////////////////////////////////////////////////////////////////////////
struct M_ServiceItem_Timer : public tagMMServiceMsg
{
	uint32_t							uTimerID = 0;
};

struct M_ServiceItem_Task : public tagMMServiceMsg
{
	std::function<void()>				pFunc = NULL;
};

struct CMMServiceItem::tagData
{
	CMMThreadPool *						pOwnerPool = NULL;
	std::recursive_mutex				TimerLock;
	std::recursive_mutex				ExcuteLock;
	std::recursive_mutex				UnInitedLock;
	bool								bExcuteLocked = false;
	bool								bUnInited = false;
	MapTimerFunc						mapTimeFunc;
};
//////////////////////////////////////////////////////////////////////////
CMMServiceItem::CMMServiceItem(CMMThreadPool *pOwnerPool)
{
	m_pData = new tagData();
	m_pData->pOwnerPool = pOwnerPool;

	return;
}

CMMServiceItem::~CMMServiceItem()
{
	MMSafeDelete(m_pData);

	return;
}

bool CMMServiceItem::Init()
{
	if (NULL == m_pData || NULL == m_pData->pOwnerPool) return false;

	KillTimerAll();
	while (false == Lock())
	{
		m_pData->pOwnerPool->Clear(this);
	}
	UnLock();

	{
		std::lock_guard<std::recursive_mutex> Lock(m_pData->UnInitedLock);

		m_pData->bUnInited = false;
	}

	return true;
}

bool CMMServiceItem::UnInit()
{
	if (NULL == m_pData || NULL == m_pData->pOwnerPool) return false;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_pData->UnInitedLock);

		m_pData->bUnInited = true;
	}

	KillTimerAll();
	m_pData->pOwnerPool->Clear(this);
	while (false == Lock())
	{
		KillTimerAll();
		m_pData->pOwnerPool->Clear(this);
	}
	UnLock();

	return true;
}

bool CMMServiceItem::IsUnInited()
{
	if (NULL == m_pData || NULL == m_pData->pOwnerPool) return true;

	std::lock_guard<std::recursive_mutex> Lock(m_pData->UnInitedLock);

	return m_pData->bUnInited;
}

bool CMMServiceItem::Send(CMMServiceItem *pDest, PtrMMServiceMsg pMessage)
{
	if (NULL == pDest || NULL == m_pData->pOwnerPool) return false;
	if (IsUnInited() || pDest->IsUnInited()) return false;

	pMessage->pSrc = this;
	pMessage->pDest = pDest;

	return m_pData->pOwnerPool->Push(pMessage);
}

bool CMMServiceItem::PushTask(std::function<void()> pFunc)
{
	auto pMessage = std::make_shared<M_ServiceItem_Task>();
	pMessage->pFunc = pFunc;

	return Send(this, pMessage);
}

bool CMMServiceItem::Lock()
{
	if (NULL == m_pData) return false;

	std::lock_guard<std::recursive_mutex> Lock(m_pData->ExcuteLock);

	if (m_pData->bExcuteLocked) return false;

	m_pData->bExcuteLocked = true;

	return true;
}

bool CMMServiceItem::UnLock()
{
	if (NULL == m_pData) return false;

	std::lock_guard<std::recursive_mutex> Lock(m_pData->ExcuteLock);

	if (false == m_pData->bExcuteLocked) return false;

	m_pData->bExcuteLocked = false;

	return true;
}

bool CMMServiceItem::Execute(PtrMMServiceMsg pMessage)
{
	if (NULL == m_pData) return false;
	if (IsUnInited()) return true;

	try
	{
		if (typeid(*pMessage) == typeid(M_ServiceItem_Timer))
		{
			std::lock_guard<std::recursive_mutex> DataLock(m_pData->TimerLock);

			auto &pTimeMsg = std::dynamic_pointer_cast<M_ServiceItem_Timer>(pMessage);
			auto pFuncIt = m_pData->mapTimeFunc.find(pTimeMsg->uTimerID);
			if (pFuncIt != m_pData->mapTimeFunc.end())
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
	if (NULL == m_pData || IsUnInited()) return;

	//add
	{
		std::lock_guard<std::recursive_mutex> lock(m_pData->TimerLock);

		m_pData->mapTimeFunc[uTimerID] = pFuncTimer;
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
	if (NULL == m_pData) return;

	//remove
	{
		std::lock_guard<std::recursive_mutex> lock(m_pData->TimerLock);

		auto FintIt = m_pData->mapTimeFunc.find(uTimerID);
		if (FintIt != m_pData->mapTimeFunc.end())
		{
			m_pData->mapTimeFunc.erase(FintIt);
		}
	}

	CMMTimerPower::GetInstance()->KillTimer(uTimerID, this);

	return;
}

void CMMServiceItem::KillTimerAll()
{
	if (NULL == m_pData) return;

	//remove
	{
		std::lock_guard<std::recursive_mutex> lock(m_pData->TimerLock);

		m_pData->mapTimeFunc.clear();
	}

	CMMTimerPower::GetInstance()->KillTimerAll(this);

	return;
}


