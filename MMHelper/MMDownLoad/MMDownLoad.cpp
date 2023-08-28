#include "stdafx.h"
#include "MMDownLoad.h"

//////////////////////////////////////////////////////////////////////////
#define DOWNLOAD_TIMEOUT				(1000)
#define DOWNLOAD_TASK					(1)
#define Max_DownloadRetry				(3)

#define TIME_TRANSVERIFY_ID				(1989)
#define TIME_NOTIFYPROGRESS_ID			(1990)

#define TIME_TRANSVERIFY_ELAPSE			(1)
#define TIME_NOTIFYPROGRESS_ELAPSE		(1000)

//notify
#define WM_DL_NOTIFY					(WM_USER + 1989)

//////////////////////////////////////////////////////////////////////////
//download callback
static size_t recive_data_fun(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, (FILE*)stream);
}

static size_t read_head_fun(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//char head[2048] = { 0 };
	//memcpy(head, ptr, size*nmemb + 1);
	return size * nmemb;
}

//////////////////////////////////////////////////////////////////////////
CMMDownLoad::CMMDownLoad()
	: CMMServiceItem(&m_ThreadPool)
{

}

CMMDownLoad::~CMMDownLoad()
{

}

bool CMMDownLoad::Init()
{
	if (false == CMMAsyncObject::Init()) return false;
	if (false == CMMServiceItem::Init()) return false;

	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	m_pHandleMulti = curl_multi_init();

	//run
	CMMTimerPower::GetInstance()->Init();
	m_ThreadPool.Run(1);

	AddTaskNotifyProgress();

	return true;
}

bool CMMDownLoad::UnInit()
{
	if (false == CMMAsyncObject::UnInit()) return false;

	CMMServiceItem::UnInit();

	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	//stop
	CMMTimerPower::GetInstance()->UnInit();
	m_ThreadPool.Stop();

	if (m_pHandleMulti)
	{
		for (int n = m_vecDownLoadTrans.size() - 1; n >= 0; n--)
		{
			tagDownLoadTrans DownLoadTrans = m_vecDownLoadTrans[n];
			RemoveDownLoadTrans(DownLoadTrans.pHandleCurl);
		}

		curl_multi_cleanup(m_pHandleMulti);
	}

	m_vecDownLoadTask.clear();
	m_vecDownLoadTrans.clear();

	return true;
}

void CMMDownLoad::OnMessage(PtrMMServiceMsg pMessage)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (typeid(*pMessage) == typeid(M_MMDownLoad_TaskAdd))
	{
		return OnSubTaskAdd(std::dynamic_pointer_cast<M_MMDownLoad_TaskAdd>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMDownLoad_TaskRemove))
	{
		return OnSubTaskRemove(std::dynamic_pointer_cast<M_MMDownLoad_TaskRemove>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMDownLoad_TransVerify))
	{
		return OnSubTransVerify(std::dynamic_pointer_cast<M_MMDownLoad_TransVerify>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMDownLoad_NotifyProgress))
	{
		return OnSubNotifyProgress(std::dynamic_pointer_cast<M_MMDownLoad_NotifyProgress>(pMessage));
	}

	return;
}

void CMMDownLoad::OnSubTaskAdd(PtrMsgMMDownLoadTaskAdd pRecvData)
{
	if (NULL == pRecvData) return;

	auto &DownLoadTask = pRecvData->DownLoadTask;

	//tasking
	if (m_vecDownLoadTrans.size() >= DOWNLOAD_TASK)
	{
		m_vecDownLoadTask.push_back(DownLoadTask);

		return;
	}

	//task
	tagDownLoadTrans DownLoadTrans;
	DownLoadTrans.DownloadTask = DownLoadTask;
	DownLoadTrans.nDownLoadID = DownLoadTask.nDownLoadID;
	DownLoadTrans.pIAttemperSink = DownLoadTask.pIAttemperSink;
	DownLoadTrans.strDownLoadUrl = CT2CA(DownLoadTask.strUrl, CP_UTF8);

	//file
	TCHAR szPath[MAX_PATH] = {};
	lstrcpyn(szPath, DownLoadTask.strPath, MMCountString(szPath));
	PathAddBackslash(szPath);
	std::string strFile = CT2CA(CMMString(szPath) + DownLoadTask.strFileName);

	//dir
	CMMString strPath;
	CMMFile::ParseFilePathName(CA2CT(strFile.c_str()), strPath, CMMString());
	int nRes = SHCreateDirectoryEx(NULL, strPath, NULL);
	if (ERROR_SUCCESS != nRes && ERROR_FILE_EXISTS != nRes && ERROR_ALREADY_EXISTS != nRes)
	{
		assert(false);
		PerformDownLoadNotify(DownLoadTask.nDownLoadID, DownLoadNotify_Error, DownLoadTask.pIAttemperSink, DownLoadError_CreateFileFail);
		return;
	}

	DownLoadTrans.pFileSave = fopen(strFile.c_str(), "wb");
	if (NULL == DownLoadTrans.pFileSave)
	{
		assert(false);
		PerformDownLoadNotify(DownLoadTask.nDownLoadID, DownLoadNotify_Error, DownLoadTask.pIAttemperSink, DownLoadError_CreateFileFail);
		return;
	}

	//curl
	DownLoadTrans.pHandleCurl = curl_easy_init();
	DownLoadTrans.pDLRecvDataFunc = recive_data_fun;
	DownLoadTrans.pDLReadHeadFunc = read_head_fun;
	if (NULL == DownLoadTrans.pHandleCurl)
	{
		assert(false);
		PerformDownLoadNotify(DownLoadTask.nDownLoadID, DownLoadNotify_Error, DownLoadTask.pIAttemperSink, DownLoadError_InitFail);
		return;
	}

	//setinfo
	curl_easy_setopt(DownLoadTrans.pHandleCurl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(DownLoadTrans.pHandleCurl, CURLOPT_URL, DownLoadTrans.strDownLoadUrl.c_str());//set down load url 
	curl_easy_setopt(DownLoadTrans.pHandleCurl, CURLOPT_WRITEDATA, DownLoadTrans.pFileSave);//set download file 
	curl_easy_setopt(DownLoadTrans.pHandleCurl, CURLOPT_WRITEFUNCTION, DownLoadTrans.pDLRecvDataFunc);//set call back fun
	curl_easy_setopt(DownLoadTrans.pHandleCurl, CURLOPT_FAILONERROR, 1L);
#ifdef CURL_DEBUG 
	curl_easy_setopt(DownLoadTrans.pHandleCurl, CURLOPT_VERBOSE, 1);
#else 
	curl_easy_setopt(DownLoadTrans.pHandleCurl, CURLOPT_HEADERFUNCTION, DownLoadTrans.pDLReadHeadFunc);//set call back fun 
#endif  

	//download
	curl_multi_add_handle(m_pHandleMulti, DownLoadTrans.pHandleCurl);//add task 
	
	int still_running;
	curl_multi_perform(m_pHandleMulti, &still_running);

	//add
	m_vecDownLoadTrans.push_back(DownLoadTrans);

	//verify
	AddTaskTransVerify();

	return;
}

void CMMDownLoad::OnSubTaskRemove(PtrMsgMMDownLoadTaskRemove pRecvData)
{
	if (NULL == pRecvData) return;

	auto &DownLoadRemove = pRecvData->DownLoadTask;

	//one
	if (-1 != DownLoadRemove.nDownLoadID)
	{
		RemoveDownLoadTask(DownLoadRemove.nDownLoadID);
		RemoveDownLoadTrans(DownLoadRemove.nDownLoadID);
		RemoveDownLoadNotify(DownLoadRemove.nDownLoadID);

		PerformActiveNextTask();

		return;
	}

	//all
	RemoveDownLoadTask(DownLoadRemove.pIAttemperSink);
	RemoveDownLoadTrans(DownLoadRemove.pIAttemperSink);
	RemoveDownLoadNotify(DownLoadRemove.pIAttemperSink);

	PerformActiveNextTask();

	return;
}

void CMMDownLoad::OnSubTransVerify(PtrMsgMMDownLoadTransVerify pRecvData)
{
	if (NULL == pRecvData) return;

	long timeout_ms = DOWNLOAD_TIMEOUT;
	CURLMcode retcode = curl_multi_timeout(m_pHandleMulti, &timeout_ms);//get timeout ms instead 
	if (-1 == timeout_ms)
	{
		timeout_ms = DOWNLOAD_TIMEOUT;
	}

	retcode = curl_multi_wait(m_pHandleMulti, NULL, 0, timeout_ms, NULL);
	if (CURLM_OK != retcode)
	{
		AddTaskTransVerify();

		return;
	}

	int still_running;
	retcode = curl_multi_perform(m_pHandleMulti, &still_running);
	if (CURLM_OK != retcode)
	{
		AddTaskTransVerify();

		return;
	}

	int msgs_left = 0;
	CURLMsg *msg = NULL;
	while ((msg = curl_multi_info_read(m_pHandleMulti, &msgs_left)) != NULL)
	{
		if (msg->msg == CURLMSG_DONE)
		{
			long http_response_code = -1;
			char *http_url = NULL;
			curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &http_response_code);
			curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &http_url);

			//done
			tagDownLoadTrans DownLoadTrans = FindDownLoadTrans(msg->easy_handle);
			PerformDownLoadDone(DownLoadTrans, msg->data.result);

			//new task
			PerformActiveNextTask();
		}
	}

	AddTaskTransVerify();

	return;
}

void CMMDownLoad::OnSubNotifyProgress(PtrMsgMMDownLoadNotifyProgress pRecvData)
{
	if (NULL == pRecvData) return;

	for (auto DownLoadTrans : m_vecDownLoadTrans)
	{
		if (NULL == DownLoadTrans.pHandleCurl) continue;

		double lfSizeTotal = 0, lfSizeDownLoad = 0, lfSizeSpeed = 0;
		curl_easy_getinfo(DownLoadTrans.pHandleCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lfSizeTotal);
		curl_easy_getinfo(DownLoadTrans.pHandleCurl, CURLINFO_SIZE_DOWNLOAD, &lfSizeDownLoad);
		curl_easy_getinfo(DownLoadTrans.pHandleCurl, CURLINFO_SPEED_DOWNLOAD, &lfSizeSpeed);
		PerformDownLoadNotify(DownLoadTrans.nDownLoadID, DownLoadNotify_Progress, DownLoadTrans.pIAttemperSink, lfSizeTotal, lfSizeDownLoad, lfSizeSpeed);
	}

	AddTaskNotifyProgress();

	return;
}

CMMDownLoad * CMMDownLoad::GetInstance()
{
	static CMMDownLoad Obj;
	return &Obj;
}

bool CMMDownLoad::PerformDownLoadStart(int nDownLoadID, LPCTSTR lpszUrl, LPCTSTR lpszPath, LPCTSTR lpszFileName, IMMDownLoadAttemperSink *pIAttemperSink)
{
	if (NULL == m_pHandleMulti
		|| MMInvalidString(lpszUrl)
		|| MMInvalidString(lpszPath)
		|| MMInvalidString(lpszFileName)
		|| NULL == pIAttemperSink) return false;

	auto pMessage = std::make_shared<M_MMDownLoad_TaskAdd>();
	pMessage->DownLoadTask.nDownLoadID = nDownLoadID;
	pMessage->DownLoadTask.strUrl = lpszUrl;
	pMessage->DownLoadTask.strPath = lpszPath;
	pMessage->DownLoadTask.strFileName = lpszFileName;
	pMessage->DownLoadTask.pIAttemperSink = pIAttemperSink;
	pMessage->DownLoadTask.strUrl.Replace(_T("\\"), _T("/"));

	Send(this, pMessage);

	return true;
}

bool CMMDownLoad::PerformDownLoadStop(int nDownLoadID, IMMDownLoadAttemperSink *pIAttemperSink)
{
	if (NULL == m_pHandleMulti
		|| NULL == pIAttemperSink) return false;

	auto pMessage = std::make_shared<M_MMDownLoad_TaskRemove>();
	pMessage->DownLoadTask.nDownLoadID = nDownLoadID;
	pMessage->DownLoadTask.pIAttemperSink = pIAttemperSink;

	Send(this, pMessage);

	return true;
}

bool CMMDownLoad::PerformDownLoadStop(IMMDownLoadAttemperSink *pIAttemperSink)
{
	if (NULL == m_pHandleMulti
		|| NULL == pIAttemperSink) return false;

	auto pMessage = std::make_shared<M_MMDownLoad_TaskRemove>();
	pMessage->DownLoadTask.pIAttemperSink = pIAttemperSink;

	Send(this, pMessage);

	return true;
}

LRESULT CMMDownLoad::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (WM_DL_NOTIFY == uMsg)
	{
		bHandled = true;
		return OnWMDownLoadNotify(wParam, lParam);
	}

	return 0;
}

LRESULT CMMDownLoad::OnWMDownLoadNotify(WPARAM wParam, LPARAM lParam)
{
	std::lock_guard<std::recursive_mutex> Lock(m_NotifyLock);

	for (auto &DownLoadNotify : m_vecDownLoadNotify)
	{
		if (NULL == DownLoadNotify.pIAttemperSink) continue;

		switch (DownLoadNotify.NotifyType)
		{
			case DownLoadNotify_Progress:
			{
				DownLoadNotify.pIAttemperSink->OnEventDownLoadProgress(DownLoadNotify.nDownLoadID, DownLoadNotify.nSizeTotal, DownLoadNotify.nSizeDownLoad, DownLoadNotify.nSizeSpeed);
				break;
			}
			case DownLoadNotify_Error:
			{
				DownLoadNotify.pIAttemperSink->OnEventDownLoadError(DownLoadNotify.nDownLoadID, DownLoadNotify.ErrorCode);
				break;
			}
			case DownLoadNotify_Ok:
			{
				DownLoadNotify.pIAttemperSink->OnEventDownLoadOk(DownLoadNotify.nDownLoadID);
				break;
			}
		}
	}

	m_vecDownLoadNotify.clear();

	return 0;
}

void CMMDownLoad::AddTaskTransVerify()
{
	SetTimer(TIME_TRANSVERIFY_ID, TIME_TRANSVERIFY_ELAPSE, [&]()
	{
		auto pMessage = std::make_shared<M_MMDownLoad_TransVerify>();
		Send(this, pMessage);
	});

	return;
}

void CMMDownLoad::AddTaskNotifyProgress()
{
	SetTimer(TIME_NOTIFYPROGRESS_ID, TIME_NOTIFYPROGRESS_ELAPSE, [&]()
	{
		auto pMessage = std::make_shared<M_MMDownLoad_NotifyProgress>();
		Send(this, pMessage);
	});

	return;
}

tagDownLoadTrans CMMDownLoad::FindDownLoadTrans(CURL *pHandleCurl)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find_if(m_vecDownLoadTrans.begin(), m_vecDownLoadTrans.end(), [&](tagDownLoadTrans &DownLoadTrans)
	{
		return DownLoadTrans.pHandleCurl == pHandleCurl;
	});
	if (FindIt == m_vecDownLoadTrans.end()) return {};

	return *FindIt;
}

tagDownLoadTrans CMMDownLoad::FindDownLoadTrans(int nDownLoadID, IMMDownLoadAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find_if(m_vecDownLoadTrans.begin(), m_vecDownLoadTrans.end(), [&](tagDownLoadTrans &DownLoadTrans)
	{
		return DownLoadTrans.nDownLoadID == nDownLoadID && DownLoadTrans.pIAttemperSink == pIAttemperSink;
	});
	if (FindIt == m_vecDownLoadTrans.end()) return {};

	return *FindIt;
}

void CMMDownLoad::RemoveDownLoadTask(int nDownLoadID)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find_if(m_vecDownLoadTask.begin(), m_vecDownLoadTask.end(), [&](tagDownLoadTask &DownLoadTask)
	{
		return nDownLoadID == DownLoadTask.nDownLoadID;
	});
	if (FindIt == m_vecDownLoadTask.end()) return;

	m_vecDownLoadTask.erase(FindIt);

	return;
}

void CMMDownLoad::RemoveDownLoadTask(IMMDownLoadAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	for (int n = m_vecDownLoadTask.size() - 1; n >= 0; n--)
	{
		auto DownLoadTask = m_vecDownLoadTask[n];
		if (DownLoadTask.pIAttemperSink != pIAttemperSink) continue;

		m_vecDownLoadTask.erase(m_vecDownLoadTask.begin() + n);
	}

	return;
}

void CMMDownLoad::RemoveDownLoadTrans(int nDownLoadID)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find_if(m_vecDownLoadTrans.begin(), m_vecDownLoadTrans.end(), [&](tagDownLoadTrans &DownLoadTrans)
	{
		return DownLoadTrans.nDownLoadID == nDownLoadID;
	});
	if (FindIt == m_vecDownLoadTrans.end()) return;

	RemoveDownLoadTrans((*FindIt).pHandleCurl);

	return;
}

void CMMDownLoad::RemoveDownLoadTrans(CURL *pHandleCurl)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find_if(m_vecDownLoadTrans.begin(), m_vecDownLoadTrans.end(), [&](tagDownLoadTrans &DownLoadTrans)
	{
		return DownLoadTrans.pHandleCurl == pHandleCurl;
	});
	if (FindIt == m_vecDownLoadTrans.end()) return;

	//close
	if ((*FindIt).pHandleCurl)
	{
		curl_multi_remove_handle(m_pHandleMulti, (*FindIt).pHandleCurl);
		curl_easy_cleanup((*FindIt).pHandleCurl);
	}
	if ((*FindIt).pFileSave)
	{
		fclose((*FindIt).pFileSave);
		(*FindIt).pFileSave = NULL;
	}

	m_vecDownLoadTrans.erase(FindIt);

	return;
}

void CMMDownLoad::RemoveDownLoadTrans(IMMDownLoadAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	for (int n = m_vecDownLoadTrans.size() - 1; n >= 0; n--)
	{
		auto DownLoadTrans = m_vecDownLoadTrans[n];
		if (DownLoadTrans.pIAttemperSink != pIAttemperSink) continue;

		RemoveDownLoadTrans(DownLoadTrans.pHandleCurl);
	}

	return;
}

void CMMDownLoad::RemoveDownLoadNotify(int nDownLoadID)
{
	std::lock_guard<std::recursive_mutex> Lock(m_NotifyLock);

	auto FindIt = find_if(m_vecDownLoadNotify.begin(), m_vecDownLoadNotify.end(), [&](tagDownLoadNotify &DownLoadNotify)
	{
		return nDownLoadID == DownLoadNotify.nDownLoadID;
	});
	if (FindIt == m_vecDownLoadNotify.end()) return;

	m_vecDownLoadNotify.erase(FindIt);

	return;
}

void CMMDownLoad::RemoveDownLoadNotify(IMMDownLoadAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_NotifyLock);

	for (int n = m_vecDownLoadNotify.size() - 1; n >= 0; n--)
	{
		auto DownLoadNotify = m_vecDownLoadNotify[n];

		if (pIAttemperSink == DownLoadNotify.pIAttemperSink)
		{
			m_vecDownLoadNotify.erase(m_vecDownLoadNotify.begin() + n);
		}
	}

	return;
}

void CMMDownLoad::PerformDownLoadDone(tagDownLoadTrans DownLoadTrans, CURLcode CurlCodeRes)
{
	//ok
	if (CURLE_OK == CurlCodeRes)
	{
		//remove
		RemoveDownLoadTrans(DownLoadTrans.nDownLoadID);

		PerformDownLoadNotify(DownLoadTrans.nDownLoadID, DownLoadNotify_Ok, DownLoadTrans.pIAttemperSink, CurlCodeRes);

		return;
	}

	//error
	bool bRetry = false;
	do
	{
		if (CURLE_HTTP_RETURNED_ERROR == CurlCodeRes)
		{
			long response_code;
			curl_easy_getinfo(DownLoadTrans.pHandleCurl, CURLINFO_RESPONSE_CODE, &response_code);
			switch (response_code)
			{
				case 301: /*重定向*/
				case 302: /*重定向*/
				case 408: /* Request Timeout */
				case 429: /* Too Many Requests (RFC6585) */
				case 500: /* Internal Server Error */
				case 502: /* Bad Gateway */
				case 503: /* Service Unavailable */
				case 504: /* Gateway Timeout */
				{
					if (DownLoadTrans.DownloadTask.nTryCount < Max_DownloadRetry)
					{
						bRetry = true;
					}

					break;
				}
				default:
					break;
			}

			break;
		}

		if ((CURLE_OPERATION_TIMEDOUT == CurlCodeRes)
			|| (CURLE_COULDNT_RESOLVE_HOST == CurlCodeRes)
			|| (CURLE_COULDNT_RESOLVE_PROXY == CurlCodeRes)
			|| (CURLE_FTP_ACCEPT_TIMEOUT == CurlCodeRes))
		{
			if (DownLoadTrans.DownloadTask.nTryCount < Max_DownloadRetry)
			{
				bRetry = true;
			}
		}

	} while (false);
	
	//remove
	RemoveDownLoadTrans(DownLoadTrans.nDownLoadID);

	//un retry
	if (false == bRetry)
	{
		PerformDownLoadNotify(DownLoadTrans.nDownLoadID, DownLoadNotify_Error, DownLoadTrans.pIAttemperSink, CurlCodeRes);

		return;
	}
	
	//retry
	tagDownLoadTask DownloadTask = DownLoadTrans.DownloadTask;
	DownloadTask.nTryCount++;
	m_vecDownLoadTask.insert(m_vecDownLoadTask.begin(), DownloadTask);
		
	return;
}

void CMMDownLoad::PerformActiveNextTask()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	int nTask = DOWNLOAD_TASK - m_vecDownLoadTrans.size();
	while (nTask-- > 0 && m_vecDownLoadTask.size() > 0)
	{
		tagDownLoadTask DownLoadTask = m_vecDownLoadTask.front();
		m_vecDownLoadTask.erase(m_vecDownLoadTask.begin());

		auto pMessage = std::make_shared<M_MMDownLoad_TaskAdd>();
		pMessage->DownLoadTask = DownLoadTask;
		Send(this, pMessage);
	}

	return;
}

void CMMDownLoad::PerformDownLoadNotify(int nDownLoadID, enDownLoadNotifyType NotifyType, IMMDownLoadAttemperSink *pIAttemperSink, int nErrorCode)
{
	std::lock_guard<std::recursive_mutex> Lock(m_NotifyLock);

	if (NULL == pIAttemperSink) return;

	tagDownLoadNotify DownLoadNotify;
	DownLoadNotify.nDownLoadID = nDownLoadID;
	DownLoadNotify.NotifyType = NotifyType;
	DownLoadNotify.ErrorCode = (enDownLoadErrorCode)nErrorCode;
	DownLoadNotify.pIAttemperSink = pIAttemperSink;
	m_vecDownLoadNotify.push_back(DownLoadNotify);

	PostMessage(WM_DL_NOTIFY, 0, 0);

	return;
}

void CMMDownLoad::PerformDownLoadNotify(int nDownLoadID, enDownLoadNotifyType NotifyType, IMMDownLoadAttemperSink *pIAttemperSink, int nSizeTotal, int nSizeDownLoad, int nSizeSpeed)
{
	std::lock_guard<std::recursive_mutex> Lock(m_NotifyLock);

	if (NULL == pIAttemperSink) return;

	tagDownLoadNotify DownLoadNotify;
	DownLoadNotify.nDownLoadID = nDownLoadID;
	DownLoadNotify.NotifyType = NotifyType;
	DownLoadNotify.pIAttemperSink = pIAttemperSink;
	DownLoadNotify.nSizeTotal = nSizeTotal;
	DownLoadNotify.nSizeDownLoad = nSizeDownLoad;
	DownLoadNotify.nSizeSpeed = nSizeSpeed;
	m_vecDownLoadNotify.push_back(DownLoadNotify);

	PostMessage(WM_DL_NOTIFY, 0, 0);

	return;
}