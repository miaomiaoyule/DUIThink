#include "stdafx.h"
#include "MMHash.h"

//////////////////////////////////////////////////////////////////////////
uint32_t CMMHash::GetHash(CMMString &str)
{
	return MM_Fnv1a_append_lpctstr(MM_FNV_offset_basis, str.c_str(), (uint32_t)str.length());
}

uint32_t CMMHash::GetHash(LPCSTR lpszStr)
{
	if (NULL == lpszStr) return MM_FNV_offset_basis;

	return MM_Fnv1a_append_bytes(MM_FNV_offset_basis, (const unsigned char*)lpszStr, sizeof(CHAR) * (uint32_t)strlen(lpszStr));
}

uint32_t CMMHash::GetHash(LPCTSTR lpszStr)
{
	if (NULL == lpszStr) return MM_FNV_offset_basis;

	return MM_Fnv1a_append_lpctstr(MM_FNV_offset_basis, lpszStr, (uint32_t)lstrlen(lpszStr));
}

uint32_t CMMHash::GetHash(const std::vector<CMMString> &vecStr)
{
	CMMString strText;
	for (auto &strResName : vecStr)
	{
		strText += strResName;
		strText += _T(';');
	}

	return CMMHash::GetHash(strText);
}