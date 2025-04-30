#include "StdAfx.h"
#include "Interface.h"

//////////////////////////////////////////////////////////////////////////
LPVOID IControlListen ::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IControlListen, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
LPVOID IDuiWndNotify::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDuiWndNotify, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////