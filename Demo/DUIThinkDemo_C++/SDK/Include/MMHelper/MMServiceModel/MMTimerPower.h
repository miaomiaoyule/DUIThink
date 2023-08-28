#ifndef __MM_TIMERPOWER_H__
#define __MM_TIMERPOWER_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
typedef std::function<void()> FuncTimer;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMTimerPower
{
	struct tagData;

private:
	CMMTimerPower();
	~CMMTimerPower();

private:
	tagData *							m_pData = NULL;

public:
	static CMMTimerPower * GetInstance();
	void Init();
	void UnInit();

	void SetTimer(uint32_t uTimerID, uint32_t uElapse, FuncTimer pCallBack, void *pOwner);
	void KillTimer(uint32_t uTimerID, void *pOwner);
	void KillTimerAll(void *pOwner);
};

#endif