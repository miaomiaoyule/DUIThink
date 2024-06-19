#include "StdAfx.h"
#include "DUIAttributeLong.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeLong)
MMImplement_ClassName(CDUIAttributeLong)

CDUIAttributeLong::CDUIAttributeLong(long lValue)
	: m_lValue(lValue)
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Long;
#endif

	return;
}

CDUIAttributeLong::~CDUIAttributeLong(void)
{
}

LPVOID CDUIAttributeLong::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeLong, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeLong & CDUIAttributeLong::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeLong &LongRight = (CDUIAttributeLong&)Right;
	m_lValue = LongRight.m_lValue;

	return *this;
}

long CDUIAttributeLong::GetValue()
{
	return m_lValue;
}

void CDUIAttributeLong::SetValue(long lValue)
{
	if (lValue == GetValue()) return;

	m_lValue = lValue;

	NotifyChange();

	return;
}

bool CDUIAttributeLong::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriNumberLong))
	{
		SetValue(atol(lpszValue));

		return true;
	}

	return false;
}

bool CDUIAttributeLong::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Key_AttriNumberLong, m_lValue);

	return true;
}