#include "StdAfx.h"
#include "DUIAttributeULong.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeULong)
MMImplement_ClassName(CDUIAttributeULong)

CDUIAttributeULong::CDUIAttributeULong()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_ULong;
#endif

	return;
}

CDUIAttributeULong::~CDUIAttributeULong(void)
{
}

LPVOID CDUIAttributeULong::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeULong, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeULong & CDUIAttributeULong::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeULong &LongRight = (CDUIAttributeULong&)Right;
	m_uValue = LongRight.m_uValue;

	return *this;
}

ULONG CDUIAttributeULong::GetValue()
{
	return m_uValue;
}

void CDUIAttributeULong::SetValue(ULONG uValue)
{
	if (uValue == GetValue()) return;

	m_uValue = uValue;

	NotifyChange();

	return;
}

bool CDUIAttributeULong::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriNumberULong))
	{
		SetValue(::strtoul(lpszValue, NULL, 10));

		return true;
	}

	return false;
}

bool CDUIAttributeULong::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Key_AttriNumberULong, (UINT)m_uValue);

	return true;
}