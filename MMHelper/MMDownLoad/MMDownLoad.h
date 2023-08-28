#ifndef __MM_DOWNLOAD_H__
#define __MM_DOWNLOAD_H__

#pragma once

#include "Define.h"

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMDownLoad 
	: public CMMAsyncObject
	, public CMMServiceItem
{
private:
	CMMDownLoad();
	~CMMDownLoad();

protected:
	std::recursive_mutex				m_DataLock;
	std::recursive_mutex				m_NotifyLock;

	//trans
	CURLM *								m_pHandleMulti = NULL;
	VecDownLoadTask						m_vecDownLoadTask;
	VecDownLoadTrans					m_vecDownLoadTrans;
	CMMThreadPool						m_ThreadPool;

	//notify
	VecDownLoadNotify					m_vecDownLoadNotify;

	//override
public:
	bool Init();
	bool UnInit();

	//override
protected:
	void OnMessage(PtrMMServiceMsg pMessage);

	void OnSubTaskAdd(PtrMsgMMDownLoadTaskAdd pRecvData);
	void OnSubTaskRemove(PtrMsgMMDownLoadTaskRemove pRecvData);
	void OnSubTransVerify(PtrMsgMMDownLoadTransVerify pRecvData);
	void OnSubNotifyProgress(PtrMsgMMDownLoadNotifyProgress pRecvData);

	//method
public:
	static CMMDownLoad * GetInstance();
	bool PerformDownLoadStart(int nDownLoadID, LPCTSTR lpszUrl, LPCTSTR lpszPath, LPCTSTR lpszFileName, IMMDownLoadAttemperSink *pIAttemperSink);
	bool PerformDownLoadStop(int nDownLoadID, IMMDownLoadAttemperSink *pIAttemperSink);
	bool PerformDownLoadStop(IMMDownLoadAttemperSink *pIAttemperSink);

	//notify
protected:
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);

	LRESULT OnWMDownLoadNotify(WPARAM wParam, LPARAM lParam);

	//help
protected:
	void AddTaskTransVerify();
	void AddTaskNotifyProgress();

	//help
protected:
	tagDownLoadTrans FindDownLoadTrans(CURL *pHandleCurl);
	tagDownLoadTrans FindDownLoadTrans(int nDownLoadID, IMMDownLoadAttemperSink *pIAttemperSink);
	void RemoveDownLoadTask(int nDownLoadID);
	void RemoveDownLoadTask(IMMDownLoadAttemperSink *pIAttemperSink);
	void RemoveDownLoadTrans(int nDownLoadID);
	void RemoveDownLoadTrans(CURL *pHandleCurl);
	void RemoveDownLoadTrans(IMMDownLoadAttemperSink *pIAttemperSink);
	void RemoveDownLoadNotify(int nDownLoadID);
	void RemoveDownLoadNotify(IMMDownLoadAttemperSink *pIAttemperSink);
	void PerformDownLoadDone(tagDownLoadTrans DownLoadTrans, CURLcode CurlCodeRes);
	void PerformActiveNextTask();
	void PerformDownLoadNotify(int nDownLoadID, enDownLoadNotifyType NotifyType, IMMDownLoadAttemperSink *pIAttemperSink, int nErrorCode);
	void PerformDownLoadNotify(int nDownLoadID, enDownLoadNotifyType NotifyType, IMMDownLoadAttemperSink *pIAttemperSink, int nSizeTotal, int nSizeDownLoad, int nSizeSpeed);
};

//////////////////////////////////////////////////////////////////////////

#endif
