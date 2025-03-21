#include "StdAfx.h"
#include "MMTCPSocketServer.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
//////////////////////////////////////////////////////////////////////////
#define TIME_DETECT_ID					(1989)
#define TIME_DETECT_ELAPSE				(20000)								//监测时间

#define TIME_BREAK_READY				(9000L)								//中断时间
#define TIME_BREAK_CONNECT				(4000L)								//中断时间

//////////////////////////////////////////////////////////////////////////
CMMSocketReadWrite::CMMSocketReadWrite(CMMTCPSocketServer *pTCPNetworkEngine, CMMThreadPool *pOwnerPool)
	: CMMServiceItem(pOwnerPool)
	, m_pTCPNetworkEngine(pTCPNetworkEngine)
{
}

CMMSocketReadWrite::~CMMSocketReadWrite(void)
{
}

bool CMMSocketReadWrite::UnInit()
{
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		m_bStop = true;
	}

	return __super::UnInit();
}

bool CMMSocketReadWrite::IsStop()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_bStop;
}

void CMMSocketReadWrite::OnMessage(PtrMMServiceMsg pMessage)
{
	if (typeid(*pMessage) == typeid(M_MMSocketReadWrite_Init))
	{
		return OnSubInit(std::dynamic_pointer_cast<M_MMSocketReadWrite_Init>(pMessage));
	}

	return;
}

void CMMSocketReadWrite::OnSubInit(shared_ptr<M_MMSocketReadWrite_Init> pRecvData)
{
	if (NULL == pRecvData || NULL == m_pTCPNetworkEngine) return;

	assert(pRecvData->hCompletionPort != NULL);
	m_hCompletionPort = pRecvData->hCompletionPort;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		m_bStop = false;
	}

	while (true)
	{
		if (IsStop()) break;

		//效验参数
		assert(m_hCompletionPort);

		//变量定义
		DWORD dwThancferred = 0;
		OVERLAPPED *pOverLapped = NULL;
		COverLapped *pSocketLapped = NULL;
		CMMSocketClientItem *pClientItem = NULL;

		//等待完成端口
		BOOL bSuccess = GetQueuedCompletionStatus(m_hCompletionPort, &dwThancferred, (PULONG_PTR)&pClientItem, &pOverLapped, INFINITE);
		if ((FALSE == bSuccess) && (ERROR_NETNAME_DELETED != GetLastError())) return;
		if ((NULL == pClientItem) && (NULL == pOverLapped)) return;

		//处理操作
		assert(pOverLapped != NULL);
		assert(pClientItem != NULL);
		pSocketLapped = CONTAINING_RECORD(pOverLapped, COverLapped, m_OverLapped);
		switch (pSocketLapped->GetOperationType())
		{
			case OperationType_Recv:	//SOCKET 数据读取
			{
				COverLappedRecv *pOverLappedRecv = (COverLappedRecv*)pSocketLapped;

				auto pMessage = std::make_shared<M_MMTCPNetworkEngine_RecvComplete>();
				pMessage->pClientItem = pClientItem;
				pMessage->pOverLappedRecv = pOverLappedRecv;
				pMessage->dwThancferred = dwThancferred;
				Send(m_pTCPNetworkEngine, pMessage);
		
				break;
			}
			case OperationType_Send:	//SOCKET 数据发送
			{
				COverLappedSend *pOverLappedSend = (COverLappedSend*)pSocketLapped;
				
				auto pMessage = std::make_shared<M_MMTCPNetworkEngine_SendComplete>();
				pMessage->pClientItem = pClientItem;
				pMessage->pOverLappedSend = pOverLappedSend;
				pMessage->dwThancferred = dwThancferred;
				Send(m_pTCPNetworkEngine, pMessage);

				break;
			}
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
CMMSocketAccept::CMMSocketAccept(CMMTCPSocketServer *pTCPNetworkEngine, CMMThreadPool *pOwnerPool)
	: CMMServiceItem(pOwnerPool)
	, m_pTCPNetworkEngine(pTCPNetworkEngine)
{
}

CMMSocketAccept::~CMMSocketAccept(void)
{
}

bool CMMSocketAccept::UnInit()
{
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		m_bStop = true;
	}

	return __super::UnInit();
}

bool CMMSocketAccept::IsStop()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_bStop;
}

void CMMSocketAccept::OnMessage(PtrMMServiceMsg pMessage)
{
	if (typeid(*pMessage) == typeid(M_MMSocketAccept_Init))
	{
		return OnSubInit(std::dynamic_pointer_cast<M_MMSocketAccept_Init>(pMessage));
	}

	return;
}

void CMMSocketAccept::OnSubInit(shared_ptr<M_MMSocketAccept_Init> pRecvData)
{
	if (NULL == pRecvData) return;

	assert(pRecvData->hCompletionPort != NULL);
	assert(pRecvData->hSocketListen != INVALID_SOCKET);
	m_hSocketListen = pRecvData->hSocketListen;
	m_hCompletionPort = pRecvData->hCompletionPort;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		m_bStop = false;
	}

	while (true)
	{
		if (IsStop()) break;

		//设置变量
		SOCKADDR_IN	SocketAddr;
		SOCKET hConnectSocket = INVALID_SOCKET;
		int nBufferSize = sizeof(SocketAddr);

		try
		{
			//监听连接
			hConnectSocket = WSAAccept(m_hSocketListen, (SOCKADDR *)&SocketAddr, &nBufferSize, NULL, NULL);
			if (INVALID_SOCKET == hConnectSocket) return;

			//add
			auto pMessage = std::make_shared<M_MMTCPNetworkEngine_Accept>();
			pMessage->hSocket = hConnectSocket;
			pMessage->uClientAddr = SocketAddr.sin_addr.S_un.S_addr;
			Send(m_pTCPNetworkEngine, pMessage);
		}
		catch (...)
		{
			if (INVALID_SOCKET != hConnectSocket) closesocket(hConnectSocket);
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
CMMSocketNotify::CMMSocketNotify(CMMTCPSocketServer *pTCPNetworkEngine, CMMThreadPool *pOwnerPool)
	: CMMServiceItem(pOwnerPool)
	, m_pTCPNetworkEngine(pTCPNetworkEngine)
{
}

CMMSocketNotify::~CMMSocketNotify(void)
{
}

bool CMMSocketNotify::IsStop()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_bStop;
}

void CMMSocketNotify::AddTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find(m_vecTCPNetworkAttemperSink.begin(), m_vecTCPNetworkAttemperSink.end(), pIAttemperSink);
	if (FindIt != m_vecTCPNetworkAttemperSink.end()) return;

	m_vecTCPNetworkAttemperSink.push_back(pIAttemperSink);

	return;
}

void CMMSocketNotify::RemoveTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find(m_vecTCPNetworkAttemperSink.begin(), m_vecTCPNetworkAttemperSink.end(), pIAttemperSink);
	if (FindIt == m_vecTCPNetworkAttemperSink.end()) return;

	m_vecTCPNetworkAttemperSink.erase(FindIt);

	return;
}

bool CMMSocketNotify::UnInit()
{
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		m_bStop = true;
	}

	m_pTCPNetworkEngine = NULL;
	m_vecTCPNetworkAttemperSink.clear();

	return __super::UnInit();
}

void CMMSocketNotify::OnMessage(PtrMMServiceMsg pMessage)
{
	if (typeid(*pMessage) == typeid(M_MMSocketNotify_Bind))
	{
		return OnSubBind(std::dynamic_pointer_cast<M_MMSocketNotify_Bind>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMSocketNotify_Shut))
	{
		return OnSubShut(std::dynamic_pointer_cast<M_MMSocketNotify_Shut>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMSocketNotify_Read))
	{
		return OnSubRead(std::dynamic_pointer_cast<M_MMSocketNotify_Read>(pMessage));
	}

	return;
}

void CMMSocketNotify::OnSubBind(shared_ptr<M_MMSocketNotify_Bind> pRecvData)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pRecvData) return;

	for (auto pIAttemperSink : m_vecTCPNetworkAttemperSink)
	{
		if (NULL == pIAttemperSink) continue;

		pIAttemperSink->OnEventTCPNetworkBind(pRecvData->dwSocketID, pRecvData->dwClientAddr);
	}

	return;
}

void CMMSocketNotify::OnSubShut(shared_ptr<M_MMSocketNotify_Shut> pRecvData)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pRecvData) return;

	for (auto pIAttemperSink : m_vecTCPNetworkAttemperSink)
	{
		if (NULL == pIAttemperSink) continue;

		pIAttemperSink->OnEventTCPNetworkShut(pRecvData->dwSocketID, pRecvData->dwClientAddr);
	}

	return;
}

void CMMSocketNotify::OnSubRead(shared_ptr<M_MMSocketNotify_Read> pRecvData)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pRecvData) return;

	for (auto pIAttemperSink : m_vecTCPNetworkAttemperSink)
	{
		if (NULL == pIAttemperSink) continue;

		auto &vecData = pRecvData->vecData;
		pIAttemperSink->OnEventTCPNetworkRead(pRecvData->dwSocketID, pRecvData->wMainCmd, pRecvData->wSubCmd, 
			vecData.size() > 0 ? vecData.data() : NULL, vecData.size());
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
CMMTCPSocketServer::CMMTCPSocketServer(bool	bEncrypt)
	: CMMServiceItem(&m_ThreadPool)
	, m_SocketAcceptWork(this, &m_ThreadPool)
	, m_SocketNotifyWork(this, &m_ThreadPool)
	, m_bEncrypt(bEncrypt)
{
	return;
}

CMMTCPSocketServer::~CMMTCPSocketServer(void)
{
	ConcludeService();

	return;
}

LPVOID CMMTCPSocketServer::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMMTCPSocketServer, Guid, dwQueryVer);

	return NULL;
}

bool CMMTCPSocketServer::StartService(WORD wServicePort, WORD wLinkCountLimit)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (m_bService)
	{
		m_strLastError = _T("网络引擎重复启动，启动操作忽略");
		return true;
	}
	if (0 == wServicePort)
	{
		wServicePort = 3000;
		m_strLastError = _T("网络引擎监听端口无效");
		return false;
	}

	try
	{
		WSADATA WSAData;
		WORD wVersionRequested = MAKEWORD(2, 2);
		int nErrorCode = WSAStartup(wVersionRequested, &WSAData);
		assert(0 == nErrorCode);

		m_wListenPort = wServicePort;
		m_wLinkCountLimit = wLinkCountLimit;

		//获取系统信息
		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		DWORD dwThreadCount = SystemInfo.dwNumberOfProcessors;

		//建立完成端口
		m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, SystemInfo.dwNumberOfProcessors);
		if (NULL == m_hCompletionPort) throw _T("网络引擎完成端口创建失败");

		//建立监听SOCKET
		struct sockaddr_in SocketAddr;
		memset(&SocketAddr, 0, sizeof(SocketAddr));
		SocketAddr.sin_addr.s_addr = INADDR_ANY;
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(m_wListenPort);
		m_hSocketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == m_hSocketServer) throw _T("网络引擎监听 SOCKET 创建失败");
		
		nErrorCode = ::bind(m_hSocketServer, (SOCKADDR*)&SocketAddr, sizeof(SocketAddr));
		if (SOCKET_ERROR == nErrorCode) throw _T("网络引擎监听端口被占用，端口绑定失败");
		
		nErrorCode = listen(m_hSocketServer, SOMAXCONN);
		if (SOCKET_ERROR == nErrorCode) throw _T("网络引擎监听端口被占用，端口监听失败");

		//service init
		Init();

		//timer power
		CMMTimerPower::GetInstance()->Init();

		//readwrite
		int ThreadRWCount = max(1, dwThreadCount / 4);
		for (int i = 0; i < ThreadRWCount; i++)
		{
			CMMSocketReadWrite *pSocketReadWrite = new CMMSocketReadWrite(this, &m_ThreadPool);

			auto pMessage = std::make_shared<M_MMSocketReadWrite_Init>();
			pMessage->hCompletionPort = m_hCompletionPort;
			Send(pSocketReadWrite, pMessage);

			m_vecSocketReadWriteWork.push_back(pSocketReadWrite);
		}

		//accept
		{
			auto pMessage = std::make_shared<M_MMSocketAccept_Init>();
			pMessage->hCompletionPort = m_hCompletionPort;
			pMessage->hSocketListen = m_hSocketServer;
			m_SocketAcceptWork.Init();
			Send(&m_SocketAcceptWork, pMessage);
		}
		
		//notify
		m_SocketNotifyWork.Init();

		//detect work
		SetTimer(TIME_DETECT_ID, TIME_DETECT_ELAPSE, [&]()
		{
			//OnTimerDetect();
		});

		//start thread
		m_ThreadPool.Run(dwThreadCount * 4);

		//设置变量
		m_bService = true;
	}
	catch (LPCTSTR pszError)
	{
		m_strLastError = pszError;
		WSACleanup();
		return false;
	}

	return true;
}

bool CMMTCPSocketServer::ConcludeService()
{
	__super::UnInit();

	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (m_bService)
	{
		WSACleanup();
	}

	m_bService = false;

	//socket
	if (INVALID_SOCKET != m_hSocketServer)
	{
		closesocket(m_hSocketServer);
		m_hSocketServer = INVALID_SOCKET;
	}
	if (m_hCompletionPort)
	{
		for (int i = 0; i < m_vecSocketReadWriteWork.size(); i++)
		{
			PostQueuedCompletionStatus(m_hCompletionPort, 0, NULL, NULL);
		}

		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	//work
	m_SocketAcceptWork.UnInit();
	m_SocketNotifyWork.UnInit();
	for (auto pSocketReadWrite : m_vecSocketReadWriteWork)
	{
		if (NULL == pSocketReadWrite) continue;

		pSocketReadWrite->UnInit();
	}
	for (auto pClientItem : m_vecActiveSocketItem)
	{
		if (NULL == pClientItem) continue;

		pClientItem->UnInit();
	}
	m_ThreadPool.Stop();

	//release
	for (auto pSocketReadWrite : m_vecSocketReadWriteWork)
	{
		MMSafeDelete(pSocketReadWrite);
	}
	for (auto pClientItem : m_vecActiveSocketItem)
	{
		MMSafeDelete(pClientItem);
	}
	m_vecSocketReadWriteWork.clear();
	m_vecActiveSocketItem.clear();

	return true;
}

bool CMMTCPSocketServer::IsServecing()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_bService;
}

WORD CMMTCPSocketServer::GetListenPort()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_wListenPort;
}

WORD CMMTCPSocketServer::GetLinkLimit()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_wLinkCountLimit;
}

WORD CMMTCPSocketServer::GetLinkCount()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_vecActiveSocketItem.size();
}

CMMString CMMTCPSocketServer::GetLastError()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_strLastError;
}

bool CMMTCPSocketServer::PerformCloseSocket(DWORD dwSocketID)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	CMMSocketClientItem *pClientItem = FindClientItem(dwSocketID);
	if (NULL == pClientItem) return false;

	auto pMessage = std::make_shared<M_MMSocketClientItem_Close>();
	Send(pClientItem, pMessage);

	return true;
}

bool CMMTCPSocketServer::PerformAllowBatch(DWORD dwSocketID, bool bAllowBatch, BYTE cbMaskBatch)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	CMMSocketClientItem *pClientItem = FindClientItem(dwSocketID);
	if (NULL == pClientItem) return false;

	auto pMessage = std::make_shared<M_MMSocketClientItem_AllowBatch>();
	pMessage->bAllow = bAllowBatch;
	pMessage->cbMaskBatch = cbMaskBatch;
	Send(pClientItem, pMessage);
	
	return true;
}

void CMMTCPSocketServer::AddTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_SocketNotifyWork.AddTCPSocketSink(pIAttemperSink);
}

void CMMTCPSocketServer::RemoveTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_SocketNotifyWork.RemoveTCPSocketSink(pIAttemperSink);
}

bool CMMTCPSocketServer::SendData(DWORD dwSocketID, WORD wMainCmd, WORD wSubCmd)
{
	return SendData(dwSocketID, wMainCmd, wSubCmd, NULL, NULL);
}

bool CMMTCPSocketServer::SendData(DWORD dwSocketID, WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (false == IsServecing()) return false;

	CMMSocketClientItem *pClientItem = FindClientItem(dwSocketID);
	if (NULL == pClientItem) return false;

	auto pMessage = std::make_shared<M_MMSocketClientItem_SendData>();
	pMessage->wMainCmd = wMainCmd;
	pMessage->wSubCmd = wSubCmd;
	pMessage->vecData.assign((BYTE*)pData, (BYTE*)pData + wDataSize);

	Send(pClientItem, pMessage);

	return true;
}

bool CMMTCPSocketServer::SendDataBatch(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize, BYTE cbMaskBatch)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (false == IsServecing()) return false;

	auto pMessage = std::make_shared<M_MMSocketClientItem_SendData>();
	pMessage->wMainCmd = wMainCmd;
	pMessage->wSubCmd = wSubCmd;
	pMessage->vecData.assign((BYTE*)pData, (BYTE*)pData + wDataSize);

	for (auto pClientItem : m_vecActiveSocketItem)
	{
		if (NULL == pClientItem) continue;
		if (false == pClientItem->IsAllowBatch()) continue;
		if (0 == (cbMaskBatch & pClientItem->GetMaskBatch())) continue;

		Send(pClientItem, pMessage);
	}

	return true;
}

void CMMTCPSocketServer::OnMessage(PtrMMServiceMsg pMessage)
{
	if (typeid(*pMessage) == typeid(M_MMTCPNetworkEngine_Accept))
	{
		return OnSubAccept(std::dynamic_pointer_cast<M_MMTCPNetworkEngine_Accept>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMTCPNetworkEngine_Closed))
	{
		return OnSubClosed(std::dynamic_pointer_cast<M_MMTCPNetworkEngine_Closed>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMTCPNetworkEngine_RecvComplete))
	{
		return OnSubRecvComplete(std::dynamic_pointer_cast<M_MMTCPNetworkEngine_RecvComplete>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMTCPNetworkEngine_SendComplete))
	{
		return OnSubSendComplete(std::dynamic_pointer_cast<M_MMTCPNetworkEngine_SendComplete>(pMessage));
	}

	Send(&m_SocketNotifyWork, pMessage);

	return;
}

void CMMTCPSocketServer::OnSubAccept(shared_ptr<M_MMTCPNetworkEngine_Accept> pRecvData)
{
	if (NULL == pRecvData || false == IsServecing()) return;

	//limit
	if (GetLinkCount() >= GetLinkLimit())
	{
		if (INVALID_SOCKET != pRecvData->hSocket)
		{
			closesocket(pRecvData->hSocket);
		}

		return;
	}

	//add
	CMMSocketClientItem *pClientItem = NULL;
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		pClientItem = new CMMSocketClientItem(pRecvData->hSocket, pRecvData->uClientAddr, &m_ThreadPool, this, m_bEncrypt);
		if (NULL == pClientItem) throw _T("申请连接对象失败");

		m_vecActiveSocketItem.push_back(pClientItem);
	}

	//work
	if (pClientItem)
	{
		CreateIoCompletionPort((HANDLE)pRecvData->hSocket, m_hCompletionPort, (ULONG_PTR)pClientItem, 0);

		auto pMessage = std::make_shared<M_MMSocketClientItem_Work>();
		Send(pClientItem, pMessage);
	}

	return;
}

void CMMTCPSocketServer::OnSubClosed(shared_ptr<M_MMTCPNetworkEngine_Closed> pRecvData)
{
	if (NULL == pRecvData || false == IsServecing()) return;

	CMMSocketClientItem *pClientItem = FindClientItem(pRecvData->dwSocketID);
	if (NULL == pClientItem) return;
	
	try
	{
		DWORD dwSocketID = pClientItem->GetSocketID();
		DWORD dwClientAddr = pClientItem->GetClientAddr();
		DWORD dwConnectTime = pClientItem->GetConnectTime();

		pClientItem->UnInit();
		
		//remove
		{
			std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

			m_vecActiveSocketItem.erase(std::remove(m_vecActiveSocketItem.begin(), m_vecActiveSocketItem.end(), pClientItem), m_vecActiveSocketItem.end());
		}

		MMSafeDelete(pClientItem);

		//notify
		auto pMessage = std::make_shared<M_MMSocketNotify_Shut>();
		pMessage->dwSocketID = dwSocketID;
		pMessage->dwClientAddr = dwClientAddr;
		Send(&m_SocketNotifyWork, pMessage);
	}
	catch (...)
	{
	}

	return;
}

void CMMTCPSocketServer::OnSubRecvComplete(shared_ptr<M_MMTCPNetworkEngine_RecvComplete> pRecvData)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pRecvData || false == IsServecing()) return;
	if (false == ValidClientItem(pRecvData->pClientItem)) return;

	auto pMessage = std::make_shared<M_MMSocketClientItem_RecvComplete>();
	pMessage->pOverLappedRecv = pRecvData->pOverLappedRecv;
	pMessage->dwThancferred = pRecvData->dwThancferred;
	Send(pRecvData->pClientItem, pMessage);

	return;
}

void CMMTCPSocketServer::OnSubSendComplete(shared_ptr<M_MMTCPNetworkEngine_SendComplete> pRecvData)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (NULL == pRecvData || false == IsServecing()) return;
	if (false == ValidClientItem(pRecvData->pClientItem)) return;

	auto pMessage = std::make_shared<M_MMSocketClientItem_SendComplete>();
	pMessage->pOverLappedSend = pRecvData->pOverLappedSend;
	pMessage->dwThancferred = pRecvData->dwThancferred;
	Send(pRecvData->pClientItem, pMessage);

	return;
}

void CMMTCPSocketServer::OnTimerDetect()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	//detect
	DWORD dwTickCount = GetTickCount();
	for (auto pClientItem : m_vecActiveSocketItem)
	{
		if (NULL == pClientItem) continue;

		DWORD dwRecvTickCount = pClientItem->GetRecvTickCount();
		if (dwRecvTickCount >= dwTickCount) continue;

		//recved overtime
		if (pClientItem->IsReadySend() && (dwTickCount - dwRecvTickCount) > TIME_BREAK_READY)
		{
			auto pMessage = std::make_shared<M_MMSocketClientItem_Close>();
			Send(pClientItem, pMessage);

			continue;
		}

		//connect overtime
		if (false == pClientItem->IsReadySend() && (dwTickCount - dwRecvTickCount) > TIME_BREAK_CONNECT)
		{
			auto pMessage = std::make_shared<M_MMSocketClientItem_Close>();
			Send(pClientItem, pMessage);

			continue;
		}

		if (pClientItem->IsReadySend())
		{
			CMD_KN_DetectSocket DetectSocket = {};
			timeb tb;
			ftime(&tb);
			DetectSocket.tSendTime = tb.time * 1000 + tb.millitm;

			SendData(pClientItem->GetSocketID(), MDM_Engine_Core, SUB_ToC_Detect, &DetectSocket, sizeof(DetectSocket));
		}
	}

	return;
}

CMMSocketClientItem * CMMTCPSocketServer::FindClientItem(DWORD dwSocketID)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	for (auto pClientItem : m_vecActiveSocketItem)
	{
		if (NULL == pClientItem) continue;
		if (dwSocketID == pClientItem->GetSocketID()) return pClientItem;
	}

	return NULL;
}

bool CMMTCPSocketServer::ValidClientItem(CMMSocketClientItem *pClientItem)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find(m_vecActiveSocketItem.begin(), m_vecActiveSocketItem.end(), pClientItem);
	if (FindIt == m_vecActiveSocketItem.end()) return false;

	return true;
}

Implement_MMCreateModule(MMTCPSocketServer);

//////////////////////////////////////////////////////////////////////////
#endif