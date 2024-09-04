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
LPVOID IDuiPropertyObject::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDuiPropertyObject, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
