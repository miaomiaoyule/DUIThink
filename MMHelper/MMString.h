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
//	void Empty();
//	int GetLength() const;
//	bool IsEmpty() const;
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
//	int ReverseFind(TCHAR ch) const;
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
//		std::size_t operator()(const CMMString &str) const noexcept
//		{
//			return _Fnv1a_append_bytes(_FNV_offset_basis, (const unsigned char*)str.GetBuffer(0), sizeof(TCHAR) * str.GetLength());
//		}
//	};
//}

//////////////////////////////////////////////////////////////////////////
class CMMString : public CAtlString
{
public:
	CMMString()
	{

	}
	CMMString(LPCSTR lpszStr)
		: CAtlString(lpszStr)
	{

	}
	CMMString(LPCSTR lpszStr, int nLen)
		: CAtlString(lpszStr, nLen)
	{

	}
	CMMString(CHAR ch, int nLen)
		: CAtlString(ch, nLen)
	{

	}
	CMMString(TCHAR ch)
		: CAtlString(ch)
	{

	}
	CMMString(TCHAR ch, int nLen)
		: CAtlString(ch, nLen)
	{

	}
	CMMString(LPCTSTR lpszStr)
		: CAtlString(lpszStr)
	{

	}
	CMMString(LPCTSTR lpszStr, int nLen)
		: CAtlString(lpszStr, nLen)
	{

	}
	CMMString(const CMMString &strSrc)
		: CAtlString(strSrc)
	{

	}
	CMMString(CString &strSrc)
		: CAtlString(strSrc.GetBuffer())
	{

	}
	CMMString Mid(_In_ int iFirst)
	{
		return CAtlString::Mid(iFirst).GetBuffer();
	}
	CMMString Mid(
		_In_ int iFirst,
		_In_ int nCount) const
	{
		return CAtlString::Mid(iFirst, nCount).GetBuffer();
	}
	_Ret_notnull_ _Post_writable_size_(nLength + 1) PXSTR GetBufferSetLength(_In_ int nLength)
	{
		int nLenOld = GetLength();
		if (nLength < nLenOld)
		{
			return CAtlString::GetBufferSetLength(nLength);
		}

		PXSTR pszBuffer = GetBuffer(nLength);

		return(pszBuffer);
	}
	CMMString & Trim()
	{
		CAtlString::Trim();

		return *this;
	}
	CMMString & Trim(TCHAR ch)
	{
		CAtlString::Trim(ch);

		return *this;
	}
	CMMString & Trim(char ch)
	{
		CAtlString::Trim(ch);

		return *this;
	}
	CMMString & Trim(_In_z_ PCXSTR pszTargets)
	{
		return TrimLeft(pszTargets).TrimRight(pszTargets);
	}
	CMMString & TrimLeft(_In_z_ PCXSTR pszTargets)
	{
		// if we're not trimming anything, we're not doing any work
		if ((pszTargets == NULL) || (*pszTargets == 0))
		{
			return(*this);
		}

		int n = 0;
		PCXSTR psz = this->GetString();
		while ((psz[n])
			&& (pszTargets[n])
			&& (pszTargets[n] == psz[n]))
		{
			n++;
		}

		if (n > 0)
		{
			psz = psz + n;

			// fix up data and length
			int iFirst = int(psz - this->GetString());
			PXSTR pszBuffer = this->GetBuffer(this->GetLength());
			psz = pszBuffer + iFirst;
			int nDataLength = this->GetLength() - iFirst;
			Checked::memmove_s(pszBuffer, (this->GetLength() + 1) * sizeof(XCHAR),
				psz, (nDataLength + 1) * sizeof(XCHAR));
			this->ReleaseBufferSetLength(nDataLength);
		}

		return(*this);
	}
	CMMString & TrimRight(_In_z_ TCHAR ch)
	{
		return TrimRight(CMMString(ch));
	}
	CMMString & TrimRight(_In_z_ PCXSTR pszTargets)
	{
		// if we're not trimming anything, we're not doing any work
		int nLenTarget = lstrlen(pszTargets);
		int nLenThis = GetLength();
		if ((pszTargets == NULL) || (*pszTargets == 0) || nLenTarget > nLenThis)
		{
			return(*this);
		}

		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)
		PCXSTR psz = this->GetString();
		PCXSTR pszLast = NULL;	
		while (--nLenTarget >= 0 
			&& --nLenThis >= 0
			&& psz[nLenThis] == pszTargets[nLenTarget])
		{
			pszLast = psz;
		}

		if (pszLast != NULL && nLenTarget < 0)
		{
			// truncate at left-most matching character
			int iLast = int(pszLast - this->GetString());
			this->Truncate(iLast);
		}

		return(*this);
	}
	int ReverseFind(TCHAR ch) const
	{
		return CAtlString::ReverseFind(ch);
	}
	int ReverseFind(TCHAR ch, int nPosFrom) const
	{
		while (nPosFrom < GetLength() && nPosFrom >= 0)
		{
			if (this->operator[](nPosFrom) == ch) return nPosFrom;

			nPosFrom--;
		}

		return -1;
	}
	int ReverseFind(LPCTSTR lpszFind, int nPosFrom = -1) const
	{
		if (MMInvalidString(lpszFind)) return -1;

		CMMString strTemp = *this;
		int nLenFind = lstrlen(lpszFind);
		-1 == nPosFrom ? nPosFrom = strTemp.GetLength() : nPosFrom;
		if (nLenFind > nPosFrom) return -1;

		for (int nSub = nPosFrom - nLenFind; nSub >= 0; nSub--)
		{
			if (nSub + nLenFind < nPosFrom)
			{
				strTemp.SetAt(nSub + nLenFind, 0);
			}
			
			if (0 == lstrcmp(strTemp.GetBuffer() + nSub, lpszFind)) return nSub;
		}
		
		return -1;
	}
	bool IsEmpty() const throw()
	{
		LPCTSTR lpszStr = *this;
		return MMInvalidString(lpszStr);
	}
	CMMString & operator += (TCHAR chRight)
	{
		CAtlString::operator+=(chRight);

		return *this;
	}
	CMMString & operator += (LPCSTR lpszRight)
	{
		CAtlString::operator+=(lpszRight);

		return *this;
	}
	CMMString & operator += (LPCTSTR lpszRight)
	{
		CAtlString::operator+=(lpszRight);

		return *this;
	}
	CMMString & operator = (const CMMString &strRight)
	{
		CAtlString::operator=(strRight);

		return *this;
	}
	CMMString & operator = (_In_ const CString &strRight)
	{
		CAtlString::operator=(strRight);

		return *this;
	}
	CMMString & operator = (_In_opt_z_ LPCTSTR lpszRight)
	{
		CAtlString::operator=(lpszRight);

		return *this;
	}
	CMMString & operator = (LPCSTR lpszRight)
	{
		CAtlString::operator=(lpszRight);

		return *this;
	}
	CMMString & operator = (TCHAR chRight)
	{
		CAtlString::operator=(chRight);

		return *this;
	}
	CMMString & operator = (char chRight)
	{
		CAtlString::operator=(chRight);

		return *this;
	}
	friend CMMString operator+(const TCHAR ch, const CMMString &src)
	{
		CMMString strTemp = ch;
		strTemp += src;

		return strTemp;
	}
	friend CMMString operator+(const CMMString &srcLeft, const CMMString &srcRight)
	{
		CMMString strTemp = srcLeft;
		strTemp += srcRight;

		return strTemp;
	}
	friend CMMString operator+(const CMMString &srcLeft, const TCHAR ch)
	{
		CMMString strTemp = srcLeft;
		strTemp += ch;

		return strTemp;
	}
	friend CMMString operator+(const CMMString &srcLeft, const char ch)
	{
		CMMString strTemp = srcLeft;
		strTemp += ch;

		return strTemp;
	}
	friend CMMString operator+(const CMMString &srcLeft, LPCTSTR lpszStr)
	{
		CMMString strTemp = srcLeft;
		strTemp += lpszStr;

		return strTemp;
	}
	friend CMMString operator+(LPCTSTR lpszStr, const CMMString &src)
	{
		CMMString strTemp = lpszStr;
		strTemp += src;

		return strTemp;
	}
};

//////////////////////////////////////////////////////////////////////////

#endif
