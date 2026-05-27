#include "stdafx.h"
#include "MMThreadPool.h"

//////////////////////////////////////////////////////////////////////////
CMMThreadPool::CMMThreadPool()
{
	return;
}

CMMThreadPool::~CMMThreadPool()
{
	return;
}

void CMMThreadPool::Run(int nThreadCount)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	while (m_bStoping)
	{
		Sleep(100);
	}

	if (m_bInited) return;

	m_bInited = true;
	m_hEvent = CreateEvent(0, FALSE, FALSE, 0);
	m_nThreadCount = nThreadCount;
	for (int i = 0; i < nThreadCount; i++)
	{
		m_vecThreadWorker.push_back(std::thread(std::bind(&CMMThreadPool::Work, this)));
	}

	SetEvent(m_hEvent);

	return;
}

void CMMThreadPool::Stop()
{
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		m_bStoping = true;
	}
	while (true)
	{
		std::lock_guard<std::recursive_mutex> Lock(m_RunningLock);
		if (m_nThreadRunning <= 0) break;

		SetEvent(m_hEvent);
	}
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		for (auto &ThreadWork : m_vecThreadWorker)
		{
			if (ThreadWork.joinable())
			{
				ThreadWork.join();
			}
		}

		m_vecThreadWorker.clear();
		m_mapServiceMsg.clear();
		MMSafeCloseHandle(m_hEvent);
		m_bInited = false;
		m_bStoping = false;
	}

	return;
}

bool CMMThreadPool::Push(PtrMMServiceMsg pMsg)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pMsg || NULL == pMsg->pDest || pMsg->pDest->IsUnInited()) return false;
	if (m_bStoping) return false;

	auto &queMsg = m_mapServiceMsg[pMsg->pDest];
	queMsg.push_back(pMsg);

	SetEvent(m_hEvent);

	return true;
}

void CMMThreadPool::Clear(CMMServiceItem *pDest)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pDest) return;

	auto FindIt = m_mapServiceMsg.find(pDest);
	if (FindIt != m_mapServiceMsg.end())
	{
		m_mapServiceMsg.erase(FindIt);
	}

	return;
}

bool CMMThreadPool::PushTask(std::function<void()> pFunc)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pFunc || m_bStoping) return false;

	m_deqFuncTask.push_back(pFunc);

	SetEvent(m_hEvent);

	return true;
}

void CMMThreadPool::Work()
{
	// Init Com
	::CoInitialize(nullptr);
	::OleInitialize(nullptr);

	{
		std::lock_guard<std::recursive_mutex> Lock(m_RunningLock);
		m_nThreadRunning++;
	}
	try
	{
		while (true)
		{
			if (true == m_bStoping)
			{
				std::lock_guard<std::recursive_mutex> Lock(m_RunningLock);
				m_nThreadRunning--;

				break;
			}

			auto queMsg = Translate();
			if (false == Dispatch(queMsg))
			{
				auto pFunc = PopTask();
				if (pFunc)
				{
					pFunc();
				}
				else
				{
					DWORD dwRes = WaitForSingleObject(m_hEvent, INFINITE);
				}
			}
		}
	}
	catch (const std::exception &err)
	{
		std::lock_guard<std::recursive_mutex> Lock(m_RunningLock);
		m_nThreadRunning--;

		assert(false);
	}
	catch (...)
	{
		std::lock_guard<std::recursive_mutex> Lock(m_RunningLock);
		m_nThreadRunning--;

		assert(false);
	}

	::CoUninitialize();
	::OleUninitialize();

	return;
}

QueMessage CMMThreadPool::Translate()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	QueMessage queMsg;
	for (auto &queMsgIt : m_mapServiceMsg)
	{
		if (NULL == queMsgIt.first) break;
		if (queMsgIt.second.size() <= 0) continue;
		if (false == queMsgIt.first->Lock()) continue;

		queMsg = std::move(queMsgIt.second);
		m_mapServiceMsg.erase(queMsgIt.first);
		break;
	}

	return queMsg;
}

std::function<void()> CMMThreadPool::PopTask()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	std::function<void()> pFunc = NULL;
	if (m_deqFuncTask.size() > 0)
	{
		pFunc = m_deqFuncTask.front();
		m_deqFuncTask.pop_front();
	}

	return pFunc;
}

bool CMMThreadPool::Dispatch(QueMessage &queMsg)
{
	if (queMsg.empty()) return false;

	CMMServiceItem *pServiceItem = NULL;
	for (auto &Msg : queMsg)
	{
		pServiceItem = Msg->pDest;
		assert(pServiceItem);

		if (true == m_bStoping)
		{
			pServiceItem->UnLock();

			return true;
		}

		pServiceItem->Execute(Msg);
	}

	pServiceItem->UnLock();

	return true;
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
