#ifndef __MM_STRING_H__
#define __MM_STRING_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
//class MMHELPER_API CMMString
//{
//public:
//	enum { MAX_LOCAL_STRING_LEN = 32 };
//
//	CMMString();
//	CMMString(const TCHAR ch, int nCount = 1);
//	CMMString(const CMMString &src);
//	CMMString(LPCTSTR lpszStr, int nLen = -1);
//	CMMString(LPCSTR lpszStr);
//	~CMMString();
//
//protected:
//	LPTSTR								m_lpszStr = NULL;
//
//	//method
//public:
//	void clear();
//	int length() const;
//	bool empty() const;
//	TCHAR GetAt(int nIndex) const;
//	void SetAt(int nIndex, TCHAR ch);
//	void Append(LPCTSTR lpszStr);
//	void Assign(LPCTSTR lpszStr, int nLength = -1);
//	void Insert(int nPos, TCHAR ch);
//	void Insert(int nPos, LPCTSTR lpszStr);
//	LPCTSTR GetBuffer(int nLength = 0) const;
//	void Resize(UINT uSize);
//
//	operator LPCTSTR() const;
//	TCHAR & operator[] (int nIndex) const;
//	const CMMString & operator=(const CMMString &src);
//	const CMMString & operator=(const TCHAR ch);
//	const CMMString & operator=(LPCTSTR lpszStr);
//#ifdef _UNICODE	    
//	const CMMString & CMMString::operator=(LPCSTR lpStr);
//	const CMMString & CMMString::operator+=(LPCSTR lpStr);
//#else			    
//	const CMMString & CMMString::operator=(LPCWSTR lpwStr);
//	const CMMString & CMMString::operator+=(LPCWSTR lpwStr);
//#endif
//	CMMString operator+(const TCHAR ch) const;
//	CMMString operator+(const CMMString &src) const;
//	CMMString operator+(LPCTSTR lpszStr) const;
//	const CMMString & operator += (const CMMString &src);
//	const CMMString & operator += (LPCTSTR lpszStr);
//	const CMMString & operator += (const TCHAR ch);
//
//	bool operator == (const CMMString &Right) const;
//	bool operator == (LPCTSTR lpszStr) const;
//	bool operator != (LPCTSTR lpszStr) const;
//	bool operator != (const CMMString &Right) const;
//	bool operator <= (LPCTSTR lpszStr) const;
//	bool operator <  (LPCTSTR lpszStr) const;
//	bool operator >= (LPCTSTR lpszStr) const;
//	bool operator >  (LPCTSTR lpszStr) const;
//
//	int Compare(LPCTSTR lpszStr) const;
//	int CompareNoCase(LPCTSTR lpszStr) const;
//
//	void MakeUpper();
//	void MakeLower();
//
//	CMMString Left(int nLength) const;
//	CMMString Mid(int iPos, int nLength = -1) const;
//	CMMString Right(int nLength) const;
//	CMMString Trim(TCHAR ch = _T(' '));
//	CMMString TrimLeft(TCHAR ch = _T(' '));
//	CMMString TrimRight(TCHAR ch = _T(' '));
//
//	int Find(TCHAR ch, int iPos = 0) const;
//	int Find(LPCTSTR lpszStr, int iPos = 0) const;
//	int rfind(TCHAR ch) const;
//	int Replace(LPCTSTR lpszFrom, LPCTSTR lpszTo);
//
//	int __cdecl Format(LPCTSTR pstrFormat, ...);
//	int __cdecl Format(LPCTSTR pstrFormat, va_list Args);
//	int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);
//};
//
//MMHELPER_API CMMString operator+(const TCHAR ch, const CMMString &src);
//MMHELPER_API CMMString operator+(LPCTSTR lpszStr, const CMMString &src);
//MMHELPER_API bool operator == (LPCTSTR lpszStr, const CMMString &src);
//
//
////////////////////////////////////////////////////////////////////////////
//namespace std
//{
//	template<> struct hash<CMMString>
//	{
//		std::size_t operator()(const CMMString &str) const
//		{
//			return _Fnv1a_append_bytes(_FNV_offset_basis, (const unsigned char*)str.GetBuffer(0), sizeof(TCHAR) * str.length());
//		}
//	};
//}

//////////////////////////////////////////////////////////////////////////
class CMMString : 
#ifdef UNICODE
	public std::wstring
#else
	public std::string
#endif
{
public:
	CMMString()
	{

	}
	CMMString(LPCSTR lpszStr)
		: std::wstring((LPCTSTR)CA2CT(lpszStr))
	{

	}
	CMMString(LPCSTR lpszStr, int nLen)
		: std::wstring((LPCTSTR)CA2CT(std::string(lpszStr, nLen).c_str()))
	{

	}
	CMMString(CHAR ch, int nCount)
		: std::wstring((LPCTSTR)CA2CT(std::string(nCount, ch).c_str()))
	{

	}
	CMMString(TCHAR ch)
		: std::wstring(1, ch)
	{

	}
	CMMString(TCHAR ch, int nCount)
		: std::wstring(nCount, ch)
	{

	}
	CMMString(LPCTSTR lpszStr)
		: std::wstring(NULL == lpszStr ? _T("") : lpszStr)
	{

	}
	CMMString(LPCTSTR lpszStr, int nLen)
		: std::wstring(NULL == lpszStr ? _T("") : lpszStr, nLen)
	{

	}
	CMMString(const CMMString &strSrc)
		: std::wstring(strSrc)
	{

	}
	CMMString(CString &strSrc)
		: std::wstring(strSrc)
	{

	}
	CMMString Mid(int nFrom) const
	{
		return length() > nFrom ? c_str() + nFrom : _T("");
	}
	CMMString Mid(int nFrom, int nCount) const
	{
		return length() > nFrom ? CMMString(c_str() + nFrom, nCount) : _T("");
	}
	CMMString Left(int nCount) const
	{
		return CMMString(c_str(), min(length(), nCount));
	}
	CMMString Right(int nCount) const
	{
		return (c_str() + max(0, (int)length() - nCount));
	}
	CMMString & MakeLower()
	{
		std::transform(begin(), end(), begin(), tolower);

		return *this;
	}
	CMMString & Trim(TCHAR ch = _T(' '))
	{
		while (front() == ch)
		{
			erase(begin());
		}
		while (back() == ch)
		{
			erase(--rbegin().base());
		}

		return *this;
	}
	CMMString & Trim(LPCTSTR pszTargets)
	{
		return TrimLeft(pszTargets).TrimRight(pszTargets);
	}
	CMMString & TrimLeft(LPCTSTR pszTargets)
	{
		int nLen = lstrlen(pszTargets);
		while (Left(nLen) == pszTargets)
		{
			erase(begin(), begin() + nLen);
		}

		return(*this);
	}
	CMMString & TrimRight(TCHAR ch)
	{
		return TrimRight(CMMString(ch).c_str());
	}
	CMMString & TrimRight(LPCTSTR pszTargets)
	{
		int nLen = lstrlen(pszTargets);
		while (Right(nLen) == pszTargets)
		{
			erase((rbegin() + nLen).base(), rbegin().base());
		}

		return(*this);
	}
	CMMString & Replace(TCHAR chSrc, TCHAR chDest)
	{
		std::replace(begin(), end(), chSrc, chDest);

		return *this;
	}
	CMMString & Replace(LPCTSTR lpszSrc, LPCTSTR lpszDest)
	{
		int nPos = find(lpszSrc);
		while (-1 != nPos)
		{
			erase(begin() + nPos, begin() + nPos + lstrlen(lpszSrc));
			insert(nPos, lpszDest);

			nPos = find(lpszSrc, nPos + lstrlen(lpszDest));
		}

		return *this;
	}
	CMMString & Format(LPCTSTR pstrFormat, va_list Args)
	{
		if (NULL == pstrFormat) return *this;

		#if _MSC_VER <= 1400
			TCHAR *szBuffer = NULL;
			int size = 512, nLen, counts;
			szBuffer = (TCHAR*)malloc(size);
			ZeroMemory(szBuffer, size);
			while (TRUE) 
			{
				counts = size / sizeof(TCHAR);
				nLen = _vsntprintf(szBuffer, counts, pstrFormat, Args);
				if (nLen != -1 && nLen < counts) 
				{
					break;
				}
				if (nLen == -1) 
				{
					size *= 2;
				}
				else 
				{
					size += 1 * sizeof(TCHAR);
				}
			
				if ((szBuffer = (TCHAR*)realloc(szBuffer, size)) != NULL) 
				{
					ZeroMemory(szBuffer, size);
				}
				else 
				{
					break;
				}
			}
			
			Assign(szBuffer);
			free(szBuffer);
			return nLen;
		#else
			int nLen = _vsntprintf(NULL, 0, pstrFormat, Args);
			int nSize = (nLen + 1) * sizeof(TCHAR);
			TCHAR *szBuffer = (TCHAR*)malloc(nSize);
			ZeroMemory(szBuffer, nSize);
			nLen = _vsntprintf(szBuffer, nLen + 1, pstrFormat, Args);
			operator +=(szBuffer);
			free(szBuffer);
		#endif
			
		return *this;
	}
	CMMString & Format(LPCTSTR pstrFormat, ...)
	{
		va_list Args;

		va_start(Args, pstrFormat);
		Format(pstrFormat, Args);
		va_end(Args);

		return *this;
	}
	void SetAt(int nPos, TCHAR ch)
	{
		if (nPos < 0 || nPos >= length()) return;

		operator[](nPos) = ch;

		return;
	}
	int find(LPCTSTR lpszFind, int nPosOffset = 0) const
	{
		return (int)__super::find(lpszFind, nPosOffset);
	}
	int find(TCHAR ch, int nPosOffset = 0) const
	{
		return (int)__super::find(ch, nPosOffset);
	}
	int rfind(LPCTSTR lpszFind, int nPosOffset = -1) const
	{
		return (int)__super::rfind(lpszFind, nPosOffset);
	}
	int rfind(TCHAR ch, int nPosOffset = -1) const
	{
		return (int)__super::rfind(ch, nPosOffset);
	}
	int CompareNoCase(LPCTSTR lpszRight)
	{
		CMMString strThis = *this;
		CMMString strRight = lpszRight;
		strThis.MakeLower();
		strRight.MakeLower();

		return lstrcmp(strThis, strRight);
	}
	TCHAR & operator[](int nPos)
	{
		return __super::operator[](nPos);
	}
	CMMString & operator = (LPCSTR lpszRight)
	{
		__super::operator =((LPCTSTR)CA2CT(lpszRight));

		return *this;
	}
	CMMString & operator = (LPCTSTR lpszRight)
	{
		__super::operator =(lpszRight);

		return *this;
	}
	LPTSTR GetBuffer(int nFrom = 0)
	{
		return (LPTSTR)(c_str() + max(0, min(nFrom, length())));
	}
	LPTSTR GetBufferSetLength(int nLen)
	{
		resize(nLen);
		operator[](nLen - 1) = '\0';

		return GetBuffer();
	}
	operator LPCTSTR() const
	{
		return c_str();
	}
	friend CMMString operator + (const CMMString &strLeft, TCHAR ch)
	{
		CMMString strTemp = strLeft;
		strTemp.operator += (ch);

		return strTemp;
	}
	friend CMMString operator + (const CMMString &strLeft, CHAR ch)
	{
		CMMString strTemp = strLeft;
		strTemp.operator += (ch);

		return strTemp;
	}
	friend CMMString operator + (TCHAR ch, const CMMString &strRight)
	{
		CMMString strTemp(ch);
		strTemp += strRight;

		return strTemp;
	}
	friend CMMString operator + (const CMMString &strLeft, LPCTSTR lpszRight)
	{
		CMMString strTemp = strLeft;
		strTemp += lpszRight;
		return strTemp;
	}
	friend CMMString operator + (const CMMString &strLeft, const CMMString &strRight)
	{
		CMMString strTemp = strLeft;
		strTemp += strRight;
		return strTemp;
	}
	friend CMMString operator + (LPCTSTR lpszLeft, const CMMString &strRight)
	{
		CMMString strLeft = lpszLeft;
		strLeft += strRight;
		return strLeft;
	}
	friend bool operator == (const CMMString &strLeft, const CMMString &strRight)
	{
		return 0 == lstrcmp(strLeft, strRight);
	}
	friend bool operator == (const CMMString &strLeft, LPCTSTR lpszRight)
	{
		return 0 == lstrcmp(strLeft, lpszRight);
	}
	friend bool operator == (LPCTSTR lpszLeft, const CMMString &strRight)
	{
		return 0 == lstrcmp(lpszLeft, strRight);
	}
	friend bool operator != (LPCTSTR lpszLeft, const CMMString &strRight)
	{
		return 0 != lstrcmp(lpszLeft, strRight);
	}
	friend bool operator != (const CMMString &strLeft, const CMMString &strRight)
	{
		return 0 != lstrcmp(strLeft, strRight);
	}
	friend bool operator != (const CMMString &strLeft, LPCTSTR lpszRight)
	{
		return 0 != lstrcmp(strLeft, lpszRight);
	}
};

//////////////////////////////////////////////////////////////////////////

#endif
