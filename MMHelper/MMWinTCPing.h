#ifndef __MM_WINTCPING_H__
#define __MM_WINTCPING_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMWinTCPing
{
public:
	CMMWinTCPing();
	~CMMWinTCPing();

protected:
	SOCKET								m_hSocket = INVALID_SOCKET;
	WSAEVENT							m_hEvent = NULL;

	//method
public:
	bool Ping(const char *szDestIP, WORD wPort, DWORD dwTimeout = 2000);
	bool Ping(DWORD dwServerIP, WORD wPort, DWORD dwTimeout = 2000);
};

//////////////////////////////////////////////////////////////////////////

#endif