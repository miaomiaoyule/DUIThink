#include "StdAfx.h"
#include "DUIAttributeText.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeText)
MMImplement_ClassName(CDUIAttributeText)

CDUIAttributeText::CDUIAttributeText()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Text;
#endif

	return;
}

CDUIAttributeText::~CDUIAttributeText(void)
{
	return;
}

LPVOID CDUIAttributeText::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeText, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

LPCTSTR CDUIAttributeText::GetValue()
{
	return CDUIGlobal::GetInstance()->GetAttriText(m_uValueHash);
}

bool CDUIAttributeText::SetValue(LPCTSTR lpszText)
{
	if (0 == lstrcmp(lpszText, GetValue())) return true;

	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriText(lpszText);

	NotifyChange();

	return true;
}

bool CDUIAttributeText::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriObjValueID))
	{
		m_uValueHash = strtoul(lpszValue, NULL, 10);

#ifdef DUI_DESIGN
		CDUIGlobal::GetInstance()->OnAttriValueIDRead(GetAttributeType(), GetValueID());
#endif

		return true;
	}

	return false;
}

uint32_t CDUIAttributeText::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttributeText::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}