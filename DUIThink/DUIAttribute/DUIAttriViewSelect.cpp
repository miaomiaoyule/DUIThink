#include "StdAfx.h"
#include "DUIAttriViewSelect.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttriViewSelect)
MMImplement_ClassName(CDUIAttriViewSelect)

CDUIAttriViewSelect::CDUIAttriViewSelect()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_ViewSelect;
#endif

	return;
}

CDUIAttriViewSelect::~CDUIAttriViewSelect(void)
{
}

LPVOID CDUIAttriViewSelect::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttriViewSelect, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

void CDUIAttriViewSelect::InitSelectItem()
{
	CMMString strDescribe = GetCurSelDescribe();

	RemoveAll();

	tagDuiCombox AttriCombox;
	AttriCombox.vecItem.push_back({ 0, _T("") });
	
	for (int n = 0; n < CDUIGlobal::GetInstance()->GetDuiCount(DuiType_View); n++)
	{
		AttriCombox.vecItem.push_back({ n + 1, CDUIGlobal::GetInstance()->GetDuiName(n, DuiType_View) });
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