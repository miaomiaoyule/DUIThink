#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_TCPSOCKETCLIENT_H__
#define __MM_TCPSOCKETCLIENT_H__

#pragma once

#include "Define.h"
#include "WinSock2.h"

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMTCPSocketClient
	: public CMMAsyncObject
	, public IMMTCPSocketClient
{
	struct tagData;

public:
	CMMTCPSocketClient(bool bEncrypt = false);
	virtual ~CMMTCPSocketClient();

	//socket
protected:
	std::recursive_mutex				m_DataLock;
	WORD								m_wSocketID = INVALID_WORD;
	SOCKET								m_hSocket = INVALID_SOCKET;
	enSocketStatus						m_SocketStatus = SocketStatus_Idle;
	VecMMTCPSocketClientSink			m_vecTCPSocketSink;
	VecSocketNotifyTask					m_vecSocketNotifyTask;

	//buffer
protected:
	bool								m_bEncrypt = false;
	tagData *							m_pData = NULL;
	std::vector<BYTE>					m_vecBufferSend;
	std::vector<BYTE>					m_vecBufferRecv;

	//count
protected:
	DWORD								m_dwSendTickCount = 0;
	DWORD								m_dwRecvTickCount = 0;
	DWORD								m_dwSendPacketCount = 0;
	DWORD								m_dwRecvPacketCount = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	void Release() { delete this; }

	//info
public:
	bool Init();
	bool UnInit();
	WORD GetSocketID();
	bool AddTCPSocketSink(IMMTCPSocketClientAttemperSink *pIAttemperSink);
	void RemoveTCPSocketSink(IMMTCPSocketClientAttemperSink *pIAttemperSink);
	enSocketStatus GetSocketStatus();
	DWORD GetLastSendTick();
	DWORD GetLastRecvTick();
	DWORD GetSendPacketCount();
	DWORD GetRecvPacketCount();

	//function
public:
	void Connect(DWORD dwServerIP, WORD wPort);
	void Connect(LPCTSTR lpszServerIP, WORD wPort);
	bool SendData(WORD wMainCmd, WORD wSubCmd);
	bool SendData(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize);
	void CloseSocket(enSocketShutReason ShutReason = SocketShut_Normal);

	//help
protected:
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
	
	//help
private:
	bool VerifyTask();
	void PerformActiveSend();
	void PerformActiveNotify();
	CMMString TransConnectErrorCode(int nErrorCode);
	void AddNotifyTask(enSocketConnectCode ConnectCode, LPCTSTR lpszDescribe);
	void AddNotifyTask(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize);
	void AddNotifyTask(enSocketShutReason ShutReason);

	//help
private:
	void SendSocketConnect(enSocketConnectCode ConnectCode, LPCTSTR lpszDescribe);
	void SendSocketRead(WORD wMainCmd, WORD wSubCmd, void *pData, WORD wDataSize);
	void SendSocketShut(enSocketShutReason ShutReason);

	//message
private:
	LRESULT OnWMSocketEvent(WPARAM wParam, LPARAM lParam);
	LRESULT OnWMSendBuffer(WPARAM wParam, LPARAM lParam);
	LRESULT OnWMSocketNotify(WPARAM wParam, LPARAM lParam);

	//message
private:
	LRESULT OnSubSocketEventConnect(WPARAM wParam, LPARAM lParam);
	LRESULT OnSubSocketEventRead(WPARAM wParam, LPARAM lParam);
	LRESULT OnSubSocketEventClose(WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////

#endif
#endif