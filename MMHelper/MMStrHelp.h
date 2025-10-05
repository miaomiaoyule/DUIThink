#ifndef __MM_STRENCODE_H__
#define __MM_STRENCODE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMStrHelp
{
public:
	static std::string FormatA(const char *lpszFmt, ...);
	static std::wstring FormatW(const wchar_t *lpszFmt, ...);
	static CMMString Format(LPCTSTR lpszFmt, ...);
	static CMMString ConvertAuto(std::string strFrom);

	//************************************
	// Description:	���ַ�������RECT
	// Parameter: 	LPCTSTR lpszStr ����_T("1,2,3,4")
	//************************************
	static RECT ParseRect(LPCTSTR lpszStr);

	//************************************
	// Returns:   	SIZE
	// Parameter: 	LPCTSTR lpszStr ����_T("1,2")
	//************************************
	static SIZE ParseSize(LPCTSTR lpszStr);

	//************************************
	// Returns:   	{1,2,3,4}
	// Parameter: 	CMMString strString ����_T("1,2,3,4")
	// Parameter: 	CMMString strSplit ����_T(",")
	//************************************
	static std::vector<int> ParseIntFromString(CMMString strString, CMMString strSplit);

	//************************************
	// Returns:   	{_T("123"), _T("456")}
	// Parameter: 	CMMString strString ����_T("123&456")
	// Parameter: 	CMMString strSplit ����_T("&")
	//************************************
	static std::vector<CMMString> ParseStrFromString(CMMString strString, CMMString strSplit);

	//************************************
	// Returns:   	{_T("123"), _T("456")}
	// Parameter: 	CMMString strString ����_T("123&456")
	// Parameter: 	CMMString strSplit ����_T("&")
	//************************************
	static std::vector<CMMString> ParseStrFromString(LPCSTR lpszString, LPCSTR lpszSplit, UINT nCodePage = CP_ACP);
};

//////////////////////////////////////////////////////////////////////////
#endif