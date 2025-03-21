#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_SOCKETCLIENTITEM_H__
#define __MM_SOCKETCLIENTITEM_H__

#pragma once

class CMMTCPSocketServer;

//////////////////////////////////////////////////////////////////////////
enum enOperationType
{
	OperationType_Send,					//��������
	OperationType_Recv,					//��������
};

//////////////////////////////////////////////////////////////////////////
class COverLapped
{
public:
	COverLapped(enOperationType OperationType);
	virtual ~COverLapped();

	//variant
public:
	WSABUF								m_WSABuffer = {};
	OVERLAPPED							m_OverLapped = {};
	const enOperationType				m_OperationType;
	BYTE								m_cbBuffer[SOCKET_TCP_BUFFER] = {};

	//��Ϣ����
public:
	//��ȡ����
	enOperationType GetOperationType() { return m_OperationType; }
};

class COverLappedSend : public COverLapped
{
public:
	COverLappedSend();
	virtual ~COverLappedSend();
};

class COverLappedRecv : public COverLapped
{
public:
	COverLappedRecv();
	virtual ~COverLappedRecv();
};

typedef std::vector<COverLappedSend*> VecOverLappedSend;
typedef std::vector<COverLappedRecv> VecOverLappedRecv;

//////////////////////////////////////////////////////////////////////////
//client item
class MMHELPER_API CMMSocketClientItem
	: public CMMServiceItem
{
public:
	CMMSocketClientItem(SOCKET hSocket, DWORD dwClientAddr, CMMThreadPool *pOwnerPool, CMMTCPSocketServer *pTCPSocketServer, bool bEncrypt = false);
	virtual ~CMMSocketClientItem(void);

	//variant
protected:
	std::recursive_mutex				m_DataLock;
	SOCKET								m_hSocket = INVALID_SOCKET;
	DWORD								m_dwSocketID = INVALID_DWORD;
	DWORD								m_dwClientAddr = 0;
	DWORD								m_dwConnectTime = 0;
	COverLappedRecv						m_OverLappedRecv;
	VecOverLappedSend					m_vecOverLappedSend;
	CMMTCPSocketServer *				m_pTCPSocketServer = NULL;

	//variant
protected:
	bool								m_bEncrypt = false;
	bool								m_bAllowBatch = true;
	bool								m_bWebsocketClient = false;				//websocket����
	BYTE								m_cbMaskBatch = MaskClient_All;
	std::vector<BYTE>					m_vecBufferSend;
	std::vector<BYTE>					m_vecBufferRecv;

	//variant
protected:
	DWORD								m_dwSendKey = 0;
	DWORD								m_dwRecvKey = 0;
	DWORD								m_dwSendTickCount = 0;
	DWORD								m_dwRecvTickCount = 0;
	DWORD								m_dwSendPacketCount = 0;
	DWORD								m_dwRecvPacketCount = 0;

	//override
public:
	bool UnInit();

	//message
protected:
	void OnMessage(PtrMMServiceMsg pMessage);
	void OnSubWork(shared_ptr<M_MMSocketClientItem_Work> pRecvData);
	void OnSubRecvComplete(shared_ptr<M_MMSocketClientItem_RecvComplete> pRecvData);
	void OnSubSendComplete(shared_ptr<M_MMSocketClientItem_SendComplete> pRecvData);
	void OnSubSendData(shared_ptr<M_MMSocketClientItem_SendData> pRecvData);
	void OnSubAllowBatch(shared_ptr<M_MMSocketClientItem_AllowBatch> pRecvData);
	void OnSubClose(shared_ptr<M_MMSocketClientItem_Close> pRecvData);

	//method
public:
	DWORD GetSocketID();
	DWORD GetClientAddr();
	DWORD GetConnectTime();
	DWORD GetSendTickCount();
	DWORD GetRecvTickCount();
	DWORD GetSendPacketCount();
	DWORD GetRecvPacketCount();
	BYTE GetMaskBatch();
	bool IsReadySend();
	bool IsAllowBatch();

	//help
protected:
	bool PerformSendData(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize);
	void PerformRecvData();
	void PerformCloseSocket();
	void PerformSendCompleted(COverLappedSend *pOverLappedSend, DWORD dwThancferred);
	void PerformRecvCompleted(COverLappedRecv *pOverLappedRecv, DWORD dwThancferred);

	//�ڲ�����
private:
	void EncryptBuffer(std::vector<BYTE> &vecBuffer);
	void DecryptBuffer(std::vector<BYTE> &vecBuffer);
	void EncryptBufferWS(std::vector<BYTE> &vecBuffer);
	void DecryptBufferWS(std::vector<BYTE> &vecBuffer);

	//��������
private:
	COverLappedSend * ProductSendOverLapped();
	bool PerformParseWSFrame(std::vector<BYTE> vecSrc, std::vector<BYTE> &vecBuffer);
	bool PerformPackWSFrame(std::vector<BYTE> vecSrc, std::vector<BYTE> &vecBuffer);
	bool PerformWSAnswer(std::vector<BYTE> vecWSHead);
};

typedef std::vector<CMMSocketClientItem *> VecSocketClientItem;

//////////////////////////////////////////////////////////////////////////

#endif
#endif