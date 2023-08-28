#include "StdAfx.h"
#include "DUIColorBase.h"

DuiImplement_CreateResource(CDUIColorBase)
MMImplement_ClassName(CDUIColorBase)

CDUIColorBase::CDUIColorBase(void)
{
	return;
}

CDUIColorBase::CDUIColorBase(LPCTSTR lpszResName, DWORD dwColor)
{
	m_strName = lpszResName;
	m_dwColor = dwColor;
	
	return;
}

CDUIColorBase::~CDUIColorBase(void)
{
}

LPVOID CDUIColorBase::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIColorBase, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

enDuiResType CDUIColorBase::GetResourceType() const
{
	return DuiResType_Color;
}

DWORD CDUIColorBase::GetColor()
{
	return m_dwColor;
}

//ÑÕÉ«Ïà¹Ø
void CDUIColorBase::SetColor(DWORD dwColor)
{
	if (m_dwColor == dwColor) return;

	m_dwColor = dwColor;

	return;
}

bool CDUIColorBase::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Resource_Key_Color))
	{
		m_dwColor = strtoul(lpszValue, NULL, 10);
		
		return true;
	}

	return false;
}

bool CDUIColorBase::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Resource_Key_Color, (UINT)m_dwColor);

	return true;
}