#include "StdAfx.h"
#include "MMEncrypt.h"

//////////////////////////////////////////////////////////////////////////////////
//��Կ����
#define ENCRYPT_KEY_LEN				8									//��Կ����

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

//��������
CMMString CMMEncrypt::XorEncrypt(CMMString strSrc)
{
	//��������
	WCHAR szEncrypData[MAX_ENCRYPT_LEN + 1] = L"";

	//������Կ
	WORD wRandKey[ENCRYPT_KEY_LEN];
	wRandKey[0] = lstrlenW(strSrc);
	for (WORD i = 1; i < MMCountArray(wRandKey); i++) wRandKey[i] = rand() % 0xFFFF;

	//����׼��
	WORD wTempCode = 0;
	WORD wTimes = ((wRandKey[0] + ENCRYPT_KEY_LEN - 1) / ENCRYPT_KEY_LEN)*ENCRYPT_KEY_LEN;

	//��������
	for (WORD i = 0; i < wTimes; i++)
	{
		if (i < wRandKey[0]) wTempCode = strSrc[i] ^ wRandKey[i%ENCRYPT_KEY_LEN];
		else wTempCode = wRandKey[i%ENCRYPT_KEY_LEN] ^ (WORD)(rand() % 0xFFFF);
		swprintf(szEncrypData + i * 8, L"%04X%04X", wRandKey[i%ENCRYPT_KEY_LEN], wTempCode);
	}

	return szEncrypData;
}

//�⿪����
CMMString CMMEncrypt::XorDecrypt(CMMString strSrc)
{
	//��������
	WCHAR szSrcData[MAX_SOURCE_LEN] = L"";

	//Ч�鳤��
	WORD wEncrypPassLen = lstrlenW(strSrc);
	if (wEncrypPassLen < ENCRYPT_KEY_LEN * 8) return _T("");

	//��ȡ����
	WCHAR szTempBuffer[5] = L"";
	szTempBuffer[MMCountArray(szTempBuffer) - 1] = 0;
	CopyMemory(szTempBuffer, (LPCWSTR)strSrc, sizeof(WCHAR) * 4);

	//��ȡ����
	WCHAR * pEnd = NULL;
	WORD wSoureLength = (WORD)wcstol(szTempBuffer, &pEnd, 16);

	//����Ч��
	assert(wEncrypPassLen == (((wSoureLength + ENCRYPT_KEY_LEN - 1) / ENCRYPT_KEY_LEN)*ENCRYPT_KEY_LEN * 8));
	if (wEncrypPassLen != (((wSoureLength + ENCRYPT_KEY_LEN - 1) / ENCRYPT_KEY_LEN)*ENCRYPT_KEY_LEN * 8)) return _T("");

	//�⿪����
	for (INT i = 0; i < wSoureLength; i++)
	{
		//��ȡ��Կ
		WCHAR szKeyBuffer[5];
		szKeyBuffer[MMCountArray(szKeyBuffer) - 1] = 0;
		szTempBuffer[MMCountArray(szTempBuffer) - 1] = 0;
		CopyMemory(szKeyBuffer, (LPCWSTR)strSrc + i * 8, sizeof(WCHAR) * 4);
		CopyMemory(szTempBuffer, (LPCWSTR)strSrc + i * 8 + 4, sizeof(WCHAR) * 4);

		//��ȡ��Կ
		WCHAR wKey = (WCHAR)wcstol(szKeyBuffer, &pEnd, 16);
		WCHAR wEncrypt = (WCHAR)wcstol(szTempBuffer, &pEnd, 16);

		//����ԭ��
		szSrcData[i] = (WCHAR)((WCHAR)wKey ^ (WCHAR)wEncrypt);
	}

	//��ֹ�ַ�
	szSrcData[wSoureLength] = 0;

	return szSrcData;
}

CMMString CMMEncrypt::MapEncrypt(CMMString strSrc)
{
	if (strSrc.empty()) return _T("");

	//��������
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

	//��������
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

	//��������
	BYTE * pDataSrc = (BYTE *)strSrc.c_str();

	//��������
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
