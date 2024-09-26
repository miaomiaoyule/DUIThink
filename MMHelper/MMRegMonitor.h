#ifndef __MM_REGMONITOR_H__
#define __MM_REGMONITOR_H__

#pragma once

//inner
//////////////////////////////////////////////////////////////////////////
struct M_MMRegMonitor_Start : public tagMMServiceMsg
{
};

//////////////////////////////////////////////////////////////////////////
interface IMMRegNotify
{
	virtual void OnRegChange(HKEY hKeyMain, CMMString strKeySub) = NULL;
};

typedef std::vector<IMMRegNotify*> VecMMRegNotify;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMRegMonitor
	: public CMMServiceItem
{
public:
	CMMRegMonitor(HKEY hKeyMain, CMMString strKeySub);
	~CMMRegMonitor();

	//variant
protected:
	std::recursive_mutex				m_DataLock;
	bool								m_bStop = false;
	HKEY								m_hKeyMonitor = NULL;
	HKEY								m_hKeyMain = NULL;
	CMMString							m_strKeySub;
	CMMThreadPool						m_ThreadPool;
	VecMMRegNotify						m_vecRegNotify;

	//method
public:
	bool Init() override;
	bool UnInit() override;
	bool IsStop() { std::lock_guard<std::recursive_mutex> Lock(m_DataLock); return m_bStop; }
	void RegisterNotify(IMMRegNotify *pIMMRegNotify);
	void UnRegisterNotify(IMMRegNotify *pIMMRegNotify);

	//override
protected:
	void OnMessage(PtrMMServiceMsg pMessage) override;
	void OnSubStart(std::shared_ptr<M_MMRegMonitor_Start> pRecvData);
};

//////////////////////////////////////////////////////////////////////////

#endif