#include "Stdafx.h"
#include "MMTCPSocketClient.h"

//////////////////////////////////////////////////////////////////////////						
#define WM_SOCKET_EVENT					(WM_USER + 1989)	
#define WM_SOCKET_SEND					(WM_USER + 1990)
#define WM_SOCKET_NOTIFY				(WM_USER + 1991)

//////////////////////////////////////////////////////////////////////////
struct CMMTCPSocketClient::tagData
{
	DWORD								dwSendKey = 0;
	DWORD								dwRecvKey = 0;

	void Init()
	{
		dwSendKey = rand();

		return;
	}

	inline void EncryptBuffer(vector<BYTE> &vecBuffer)
	{
		if (vecBuffer.empty()) return;

		vecBuffer = CMMEncrypt::MapEncrypt(vecBuffer, dwSendKey);

		return;
	}

	inline void DecryptBuffer(vector<BYTE> &vecBuffer)
	{
		if (vecBuffer.empty()) return;

		vecBuffer = CMMEncrypt::MapDecrypt(vecBuffer, dwRecvKey);

		return;
	}
};

//////////////////////////////////////////////////////////////////////////
CMMTCPSocketClient::CMMTCPSocketClient(bool bEncrypt)
{
	m_pData = new tagData();

	m_bEncrypt = bEncrypt;

	//init
	WSADATA WSAData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	int nErrorCode = WSAStartup(wVersionRequested, &WSAData);
	assert(0 == nErrorCode);

	return;
}

CMMTCPSocketClient::~CMMTCPSocketClient()
{
	WSACleanup();

	MMSafeDelete(m_pData);

	return;
}

LPVOID CMMTCPSocketClient::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMMTCPSocketClient, Guid, dwQueryVer);
	QUERYINTERFACE_IMMUNKNOWN(IMMTCPSocketClient, Guid, dwQueryVer);
	return NULL;
}

bool CMMTCPSocketClient::Init()
{
	if (false == CMMAsyncObject::Init()) return false;

	//key
	m_pData->Init();

	m_vecBufferSend.clear();
	m_vecBufferRecv.clear();

	return true;
}

bool CMMTCPSocketClient::UnInit()
{
	if (false == CMMAsyncObject::UnInit()) return false;

	CloseSocket(SocketShut_Normal);

	return true;
}

WORD CMMTCPSocketClient::GetSocketID()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_wSocketID;
}

bool CMMTCPSocketClient::AddTCPSocketSink(IMMTCPSocketClientAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find(m_vecTCPSocketSink.begin(), m_vecTCPSocketSink.end(), pIAttemperSink);
	if (FindIt != m_vecTCPSocketSink.end()) return true;

	m_vecTCPSocketSink.push_back(pIAttemperSink);

	return true;
}

void CMMTCPSocketClient::RemoveTCPSocketSink(IMMTCPSocketClientAttemperSink *pIAttemperSink)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find(m_vecTCPSocketSink.begin(), m_vecTCPSocketSink.end(), pIAttemperSink);
	if (FindIt == m_vecTCPSocketSink.end()) return;

	m_vecTCPSocketSink.erase(FindIt);

	return;
}

enSocketStatus CMMTCPSocketClient::GetSocketStatus()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_SocketStatus;
}

DWORD CMMTCPSocketClient::GetLastSendTick()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwSendTickCount;
}

DWORD CMMTCPSocketClient::GetLastRecvTick()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwRecvTickCount;
}

DWORD CMMTCPSocketClient::GetSendPacketCount()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwSendPacketCount;
}

DWORD CMMTCPSocketClient::GetRecvPacketCount()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwRecvPacketCount;
}

void CMMTCPSocketClient::Connect(DWORD dwServerIP, WORD wPort)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (SocketStatus_Idle != GetSocketStatus()) return;

	try
	{
		//verify
		if (INVALID_SOCKET != m_hSocket) throw _T("连接 SOCKET 句柄已经存在！");
		if (SocketStatus_Idle != m_SocketStatus) throw _T("连接状态不是等待连接状态！");
		if (INADDR_NONE == dwServerIP) throw _T("目标服务器地址格式不正确，请检查后再次尝试！");

		//info
		m_vecBufferSend.clear();
		m_vecBufferRecv.clear();
		m_vecSocketNotifyTask.clear();
		m_dwSendTickCount = GetTickCount();
		m_dwRecvTickCount = GetTickCount();
		m_dwSendPacketCount = 0;
		m_dwRecvPacketCount = 0;

		m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == m_hSocket) throw _T("SOCKET 创建失败！");
		if (NULL == m_hWnd) throw _T("SOCKET 内部窗口创建失败！");

		//address
		m_wSocketID = m_hSocket;
		SOCKADDR_IN SocketAddr;
		memset(&SocketAddr, 0, sizeof(SocketAddr));
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr = dwServerIP;

		//connect
		WSASetLastError(0);

		int nResCode = WSAAsyncSelect(m_hSocket, m_hWnd, WM_SOCKET_EVENT, FD_READ | FD_CONNECT | FD_CLOSE);
		if (SOCKET_ERROR == nResCode) throw _T("绑定内部窗口错误！");

		nResCode = connect(m_hSocket, (SOCKADDR *)&SocketAddr, sizeof(SocketAddr));
		if (SOCKET_ERROR == nResCode)
		{
			int nErrorCode = WSAGetLastError();
			if (nErrorCode != WSAEWOULDBLOCK)
			{
				static TCHAR szBuffer[MAX_PATH] = {};
				_sntprintf(szBuffer, MMCountString(szBuffer), _T("连接发生错误，错误代码 [ %d ]！"), nErrorCode);
				throw szBuffer;
			}
		}

		//status
		m_SocketStatus = SocketStatus_Wait;
	}
	catch (LPCTSTR lpszError)
	{
		AddNotifyTask(SocketConnect_Fail, lpszError);
		CloseSocket(SocketShut_Inside);
	}
	catch (...)
	{
		AddNotifyTask(SocketConnect_Exception, _T("连接产生未知异常错误！"));
		CloseSocket(SocketShut_Inside);
	}

	return;
}

void CMMTCPSocketClient::Connect(const LPCTSTR lpszServerIP, WORD wPort)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	assert(0 != wPort);
	assert(NULL != lpszServerIP);
	if (MMInvalidString(lpszServerIP) || (0 == wPort))
	{
		AddNotifyTask(SocketConnect_Fail, _T("地址端口不合法！"));

		return;
	}

	return Connect(CMMService::StringToIPAddr(lpszServerIP), wPort);
}

bool CMMTCPSocketClient::SendData(WORD wMainCmd, WORD wSubCmd)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (false == VerifyTask())
	{
		assert(false);
		return false;
	}

	//pack
	tagTCPHead TCPHead = {};
	TCPHead.wMainCmd = wMainCmd;
	TCPHead.wSubCmd = wSubCmd;
	TCPHead.wPacketSize = sizeof(TCPHead);

	//key
	if (m_bEncrypt && 0 == GetSendPacketCount())
	{
		TCPHead.dwReserve = m_pData->dwSendKey;
	}

	m_dwSendPacketCount++;
	m_vecBufferSend.insert(m_vecBufferSend.end(), (BYTE*)&TCPHead, (BYTE*)&TCPHead + sizeof(TCPHead));

	PerformActiveSend();

	return true;
}

bool CMMTCPSocketClient::SendData(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (false == VerifyTask())
	{
		assert(false);
		return false;
	}

	//pack
	tagTCPHead TCPHead = {};
	TCPHead.wMainCmd = wMainCmd;
	TCPHead.wSubCmd = wSubCmd;

	vector<BYTE> vecData((BYTE*)pData, (BYTE*)pData + wDataSize);
	if (m_bEncrypt)
	{
		m_pData->EncryptBuffer(vecData);
	}

	//key
	if (m_bEncrypt && 0 == GetSendPacketCount())
	{
		TCPHead.dwReserve = m_pData->dwSendKey;
	}

	//insert
	m_dwSendPacketCount++;
	TCPHead.wPacketSize = sizeof(tagTCPHead) + vecData.size();
	m_vecBufferSend.insert(m_vecBufferSend.end(), (BYTE*)&TCPHead, (BYTE*)&TCPHead + sizeof(TCPHead));
	m_vecBufferSend.insert(m_vecBufferSend.end(), vecData.begin(), vecData.end());

	PerformActiveSend();

	return true;
}

void CMMTCPSocketClient::CloseSocket(enSocketShutReason ShutReason)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (INVALID_SOCKET != m_hSocket)
	{
		WSAAsyncSelect(m_hSocket, m_hWnd, WM_SOCKET_EVENT, 0);

		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	if (SocketStatus_Connect == m_SocketStatus)
	{
		AddNotifyTask(ShutReason);
	}

	//variant
	m_SocketStatus = SocketStatus_Idle;
	m_vecBufferSend.clear();
	m_vecBufferRecv.clear();
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;

	return;
}

LRESULT CMMTCPSocketClient::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (uMsg)
	{
		case WM_SOCKET_EVENT:
		{
			bHandled = true;

			return OnWMSocketEvent(wParam, lParam);
		}
		case WM_SOCKET_SEND:
		{
			bHandled = true;

			return OnWMSendBuffer(wParam, lParam);
		}
		case WM_SOCKET_NOTIFY:
		{
			bHandled = true;

			return OnWMSocketNotify(wParam, lParam);
		}
	}

	return 0;
}

bool CMMTCPSocketClient::VerifyTask()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return INVALID_SOCKET != m_hSocket && SocketStatus_Connect == m_SocketStatus;
}

void CMMTCPSocketClient::PerformActiveSend()
{
	PostMessage(WM_SOCKET_SEND, NULL, NULL);

	return;
}

void CMMTCPSocketClient::PerformActiveNotify()
{
	PostMessage(WM_SOCKET_NOTIFY, NULL, NULL);

	return;
}

CMMString CMMTCPSocketClient::TransConnectErrorCode(int nErrorCode)
{
	switch (nErrorCode)
	{
		case 0:
		{
			return _T("连接成功");
		}
		case WSAEADDRNOTAVAIL:
		{
			return _T("目标服务器地址格式不正确，请检查后再次尝试！");
		}
		case WSAECONNREFUSED:
		{
			return _T("目标服务器繁忙或者没有启动！");
		}
		case WSAETIMEDOUT:
		{
			return _T("连接超时，可能是目标服务器不存在或者服务器地址格式不正确！");
		}
		case WSAEHOSTUNREACH:
		{
			return _T("网络连接失败，请检查是否已经成功拨号和连接 Internet ！");
		}
		default:
		{
			return CMMStrHelp::Format(_T("连接错误号：%d，详细错误信息请参考操作帮助手册！"), nErrorCode);
		}
	}

	return _T("未知错误！");
}

void CMMTCPSocketClient::AddNotifyTask(enSocketConnectCode ConnectCode, LPCTSTR lpszDescribe)
{
	m_vecSocketNotifyTask.push_back(tagSocketNotifyTask());
	m_vecSocketNotifyTask.back().SocketNotifyType = SocketNotify_Link;
	m_vecSocketNotifyTask.back().SocketConnectCode = ConnectCode;
	m_vecSocketNotifyTask.back().strErrorDescribe = lpszDescribe;

	PerformActiveNotify();

	return;
}

void CMMTCPSocketClient::AddNotifyTask(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize)
{
	m_vecSocketNotifyTask.push_back(tagSocketNotifyTask());
	m_vecSocketNotifyTask.back().SocketNotifyType = SocketNotify_Read;
	m_vecSocketNotifyTask.back().wMainCmd = wMainCmd;
	m_vecSocketNotifyTask.back().wSubCmd = wSubCmd;
	m_vecSocketNotifyTask.back().vecData.assign((BYTE*)pData, (BYTE*)pData + wDataSize);

	PerformActiveNotify();

	return;
}

void CMMTCPSocketClient::AddNotifyTask(enSocketShutReason ShutReason)
{
	m_vecSocketNotifyTask.push_back(tagSocketNotifyTask());
	m_vecSocketNotifyTask.back().SocketNotifyType = SocketNotify_Shut;
	m_vecSocketNotifyTask.back().SocketShutReason = ShutReason;

	PerformActiveNotify();

	return;
}

void CMMTCPSocketClient::SendSocketConnect(enSocketConnectCode ConnectCode, LPCTSTR lpszDescribe)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	for (auto pITCPSocketSink : m_vecTCPSocketSink)
	{
		if (NULL == pITCPSocketSink) continue;

		pITCPSocketSink->OnEventTCPSocketLink(GetSocketID(), ConnectCode, lpszDescribe);
	}

	return;
}

void CMMTCPSocketClient::SendSocketRead(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	for (auto pITCPSocketSink : m_vecTCPSocketSink)
	{
		if (NULL == pITCPSocketSink) continue;

		bool bHandled = false;
		pITCPSocketSink->OnEventTCPSocketRead(GetSocketID(), wMainCmd, wSubCmd, pData, wDataSize, bHandled);

		if (bHandled) break;
	}

	return;
}

void CMMTCPSocketClient::SendSocketShut(enSocketShutReason ShutReason)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	for (auto pITCPSocketSink : m_vecTCPSocketSink)
	{
		if (NULL == pITCPSocketSink) continue;

		pITCPSocketSink->OnEventTCPSocketShut(GetSocketID(), ShutReason);
	}

	return;
}

LRESULT	CMMTCPSocketClient::OnWMSocketEvent(WPARAM wParam, LPARAM lParam)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:
		{
			return OnSubSocketEventConnect(wParam, lParam);
		}
		case FD_READ:
		{
			return OnSubSocketEventRead(wParam, lParam);
		}
		case FD_CLOSE:
		{
			return OnSubSocketEventClose(wParam, lParam);
		}
	}

	return 0;
}

LRESULT CMMTCPSocketClient::OnWMSendBuffer(WPARAM wParam, LPARAM lParam)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (false == VerifyTask()) return 0;

	m_dwSendTickCount = GetTickCount();

	while (m_vecBufferSend.size() > 0)
	{
		int nDataSize = min(SOCKET_TCP_BUFFER, m_vecBufferSend.size());

		int nSend = send(m_hSocket, (char*)m_vecBufferSend.data(), nDataSize, 0);
		if (SOCKET_ERROR == nSend)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				break;
			}

			break;
		}

		m_vecBufferSend.erase(m_vecBufferSend.begin(), m_vecBufferSend.begin() + nSend);
	}
	if (m_vecBufferSend.size() > 0)
	{
		PerformActiveSend();
	}

	return 0;
}

LRESULT CMMTCPSocketClient::OnWMSocketNotify(WPARAM wParam, LPARAM lParam)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	while (false == m_vecSocketNotifyTask.empty())
	{
		auto NotifyTask = m_vecSocketNotifyTask.front();
		m_vecSocketNotifyTask.erase(m_vecSocketNotifyTask.begin());

		switch (NotifyTask.SocketNotifyType)
		{
			case SocketNotify_Link:
			{
				SendSocketConnect(NotifyTask.SocketConnectCode, NotifyTask.strErrorDescribe);

				break;
			}
			case SocketNotify_Read:
			{
				SendSocketRead(NotifyTask.wMainCmd, NotifyTask.wSubCmd, NotifyTask.vecData.size() > 0 ? NotifyTask.vecData.data() : NULL, NotifyTask.vecData.size());

				break;
			}
			case SocketNotify_Shut:
			{
				SendSocketShut(NotifyTask.SocketShutReason);

				break;
			}
		}
	}

	m_vecSocketNotifyTask.clear();

	return 0;
}

LRESULT CMMTCPSocketClient::OnSubSocketEventConnect(WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);
	CMMString strDescribe = TransConnectErrorCode(nErrorCode);

	if (0 == nErrorCode)
	{
		m_SocketStatus = SocketStatus_Connect;

		AddNotifyTask(SocketConnect_Success, strDescribe);

		return 0;
	}

	AddNotifyTask(SocketConnect_Fail, strDescribe);

	CloseSocket(SocketShut_Inside);

	return 0;
}

LRESULT CMMTCPSocketClient::OnSubSocketEventRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		std::vector<BYTE> vecBuffer;
		vecBuffer.resize(SOCKET_TCP_BUFFER);
		int nResCode = recv(m_hSocket, (char *)vecBuffer.data(), vecBuffer.size(), 0);
		if (SOCKET_ERROR == nResCode) throw _T("网络连接关闭，读取数据失败");

		m_vecBufferRecv.insert(m_vecBufferRecv.end(), vecBuffer.begin(), vecBuffer.begin() + nResCode);
		m_dwRecvTickCount = GetTickCount();

		if (m_vecBufferRecv.size() <= 0) return 0;

		//parse
		while (m_vecBufferRecv.size() >= sizeof(tagTCPHead))
		{
			tagTCPHead TCPHead = *(tagTCPHead*)m_vecBufferRecv.data();
			if (m_vecBufferRecv.size() < TCPHead.wPacketSize) break;
			if (TCPHead.wPacketSize < sizeof(tagTCPHead)) throw _T("数据包wPacketSize大小错误！");

			auto DataBegin = m_vecBufferRecv.data() + sizeof(tagTCPHead);
			int nDataSize = TCPHead.wPacketSize - sizeof(tagTCPHead);
			std::vector<BYTE> vecData(DataBegin, DataBegin + nDataSize);

			//decrypt key
			if (0 == GetRecvPacketCount())
			{
				m_pData->dwRecvKey = TCPHead.dwReserve;
			}

			//remove
			m_vecBufferRecv.erase(m_vecBufferRecv.begin(), m_vecBufferRecv.begin() + TCPHead.wPacketSize);

			//decrypt
			if (0 != m_pData->dwRecvKey)
			{
				m_pData->DecryptBuffer(vecData);
			}

			//command
			//if (MDM_KN_COMMAND == pHead->wMainCmd)
			//{
			//	switch (pHead->wSubCmd)
			//	{
			//		case SUB_KN_DETECT_SOCKET:	//网络检测
			//		{
			//			//发送数据
			//			SendData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, pDataBuffer, wDataSize);
			//			
			//			break;
			//		}
			//	}

			//	continue;
			//}

			m_dwRecvPacketCount++;

			//notify
			AddNotifyTask(TCPHead.wMainCmd, TCPHead.wSubCmd, vecData.size() > 0 ? vecData.data() : NULL, vecData.size());
		};
	}
	catch (LPCTSTR lpszError)
	{
		CloseSocket(SocketShut_Inside);
	}
	catch (...)
	{
		CloseSocket(SocketShut_Inside);
	}

	return 0;
}

LRESULT CMMTCPSocketClient::OnSubSocketEventClose(WPARAM wParam, LPARAM lParam)
{
	CloseSocket(SocketShut_Remote);

	return 0;
}

Implement_MMCreateModule(MMTCPSocketClient);

//////////////////////////////////////////////////////////////////////////