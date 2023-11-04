#include "stdafx.h"
#include "MMTimerPower.h"

//////////////////////////////////////////////////////////////////////////
struct tagTimerNode
{
	void *								pOwner = nullptr;
	uint32_t							uTimeID = 0;
	uint32_t							uTimeTarget = 0;
	uint32_t							uElapse = 0;
	FuncTimer							pCallBack = NULL;

	tagTimerNode(void *pOwner, uint32_t uTimeID, uint32_t uTimeTarget, uint32_t uElapse, FuncTimer pCallBack)
		: pOwner(pOwner)
		, uTimeID(uTimeID)
		, uTimeTarget(uTimeTarget)
		, uElapse(uElapse)
		, pCallBack(pCallBack)
	{

	}
	bool operator < (const tagTimerNode &NodeComp) const
	{
		return uTimeTarget < NodeComp.uTimeTarget;
	}
	bool operator == (const tagTimerNode &NodeComp)
	{
		return (uTimeID == NodeComp.uTimeID && pOwner == NodeComp.pOwner);
	}
};

struct CMMTimerPower::tagData
{
	std::recursive_mutex				DataLock;
	bool								bInited = false;
	uint32_t							uTimeCur = 0;
	std::deque<tagTimerNode>			queTimerTask;
	std::thread							ThreadPower;
	bool								bStop = false;

	void Init()
	{
		{
			std::lock_guard<std::recursive_mutex> Lock(DataLock);

			if (bInited) return;

			bInited = true;
			bStop = false;
		}

		ThreadPower = std::thread([&]()
		{
			// Init Com
			::CoInitialize(nullptr);
			::OleInitialize(nullptr);

			uTimeCur = GetTickCount();

			while (false == bStop)
			{
				PowerTime();

				Sleep(1);
			}

			::CoUninitialize();
			::OleUninitialize();
		});

		return;
	}

	void UnInit()
	{
		bStop = true;
		if (ThreadPower.joinable())
		{
			ThreadPower.join();
		}

		//info
		{
			std::lock_guard<std::recursive_mutex> Lock(DataLock);

			bInited = false;
		}

		return;
	}

	void SetTimer(uint32_t uTimerID, uint32_t uElapse, FuncTimer pCallBack, void *pOwner)
	{
		if (bStop || NULL == pCallBack) return;

		KillTimer(uTimerID, pOwner);

		{
			std::lock_guard<std::recursive_mutex> Lock(DataLock);

			uint32_t uTimeTarget = uTimeCur + uElapse;
			queTimerTask.push_back({ pOwner, uTimerID, uTimeTarget, uElapse, pCallBack });
			std::sort(queTimerTask.begin(), queTimerTask.end(), [&](tagTimerNode &First, tagTimerNode &Next)
			{
				return First < Next;
			});
		}

		return;
	}

	void KillTimer(uint32_t uTimerID, void *pOwner)
	{
		std::lock_guard<std::recursive_mutex> Lock(DataLock);

		auto pFind = find_if(queTimerTask.begin(), queTimerTask.end(),
			[&](tagTimerNode &NodeComp)
		{
			return (NodeComp.pOwner == pOwner && NodeComp.uTimeID == uTimerID);
		});

		if (pFind != queTimerTask.end()) queTimerTask.erase(pFind);

		return;
	}

	void KillTimerAll(void *pOwner)
	{
		std::lock_guard<std::recursive_mutex> Lock(DataLock);

		while (true)
		{
			auto pFind = find_if(queTimerTask.begin(), queTimerTask.end(),
				[&](tagTimerNode &NodeComp)
			{
				return (NodeComp.pOwner == pOwner);
			});

			if (pFind != queTimerTask.end())
			{
				queTimerTask.erase(pFind);
				continue;
			}

			break;
		}

		return;
	}

	void PowerTime()
	{
		std::lock_guard<std::recursive_mutex> Lock(DataLock);

		uTimeCur = GetTickCount();

		for (auto &TimerNode : queTimerTask)
		{
			if (TimerNode.uTimeTarget <= uTimeCur)
			{
				TimerNode.pCallBack();
				TimerNode.uTimeTarget = uTimeCur + TimerNode.uElapse;
				continue;
			}

			break;
		}
		std::sort(queTimerTask.begin(), queTimerTask.end(), [&](tagTimerNode &First, tagTimerNode &Next)
		{
			return First < Next;
		});

		return;
	}
};

//////////////////////////////////////////////////////////////////////////

CMMTimerPower::CMMTimerPower()
{
	m_pData = new tagData();

	return;
}

CMMTimerPower::~CMMTimerPower()
{
	UnInit();

	return;
}

CMMTimerPower * CMMTimerPower::GetInstance()
{
	static CMMTimerPower Obj;
	return &Obj;
}

void CMMTimerPower::Init()
{
	if (NULL == m_pData) return;

	m_pData->Init();

	return;
}

void CMMTimerPower::UnInit()
{
	if (NULL == m_pData) return;

	m_pData->UnInit();

	MMSafeDelete(m_pData);

	return;
}

void CMMTimerPower::SetTimer(uint32_t uTimerID, uint32_t uElapse, FuncTimer pCallBack, void *pOwner)
{
	if (NULL == m_pData) return;

	return m_pData->SetTimer(uTimerID, uElapse, pCallBack, pOwner);
}

void CMMTimerPower::KillTimer(uint32_t uTimerID, void *pOwner)
{
	if (NULL == m_pData) return;

	return m_pData->KillTimer(uTimerID, pOwner);
}

void CMMTimerPower::KillTimerAll(void *pOwner)
{
	if (NULL == m_pData) return;

	return m_pData->KillTimerAll(pOwner);
}
