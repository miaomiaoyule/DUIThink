#include "stdafx.h"
#include "MMThreadPool.h"

typedef std::deque<PtrMMServiceMsg> QueMessage;
typedef std::unordered_map<CMMServiceItem*, QueMessage> MapSrvMessage;

struct CMMThreadPool::tagData
{
	bool								bInited = false;
	bool								bStoping = false;
	MapSrvMessage						mapServiceMsg;
	std::recursive_mutex				DataLock;
	std::recursive_mutex				RunningLock;
	int									nThreadCount = 0;
	int									nThreadRunning = 0;
	std::vector<std::thread>			vecThreadWorker;
	HANDLE								hEvent = NULL;

	void Run(int nThreadCount)
	{
		std::lock_guard<std::recursive_mutex> Lock(DataLock);

		while (bStoping)
		{
			Sleep(100);
		}

		if (bInited) return;

		bInited = true;
		hEvent = CreateEvent(0, FALSE, FALSE, 0);
		this->nThreadCount = nThreadCount;
		for (int i = 0; i < nThreadCount; i++)
		{
			vecThreadWorker.push_back(std::thread(std::bind(&CMMThreadPool::tagData::Work, this)));
		}

		SetEvent(hEvent);

		return;
	}

	void Stop()
	{
		{
			std::lock_guard<std::recursive_mutex> Lock(DataLock);

			bStoping = true;
		}
		while (true)
		{
			std::lock_guard<std::recursive_mutex> Lock(RunningLock);
			if (nThreadRunning <= 0) break;

			SetEvent(hEvent);
		}
		{
			std::lock_guard<std::recursive_mutex> Lock(DataLock);
			
			for (auto &ThreadWork : vecThreadWorker)
			{
				if (ThreadWork.joinable())
				{
					ThreadWork.join();
				}
			}

			vecThreadWorker.clear();
			mapServiceMsg.clear();
			MMSafeCloseHandle(hEvent);
			bInited = false;
			bStoping = false;
		}

		return;
	}

	void Work()
	{
		// Init Com
		::CoInitialize(nullptr);
		::OleInitialize(nullptr);

		{
			std::lock_guard<std::recursive_mutex> Lock(RunningLock);
			nThreadRunning++;
		}

		while (true)
		{
			if (true == bStoping)
			{
				std::lock_guard<std::recursive_mutex> Lock(RunningLock);
				nThreadRunning--;

				break;
			}

			auto queMsg = Translate();
			if (false == Dispatch(queMsg))
			{
				DWORD dwRes = WaitForSingleObject(hEvent, INFINITE);
			}
		}

		::CoUninitialize();
		::OleUninitialize();

		return;
	}

	QueMessage Translate()
	{
		std::lock_guard<std::recursive_mutex> Lock(DataLock);

		QueMessage queMsg;
		for (auto &queMsgIt : mapServiceMsg)
		{
			if (NULL == queMsgIt.first) break;
			if (queMsgIt.second.size() <= 0) continue;
			if (false == queMsgIt.first->Lock()) continue;

			queMsg = std::move(queMsgIt.second);
			mapServiceMsg.erase(queMsgIt.first);
			break;
		}

		return queMsg;
	}

	bool Dispatch(QueMessage &queMsg)
	{
		if (queMsg.empty()) return false;

		CMMServiceItem *pServiceItem = NULL;
		for (auto &Msg : queMsg)
		{
			pServiceItem = Msg->pDest;
			assert(pServiceItem);

			if (true == bStoping)
			{
				pServiceItem->UnLock();

				return true;
			}

			pServiceItem->Execute(Msg);
		}
		
		pServiceItem->UnLock();

		return true;
	}

	bool Push(PtrMMServiceMsg pMessage)
	{
		std::lock_guard<std::recursive_mutex> Lock(DataLock);

		if (NULL == pMessage || NULL == pMessage->pDest || pMessage->pDest->IsUnInited()) return false;
		if (bStoping) return false;

		auto &queMsg = mapServiceMsg[pMessage->pDest];
		queMsg.push_back(pMessage);

		SetEvent(hEvent);

		return true;
	}

	void Clear(CMMServiceItem *pDest)
	{
		std::lock_guard<std::recursive_mutex> DataLock(DataLock);

		if (NULL == pDest) return;

		auto FindIt = mapServiceMsg.find(pDest);
		if (FindIt != mapServiceMsg.end())
		{
			mapServiceMsg.erase(FindIt);
		}

		return;
	}
};

//////////////////////////////////////////////////////////////////////////
CMMThreadPool::CMMThreadPool()
{
	m_pData = new tagData();

	return;
}

CMMThreadPool::~CMMThreadPool()
{
	MMSafeDelete(m_pData);

	return;
}

void CMMThreadPool::Run(int nThreadCount)
{
	if (NULL == m_pData) return;

	m_pData->Run(nThreadCount);

	return;
}

void CMMThreadPool::Stop()
{
	if (NULL == m_pData) return;

	m_pData->Stop();

	return;
}

bool CMMThreadPool::Push(PtrMMServiceMsg pMsg)
{
	if (NULL == m_pData) return false;

	return m_pData->Push(pMsg);
}

void CMMThreadPool::Clear(CMMServiceItem *pDest)
{
	if (NULL == m_pData) return;

	return m_pData->Clear(pDest);
}

//////////////////////////////////////////////////////////////////////////
CMMThreadPoolSingle::CMMThreadPoolSingle()
{

}

CMMThreadPoolSingle * CMMThreadPoolSingle::GetInstance()
{
	static CMMThreadPoolSingle Object;
	return &Object;
}
