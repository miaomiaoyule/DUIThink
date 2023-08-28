#include "StdAfx.h"
#include "DUIAttributeGroup.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeGroup)
MMImplement_ClassName(CDUIAttributeGroup)

CDUIAttributeGroup::CDUIAttributeGroup()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Group;
#endif

	return;
}

CDUIAttributeGroup::~CDUIAttributeGroup(void)
{
	m_vecAttribute.clear();

	return;
}

LPVOID CDUIAttributeGroup::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeGroup, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeGroup & CDUIAttributeGroup::operator = (CDUIAttributeGroup &Right)
{
	__super::operator = (Right);

	if (m_vecAttribute.size() != Right.m_vecAttribute.size())
	{
		assert(false);
		return *this;
	}
	for (int n = 0; n < m_vecAttribute.size(); n++)
	{
		*m_vecAttribute[n] = *Right.m_vecAttribute[n];
	}

	return *this;
}

VecAttributeObj CDUIAttributeGroup::GetVecAttribute() const
{
	return m_vecAttribute;
}

bool CDUIAttributeGroup::SwapAttribute(CDUIAttributeObject *pObjectFirst, CDUIAttributeObject *pObjectNext)
{
	auto FindFirst = find(m_vecAttribute.begin(), m_vecAttribute.end(), pObjectFirst);
	auto FindNext = find(m_vecAttribute.begin(), m_vecAttribute.end(), pObjectNext);
	if (FindFirst == m_vecAttribute.end() || FindNext == m_vecAttribute.end()) return false;

	int nIndexFirst = FindFirst - m_vecAttribute.begin();
	int nIndexNext = FindNext - m_vecAttribute.begin();
	std::swap(m_vecAttribute[nIndexFirst], m_vecAttribute[nIndexNext]);

	return true;
}

bool CDUIAttributeGroup::SetOwner(CDUIPropertyObject *pOwner)
{
	if (false == __super::SetOwner(pOwner)) return  false;

	for (int nIndex = 0; nIndex < m_vecAttribute.size(); nIndex++)
	{
		if (NULL == m_vecAttribute[nIndex]) continue;

		m_vecAttribute[nIndex]->SetOwner(m_pOwner);
	}

	return true;
}

bool CDUIAttributeGroup::AddAttribute(CDUIAttributeObject *pObject)
{
	if (NULL == pObject) return false;

	pObject->SetGroup(this);
	pObject->SetOwner(m_pOwner);
	m_vecAttribute.push_back(pObject);

	return true;
}

bool CDUIAttributeGroup::RemoveAttribute(CDUIAttributeObject *pObject)
{
	for (int nIndex = 0; nIndex < m_vecAttribute.size(); nIndex++)
	{
		if (m_vecAttribute[nIndex] == pObject)
		{
			m_vecAttribute.erase(m_vecAttribute.begin() + nIndex);

			return true;
		}
	}

	return false;
}

bool CDUIAttributeGroup::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	return false;
}

bool CDUIAttributeGroup::SaveAttribute(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode || NULL == pNode->GetDocument()) return false;

	for (int nIndex = 0; nIndex < m_vecAttribute.size(); nIndex++)
	{
		CDUIAttributeObject *pAttribute = m_vecAttribute[nIndex];
		if (NULL == pAttribute || false == pAttribute->IsModified()) continue;

		tinyxml2::XMLElement *pNodeAttribute = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(pAttribute->GetAttributeName()));
		if (NULL == pNodeAttribute) continue;

		pAttribute->SaveAttribute(pNodeAttribute);

		pNode->LinkEndChild(pNodeAttribute);
	}

	return true;
}
