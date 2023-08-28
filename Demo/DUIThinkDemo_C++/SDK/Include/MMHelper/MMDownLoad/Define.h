#ifndef __MM_DEFINEDOWNLOAD_H__
#define __MM_DEFINEDOWNLOAD_H__

#pragma once

#define CURL_STATICLIB

#include "../../Include/curl/curl.h"
#include "../../Include/curl/easy.h"

//////////////////////////////////////////////////////////////////////////
//inner
typedef size_t(*PDLRecvDataFunc)(void *ptr, size_t size, size_t nmemb, void *stream);
typedef size_t(*PDLReadHeadFunc)(void *ptr, size_t size, size_t nmemb, void *stream);

interface IMMDownLoadAttemperSink;

//////////////////////////////////////////////////////////////////////////
//inner
enum enDownLoadNotifyType
{
	DownLoadNotify_None,
	DownLoadNotify_Progress,											//download progress
	DownLoadNotify_Error,
	DownLoadNotify_Ok,
};

enum enDownLoadErrorCode
{
	DownLoadError_None = 1000,
	DownLoadError_InitFail,
	DownLoadError_CreateFileFail,
	DownLoadError_StopByOperator,
};

struct tagDownLoadTask
{
	int									nDownLoadID = -1;
	int									nTryCount = 0;
	CMMString							strUrl;
	CMMString							strPath;
	CMMString							strFileName;
	IMMDownLoadAttemperSink *			pIAttemperSink = NULL;
};

struct tagDownLoadTrans
{
	tagDownLoadTask						DownloadTask;
	int									nDownLoadID = 0;
	CURL *								pHandleCurl = NULL;
	FILE *								pFileSave = NULL;
	std::string							strDownLoadUrl;
	PDLRecvDataFunc						pDLRecvDataFunc = NULL;
	PDLReadHeadFunc						pDLReadHeadFunc = NULL;
	IMMDownLoadAttemperSink *			pIAttemperSink = NULL;
};

struct tagDownLoadNotify
{
	int									nDownLoadID = 0;
	enDownLoadNotifyType				NotifyType = DownLoadNotify_None;
	enDownLoadErrorCode					ErrorCode = DownLoadError_None;
	IMMDownLoadAttemperSink *			pIAttemperSink = NULL;
	int									nSizeTotal = 0;
	int									nSizeDownLoad = 0;
	int									nSizeSpeed = 0;
};

typedef std::vector<tagDownLoadTask> VecDownLoadTask;
typedef std::vector<tagDownLoadTrans> VecDownLoadTrans;
typedef std::vector<tagDownLoadNotify> VecDownLoadNotify;

//////////////////////////////////////////////////////////////////////////
//inner
struct M_MMDownLoad_TaskAdd : public tagMMServiceMsg
{
	tagDownLoadTask						DownLoadTask;
};
struct M_MMDownLoad_TaskRemove : public tagMMServiceMsg
{
	tagDownLoadTask						DownLoadTask;
};
struct M_MMDownLoad_TransVerify : public tagMMServiceMsg
{
};
struct M_MMDownLoad_NotifyProgress : public tagMMServiceMsg
{
};

typedef std::shared_ptr<M_MMDownLoad_TaskAdd> PtrMsgMMDownLoadTaskAdd;
typedef std::shared_ptr<M_MMDownLoad_TaskRemove> PtrMsgMMDownLoadTaskRemove;
typedef std::shared_ptr<M_MMDownLoad_TransVerify> PtrMsgMMDownLoadTransVerify;
typedef std::shared_ptr<M_MMDownLoad_NotifyProgress> PtrMsgMMDownLoadNotifyProgress;

//////////////////////////////////////////////////////////////////////////
//outer
interface IMMDownLoadAttemperSink
{
	virtual void OnEventDownLoadProgress(int nDownLoadID, int nSizeTotal, int nSizeDownLoad, int nSizeSpeed) = NULL;
	virtual void OnEventDownLoadError(int nDownLoadID, enDownLoadErrorCode ErrorCode) = NULL;
	virtual void OnEventDownLoadOk(int nDownLoadID) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#endif