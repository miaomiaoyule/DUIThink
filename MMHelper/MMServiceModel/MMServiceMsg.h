#ifndef __MM_SERVICEMSG_H__
#define __MM_SERVICEMSG_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CMMServiceItem;
struct tagMMServiceMsg
{
public:
	virtual ~tagMMServiceMsg() {}

	CMMServiceItem *				pSrc = NULL;
	CMMServiceItem *				pDest = NULL;
};

typedef std::shared_ptr<tagMMServiceMsg> PtrMMServiceMsg;
typedef std::deque<PtrMMServiceMsg> QueMessage;

struct M_ServiceItem_Timer : public tagMMServiceMsg
{
	uint32_t							uTimerID = 0;
};

struct M_ServiceItem_Task : public tagMMServiceMsg
{
	std::function<void()>				pFunc = NULL;
};

//////////////////////////////////////////////////////////////////////////
#endif