#include "StdAfx.h"
#include "DUIAttributeCursor.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeCursor)
MMImplement_ClassName(CDUIAttributeCursor)

CDUIAttributeCursor::CDUIAttributeCursor()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Cursor;
#endif

	return;
}

CDUIAttributeCursor::~CDUIAttributeCursor(void)
{
}

LPVOID CDUIAttributeCursor::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeCursor, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}
