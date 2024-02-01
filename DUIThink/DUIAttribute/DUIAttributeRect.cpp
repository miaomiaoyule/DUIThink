#include "StdAfx.h"
#include "DUIAttributeRect.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeRect)
MMImplement_ClassName(CDUIAttributeRect)

CDUIAttributeRect::CDUIAttributeRect()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Rect;
#endif

	return;
}

CDUIAttributeRect::~CDUIAttributeRect(void)
{
	return;
}

LPVOID CDUIAttributeRect::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeRect, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIRect CDUIAttributeRect::GetValue()
{
	return CDUIGlobal::GetInstance()->GetAttriRect(m_uValueHash);
}

void CDUIAttributeRect::SetValue(long lLeft, long lTop, long lRight, long lBottom)
{
	if (RECT{ lLeft, lTop, lRight, lBottom } == GetValue()) return;

	SetValue(RECT{ lLeft, lTop, lRight, lBottom });

	return;
}

void CDUIAttributeRect::SetValue(const RECT &rect)
{
	if (rect == GetValue()) return;

	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriRect(rect);

	NotifyChange();

	return;
}

void CDUIAttributeRect::SetLeft(long lLeft)
{
	CDUIRect rcValue = GetValue();
	rcValue.left = lLeft;
	SetValue(rcValue);

	return;
}

void CDUIAttributeRect::SetTop(long lTop)
{
	CDUIRect rcValue = GetValue();
	rcValue.top = lTop;
	SetValue(rcValue);

	return;
}

void CDUIAttributeRect::SetRight(long lRight)
{
	CDUIRect rcValue = GetValue();
	rcValue.right = lRight;
	SetValue(rcValue);

	return;
}

void CDUIAttributeRect::SetBottom(long lBottom)
{
	CDUIRect rcValue = GetValue();
	rcValue.bottom = lBottom;
	SetValue(rcValue);

	return;
}

bool CDUIAttributeRect::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
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

uint32_t CDUIAttributeRect::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttributeRect::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}