#include "StdAfx.h"
#include "IDuiInterface.h"

//////////////////////////////////////////////////////////////////////////
LPVOID IDuiInterface::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDuiInterface, Guid, dwQueryVer);
	QUERYINTERFACE(IMMUnknown, Guid, dwQueryVer);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
LPVOID IDuiResourceCallBack::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDuiResourceCallBack, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
LPVOID IDuiControlListen::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDuiControlListen, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
LPVOID IDuiWndNotify::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDuiWndNotify, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
