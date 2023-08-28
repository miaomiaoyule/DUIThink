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

	strRes.FormatV(lpszFmt, args);

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
	if (strString.IsEmpty() || strSplit.IsEmpty()) return vecInt;

	int nIndex = strString.Find(strSplit, 0), nIndexArr = 0;
	while (0 <= nIndex && nIndex <= strString.GetLength())
	{
		CMMString strTemp = strString.Left(nIndex);

		//È¥µô·ÇÊý×Ö×Ö·û
		CMMString strNum;
		for (int nIndexTemp = 0; nIndexTemp < strTemp.GetLength(); nIndexTemp++)
		{
			if (_T('0') <= strTemp[nIndexTemp] && strTemp[nIndexTemp] <= _T('9'))
			{
				strNum += strTemp[nIndexTemp];
			}
		}

		//»ñµÃID
		vecInt.push_back(_ttoi(strNum.GetBuffer(0)));

		//ÏÂ¸ö×Ö·û¶Î
		strString = strString.GetBuffer(nIndex + 1);
		nIndex = strString.Find(strSplit, 0);

		//Ê£Óà×Ö·û
		if (-1 == nIndex && strString.GetLength() > 0) nIndex = strString.GetLength();
	}

	return vecInt;
}

std::vector<CMMString> CMMStrHelp::ParseStrFromString(CMMString strString, CMMString strSplit)
{
	std::vector<CMMString> vecResult;
	int pos = strString.Find(strSplit, 0);
	while (pos >= 0)
	{
		vecResult.push_back(strString.Left(pos));
		strString = strString.Right(strString.GetLength() - pos - strSplit.GetLength());
		pos = strString.Find(strSplit);
	}

	if (false == strString.IsEmpty())
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
				CMMStringA strSub(lpszString, lpszSub - lpszString);
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