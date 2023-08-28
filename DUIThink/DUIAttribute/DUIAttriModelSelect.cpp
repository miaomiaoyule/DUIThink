#include "StdAfx.h"
#include "DUIAttriModelSelect.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttriModelSelect)
MMImplement_ClassName(CDUIAttriModelSelect)

CDUIAttriModelSelect::CDUIAttriModelSelect()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_ModelSelect;
#endif

	return;
}

CDUIAttriModelSelect::~CDUIAttriModelSelect(void)
{
}

LPVOID CDUIAttriModelSelect::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttriModelSelect, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

void CDUIAttriModelSelect::InitSelectItem()
{
	CMMString strDescribe = GetCurSelDescribe();

	RemoveAll();

	tagDuiCombox AttriCombox;
	AttriCombox.vecItem.push_back({ 0, _T("") });
	
	MMInterfaceHelper(CDUITreeViewCtrl, GetOwner(), pTreeViewCtrl);
	if (pTreeViewCtrl)
	{
		for (int n = 0; n < CDUIGlobal::GetInstance()->GetDuiCount(DuiType_ModelTreeNode); n++)
		{
			AttriCombox.vecItem.push_back({ n + 1, CDUIGlobal::GetInstance()->GetDuiName(n, DuiType_ModelTreeNode) });
		}		
	}
	else
	{
		for (int n = 0; n < CDUIGlobal::GetInstance()->GetDuiCount(DuiType_ModelListItem); n++)
		{
			AttriCombox.vecItem.push_back({ n + 1, CDUIGlobal::GetInstance()->GetDuiName(n, DuiType_ModelListItem) });
		}
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