#include "StdAfx.h"
#include "DUIAttributeCtrlID.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeCtrlID)
MMImplement_ClassName(CDUIAttributeCtrlID)

CDUIAttributeCtrlID::CDUIAttributeCtrlID()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_CtrlID;
#endif

	return;
}

CDUIAttributeCtrlID::~CDUIAttributeCtrlID(void)
{
}

LPVOID CDUIAttributeCtrlID::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeCtrlID, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

bool CDUIAttributeCtrlID::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (0 == strcmp(lpszName, Dui_Key_AttriNumberULong))
	{
		__super::SetAttribute(lpszName, lpszValue);

#ifdef DUI_DESIGN
		CDUIGlobal::GetInstance()->OnAttriValueIDRead(GetAttributeType(), GetValue());
#endif

		return true;
	}

	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	return false;
}