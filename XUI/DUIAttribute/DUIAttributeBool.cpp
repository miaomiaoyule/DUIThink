#include "StdAfx.h"
#include "DUIAttributeBool.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeBool)
MMImplement_ClassName(CDUIAttributeBool)

CDUIAttributeBool::CDUIAttributeBool(bool bValue)
	: m_bValue(bValue)
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Bool;
#endif

	return;
}

CDUIAttributeBool::~CDUIAttributeBool(void)
{
}

LPVOID CDUIAttributeBool::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeBool, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeBool & CDUIAttributeBool::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeBool &BoolRight = (CDUIAttributeBool&)Right;
	m_bValue = BoolRight.m_bValue;

	return *this;
}

bool CDUIAttributeBool::GetValue()
{
	return m_bValue;
}

void CDUIAttributeBool::SetValue(bool bValue)
{
	if (m_bValue == bValue) return;

	m_bValue = bValue;

	NotifyChange();

	return;
}

bool CDUIAttributeBool::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriBoolValue))
	{
		SetValue(atoi(lpszValue));

		return true;
	}

	return false;
}

bool CDUIAttributeBool::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Key_AttriBoolValue, (int)m_bValue);

	return true;
}
