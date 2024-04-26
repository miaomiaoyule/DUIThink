#include "StdAfx.h"
#include "MMEncrypt.h"

//////////////////////////////////////////////////////////////////////////////////
//密钥长度
#define ENCRYPT_KEY_LEN				8									//密钥长度

//////////////////////////////////////////////////////////////////////////
inline WORD SeedRandMap(WORD wSeed)
{
	DWORD dwHold = wSeed;
	return (WORD)((dwHold = dwHold * 241103L + 2533101L) >> 16);
}

//////////////////////////////////////////////////////////////////////////////////
CMMEncrypt::CMMEncrypt()
{
}

CMMEncrypt::~CMMEncrypt()
{
}

//生成密文
CMMString CMMEncrypt::XorEncrypt(CMMString strSrc)
{
	//变量定义
	WCHAR szEncrypData[MAX_ENCRYPT_LEN + 1] = L"";

	//生成密钥
	WORD wRandKey[ENCRYPT_KEY_LEN];
	wRandKey[0] = lstrlenW(strSrc);
	for (WORD i = 1; i < MMCountArray(wRandKey); i++) wRandKey[i] = rand() % 0xFFFF;

	//步骤准备
	WORD wTempCode = 0;
	WORD wTimes = ((wRandKey[0] + ENCRYPT_KEY_LEN - 1) / ENCRYPT_KEY_LEN)*ENCRYPT_KEY_LEN;

	//生成密文
	for (WORD i = 0; i < wTimes; i++)
	{
		if (i < wRandKey[0]) wTempCode = strSrc[i] ^ wRandKey[i%ENCRYPT_KEY_LEN];
		else wTempCode = wRandKey[i%ENCRYPT_KEY_LEN] ^ (WORD)(rand() % 0xFFFF);
		swprintf(szEncrypData + i * 8, L"%04X%04X", wRandKey[i%ENCRYPT_KEY_LEN], wTempCode);
	}

	return szEncrypData;
}

//解开密文
CMMString CMMEncrypt::XorDecrypt(CMMString strSrc)
{
	//变量定义
	WCHAR szSrcData[MAX_SOURCE_LEN] = L"";

	//效验长度
	WORD wEncrypPassLen = lstrlenW(strSrc);
	if (wEncrypPassLen < ENCRYPT_KEY_LEN * 8) return _T("");

	//提取长度
	WCHAR szTempBuffer[5] = L"";
	szTempBuffer[MMCountArray(szTempBuffer) - 1] = 0;
	CopyMemory(szTempBuffer, (LPCWSTR)strSrc, sizeof(WCHAR) * 4);

	//获取长度
	WCHAR * pEnd = NULL;
	WORD wSoureLength = (WORD)wcstol(szTempBuffer, &pEnd, 16);

	//长度效验
	assert(wEncrypPassLen == (((wSoureLength + ENCRYPT_KEY_LEN - 1) / ENCRYPT_KEY_LEN)*ENCRYPT_KEY_LEN * 8));
	if (wEncrypPassLen != (((wSoureLength + ENCRYPT_KEY_LEN - 1) / ENCRYPT_KEY_LEN)*ENCRYPT_KEY_LEN * 8)) return _T("");

	//解开密码
	for (INT i = 0; i < wSoureLength; i++)
	{
		//获取密钥
		WCHAR szKeyBuffer[5];
		szKeyBuffer[MMCountArray(szKeyBuffer) - 1] = 0;
		szTempBuffer[MMCountArray(szTempBuffer) - 1] = 0;
		CopyMemory(szKeyBuffer, (LPCWSTR)strSrc + i * 8, sizeof(WCHAR) * 4);
		CopyMemory(szTempBuffer, (LPCWSTR)strSrc + i * 8 + 4, sizeof(WCHAR) * 4);

		//提取密钥
		WCHAR wKey = (WCHAR)wcstol(szKeyBuffer, &pEnd, 16);
		WCHAR wEncrypt = (WCHAR)wcstol(szTempBuffer, &pEnd, 16);

		//生成原文
		szSrcData[i] = (WCHAR)((WCHAR)wKey ^ (WCHAR)wEncrypt);
	}

	//终止字符
	szSrcData[wSoureLength] = 0;

	return szSrcData;
}

CMMString CMMEncrypt::MapEncrypt(CMMString strSrc)
{
	if (strSrc.empty()) return _T("");

	//变量定义
	BYTE * pDataSrc = (BYTE *)strSrc.c_str();

	for (UINT i = 0; i < strSrc.length() * sizeof(TCHAR); i++)
	{
		pDataSrc[i] = g_cbEncryptMap[pDataSrc[i]];
	}

	return strSrc;
}

std::vector<BYTE> CMMEncrypt::MapEncrypt(std::vector<BYTE> vecData)
{
	for (int n = 0; n < vecData.size(); n++)
	{
		vecData[n] = g_cbEncryptMap[vecData[n]];
	}

	return vecData;
}

std::vector<BYTE> CMMEncrypt::MapEncrypt(std::vector<BYTE> vecData, DWORD dwMapKey)
{
	for (int n = 0; n < vecData.size(); n++)
	{
		vecData[n] = g_cbEncryptMap[vecData[n]];
	}
	
	if (vecData.empty()) return vecData;

	//加密数据
	DWORD dwKey = dwMapKey;
	WORD *pSeed = (WORD *)(vecData.data());
	DWORD *pEncrypt = (DWORD *)(vecData.data());
	WORD wCount = (vecData.size()) / sizeof(DWORD);
	for (int i = 0; i < wCount; i++)
	{
		*pEncrypt++ ^= dwKey;
		dwKey = SeedRandMap(*pSeed++);
		dwKey |= ((DWORD)SeedRandMap(*pSeed++)) << 16;
		dwKey ^= dwMapKey;
	}

	return vecData;
}

CMMString CMMEncrypt::MapDecrypt(CMMString strSrc)
{
	if (strSrc.empty()) return _T("");

	//变量定义
	BYTE * pDataSrc = (BYTE *)strSrc.c_str();

	//解密数据
	for (UINT i = 0; i < strSrc.length() * sizeof(TCHAR); i++)
	{
		pDataSrc[i] = g_cbDecryptMap[pDataSrc[i]];
	}

	return strSrc;
}

std::vector<BYTE> CMMEncrypt::MapDecrypt(std::vector<BYTE> vecData)
{
	for (int i = 0; i < vecData.size(); ++i)
	{
		vecData[i] = g_cbDecryptMap[vecData[i]];
	}

	return vecData;
}

std::vector<BYTE> CMMEncrypt::MapDecrypt(std::vector<BYTE> vecData, DWORD dwMapKey)
{
	if (vecData.empty()) return vecData;

	DWORD dwKey = dwMapKey;
	DWORD dwDecryptKey = dwMapKey;
	WORD *pSeed = (WORD*)vecData.data();
	DWORD *pDecrypt = (DWORD*)vecData.data();
	WORD wCount = (WORD)(vecData.size() / sizeof(DWORD));
	for (WORD i = 0; i < wCount; i++)
	{
		dwKey = SeedRandMap(*pSeed++);
		dwKey |= ((DWORD)SeedRandMap(*pSeed++)) << 16;
		dwKey ^= dwMapKey;
		*pDecrypt++ ^= dwDecryptKey;
		dwDecryptKey = dwKey;
	}

	for (int i = 0; i < vecData.size(); ++i)
	{
		vecData[i] = g_cbDecryptMap[vecData[i]];
	}

	return vecData;
}

//////////////////////////////////////////////////////////////////////////////////
