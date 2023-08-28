#ifndef __MM_DEFINETCPSOCKETSERVER_H__
#define __MM_DEFINETCPSOCKETSERVER_H__

//////////////////////////////////////////////////////////////////////////////////
#define MDM_Engine_Core					(1)
#define SUB_ToS_Validate				(1)

#define SUB_ToC_Detect					(1000)
#define SUB_ToC_Validate				(1002)

//////////////////////////////////////////////////////////////////////////
class COverLappedRecv;
class COverLappedSend;
class CMMSocketClientItem;

//////////////////////////////////////////////////////////////////////////
struct CMD_KN_DetectSocket
{
	__time64_t							tSendTime = 0;
	__time64_t							tRecvTime = 0;
};

//////////////////////////////////////////////////////////////////////////
struct M_MMSocketReadWrite_Init : public tagMMServiceMsg
{
	HANDLE								hCompletionPort = NULL;
};

//////////////////////////////////////////////////////////////////////////
struct M_MMSocketAccept_Init : public tagMMServiceMsg
{
	SOCKET								hSocketListen = INVALID_SOCKET;
	HANDLE								hCompletionPort = NULL;
};

//////////////////////////////////////////////////////////////////////////
struct M_MMSocketNotify_Bind : public tagMMServiceMsg
{
	DWORD								dwSocketID = 0;
	DWORD								dwClientAddr = 0;
};
struct M_MMSocketNotify_Shut : public tagMMServiceMsg
{
	DWORD								dwSocketID = 0;
	DWORD								dwClientAddr = 0;
};
struct M_MMSocketNotify_Read : public tagMMServiceMsg
{
	DWORD								dwSocketID = 0;
	WORD								wMainCmd = 0;
	WORD								wSubCmd = 0;
	std::vector<BYTE>					vecData;
};

//////////////////////////////////////////////////////////////////////////
struct M_MMTCPNetworkEngine_Accept : public tagMMServiceMsg
{
	SOCKET								hSocket = INVALID_SOCKET;
	ULONG								uClientAddr = 0;
};
struct M_MMTCPNetworkEngine_Closed : public tagMMServiceMsg
{
	DWORD								dwSocketID = 0;
};
struct M_MMTCPNetworkEngine_RecvComplete : public tagMMServiceMsg
{
	CMMSocketClientItem *				pClientItem = NULL;
	COverLappedRecv *					pOverLappedRecv = NULL;
	DWORD								dwThancferred = 0;
};
struct M_MMTCPNetworkEngine_SendComplete : public tagMMServiceMsg
{
	CMMSocketClientItem *				pClientItem = NULL;
	COverLappedSend *					pOverLappedSend = NULL;
	DWORD								dwThancferred = 0;
};

//////////////////////////////////////////////////////////////////////////
struct M_MMSocketClientItem_Work : public tagMMServiceMsg
{
};
struct M_MMSocketClientItem_RecvComplete : public tagMMServiceMsg
{
	COverLappedRecv *					pOverLappedRecv = NULL;
	DWORD								dwThancferred = 0;
};
struct M_MMSocketClientItem_SendComplete : public tagMMServiceMsg
{
	COverLappedSend *					pOverLappedSend = NULL;
	DWORD								dwThancferred = 0;
};
struct M_MMSocketClientItem_SendData : public tagMMServiceMsg
{
	WORD								wMainCmd = 0;
	WORD								wSubCmd = 0;
	std::vector<BYTE>					vecData;
};
struct M_MMSocketClientItem_AllowBatch : public tagMMServiceMsg
{
	bool								bAllow = false;
	BYTE								cbMaskBatch = 0;
};
struct M_MMSocketClientItem_Close : public tagMMServiceMsg
{
};

//////////////////////////////////////////////////////////////////////////////////

#endif