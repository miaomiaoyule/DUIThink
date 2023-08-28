#ifndef __MM_TCPNETWORKENGINE_H__
#define __MM_TCPNETWORKENGINE_H__

//////////////////////////////////////////////////////////////////////////
#include <Comutil.h>
#include <ICrsint.h>
#include <Process.h>
#include <Wininet.h>
#include <WinSock2.h>

//////////////////////////////////////////////////////////////////////////
enum WS_Status
{
	WS_STATUS_CONNECT = 0,
	WS_STATUS_UNCONNECT = 1,
};

enum WS_FrameType
{
	WS_EMPTY_FRAME = 0xF0,
	WS_ERROR_FRAME = 0xF1,
	WS_CONTINUATION_FRAME = 0x00,
	WS_TEXT_FRAME = 0x01,
	WS_BINARY_FRAME = 0x02,
	WS_PING_FRAME = 0x09,
	WS_PONG_FRAME = 0x0A,
	WS_OPENING_FRAME = 0xF3,
	WS_CLOSING_FRAME = 0x08
};

//////////////////////////////////////////////////////////////////////////
//Websocket数据包数据头信息
struct WebSocketStreamHeader
{
	unsigned int						header_size;						//数据包头大小
	int									mask_offset;						//掩码偏移
	unsigned int						payload_size;						//数据大小
	bool								fin;								//帧标记
	bool								masked;								//掩码
	unsigned char						opcode;								//操作码
	unsigned char						res[3];
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMSocketReadWrite
	: public CMMServiceItem
{
public:
	CMMSocketReadWrite(CMMTCPSocketServer *pTCPNetworkEngine, CMMThreadPool *pOwnerPool);
	virtual ~CMMSocketReadWrite(void);

	//variant
protected:
	std::recursive_mutex				m_DataLock;
	bool								m_bStop = false;
	HANDLE								m_hCompletionPort = NULL;
	CMMTCPSocketServer *				m_pTCPNetworkEngine = NULL;

	//method
public:
	bool UnInit();
	bool IsStop();

	//override
protected:
	void OnMessage(PtrMMServiceMsg pMessage);
	void OnSubInit(shared_ptr<M_MMSocketReadWrite_Init> pRecvData);
};

typedef std::vector<CMMSocketReadWrite*> VecSocketReadWrite;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMSocketAccept
	: public CMMServiceItem
{
public:
	CMMSocketAccept(CMMTCPSocketServer *pTCPNetworkEngine, CMMThreadPool *pOwnerPool);
	virtual ~CMMSocketAccept(void);

	//variant
protected:
	std::recursive_mutex				m_DataLock;
	bool								m_bStop = false;
	SOCKET								m_hSocketListen = INVALID_SOCKET;
	HANDLE								m_hCompletionPort = NULL;	
	CMMTCPSocketServer *				m_pTCPNetworkEngine = NULL;

	//method
public:
	bool UnInit();
	bool IsStop();

	//override
protected:	
	void OnMessage(PtrMMServiceMsg pMessage);
	void OnSubInit(shared_ptr<M_MMSocketAccept_Init> pRecvData);
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMSocketNotify
	: public CMMServiceItem
{
public:
	CMMSocketNotify(CMMTCPSocketServer *pTCPNetworkEngine, CMMThreadPool *pOwnerPool);
	virtual ~CMMSocketNotify(void);

	//variant
protected:
	std::recursive_mutex				m_DataLock;
	bool								m_bStop = false;
	CMMTCPSocketServer *				m_pTCPNetworkEngine = NULL;
	VecTCPSocketServerAttemperSink		m_vecTCPNetworkAttemperSink;

	//method
public:
	bool UnInit();
	bool IsStop();
	void AddTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink);
	void RemoveTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink);

	//override
protected:
	void OnMessage(PtrMMServiceMsg pMessage);
	void OnSubBind(shared_ptr<M_MMSocketNotify_Bind> pRecvData);
	void OnSubShut(shared_ptr<M_MMSocketNotify_Shut> pRecvData);
	void OnSubRead(shared_ptr<M_MMSocketNotify_Read> pRecvData);
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMTCPSocketServer
	: public IMMTCPSocketServer
	, public CMMServiceItem
{
	friend class CMMSocketAccept;

public:
	CMMTCPSocketServer(bool	bEncrypt = false);
	virtual ~CMMTCPSocketServer(void);

	//variant
protected:
	std::recursive_mutex				m_DataLock;
	bool								m_bService = false;
	bool								m_bEncrypt = false;
	WORD								m_wListenPort = 0;
	WORD								m_wLinkCountLimit = 0;
	SOCKET								m_hSocketServer = INVALID_SOCKET;
	HANDLE								m_hCompletionPort = NULL;
	CMMString							m_strLastError;

	//work
protected:
	CMMSocketAccept						m_SocketAcceptWork;
	CMMSocketNotify						m_SocketNotifyWork;
	VecSocketReadWrite					m_vecSocketReadWriteWork;		
	VecSocketClientItem					m_vecActiveSocketItem;
	CMMThreadPool						m_ThreadPool;

	//method
public:
	LPVOID QueryInterface(const IID & Guid, DWORD dwQueryVer);
	void Release() { delete this; }

	//service
public:
	bool StartService(WORD wServicePort, WORD wLinkCountLimit);
	bool ConcludeService();
	bool IsServecing();
	WORD GetListenPort();
	WORD GetLinkLimit();
	WORD GetLinkCount();
	CMMString GetLastError();

	//socket
public:
	bool PerformCloseSocket(DWORD dwSocketID);
	bool PerformAllowBatch(DWORD dwSocketID, bool bAllowBatch, BYTE cbMaskBatch);

	//notify
public:
	void AddTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink);
	void RemoveTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink);

	//send
public:
	bool SendData(DWORD dwSocketID, WORD wMainCmd, WORD wSubCmd);
	bool SendData(DWORD dwSocketID, WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize);
	bool SendDataBatch(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize, BYTE cbMaskBatch);

	//messsage
protected:
	void OnMessage(PtrMMServiceMsg pMessage);
	void OnSubAccept(shared_ptr<M_MMTCPNetworkEngine_Accept> pRecvData);
	void OnSubClosed(shared_ptr<M_MMTCPNetworkEngine_Closed> pRecvData);
	void OnSubRecvComplete(shared_ptr<M_MMTCPNetworkEngine_RecvComplete> pRecvData);
	void OnSubSendComplete(shared_ptr<M_MMTCPNetworkEngine_SendComplete> pRecvData);

	void OnTimerDetect();

	//help
protected:
	CMMSocketClientItem * FindClientItem(DWORD dwSocketID);
	bool ValidClientItem(CMMSocketClientItem *pClientItem);
};

//////////////////////////////////////////////////////////////////////////

#endif