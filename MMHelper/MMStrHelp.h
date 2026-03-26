#ifndef __MM_STRENCODE_H__
#define __MM_STRENCODE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
struct tagMMStringEmoji
{
	bool								bEmoji = false;
	CMMString							strText;
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMStrHelp
{
public:
	static std::string FormatA(const char *lpszFmt, ...);
	static std::wstring FormatW(const wchar_t *lpszFmt, ...);
	static CMMString Format(LPCTSTR lpszFmt, ...);
	static CMMString ConvertAuto(std::string strFrom);
	static RECT ParseRect(LPCTSTR lpszStr);
	static SIZE ParseSize(LPCTSTR lpszStr);
	static std::vector<int> ParseIntFromString(CMMString strString, CMMString strSplit);
	static std::vector<CMMString> ParseStrFromString(CMMString strString, CMMString strSplit);
	static std::vector<CMMString> ParseStrFromString(LPCSTR lpszString, LPCSTR lpszSplit, UINT nCodePage = CP_ACP);
	static std::vector<tagMMStringEmoji> ParseStringForEmoji(const CMMString &strString);
};

//////////////////////////////////////////////////////////////////////////
#endif