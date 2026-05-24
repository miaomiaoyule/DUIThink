#ifndef __MM_SERVICEITEM_H__
#define __MM_SERVICEITEM_H__

#pragma once

#include "MMTimerPower.h"
//////////////////////////////////////////////////////////////////////////

typedef std::unordered_map<uint32_t, FuncTimer> MapTimerFunc;

class MMHELPER_API CMMServiceItem
{
	struct tagData;
	friend class CMMThreadPool;

public:
	CMMServiceItem(CMMThreadPool *pOwnerPool);
	virtual ~CMMServiceItem();

protected:
	CMMThreadPool *								m_pOwnerPool = NULL;
	std::recursive_mutex						m_TimerLock;
	std::recursive_mutex						m_ExcuteLock;
	std::recursive_mutex						m_UnInitedLock;
	bool										m_bExcuteLocked = false;
	bool										m_bUnInited = false;
	MapTimerFunc								m_mapTimeFunc;

public:
	virtual bool Init();
	virtual bool UnInit();
	bool IsUnInited();
	bool Send(CMMServiceItem *pDest, PtrMMServiceMsg pMessage);
	bool PushTask(std::function<void()> pFunc);
	virtual void OnMessage(PtrMMServiceMsg pMessage) {}

protected:
	bool Lock();
	bool UnLock();
	bool Execute(PtrMMServiceMsg pMessage);
	void SetTimer(uint32_t uTimerID, uint32_t uElapse, FuncTimer pFuncTimer);
	void KillTimer(uint32_t uTimerID);
	void KillTimerAll();
};

//////////////////////////////////////////////////////////////////////////
#endif