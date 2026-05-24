#ifndef __MM_THREADPOOL_H__
#define __MM_THREADPOOL_H__

#pragma once

class CMMThreadPool;
typedef std::unordered_map<CMMServiceItem*, QueMessage> MapSrvMessage;

//////////////////////////////////////////////////////////////////////////
//thread pool
class MMHELPER_API CMMThreadPool
{
	struct tagData;
	friend class CMMServiceItem;

public:
	CMMThreadPool();
	~CMMThreadPool();

private:
	bool										m_bInited = false;
	bool										m_bStoping = false;
	MapSrvMessage								m_mapServiceMsg;
	std::recursive_mutex						m_DataLock;
	std::recursive_mutex						m_RunningLock;
	int											m_nThreadCount = 0;
	int											m_nThreadRunning = 0;
	std::vector<std::thread>					m_vecThreadWorker;
	std::deque<std::function<void()>>			m_deqFuncTask;
	HANDLE										m_hEvent = NULL;

public:
	void Run(int nThreadCount = 1);
	void Stop();
	bool Push(PtrMMServiceMsg pMsg);
	void Clear(CMMServiceItem *pDest);
	bool PushTask(std::function<void()> pFunc);

protected:
	void Work();
	QueMessage Translate();
	std::function<void()> PopTask();
	bool Dispatch(QueMessage &queMsg);
};

//////////////////////////////////////////////////////////////////////////
//thread pool single
class MMHELPER_API CMMThreadPoolSingle : public CMMThreadPool
{
private:
	CMMThreadPoolSingle();

public:
	static CMMThreadPoolSingle * GetInstance();
};

//////////////////////////////////////////////////////////////////////////

#endif