#ifndef __MM_THREADPOOL_H__
#define __MM_THREADPOOL_H__

#pragma once

class CMMThreadPool;

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
	tagData *							m_pData = NULL;

public:
	void Run(int nThreadCount = 1);
	void Stop();
	bool Push(PtrMMServiceMsg pMsg);
	void Clear(CMMServiceItem *pDest);
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