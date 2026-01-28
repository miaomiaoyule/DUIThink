#include "StdAfx.h"
#include "DUIResourceBase.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateResource(CDUIResourceBase)
MMImplement_ClassName(CDUIResourceBase)

CDUIResourceBase::CDUIResourceBase()
{
}

CDUIResourceBase::~CDUIResourceBase(void)
{
}

LPVOID CDUIResourceBase::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIResourceBase, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

LPCTSTR CDUIResourceBase::GetResourceName() const
{
	return m_strName;
}

enDuiResType CDUIResourceBase::GetResourceType() const
{
	return DuiResType_None;
}

void CDUIResourceBase::ReleaseResource()
{
	return;
}

bool CDUIResourceBase::IsDesign() 
{
	return m_bDesign; 
}

void CDUIResourceBase::SetDesign(bool bDesign)
{
	m_bDesign = bDesign;

	return;
}

int CDUIResourceBase::GetIndex()
{
#ifdef DUI_DESIGN
	return m_nIndex;
#endif

	return 0;
}

void CDUIResourceBase::SetIndex(int nIndex)
{
#ifdef DUI_DESIGN
	m_nIndex = nIndex;
#endif

	return;
}

bool CDUIResourceBase::SetResourceName(const CMMString &strName)
{
	if (strName.empty() || strName == GetResourceName()) return false;

	m_strName = strName;

	return true;
}

void CDUIResourceBase::ConstructResource(int nDpi)
{
	return;
}

bool CDUIResourceBase::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (NULL == lpszName || NULL == lpszValue)
	{
		ASSERT(false);
		return true;
	}
	if (0 == strcmp(lpszName, Dui_Resource_Key_ResName))
	{
		m_strName = CA2CT(lpszValue, CP_UTF8);

		return true;
	}

	return false;
}

bool CDUIResourceBase::SaveAttribute(tinyxml2::XMLElement *pNode)
{
	if (!pNode) return false;

	pNode->SetAttribute(Dui_Resource_Key_ResName, (LPSTR)CT2CA(m_strName, CP_UTF8));

	return true;
}

void CDUIResourceBase::OnDpiChanged(int nScale)
{
	return;
}