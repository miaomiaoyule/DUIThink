#include "stdafx.h"
#include "MMStrHelp.h"

//////////////////////////////////////////////////////////////////////////
std::string CMMStrHelp::FormatA(const char *lpszFmt, ...)
{
	std::string strRes;
	va_list args;
	va_start(args, lpszFmt);

	{
		int nLength = _vscprintf(lpszFmt, args);
		nLength += 1; 
		strRes.resize(nLength);
		_vsnprintf((LPSTR)strRes.data(), nLength, lpszFmt, args);
	}

	va_end(args);

	return strRes;
}

std::wstring CMMStrHelp::FormatW(const wchar_t *lpszFmt, ...)
{
	CStringW strRes;
	va_list args;
	va_start(args, lpszFmt);

	strRes.FormatV(lpszFmt, args);

	va_end(args);

	return strRes;
}

CMMString CMMStrHelp::Format(LPCTSTR lpszFmt, ...)
{
	CMMString strRes;
	va_list args;
	va_start(args, lpszFmt);

	strRes.Format(lpszFmt, args);

	va_end(args);

	return strRes;
}

CMMString CMMStrHelp::ConvertAuto(std::string strFrom)
{
	if (CMMFile::IsUTF8Encode(std::vector<BYTE>(strFrom.begin(), strFrom.end())))
	{
		return ((LPCTSTR)CA2CT(strFrom.c_str(), CP_UTF8));
	}
	
	return ((LPCTSTR)CA2CT(strFrom.c_str()));
}

RECT CMMStrHelp::ParseRect(LPCTSTR lpszStr)
{
	RECT rc = {};
	LPTSTR pstr = NULL;

	rc.left = _tcstol(lpszStr, &pstr, 10);		assert(pstr);
	rc.top = _tcstol(pstr + 1, &pstr, 10);		assert(pstr);
	rc.right = _tcstol(pstr + 1, &pstr, 10);	assert(pstr);
	rc.bottom = _tcstol(pstr + 1, &pstr, 10);	assert(pstr);

	return rc;
}

SIZE CMMStrHelp::ParseSize(LPCTSTR lpszStr)
{
	SIZE sz = {};
	LPTSTR pstr = NULL;

	sz.cx = _tcstol(lpszStr, &pstr, 10);		assert(pstr);
	sz.cy = _tcstol(pstr + 1, &pstr, 10);		assert(pstr);

	return sz;
}

std::vector<int> CMMStrHelp::ParseIntFromString(CMMString strString, CMMString strSplit)
{
	vector<int> vecInt;
	if (strString.empty() || strSplit.empty()) return vecInt;

	int nIndex = strString.find(strSplit, 0), nIndexArr = 0;
	while (0 <= nIndex && nIndex <= strString.length())
	{
		CMMString strTemp = strString.Left(nIndex);

		//去掉非数字字符
		CMMString strNum;
		for (int nIndexTemp = 0; nIndexTemp < strTemp.length(); nIndexTemp++)
		{
			if (_T('0') <= strTemp[nIndexTemp] && strTemp[nIndexTemp] <= _T('9'))
			{
				strNum += strTemp[nIndexTemp];
			}
		}

		//获得ID
		vecInt.push_back(_ttoi(strNum));

		//下个字符段
		strString = strString.Mid(nIndex + 1);
		nIndex = strString.find(strSplit, 0);

		//剩余字符
		if (-1 == nIndex && strString.length() > 0) nIndex = strString.length();
	}

	return vecInt;
}

std::vector<CMMString> CMMStrHelp::ParseStrFromString(CMMString strString, CMMString strSplit)
{
	std::vector<CMMString> vecResult;
	int pos = strString.find(strSplit, 0);
	while (pos >= 0)
	{
		vecResult.push_back(strString.Left(pos));
		strString = strString.Right(strString.length() - pos - strSplit.length());
		pos = strString.find(strSplit);
	}

	if (false == strString.empty())
	{
		vecResult.push_back(strString);
	}

	return vecResult;
}

std::vector<CMMString> CMMStrHelp::ParseStrFromString(LPCSTR lpszString, LPCSTR lpszSplit, UINT nCodePage)
{
	std::vector<CMMString> vecResult;
	LPCSTR lpszSub = strstr(lpszString, lpszSplit);
	while (lpszSub)
	{
		switch (nCodePage)
		{
			case CP_ACP:
			{
				vecResult.push_back(CMMString(lpszString, lpszSub - lpszString));

				break;
			}
			default:
			{
				CStringA strSub(lpszString, lpszSub - lpszString);
				vecResult.push_back((LPCTSTR)CA2CT(strSub, nCodePage));

				break;
			}
		}
		
		lpszString = lpszSub + strlen(lpszSplit);
		lpszSub = strstr(lpszString, lpszSplit);
	}

	if (false == MMInvalidString(lpszString))
	{
		switch (nCodePage)
		{
			case CP_ACP:
			{
				vecResult.push_back(lpszString);

				break;
			}
			default:
			{
				vecResult.push_back((LPCTSTR)CA2CT(lpszString, nCodePage));

				break;
			}
		}
	}

	return vecResult;
}

std::vector<tagMMStringEmoji> CMMStrHelp::ParseStringForEmoji(const CMMString &strString)
{
	std::vector<tagMMStringEmoji> vecStringEmoji;
	int nLen = strString.length();
	if (nLen == 0) return vecStringEmoji;

	// 预分配适当的空间，降低频繁 push_back 带来的内存重分配开销
	vecStringEmoji.reserve(nLen);

	for (int n = 0; n < nLen; )
	{
		wchar_t c = strString[n];
		bool bEmoji = false;
		int step = 1;

		// 1. 判断是否是高阶代理项（占2个wchar_t的Emoji及扩展字符）
		if (c >= 0xD800 && c <= 0xDBFF && (n + 1 < nLen))
		{
			wchar_t cNext = strString[n + 1];
			if (cNext >= 0xDC00 && cNext <= 0xDFFF)
			{
				bEmoji = true;
				step = 2;
			}
		}
		// 2. 拦截部分存在于 BMP 基础平面内的老式符号/Emoji (U+2600 ~ U+27BF 等)
		else if ((c >= 0x2600 && c <= 0x27BF) || (c >= 0x2300 && c <= 0x23FF) || c == 0x200D)
		{
			bEmoji = true;
		}

		// 追加
		vecStringEmoji.push_back({ bEmoji, c });
		if (step == 2)
		{
			vecStringEmoji.back().strText += strString[n + 1];
		}

		n += step;
	}

	return vecStringEmoji;
}