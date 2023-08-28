#include "StdAfx.h"
#include "MMModule.h"

//////////////////////////////////////////////////////////////////////////
//产品版本
BYTE GetProductVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[3];
}

//主要版本
BYTE GetMainVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[2];
}

//次要版本
BYTE GetSubVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[1];
}

//次要版本
BYTE GetBuildVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[0];
}

//版本比较
bool InterfaceVersionCompare(DWORD dwQueryVer, DWORD dwInterfaceVer)
{
	if (GetSubVer(dwQueryVer) > GetSubVer(dwInterfaceVer)) return false;
	if (GetMainVer(dwQueryVer) != GetMainVer(dwInterfaceVer)) return false;
	if (GetBuildVer(dwQueryVer) != GetBuildVer(dwInterfaceVer)) return false;
	if (GetProductVer(dwQueryVer) != GetProductVer(dwInterfaceVer)) return false;
	return true;
};

//////////////////////////////////////////////////////////////////////////