#include "stdafx.h"
#include "MMWinTCPing.h"

//////////////////////////////////////////////////////////////////////////
CMMWinTCPing::CMMWinTCPing()
{
	WSADATA WSAData;
	//WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
	{
		/*如果初始化不成功则报错，GetLastError()返回发生的错误信息*/
		printf("WSAStartup() failed: %d\n", GetLastError());
		
		return;
	}

	m_hEvent = WSACreateEvent();
	m_hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	WSAEventSelect(m_hSocket, m_hEvent, FD_READ | FD_CONNECT);

	return;
}

CMMWinTCPing::~CMMWinTCPing()
{
	if (m_hEvent)
	{
		WSAEventSelect(m_hSocket, m_hEvent, NULL);
		WSACloseEvent(m_hEvent);

		m_hEvent = NULL;
	}
	if (INVALID_SOCKET != m_hSocket)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	WSACleanup();

	return;
}

bool CMMWinTCPing::Ping(const char *szDestIP, WORD wPort, DWORD dwTimeout)
{
	if (NULL != szDestIP)
	{
		return Ping(inet_addr(szDestIP), wPort, dwTimeout);
	}

	return false;
}

bool CMMWinTCPing::Ping(DWORD dwServerIP, WORD wPort, DWORD dwTimeout)
{
	try
	{
		//verify
		if (INADDR_NONE == dwServerIP) throw _T("目标服务器地址格式不正确，请检查后再次尝试！");
		if (INVALID_SOCKET == m_hSocket) throw _T("SOCKET 创建失败！");

		DWORD dwTickCount = GetTickCount();

		//address
		SOCKADDR_IN SocketAddr;
		memset(&SocketAddr, 0, sizeof(SocketAddr));
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr = dwServerIP;
		int nAddrLen = sizeof(SocketAddr);

		//发送报文
		if (SOCKET_ERROR == connect(m_hSocket, (SOCKADDR *)&SocketAddr, nAddrLen))
		{
			int nErrorCode = WSAGetLastError();
			if (WSAEWOULDBLOCK != WSAEWOULDBLOCK)
			{
				return false;
			}
		}

		//recv
		do 
		{
			BYTE szBuffer[32] = {};

			//接收响应报文
			if (WSAWaitForMultipleEvents(1, &m_hEvent, FALSE, 100, FALSE) != WSA_WAIT_TIMEOUT)
			{
				WSANETWORKEVENTS netEvent;
				WSAEnumNetworkEvents(m_hSocket, m_hEvent, &netEvent);

				if (netEvent.lNetworkEvents & FD_CONNECT)
				{
					return true;
				}
				if (netEvent.lNetworkEvents & FD_READ)
				{
					int nPacketSize = recvfrom(m_hSocket, (char*)szBuffer, sizeof(szBuffer) / sizeof(BYTE), 0, (SOCKADDR *)&SocketAddr, &nAddrLen);
					if (nPacketSize != SOCKET_ERROR)
					{
						return true;
					}
				}
			}

		} while (GetTickCount() - dwTickCount <= dwTimeout);

		return false;
	}
	catch (LPCTSTR lpszError)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}

	return false;
}