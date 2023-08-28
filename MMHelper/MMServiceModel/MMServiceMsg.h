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

//////////////////////////////////////////////////////////////////////////
#endif