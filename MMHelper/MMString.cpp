#include "StdAfx.h"
#include "MMString.h"

////////////////////////////////////////////////////////////////////////////
//CMMString::CMMString()
//{
//	m_lpszStr = (TCHAR*)malloc(sizeof(TCHAR));
//	m_lpszStr[0] = _T('\0');
//
//	return;
//}
//
//CMMString::CMMString(const TCHAR ch, int nCount)
//{
//	for (int n = 0; n < nCount; n++)
//	{
//		operator += (ch);
//	}
//
//	return;
//}
//
//CMMString::CMMString(LPCTSTR lpszStr, int nLen)
//{
//	Assign(lpszStr, nLen);
//}
//
//CMMString::CMMString(LPCSTR lpszStr)
//{
//	operator=(lpszStr);
//
//	return;
//}
//
//CMMString::CMMString(const CMMString &src)
//{
//	Assign(src.m_lpszStr);
//}
//
//CMMString::~CMMString()
//{
//	if (m_lpszStr) free(m_lpszStr);
//
//	m_lpszStr = NULL;
//
//	return;
//}
//
//int CMMString::length() const
//{
//	return m_lpszStr ? (int)_tcslen(m_lpszStr) : 0;
//}
//
//CMMString::operator LPCTSTR() const
//{
//	return m_lpszStr;
//}
//
//void CMMString::Append(LPCTSTR lpszStr)
//{
//	if (NULL == lpszStr) return;
//
//	return Insert(length(), lpszStr);
//}
//
//void CMMString::Assign(LPCTSTR lpszStr, int cchMax)
//{
//	if (lpszStr == NULL) lpszStr = _T("");
//	cchMax = (cchMax < 0 ? (int)_tcslen(lpszStr) : cchMax);
//
//	if (cchMax > length() || NULL == m_lpszStr)
//	{
//		m_lpszStr = static_cast<LPTSTR>(realloc(m_lpszStr, (cchMax + 1) * sizeof(TCHAR)));
//		memset(m_lpszStr, 0, (cchMax + 1) * sizeof(TCHAR));
//	}
//
//	_tcsncpy(m_lpszStr, lpszStr, cchMax);
//	m_lpszStr[cchMax] = '\0';
//
//	return;
//}
//
//void CMMString::Insert(int nPos, TCHAR ch)
//{
//	return Insert(nPos, CMMString(ch));
//}
//
//void CMMString::Insert(int nPos, LPCTSTR lpszStr)
//{
//	if (nPos < 0 || NULL == lpszStr) return;
//
//	nPos = max(0, nPos);
//	nPos = min(length(), nPos);
//	int nLenAdd = (int)_tcslen(lpszStr);
//	int nLenStr = length();
//	int nNewLength = nLenStr + nLenAdd;
//	m_lpszStr = static_cast<LPTSTR>(realloc(m_lpszStr, (nNewLength + 1) * sizeof(TCHAR)));
//	memset(m_lpszStr + nLenStr, 0, sizeof(TCHAR) * (nLenAdd + 1));
//	CopyMemory(m_lpszStr + nPos + nLenAdd, m_lpszStr + nPos, (length() - nPos) * sizeof(TCHAR));
//	CopyMemory(m_lpszStr + nPos, lpszStr, nLenAdd * sizeof(TCHAR));
//
//	return;
//}
//
//bool CMMString::empty() const
//{
//	return NULL == m_lpszStr || m_lpszStr[0] == '\0';
//}
//
//void CMMString::clear()
//{
//	if (m_lpszStr) free(m_lpszStr);
//
//	m_lpszStr = (TCHAR*)malloc(sizeof(TCHAR));
//	m_lpszStr[0] = _T('\0');
//
//	return;
//}
//
//LPCTSTR CMMString::GetBuffer(int nLength) const
//{
//	if (NULL == m_lpszStr) return NULL;
//
//	nLength = max(0, min(nLength, length()));
//
//	return m_lpszStr + nLength;
//}
//
//void CMMString::Resize(UINT uSize)
//{
//	m_lpszStr = static_cast<LPTSTR>(realloc(m_lpszStr, uSize * sizeof(TCHAR)));
//	m_lpszStr[uSize - 1] = 0;
//
//	return;
//}
//
//TCHAR CMMString::GetAt(int nIndex) const
//{
//	return m_lpszStr[nIndex];
//}
//
//void CMMString::SetAt(int nIndex, TCHAR ch)
//{
//	assert(nIndex >= 0 && nIndex < length());
//	m_lpszStr[nIndex] = ch;
//}
//
//TCHAR & CMMString::operator[] (int nIndex) const
//{
//	return m_lpszStr[nIndex];
//}
//
//const CMMString& CMMString::operator=(const CMMString &src)
//{
//	Assign(src);
//	return *this;
//}
//
//const CMMString& CMMString::operator=(LPCTSTR lpszStr)
//{
//	if (lpszStr)
//	{
//		assert(false == ::IsBadStringPtr(lpszStr, -1));
//		Assign(lpszStr);
//	}
//	else
//	{
//		clear();
//	}
//
//	return *this;
//}
//
//#ifdef _UNICODE
//
//const CMMString& CMMString::operator=(LPCSTR lpStr)
//{
//	if (lpStr)
//	{
//		assert(!::IsBadStringPtrA(lpStr, -1));
//		int cchStr = (int)::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, NULL, 0) + 1;
//		LPWSTR pwstr = (LPWSTR)malloc(cchStr * sizeof(WCHAR));
//		if (pwstr != NULL) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr);
//		Assign(pwstr);
//	}
//	else
//	{
//		clear();
//	}
//
//	return *this;
//}
//
//const CMMString& CMMString::operator+=(LPCSTR lpStr)
//{
//	if (lpStr)
//	{
//		assert(!::IsBadStringPtrA(lpStr, -1));
//		int cchStr = (int)::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, NULL, 0) + 1;
//		LPWSTR pwstr = (LPWSTR)malloc(cchStr * sizeof(WCHAR));
//		if (pwstr != NULL) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr);
//		Append(pwstr);
//	}
//
//	return *this;
//}
//
//#else
//
//const CMMString& CMMString::operator=(LPCWSTR lpwStr)
//{
//	if (lpwStr)
//	{
//		assert(!::IsBadStringPtrW(lpwStr, -1));
//		int cchStr = ((int)wcslen(lpwStr) * 2) + 1;
//		LPSTR pstr = (LPSTR)malloc(cchStr);
//		if (pstr != NULL) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
//		Assign(pstr);
//	}
//	else
//	{
//		clear();
//	}
//
//	return *this;
//}
//
//const CMMString& CMMString::operator+=(LPCWSTR lpwStr)
//{
//	if (lpwStr)
//	{
//		assert(!::IsBadStringPtrW(lpwStr, -1));
//		int cchStr = ((int)wcslen(lpwStr) * 2) + 1;
//		LPSTR pstr = (LPSTR)malloc(cchStr);
//		if (pstr != NULL) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
//		Append(pstr);
//	}
//
//	return *this;
//}
//
//#endif // _UNICODE
//
//const CMMString& CMMString::operator=(const TCHAR ch)
//{
//	Assign(&ch, 1);
//
//	return *this;
//}
//
//CMMString CMMString::operator+(const TCHAR ch) const
//{
//	TCHAR szTemp[2] = {};
//	szTemp[0] = ch;
//
//	CMMString sTemp = *this;
//	sTemp.Append(szTemp);
//	return sTemp;
//}
//
//CMMString CMMString::operator+(const CMMString &src) const
//{
//	CMMString sTemp = *this;
//	sTemp.Append(src);
//	return sTemp;
//}
//
//CMMString CMMString::operator+(LPCTSTR lpszStr) const
//{
//	if (lpszStr)
//	{
//		assert(false == ::IsBadStringPtr(lpszStr, -1));
//		CMMString sTemp = *this;
//		sTemp.Append(lpszStr);
//		return sTemp;
//	}
//
//	return *this;
//}
//
//const CMMString& CMMString::operator += (const CMMString &src)
//{
//	Append(src);
//	return *this;
//}
//
//const CMMString& CMMString::operator += (LPCTSTR lpszStr)
//{
//	if (lpszStr)
//	{
//		assert(false == ::IsBadStringPtr(lpszStr, -1));
//		Append(lpszStr);
//	}
//
//	return *this;
//}
//
//const CMMString& CMMString::operator += (const TCHAR ch)
//{
//	TCHAR str[] = { ch, '\0' };
//	Append(str);
//	return *this;
//}
//
//bool CMMString::operator == (LPCTSTR lpszStr) const { return (Compare(lpszStr) == 0); };
//bool CMMString::operator == (const CMMString &Right) const { return (Compare(Right) == 0); };
//bool CMMString::operator != (LPCTSTR lpszStr) const { return (Compare(lpszStr) != 0); };
//bool CMMString::operator != (const CMMString &Right) const { return (Compare(Right) != 0); };
//bool CMMString::operator <= (LPCTSTR lpszStr) const { return (Compare(lpszStr) <= 0); };
//bool CMMString::operator <  (LPCTSTR lpszStr) const { return (Compare(lpszStr) < 0); };
//bool CMMString::operator >= (LPCTSTR lpszStr) const { return (Compare(lpszStr) >= 0); };
//bool CMMString::operator >  (LPCTSTR lpszStr) const { return (Compare(lpszStr) > 0); };
//
//int CMMString::Compare(LPCTSTR lpszStr) const
//{
//	return m_lpszStr && lpszStr ? _tcscmp(m_lpszStr, lpszStr) : -1;
//}
//
//int CMMString::CompareNoCase(LPCTSTR lpszStr) const
//{
//	return m_lpszStr ? _tcsicmp(m_lpszStr, lpszStr) : -1;
//}
//
//void CMMString::MakeUpper()
//{
//	m_lpszStr ? _tcsupr(m_lpszStr) : NULL;
//}
//
//void CMMString::MakeLower()
//{
//	m_lpszStr ? _tcslwr(m_lpszStr) : NULL;
//}
//
//CMMString CMMString::Left(int iLength) const
//{
//	if (iLength < 0) iLength = 0;
//	if (iLength > length()) iLength = length();
//	return CMMString(m_lpszStr, iLength);
//}
//
//CMMString CMMString::Mid(int iPos, int iLength) const
//{
//	if (iLength < 0) iLength = length() - iPos;
//	if (iPos + iLength > length()) iLength = length() - iPos;
//	if (iLength <= 0) return CMMString();
//	return CMMString(m_lpszStr + iPos, iLength);
//}
//
//CMMString CMMString::Right(int iLength) const
//{
//	int iPos = length() - iLength;
//	if (iPos < 0)
//	{
//		iPos = 0;
//		iLength = length();
//	}
//
//	return CMMString(m_lpszStr + iPos, iLength);
//}
//
//CMMString CMMString::Trim(TCHAR ch)
//{
//	TrimLeft(ch);
//	TrimRight(ch);
//
//	return *this;
//}
//
//CMMString CMMString::TrimLeft(TCHAR ch/* = _T(' ')*/)
//{
//	if (NULL == m_lpszStr) return *this;
//
//	int nLen = length();
//	for (int nPos = 0; nPos < nLen; nPos++)
//	{
//		if (ch == m_lpszStr[nPos]) continue;
//
//		CopyMemory(m_lpszStr, m_lpszStr + nPos, sizeof(TCHAR) * nLen);
//		m_lpszStr[nLen - nPos] = _T('\0');
//
//		break;
//	}
//	
//	return *this;
//}
//
//CMMString CMMString::TrimRight(TCHAR ch/* = _T(' ')*/)
//{
//	if (NULL == m_lpszStr) return *this;
//
//	int nLen = length();
//	for (int nPos = nLen - 1; nPos >= 0; nPos--)
//	{
//		if (ch == m_lpszStr[nPos])
//		{
//			m_lpszStr[nPos] = _T('\0');
//
//			continue;
//		}
//
//		break;
//	}
//
//	return *this;
//}
//
//int CMMString::Find(TCHAR ch, int iPos /*= 0*/) const
//{
//	assert(iPos >= 0 && iPos <= length());
//	if (iPos != 0 && (iPos < 0 || iPos >= length())) return -1;
//	LPCTSTR p = _tcschr(m_lpszStr + iPos, ch);
//	if (p == NULL) return -1;
//	return (int)(p - m_lpszStr);
//}
//
//int CMMString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
//{
//	assert(false == ::IsBadStringPtr(pstrSub, -1));
//	assert(iPos >= 0 && iPos <= length());
//	if (iPos != 0 && (iPos < 0 || iPos > length())) return -1;
//	LPCTSTR p = _tcsstr(m_lpszStr + iPos, pstrSub);
//	if (p == NULL) return -1;
//	return (int)(p - m_lpszStr);
//}
//
//int CMMString::rfind(TCHAR ch) const
//{
//	LPCTSTR p = _tcsrchr(m_lpszStr, ch);
//	if (p == NULL) return -1;
//	return (int)(p - m_lpszStr);
//}
//
//int CMMString::Replace(LPCTSTR lpszFrom, LPCTSTR lpszTo)
//{
//	if (MMInvalidString(lpszFrom)) return 0;
//
//	int nPos = Find(lpszFrom);
//	if (nPos < 0) return 0;
//
//	CMMString strTemp;
//	int nCount = 0;
//	int cchFrom = (int)_tcslen(lpszFrom);
//	int cchTo = (int)_tcslen(lpszTo);
//	while (nPos >= 0)
//	{
//		strTemp = Left(nPos);
//		strTemp += lpszTo;
//		strTemp += Mid(nPos + cchFrom);
//		Assign(strTemp);
//		
//		nPos = Find(lpszFrom, nPos + cchTo);
//		nCount++;
//	}
//
//	return nCount;
//}
//
//int CMMString::Format(LPCTSTR pstrFormat, ...)
//{
//	int nRet;
//	va_list Args;
//
//	va_start(Args, pstrFormat);
//	nRet = Format(pstrFormat, Args);
//	va_end(Args);
//
//	return nRet;
//
//}
//
//int CMMString::Format(LPCTSTR pstrFormat, va_list Args)
//{
//#if _MSC_VER <= 1400
//	TCHAR *szBuffer = NULL;
//	int size = 512, nLen, counts;
//	szBuffer = (TCHAR*)malloc(size);
//	ZeroMemory(szBuffer, size);
//	while (TRUE) {
//		counts = size / sizeof(TCHAR);
//		nLen = _vsntprintf(szBuffer, counts, pstrFormat, Args);
//		if (nLen != -1 && nLen < counts) {
//			break;
//		}
//		if (nLen == -1) {
//			size *= 2;
//		}
//		else {
//			size += 1 * sizeof(TCHAR);
//		}
//
//		if ((szBuffer = (TCHAR*)realloc(szBuffer, size)) != NULL) {
//			ZeroMemory(szBuffer, size);
//		}
//		else {
//			break;
//		}
//	}
//
//	Assign(szBuffer);
//	free(szBuffer);
//	return nLen;
//#else
//	int nLen, nSize;
//	TCHAR *szBuffer;
//	nLen = _vsntprintf(NULL, 0, pstrFormat, Args);
//	nSize = (nLen + 1) * sizeof(TCHAR);
//	szBuffer = (TCHAR*)malloc(nSize);
//	ZeroMemory(szBuffer, nSize);
//	nLen = _vsntprintf(szBuffer, nLen + 1, pstrFormat, Args);
//	Assign(szBuffer);
//	free(szBuffer);
//	return nLen;
//
//#endif
//}
//
//int CMMString::SmallFormat(LPCTSTR pstrFormat, ...)
//{
//	CMMString sFormat = pstrFormat;
//	TCHAR szBuffer[64] = {};
//	va_list argList;
//	va_start(argList, pstrFormat);
//	int iRet = ::_vsntprintf(szBuffer, sizeof(szBuffer), sFormat, argList);
//	va_end(argList);
//	Assign(szBuffer);
//	return iRet;
//}
//
//CMMString operator+(const TCHAR ch, const CMMString &src)
//{
//	CMMString strTemp = ch;
//	strTemp += src;
//
//	return strTemp;
//}
//
//CMMString operator+(LPCTSTR lpszStr, const CMMString &src)
//{
//	CMMString strTemp = lpszStr;
//	strTemp += src;
//
//	return strTemp;
//}
//
//bool operator == (LPCTSTR lpszStr, const CMMString &src)
//{
//	return src == lpszStr;
//}