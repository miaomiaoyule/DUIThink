#ifndef __MM_ITCPSOCKETCLIENT_H__
#define __MM_ITCPSOCKETCLIENT_H__

#pragma once

//////////////////////////////////////////////////////////////////////////////////
//tcp notify ui thread
#define VER_IMMTCPSocketClientAttemperSink INTERFACE_VERSION(1,1)
static const GUID IID_IMMTCPSocketClientAttemperSink = { 0xdb0f09dd,0x65e4,0x441f,0x0094,0xb1,0xfe,0x80,0x8c,0x1d,0xf2,0x65 };
interface IMMTCPSocketClientAttemperSink : public IMMUnknown
{
	//notify, return false it will close socket
public:
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) { QUERYINTERFACE(IMMTCPSocketClientAttemperSink, Guid, dwQueryVer); return NULL; }
	virtual bool OnEventTCPSocketLink(WORD wSocketID, enSocketConnectCode ConnectCode, LPCTSTR lpszDescribe) = NULL;
	virtual bool OnEventTCPSocketShut(WORD wSocketID, enSocketShutReason ShutReason) = NULL;
	virtual bool OnEventTCPSocketRead(WORD wSocketID, WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize, bool &bHandled) = NULL;
};

typedef std::vector<IMMTCPSocketClientAttemperSink*> VecMMTCPSocketClientSink;

//////////////////////////////////////////////////////////////////////////
//tcp interface
#define VER_IMMTCPSocketClient INTERFACE_VERSION(1,1)
static const GUID IID_IMMTCPSocketClient = { 0xa870d592,0x447a,0x40bd,0x00ae,0x4e,0x5e,0x64,0xde,0xce,0x22,0xed };
interface IMMTCPSocketClient : public IMMUnknown
{
	//info
public:
	virtual bool Init() = NULL;
	virtual bool UnInit() = NULL;
	virtual WORD GetSocketID() = NULL;
	virtual bool AddTCPSocketSink(IMMTCPSocketClientAttemperSink *pIAttemperSink) = NULL;
	virtual void RemoveTCPSocketSink(IMMTCPSocketClientAttemperSink *pIAttemperSink) = NULL;
	virtual enSocketStatus GetSocketStatus() = NULL;
	virtual DWORD GetLastSendTick() = NULL;
	virtual DWORD GetLastRecvTick() = NULL;
	virtual DWORD GetSendPacketCount() = NULL;
	virtual DWORD GetRecvPacketCount() = NULL;

	//function
public:
	virtual void Connect(DWORD dwServerIP, WORD wPort) = NULL;
	virtual void Connect(LPCTSTR lpszServerIP, WORD wPort) = NULL;
	virtual bool SendData(WORD wMainCmd, WORD wSubCmd) = NULL;
	virtual bool SendData(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize) = NULL;
	virtual void CloseSocket(enSocketShutReason ShutReason = SocketShut_Normal) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#endif