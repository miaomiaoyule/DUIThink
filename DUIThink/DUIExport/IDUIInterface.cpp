#include "StdAfx.h"
#include "IDUIInterface.h"

//////////////////////////////////////////////////////////////////////////
LPVOID IDUIInterface::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDUIInterface, Guid, dwQueryVer);
	QUERYINTERFACE(IMMUnknown, Guid, dwQueryVer);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
LPVOID IDUIPropertyObject::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDUIPropertyObject, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
