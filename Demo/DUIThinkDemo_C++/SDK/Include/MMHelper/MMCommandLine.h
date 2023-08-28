#ifndef __MM_COMMANDLINE_H__
#define __MM_COMMANDLINE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////////////
/**
 * cmdline use / split, key and value cannot have /
 * for example£º"/Key1 Value1 /Key2 Value2 /Key3 Value3"
 */
class MMHELPER_API CMMCommandLine
{
public:
	CMMCommandLine(LPCTSTR lpszCmdLine);
	virtual ~CMMCommandLine();

	//variant
protected:
	CMMString							m_strCmdLine;
	std::map<CMMString, CMMString>		m_mapCmdGroup;

	//method
public:
	bool Get(IN CMMString strKey, OUT CMMString &strValue);
	bool IsExist(IN CMMString strKey);
	std::map<CMMString, CMMString> GetGroup();
};

//////////////////////////////////////////////////////////////////////////////////

#endif