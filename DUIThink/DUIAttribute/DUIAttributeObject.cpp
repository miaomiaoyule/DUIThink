#include "StdAfx.h"
#include "DUIAttributeObject.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeObject)
MMImplement_ClassName(CDUIAttributeObject)

CDUIAttributeObject::CDUIAttributeObject()
{
	return;
}

CDUIAttributeObject::~CDUIAttributeObject(void)
{
	return;
}

LPVOID CDUIAttributeObject::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeObject, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeObject & CDUIAttributeObject::operator = (CDUIAttributeObject &Right)
{
	m_uNameHash = Right.m_uNameHash;
	m_bModified = Right.m_bModified;
	SetValueID(Right.GetValueID());

#ifdef DUI_DESIGN
	m_DuiAttributeType = Right.m_DuiAttributeType;
	m_strAttriDescr = Right.m_strAttriDescr;
	m_bVisible = Right.m_bVisible;
#endif

	return *this;
}

CMMString CDUIAttributeObject::FormatInfo()
{
	return CMMStrHelp::Format(_T("%u"), GetValueID());
}

LPCTSTR CDUIAttributeObject::GetAttributeName()
{
	return CDUIGlobal::GetInstance()->GetAttriName(m_uNameHash);
}

enDuiAttributeType CDUIAttributeObject::GetAttributeType()
{
#ifdef DUI_DESIGN
	return m_DuiAttributeType;
#endif

	return DuiAttribute_None;
}

CDUIPropertyObject * CDUIAttributeObject::GetOwner()
{
	return m_pOwner;
}

CDUIWndManager * CDUIAttributeObject::GetOwnerWndManager()
{
	return m_pOwner ? m_pOwner->GetWndManager() : NULL;
}

CDUIAttributeGroup * CDUIAttributeObject::GetGroup()
{
#ifdef DUI_DESIGN
	return m_pGroup;
#endif

	return NULL;
}

LPCTSTR CDUIAttributeObject::GetGroupName()
{
#ifdef DUI_DESIGN
	return m_pGroup ? m_pGroup->GetAttributeName() : _T("");
#endif

	return _T("");
}

LPCTSTR CDUIAttributeObject::GetAttributeDescr()
{
#ifdef DUI_DESIGN
	return m_strAttriDescr;
#endif

	return _T("");
}

bool CDUIAttributeObject::SetAttributeDescr(LPCTSTR lpszDescribe)
{
#ifdef DUI_DESIGN
	m_strAttriDescr = lpszDescribe;

	NotifyChange();
#endif

	return true;
}

bool CDUIAttributeObject::IsVisible()
{
#ifdef DUI_DESIGN
	return m_bVisible;
#endif

	return true;
}

void CDUIAttributeObject::SetVisible(bool bVisible)
{
#ifdef DUI_DESIGN
	m_bVisible = bVisible;
#endif

	return;
}

bool CDUIAttributeObject::IsModified()
{
	return m_bModified;
}

bool CDUIAttributeObject::empty()
{
	return false;
}

int CDUIAttributeObject::GetScale()
{
	return m_pOwner ? m_pOwner->GetScale() : 100;
}

bool CDUIAttributeObject::SetOwner(CDUIPropertyObject *pOwner)
{
	if (m_pOwner) return false;

	m_pOwner = pOwner;

	return true;
}

void CDUIAttributeObject::SetAttributeName(LPCTSTR lpszName)
{
	if (MMInvalidString(lpszName)) return;

	m_uNameHash = CDUIGlobal::GetInstance()->SetAttriName(lpszName);

	NotifyChange();

	return;
}

void CDUIAttributeObject::SetGroup(CDUIAttributeGroup *pGroup)
{
	if (NULL == pGroup || pGroup == GetGroup()) return;

#ifdef DUI_DESIGN
	m_pGroup = pGroup;
#endif

	return;
}

bool CDUIAttributeObject::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (NULL == lpszName || NULL == lpszValue)
	{
		ASSERT(false);
		return true;
	}
	if (0 == strcmp(lpszName, Dui_Key_AttriObjNameID))
	{
		m_uNameHash = strtoul(lpszValue, NULL, 10);

		return true;
	}
	if (0 == strcmp(lpszName, Dui_Key_AttriObjModified))
	{
		m_bModified = (atol(lpszValue));

		return true;
	}
#ifdef DUI_DESIGN
	if (0 == strcmp(lpszName, Dui_Key_AttriObjDescr))
	{
		m_strAttriDescr = lpszValue;

		return true;
	}
	if (0 == strcmp(lpszName, Dui_Key_AttriObjVisible))
	{
		SetVisible(atol(lpszValue));

		return true;
	}
#endif

	return false;
}

bool CDUIAttributeObject::SaveAttribute(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode || NULL == pNode->GetDocument()) return false;

#ifdef DUI_DESIGN
	pNode->SetAttribute(Dui_Key_AttriObjDescr, (LPSTR)CT2CA(GetAttributeDescr()));
#endif
	pNode->SetAttribute(Dui_Key_AttriObjNameID, m_uNameHash);
	pNode->SetAttribute(Dui_Key_AttriObjVisible, (int)IsVisible());
	pNode->SetAttribute(Dui_Key_AttriObjModified, (int)m_bModified);
	pNode->SetAttribute(Dui_Key_AttriObjValueID, GetValueID());

	return true;
}

uint32_t CDUIAttributeObject::GetValueID()
{
	return 0;
}

void CDUIAttributeObject::SetValueID(uint32_t uValueID)
{
	return;
}

void CDUIAttributeObject::NotifyChange()
{
	if (NULL == m_pOwner || false == m_pOwner->IsInitComplete()) return;

	m_bModified = true;

#ifndef DUI_DESIGN
	return;
#endif

	m_pOwner->OnAttributeChange(this);

	return;
}