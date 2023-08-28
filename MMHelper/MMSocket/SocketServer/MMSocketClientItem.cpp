#include "Stdafx.h"
#include "MMSocketClientItem.h"
#include "WebSocket/SHA1.h"

//////////////////////////////////////////////////////////////////////////
COverLapped::COverLapped(enOperationType OperationType)
	: m_OperationType(OperationType)
{
	m_WSABuffer.len = 0;
	m_WSABuffer.buf = (char *)m_cbBuffer;
}

COverLapped::~COverLapped()
{
}

COverLappedSend::COverLappedSend()
	: COverLapped(OperationType_Send)
{
}

COverLappedSend::~COverLappedSend()
{
}

COverLappedRecv::COverLappedRecv()
	: COverLapped(OperationType_Recv)
{
}

COverLappedRecv::~COverLappedRecv()
{
}

//////////////////////////////////////////////////////////////////////////
CMMSocketClientItem::CMMSocketClientItem(SOCKET hSocket, DWORD dwClientAddr, CMMThreadPool *pOwnerPool, CMMTCPSocketServer *pTCPSocketServer, bool bEncrypt)
	: CMMServiceItem(pOwnerPool)
	, m_pTCPSocketServer(pTCPSocketServer)
	, m_bEncrypt(bEncrypt)
{
	m_hSocket = hSocket;
	m_dwSocketID = hSocket;
	m_dwClientAddr = dwClientAddr;
	m_dwSendKey = rand();
	m_dwConnectTime = GetTickCount();

	return;
}

CMMSocketClientItem::~CMMSocketClientItem(void)
{
	return;
}

bool CMMSocketClientItem::UnInit()
{
	PerformCloseSocket();

	return __super::UnInit();
}

void CMMSocketClientItem::OnMessage(PtrMMServiceMsg pMessage)
{
	if (typeid(*pMessage) == typeid(M_MMSocketClientItem_Work))
	{
		return OnSubWork(std::dynamic_pointer_cast<M_MMSocketClientItem_Work>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMSocketClientItem_RecvComplete))
	{
		return OnSubRecvComplete(std::dynamic_pointer_cast<M_MMSocketClientItem_RecvComplete>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMSocketClientItem_SendComplete))
	{
		return OnSubSendComplete(std::dynamic_pointer_cast<M_MMSocketClientItem_SendComplete>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMSocketClientItem_SendData))
	{
		return OnSubSendData(std::dynamic_pointer_cast<M_MMSocketClientItem_SendData>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMSocketClientItem_AllowBatch))
	{
		return OnSubAllowBatch(std::dynamic_pointer_cast<M_MMSocketClientItem_AllowBatch>(pMessage));
	}
	if (typeid(*pMessage) == typeid(M_MMSocketClientItem_Close))
	{
		return OnSubClose(std::dynamic_pointer_cast<M_MMSocketClientItem_Close>(pMessage));
	}

	return;
}

void CMMSocketClientItem::OnSubWork(shared_ptr<M_MMSocketClientItem_Work> pRecvData)
{
	if (NULL == pRecvData) return;

	//notify
	auto pMessage = std::make_shared<M_MMSocketNotify_Bind>();
	pMessage->dwSocketID = GetSocketID();
	pMessage->dwClientAddr = GetClientAddr();
	Send(m_pTCPSocketServer, pMessage);

	PerformRecvData();

	return;
}

void CMMSocketClientItem::OnSubRecvComplete(shared_ptr<M_MMSocketClientItem_RecvComplete> pRecvData)
{
	if (NULL == pRecvData) return;

	PerformRecvCompleted(pRecvData->pOverLappedRecv, pRecvData->dwThancferred);

	return;
}

void CMMSocketClientItem::OnSubSendComplete(shared_ptr<M_MMSocketClientItem_SendComplete> pRecvData)
{
	if (NULL == pRecvData) return;

	PerformSendCompleted(pRecvData->pOverLappedSend, pRecvData->dwThancferred);

	return;
}

void CMMSocketClientItem::OnSubSendData(shared_ptr<M_MMSocketClientItem_SendData> pRecvData)
{
	if (NULL == pRecvData) return;

	PerformSendData(pRecvData->wMainCmd, pRecvData->wSubCmd, pRecvData->vecData.size() > 0 ? pRecvData->vecData.data() : NULL, pRecvData->vecData.size());

	return;
}

void CMMSocketClientItem::OnSubAllowBatch(shared_ptr<M_MMSocketClientItem_AllowBatch> pRecvData)
{
	if (NULL == pRecvData) return;

	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	m_bAllowBatch = pRecvData->bAllow;
	m_cbMaskBatch = pRecvData->cbMaskBatch;

	return;
}

void CMMSocketClientItem::OnSubClose(shared_ptr<M_MMSocketClientItem_Close> pRecvData)
{
	if (NULL == pRecvData) return;

	PerformCloseSocket();

	return;
}

DWORD CMMSocketClientItem::GetSocketID()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwSocketID;
}

DWORD CMMSocketClientItem::GetClientAddr()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwClientAddr;
}

DWORD CMMSocketClientItem::GetConnectTime()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwConnectTime;
}

DWORD CMMSocketClientItem::GetSendTickCount()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwSendTickCount;
}

DWORD CMMSocketClientItem::GetRecvTickCount()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwRecvTickCount;
}

DWORD CMMSocketClientItem::GetSendPacketCount()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwSendPacketCount;
}

DWORD CMMSocketClientItem::GetRecvPacketCount()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwRecvPacketCount;
}

BYTE CMMSocketClientItem::GetMaskBatch()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_cbMaskBatch;
}

bool CMMSocketClientItem::IsReadySend()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_dwRecvPacketCount > 0L;
}

bool CMMSocketClientItem::IsAllowBatch()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_bAllowBatch;
}

bool CMMSocketClientItem::PerformSendData(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (INVALID_SOCKET == m_hSocket || GetRecvPacketCount() <= 0) return false;

	COverLappedSend *pOverLappedSend = ProductSendOverLapped();
	assert(pOverLappedSend);
	if (NULL == pOverLappedSend) return false;

	//pack
	tagTCPHead TCPHead = {};
	TCPHead.wMainCmd = wMainCmd;
	TCPHead.wSubCmd = wSubCmd;

	vector<BYTE> vecData((BYTE*)pData, (BYTE*)pData + wDataSize);
	if (m_bEncrypt)
	{
		m_bWebsocketClient ? EncryptBufferWS(vecData) : EncryptBuffer(vecData);
	}

	//key
	if (m_bEncrypt && 0 == GetSendPacketCount())
	{
		TCPHead.dwReserve = m_dwSendKey;
	}

	//buffer
	TCPHead.wPacketSize = sizeof(tagTCPHead) + vecData.size();
	m_vecBufferSend.insert(m_vecBufferSend.end(), (BYTE*)&TCPHead, (BYTE*)&TCPHead + sizeof(TCPHead));
	m_vecBufferSend.insert(m_vecBufferSend.end(), vecData.begin(), vecData.end());
	m_dwSendPacketCount++;

	//extract
	int nDataSize = min(SOCKET_TCP_BUFFER, m_vecBufferSend.size());
	CopyMemory(pOverLappedSend->m_cbBuffer, m_vecBufferSend.data(), nDataSize);
	pOverLappedSend->m_WSABuffer.len = nDataSize;
	m_vecBufferSend.erase(m_vecBufferSend.begin(), m_vecBufferSend.begin() + nDataSize);

	//send
	DWORD dwThancferred = 0;
	int nResCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
	if (0 == nResCode)
	{
		PerformSendCompleted(pOverLappedSend, dwThancferred);

		return true;
	}
	if ((SOCKET_ERROR == nResCode) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		PerformCloseSocket();

		return false;
	}

	return true;
}

void CMMSocketClientItem::PerformRecvData()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (INVALID_SOCKET == m_hSocket) return;

	//接收数据
	DWORD dwThancferred = 0, dwFlags = 0;
	int nResCode = WSARecv(m_hSocket, &m_OverLappedRecv.m_WSABuffer, 1, &dwThancferred, &dwFlags, &m_OverLappedRecv.m_OverLapped, NULL);
	/*if (0 == nResCode)
	{
		PerformRecvCompleted(&m_OverLappedRecv, dwThancferred);

		return;
	}*/
	if ((SOCKET_ERROR == nResCode) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		PerformCloseSocket();

		return;
	}

	return;
}

void CMMSocketClientItem::PerformCloseSocket()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	//close
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	m_vecOverLappedSend.clear();
	m_vecBufferSend.clear();
	m_vecBufferRecv.clear();

	//notify
	if (m_pTCPSocketServer)
	{
		auto pMessage = std::make_shared<M_MMTCPNetworkEngine_Closed>();
		pMessage->dwSocketID = GetSocketID();
		Send(m_pTCPSocketServer, pMessage);
	}

	return;
}

void CMMSocketClientItem::PerformSendCompleted(COverLappedSend *pOverLappedSend, DWORD dwThancferred)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	assert(pOverLappedSend);

	if (INVALID_SOCKET == m_hSocket
		|| NULL == pOverLappedSend
		|| m_vecOverLappedSend.size() <= 0
		|| pOverLappedSend != m_vecOverLappedSend[0]) return;

	m_vecOverLappedSend.erase(m_vecOverLappedSend.begin());

	//send time
	m_dwSendTickCount = GetTickCount();

	//send
	if (m_vecOverLappedSend.size() <= 0) return;

	pOverLappedSend = m_vecOverLappedSend[0];
	int nResCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
	if (0 == nResCode)
	{
		PerformSendCompleted(pOverLappedSend, dwThancferred);

		return;
	}
	if ((SOCKET_ERROR == nResCode) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		PerformCloseSocket();

		return;
	}

	return;
}

void CMMSocketClientItem::PerformRecvCompleted(COverLappedRecv *pOverLappedRecv, DWORD dwThancferred)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (INVALID_SOCKET == m_hSocket) return;

	//recv time
	m_dwRecvTickCount = GetTickCount();
	std::vector<BYTE> vecBuffer;
	vecBuffer.resize(SOCKET_TCP_BUFFER);
	int nResCode = recv(m_hSocket, (char*)vecBuffer.data(), vecBuffer.size(), 0);
	
	//remote close
	if (nResCode <= 0)
	{
		PerformCloseSocket();

		return;
	}

	//adjust
	vecBuffer.resize(nResCode);

	//websocket请求
	if (PerformWSAnswer(vecBuffer)) return;

	//websocket头
	if (m_bWebsocketClient)
	{
		std::vector<BYTE> vecData;
		if (false == PerformParseWSFrame(vecBuffer, vecData))
		{
			PerformCloseSocket();
			
			return;
		}

		m_vecBufferRecv.insert(m_vecBufferRecv.end(), vecData.begin(), vecData.end());
	}
	else
	{
		m_vecBufferRecv.insert(m_vecBufferRecv.end(), vecBuffer.begin(), vecBuffer.end());
	}

	//parse
	try
	{
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
				m_dwRecvKey = TCPHead.dwReserve;
			}

			//remove
			m_vecBufferRecv.erase(m_vecBufferRecv.begin(), m_vecBufferRecv.begin() + TCPHead.wPacketSize);

			//decrypt
			if (0 != m_dwRecvKey)
			{
				m_bWebsocketClient ? DecryptBufferWS(vecData) : DecryptBuffer(vecData);
			}

			m_dwRecvPacketCount++;

			//command
			if (MDM_Engine_Core == TCPHead.wMainCmd)
			{
				switch (TCPHead.wSubCmd)
				{
					case SUB_ToS_Validate:
					{
						PerformSendData(MDM_Engine_Core, SUB_ToC_Validate, NULL, NULL);

						break;
					}
					default:
					{
						throw _T("非法命令码");
					}
				}

				continue;
			}

			//notify
			auto pMessage = std::make_shared<M_MMSocketNotify_Read>();
			pMessage->dwSocketID = GetSocketID();
			pMessage->wMainCmd = TCPHead.wMainCmd;
			pMessage->wSubCmd = TCPHead.wSubCmd;
			pMessage->vecData = vecData;
			Send(m_pTCPSocketServer, pMessage);
		}
	}
	catch (LPCTSTR lpszException)
	{
		PerformCloseSocket();
		return;
	}
	catch (...)
	{
		PerformCloseSocket();
		return;
	}

	return PerformRecvData();
}

COverLappedSend * CMMSocketClientItem::ProductSendOverLapped()
{
	try
	{
		m_vecOverLappedSend.push_back(new COverLappedSend());
		return m_vecOverLappedSend.back();
	}
	catch (...) {}

	return NULL;
}

bool CMMSocketClientItem::PerformParseWSFrame(std::vector<BYTE> vecSrc, std::vector<BYTE> &vecBuffer)
{
	try
	{
		if (vecSrc.size() <= 0) return false;

		//解析包头
		WebSocketStreamHeader WSHeader = {};

		const unsigned char *buf = vecSrc.data();
		WSHeader.fin = buf[0] & 0x80;
		WSHeader.masked = buf[1] & 0x80;
		unsigned char stream_size = buf[1] & 0x7F;

		WSHeader.opcode = buf[0] & 0x0F;
		if (WSHeader.opcode == WS_FrameType::WS_CONTINUATION_FRAME)
		{
			//连续帧  
			return false;
		}
		else if (WSHeader.opcode == WS_FrameType::WS_TEXT_FRAME)
		{
			//文本帧  
		}
		else if (WSHeader.opcode == WS_FrameType::WS_BINARY_FRAME)
		{
			//二进制帧
		}
		else if (WSHeader.opcode == WS_FrameType::WS_CLOSING_FRAME)
		{
			//连接关闭消息  
			return false;
		}
		else if (WSHeader.opcode == WS_FrameType::WS_PING_FRAME)
		{
			//  ping  
			return false;
		}
		else if (WSHeader.opcode == WS_FrameType::WS_PONG_FRAME)
		{
			// pong  
			return false;
		}
		else
		{
			//非法帧  
			return false;
		}

		if (stream_size <= 125)
		{
			//  small stream  
			WSHeader.header_size = 6;
			WSHeader.payload_size = stream_size;
			WSHeader.mask_offset = 2;
		}
		else if (stream_size == 126)
		{
			//  medium stream   
			WSHeader.header_size = 8;
			unsigned short s = 0;
			memcpy(&s, (const char*)&buf[2], 2);
			WSHeader.payload_size = ntohs(s);
			WSHeader.mask_offset = 4;
		}
		else if (stream_size == 127)
		{
			unsigned long long l = 0;
			memcpy(&l, (const char*)&buf[2], 8);

			WSHeader.payload_size = l;
			WSHeader.mask_offset = 10;
		}
		else
		{
			//Couldnt decode stream size 非法大小数据包  
			return false;
		}

		/*  if (header->payload_size > MAX_WEBSOCKET_BUFFER) {
		return false;
		} */

		//解析数据
		const unsigned char *final_buf = vecSrc.data();
		if (vecSrc.size() < WSHeader.header_size + 1)
		{
			return false;
		}

		char masks[4] = {};
		memcpy(masks, final_buf + WSHeader.mask_offset, 4);
		vecBuffer.assign(final_buf + WSHeader.mask_offset + 4, final_buf + WSHeader.mask_offset + 4 + WSHeader.payload_size);

		for (INT_PTR i = 0; i < vecBuffer.size(); ++i)
		{
			vecBuffer[i] = (vecBuffer[i] ^ masks[i % 4]);
		}

		//如果是文本包，在数据最后加一个结束字符“\0”
		if (WSHeader.opcode == WS_FrameType::WS_TEXT_FRAME)
			vecBuffer.push_back('\0');

		return true;
	}
	catch (...)
	{
		return false;
	}
}

//websocket打包
bool CMMSocketClientItem::PerformPackWSFrame(std::vector<BYTE> vecSrc, std::vector<BYTE> &vecBuffer)
{
	try
	{
		if (vecSrc.size() > 32767)
		{
			// 暂不支持这么长的数据  
			return false;
		}

		uint8_t payloadFieldExtraBytes = (vecSrc.size() <= 0x7d) ? 0 : 2;
		// header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节  
		uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
		vector<uint8_t> vecFrameHeader;
		vecFrameHeader.resize(frameHeaderSize);

		// fin位为1, 扩展位为0, 操作位为frameType  
		vecFrameHeader[0] = static_cast<uint8_t>(0x80 | WS_BINARY_FRAME);

		// 填充数据长度
		if (vecSrc.size() <= 0x7d)
		{
			vecFrameHeader[1] = static_cast<uint8_t>(vecSrc.size());
		}
		else
		{
			vecFrameHeader[1] = 0x7e;
			uint16_t len = htons(vecSrc.size());
			memcpy(&vecFrameHeader[2], &len, payloadFieldExtraBytes);
		}

		// 填充数据  
		vecBuffer.assign(vecFrameHeader.begin(), vecFrameHeader.end());
		vecBuffer.insert(vecBuffer.end(), vecSrc.begin(), vecSrc.end());

		return true;
	}
	catch (...)
	{
		return false;
	}
}

//websocket回复
bool CMMSocketClientItem::PerformWSAnswer(std::vector<BYTE> vecWSHead)
{
	try
	{
		if (0 != m_dwRecvPacketCount || vecWSHead.size() <= 0) return false;

		string strRequest = (char*)vecWSHead.data();

		//是否websocket请求
		size_t i = strRequest.find("GET");
		if (i == std::string::npos) return false;

		//组装回复
		string strResult;

		//得到客户端请求信息的key
		std::string tempKey = strRequest;
		i = tempKey.find("Sec-WebSocket-Key");
		if (i == std::string::npos) return false;

		tempKey = tempKey.substr(i + 19, 24);

		//拼接协议返回给客户端
		strResult = "HTTP/1.1 101 Switching Protocols\r\n";
		strResult += "Connection: upgrade\r\n";
		strResult += "Sec-WebSocket-Accept: ";

		std::string realKey = tempKey;
		realKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		SHA1 sha;
		unsigned int message_digest[5];
		sha.Reset();
		sha << realKey.c_str();
		sha.Result(message_digest);
		for (int i = 0; i < 5; i++)
		{
			message_digest[i] = htonl(message_digest[i]);
		}
		realKey = CMMService::EncryptBase64(reinterpret_cast<const unsigned char*>(message_digest), 20);
		realKey += "\r\n";
		strResult += realKey.c_str();
		strResult += "Upgrade: websocket\r\n\r\n";

		if (strResult.length() <= 0) return false;

		//send
		COverLappedSend *pOverLappedSend = ProductSendOverLapped();
		if (NULL == pOverLappedSend) return false;

		int nDataSize = min(SOCKET_TCP_BUFFER, strResult.size());
		CopyMemory(pOverLappedSend->m_cbBuffer, strResult.data(), nDataSize);
		pOverLappedSend->m_WSABuffer.len = nDataSize;

		DWORD dwThancferred = 0;
		int nResCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
		if (0 == nResCode)
		{
			PerformSendCompleted(pOverLappedSend, dwThancferred);

			return true;
		}
		if ((SOCKET_ERROR == nResCode) && (WSA_IO_PENDING != WSAGetLastError()))
		{
			PerformCloseSocket();

			return false;
		}

		m_dwRecvPacketCount++;
		m_bWebsocketClient = true;

		PerformRecvData();

		return true;
	}
	catch (...)
	{
		return false;
	}
}

void CMMSocketClientItem::EncryptBuffer(std::vector<BYTE> &vecBuffer)
{
	if (vecBuffer.empty()) return;

	vecBuffer = CMMEncrypt::MapEncrypt(vecBuffer, m_dwSendKey);

	return;
}

void CMMSocketClientItem::DecryptBuffer(std::vector<BYTE> &vecBuffer)
{
	if (vecBuffer.empty()) return;

	vecBuffer = CMMEncrypt::MapDecrypt(vecBuffer, m_dwRecvKey);

	return;
}

void CMMSocketClientItem::EncryptBufferWS(std::vector<BYTE> &vecBuffer)
{
	////效验参数
	//ASSERT(wDataSize >= sizeof(TCP_Head));
	//ASSERT(wDataSize <= (SOCKET_TCP_BUFFER));
	//ASSERT(wBufferSize >= (wDataSize + 2 * sizeof(DWORD)));

	////效验码与字节映射
	//for (WORD i = sizeof(TCP_Info); i < wDataSize; i++)
	//{
	//	pcbDataBuffer[i] = g_SendByteMap[pcbDataBuffer[i]];
	//}

	////填写信息头
	//TCP_Head * pHead = (TCP_Head *)pcbDataBuffer;
	//pHead->TCPInfo.cbCheckCode = 0;
	//pHead->TCPInfo.wPacketSize = wDataSize;
	//pHead->TCPInfo.cbDataKind = SOCKET_VER;

	////设置变量
	//m_dwSendPacketCount++;

	////打包websocket
	//BYTE cbDataBuffer[SOCKET_TCP_BUFFER] = {};
	//WSEncodeFrame(pcbDataBuffer, wDataSize, cbDataBuffer, sizeof(cbDataBuffer), (DWORD&)wDataSize);
	//CopyMemory(pcbDataBuffer, cbDataBuffer, wDataSize);

	return;
}

void CMMSocketClientItem::DecryptBufferWS(std::vector<BYTE> &vecBuffer)
{
	////效验参数
	//ASSERT(wDataSize >= sizeof(TCP_Head));
	//ASSERT(((TCP_Head *)pcbDataBuffer)->TCPInfo.wPacketSize == wDataSize);

	////效验码与字节映射
	//TCP_Head * pHead = (TCP_Head *)pcbDataBuffer;

	//BYTE cbCheckCode = pHead->TCPInfo.cbCheckCode;

	//for (WORD i = sizeof(TCP_Info); i < wDataSize; i++)
	//{
	//	pcbDataBuffer[i] = (255 & g_RecvByteMap[pcbDataBuffer[i]]);
	//}

	//return wDataSize;
}
