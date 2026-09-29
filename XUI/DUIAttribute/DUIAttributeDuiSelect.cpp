#include "StdAfx.h"
#include "DUIAttributeDuiSelect.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeDuiSelect)
MMImplement_ClassName(CDUIAttributeDuiSelect)

CDUIAttributeDuiSelect::CDUIAttributeDuiSelect()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_DuiSelect;
#endif

	return;
}

CDUIAttributeDuiSelect::~CDUIAttributeDuiSelect(void)
{
}

LPVOID CDUIAttributeDuiSelect::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeDuiSelect, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

void CDUIAttributeDuiSelect::InitSelectItem()
{
	CMMString strDescribe = GetCurSelDescribe();

	RemoveAll();

	tagDuiCombox AttriCombox;
	AttriCombox.vecItem.push_back({ 0, _T("") });
	
	for (int n = 0; n < CDUIGlobal::GetInstance()->GetDuiCount(m_DuiType); n++)
	{
		AttriCombox.vecItem.push_back({ n + 1, CDUIGlobal::GetInstance()->GetDuiName(n, m_DuiType) });
	}
	for (int n = 0; n < AttriCombox.vecItem.size(); n++)
	{
		if (AttriCombox.vecItem[n].strDescribe == strDescribe)
		{
			m_nCurSel = n;

			break;
		}
	}

	SetCombox(AttriCombox);

	return;
}