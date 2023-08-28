#include "StdAfx.h"
#include "MMCommandLine.h"

//////////////////////////////////////////////////////////////////////////////////
CMMCommandLine::CMMCommandLine(LPCTSTR lpszCmdLine)
	: m_strCmdLine(lpszCmdLine)
{
	int nPosStart = 0;

	do 
	{
		nPosStart = m_strCmdLine.Find(_T('/'), nPosStart);
		if (-1 == nPosStart) break;

		CMMString strGroup;
		int nPosFinish = m_strCmdLine.Find(_T('/'), nPosStart + 1);
		if (-1 == nPosFinish)
		{
			strGroup = m_strCmdLine.Mid(nPosStart + 1);
			nPosStart = m_strCmdLine.GetLength();
		}
		else
		{
			strGroup = m_strCmdLine.Mid(nPosStart + 1, nPosFinish - nPosStart - 1);
			nPosStart = nPosFinish;
		}

		auto vecItem = CMMStrHelp::ParseStrFromString(strGroup, _T(' '));
		CMMString strKey, strValue;
		if (vecItem.size() > 0)
		{
			vecItem[0].Trim();
			strKey = vecItem[0];
		}
		if (vecItem.size() > 1)
		{
			vecItem[1].Trim();
			strValue = vecItem[1];
		}

		m_mapCmdGroup.insert(make_pair(strKey, strValue));

	} while (true);
	
	return;
}

CMMCommandLine::~CMMCommandLine()
{
}

bool CMMCommandLine::Get(IN CMMString strKey, OUT CMMString &strValue)
{
	auto FindIt = m_mapCmdGroup.find(strKey);
	if (FindIt == m_mapCmdGroup.end()) return false;

	strValue = FindIt->second;

	return true;
}

bool CMMCommandLine::IsExist(IN CMMString strKey)
{
	return m_mapCmdGroup.find(strKey) != m_mapCmdGroup.end();
}

std::map<CMMString, CMMString> CMMCommandLine::GetGroup()
{
	return m_mapCmdGroup;
}

//////////////////////////////////////////////////////////////////////////////////
