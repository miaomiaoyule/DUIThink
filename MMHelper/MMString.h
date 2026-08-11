#ifndef __MM_STRING_H__
#define __MM_STRING_H__

#pragma once

////////////////////////////////////////////////////////////////////////////
#if defined(DuiPlatform_SDL)
inline std::string WStringToUtf8(const std::wstring& ws)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(ws);
}

inline std::wstring Utf8ToWString(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(str);
}

inline std::wstring GbkToWString(const std::string &str)
{
	if (str.empty()) return std::wstring();

#ifdef DUI_HAVE_ICONV
	iconv_t cd = iconv_open("WCHAR_T", "GBK");
	if (cd == (iconv_t)-1) {
		// iconv 不可用，回退
		std::wstring out;
		out.reserve(str.size());
		for (unsigned char c : str) out.push_back(static_cast<wchar_t>(c));
		return out;
	}
	size_t inbytes = str.size();
	size_t outbytes = (inbytes + 1) * sizeof(wchar_t);
	std::vector<char> outbuf(outbytes);
	char *inptr = const_cast<char *>(str.data());
	char *outptr = outbuf.data();
	size_t res = iconv(cd, &inptr, &inbytes, &outptr, &outbytes);
	iconv_close(cd);
	if (res == (size_t)-1) {
		// 转换失败，回退逐字节扩展
		std::wstring out;
		out.reserve(str.size());
		for (unsigned char c : str) out.push_back(static_cast<wchar_t>(c));
		return out;
	}
	size_t written = (outbuf.size() - outbytes) / sizeof(wchar_t);
	return std::wstring(reinterpret_cast<wchar_t *>(outbuf.data()), written);
#else
	// 无 iconv：保底逐字节扩展（结果可能不是正确的中文）
	std::wstring out;
	out.reserve(str.size());
	for (unsigned char c : str) out.push_back(static_cast<wchar_t>(c));
	return out;
#endif
}

inline std::wstring AnsiToWString(const std::string &str)
{
	if (str.empty()) return std::wstring();

	// POSIX 平台：ANSI 通常就是某个本地编码，尝试用 iconv 从 "CHAR" -> "WCHAR_T"
#ifdef DUI_HAVE_ICONV
	iconv_t cd = iconv_open("WCHAR_T", "CHAR");
	if (cd == (iconv_t)-1) {
		std::wstring out;
		out.reserve(str.size());
		for (unsigned char c : str) out.push_back(static_cast<wchar_t>(c));
		return out;
	}
	size_t inbytes = str.size();
	size_t outbytes = (inbytes + 1) * sizeof(wchar_t);
	std::vector<char> outbuf(outbytes);
	char *inptr = const_cast<char *>(str.data());
	char *outptr = outbuf.data();
	size_t res = iconv(cd, &inptr, &inbytes, &outptr, &outbytes);
	iconv_close(cd);
	if (res == (size_t)-1) {
		std::wstring out;
		out.reserve(str.size());
		for (unsigned char c : str) out.push_back(static_cast<wchar_t>(c));
		return out;
	}
	size_t written = (outbuf.size() - outbytes) / sizeof(wchar_t);
	return std::wstring(reinterpret_cast<wchar_t *>(outbuf.data()), written);
#else
	std::wstring out;
	out.reserve(str.size());
	for (unsigned char c : str) out.push_back(static_cast<wchar_t>(c));
	return out;
#endif
}
#endif

////////////////////////////////////////////////////////////////////////////
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
#if defined(DuiPlatform_SDL)
	CMMString(LPCSTR lpszStr)
		: std::wstring(GbkToWString(NULL == lpszStr ? ("") : lpszStr))
	{

	}
	CMMString(LPCSTR lpszStr, int nLen)
		: std::wstring(GbkToWString(std::string(NULL == lpszStr ? ("") : lpszStr, nLen < 0 ? strlen(lpszStr) : nLen)))
	{

	}
#else
	CMMString(LPCSTR lpszStr)
		: std::wstring((LPCTSTR)CA2CT(NULL == lpszStr ? ("") : lpszStr))
	{

	}
	CMMString(LPCSTR lpszStr, int nLen)
		: std::wstring((LPCTSTR)CA2CT(std::string(NULL == lpszStr ? ("") : lpszStr, nLen < 0 ? strlen(lpszStr) : nLen).c_str()))
	{

	}
#endif
	CMMString(CHAR ch, int nCount)
		: CMMString((TCHAR)ch, nCount)
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
		: std::wstring(NULL == lpszStr ? _T("") : lpszStr, nLen < 0 ? lstrlen(lpszStr) : nLen)
	{

	}
	CMMString(const CMMString &strSrc)
		: std::wstring(strSrc)
	{

	}
	CMMString(const std::wstring &strSrc)
		: std::wstring(strSrc)
	{

	}
#ifndef DuiPlatform_SDL
	CMMString(CString &strSrc)
		: std::wstring(strSrc)
	{

	}
#endif
	CMMString Mid(int nFrom) const
	{
		return length() > nFrom ? c_str() + nFrom : _T("");
	}
	CMMString Mid(int nFrom, int nCount) const
	{
		return length() > nFrom ? CMMString(c_str() + nFrom, nCount) : CMMString(_T(""));
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
		while (length() > 0 && (front() == ch || (_T(' ') == ch && (_T('\t') == front() || _T('\n') == front()))))
		{
			erase(begin());
		}
		while (length() > 0 && (back() == ch || (_T(' ') == ch && (_T('\t') == back() || _T('\n') == back()))))
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
		if (lstrlen(lpszSrc) <= 0) return *this;

		CMMString strTemp;
		int nPosFrom = 0;
		int nPosFind = find(lpszSrc);
		while (-1 != nPosFind)
		{
			strTemp += Mid(nPosFrom, nPosFind - nPosFrom);
			strTemp += lpszDest;
			nPosFrom = nPosFind + lstrlen(lpszSrc);
			nPosFind = find(lpszSrc, nPosFrom);
		}
		if (false == strTemp.empty())
		{
			if (nPosFrom < length())
			{
				strTemp += GetBuffer(nPosFrom);
			}

			*this = strTemp;
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
			operator = (szBuffer);
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
	CMMString & AppendFormat(LPCTSTR pstrFormat, ...)
	{
		va_list Args;

		va_start(Args, pstrFormat);
		CMMString strTemp;
		strTemp.Format(pstrFormat, Args);
		operator += (strTemp);
		va_end(Args);

		return *this;
	}
	bool empty() const
	{
		return (length() > 0 && _T('\0') == at(0)) || __super::empty();
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

		return strThis.compare(strRight);
	}
	TCHAR & operator[](int nPos)
	{
		return __super::operator[](nPos);
	}
	CMMString & operator = (LPCSTR lpszRight)
	{
		if (NULL == lpszRight) return *this;

#if defined(DuiPlatform_SDL)
		__super::operator = (GbkToWString(lpszRight));
#else
		__super::operator = ((LPCTSTR)CA2CT(lpszRight));
#endif
		return *this;
	}
	CMMString & operator = (LPCTSTR lpszRight)
	{
		if (NULL == lpszRight) return *this;

		__super::operator = (lpszRight);

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
		return 0 == strLeft.compare(strRight);
	}
	friend bool operator == (const CMMString &strLeft, LPCTSTR lpszRight)
	{
		return 0 == strLeft.compare(lpszRight);
	}
	friend bool operator == (LPCTSTR lpszLeft, const CMMString &strRight)
	{
		return 0 == strRight.compare(lpszLeft);
	}
	friend bool operator != (LPCTSTR lpszLeft, const CMMString &strRight)
	{
		return 0 != strRight.compare(lpszLeft);
	}
	friend bool operator != (const CMMString &strLeft, const CMMString &strRight)
	{
		return 0 != strLeft.compare(strRight);
	}
	friend bool operator != (const CMMString &strLeft, LPCTSTR lpszRight)
	{
		return 0 != strLeft.compare(lpszRight);
	}
};

//hash
namespace std
{
	template<> struct hash<CMMString>
	{
		inline uint32_t operator()(const CMMString &str) const
		{
			//forbid CMMHash::GetHash(str) to avoid dependency, and use FNV-1a algorithm directly here
			//if CMMHash::GetHash(str) connot delay load MMHelper dll
			return MM_Fnv1a_append_bytes(MM_FNV_offset_basis, (const unsigned char*)str.c_str(), sizeof(TCHAR) * str.length());
		}
	};
}

#if defined(DuiPlatform_SDL)
inline std::string MMStringToUtf8(const std::wstring &ws) { return WStringToUtf8(ws); }
inline std::wstring Utf8ToMMString(const std::string &str) { return Utf8ToWString(str); }

inline CMMString CA2CT(const std::string &s, int nCodePage = CP_ACP)
{
	if (s.empty()) return {};

	// UTF-8: stable path, avoid codecvt_byname + unique_ptr
	if (CP_UTF8 == nCodePage)
	{
		try
		{
			return Utf8ToWString(s);
		}
		catch (...)
		{
			return L"";
		}
	}

	try
	{
		// codecvt_byname dtor is protected; cannot use unique_ptr/default_delete.
		// wstring_convert takes ownership of the new facet.
		const char *pszLocale = "zh_CN.GB18030";
		std::wstring_convert<std::codecvt_byname<wchar_t, char, std::mbstate_t>> conv(
			new std::codecvt_byname<wchar_t, char, std::mbstate_t>(pszLocale));
		return conv.from_bytes(s);
	}
	catch (...)
	{
		try
		{
			return Utf8ToWString(s);
		}
		catch (...)
		{
			return L"";
		}
	}
}

inline std::string CT2CA(const CMMString &s, int nCodePage = CP_ACP)
{
	if (s.empty()) return std::string();

	if (nCodePage == CP_UTF8)
	{
		try 
		{
			return WStringToUtf8(s);
		}
		catch (...) 
		{
			return std::string();
		}
	}

	try
	{
		const char *pszLocale = "zh_CN.GB18030";
		std::wstring_convert<std::codecvt_byname<wchar_t, char, std::mbstate_t>> conv(
			new std::codecvt_byname<wchar_t, char, std::mbstate_t>(pszLocale));
		return conv.to_bytes(s);
	}
	catch (...)
	{
		try 
		{
			return WStringToUtf8(s);
		}
		catch (...) 
		{
			return std::string();
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

#endif
