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

		//È¥µô·ÇÊý×Ö×Ö·û
		CMMString strNum;
		for (int nIndexTemp = 0; nIndexTemp < strTemp.length(); nIndexTemp++)
		{
			if (_T('0') <= strTemp[nIndexTemp] && strTemp[nIndexTemp] <= _T('9'))
			{
				strNum += strTemp[nIndexTemp];
			}
		}

		//»ñµÃID
		vecInt.push_back(_ttoi(strNum));

		//ÏÂ¸ö×Ö·û¶Î
		strString = strString.Mid(nIndex + 1);
		nIndex = strString.find(strSplit, 0);

		//Ê£Óà×Ö·û
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