#ifndef __MM_ITCPSOCKETSERVER_H__
#define __MM_ITCPSOCKETSERVER_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_IMMTCPSocketServerAttemperSink INTERFACE_VERSION(1,1)
static const GUID IID_IMMTCPSocketServerAttemperSink = { 0x9759ffb3,0x5bec,0x4199,0x0091,0xef,0x49,0x5b,0xca,0xdc,0x00,0x98 };
interface IMMTCPSocketServerAttemperSink : public IMMUnknown
{
	virtual bool OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientAddr) = NULL;
	virtual bool OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientAddr) = NULL;
	virtual bool OnEventTCPNetworkRead(DWORD dwSocketID, WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize) = NULL;
};

typedef std::vector<IMMTCPSocketServerAttemperSink*> VecTCPSocketServerAttemperSink;

//////////////////////////////////////////////////////////////////////////
#define VER_IMMTCPSocketServer INTERFACE_VERSION(1,1)
static const GUID IID_IMMTCPSocketServer = { 0x9aa6931f,0x417f,0x43a7,0x86,0xab,0x56,0x10,0xe4,0x34,0x1c,0x17 };
interface IMMTCPSocketServer : public IMMUnknown
{
	//service
public:
	virtual bool StartService(WORD wServicePort, WORD wLinkCountLimit) = NULL;
	virtual bool ConcludeService() = NULL;
	virtual WORD GetListenPort() = NULL;
	virtual WORD GetLinkLimit() = NULL;
	virtual WORD GetLinkCount() = NULL;
	virtual CMMString GetLastError() = NULL;

	//socket
public:
	virtual bool PerformCloseSocket(DWORD dwSocketID) = NULL;
	virtual bool PerformAllowBatch(DWORD dwSocketID, bool bAllowBatch, BYTE cbMaskBatch) = NULL;

	//notify
public:
	virtual void AddTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink) = NULL;
	virtual void RemoveTCPSocketSink(IMMTCPSocketServerAttemperSink *pIAttemperSink) = NULL;

	//send
public:
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmd, WORD wSubCmd) = NULL;
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize) = NULL;
	virtual bool SendDataBatch(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize, BYTE cbBatchMask) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#endif