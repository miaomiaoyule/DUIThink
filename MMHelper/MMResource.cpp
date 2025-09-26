#include "StdAfx.h"
#include "MMCommandLine.h"

//////////////////////////////////////////////////////////////////////////////////
CMMResource::CMMResource()
{
	return;
}

CMMResource::~CMMResource()
{
	return;
}

bool CMMResource::Load(IN HMODULE hModule, IN CMMString strResType, IN LPCTSTR lpszResName, OUT std::vector<BYTE> &vecData)
{
	//load res
	HRSRC hResource = ::FindResource(hModule, lpszResName, strResType);
	if (NULL == hResource) return false;

	DWORD dwSize = ::SizeofResource(hModule, hResource);
	if (0 == dwSize) return false;

	HGLOBAL hGlobal = ::LoadResource(hModule, hResource);
	if (NULL == hGlobal) return false;

	vecData.resize(dwSize);
	::CopyMemory(vecData.data(), (LPBYTE)::LockResource(hGlobal), dwSize);
	::FreeResource(hGlobal);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
