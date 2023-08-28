#include "StdAfx.h"
#include "Interface.h"

//////////////////////////////////////////////////////////////////////////
LPVOID IControlListen ::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IControlListen, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
LPVOID IDUIWndNotify::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDUIWndNotify, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////