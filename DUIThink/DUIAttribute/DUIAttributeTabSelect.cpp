#include "StdAfx.h"
#include "DUIAttributeTabSelect.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeTabSelect)
MMImplement_ClassName(CDUIAttributeTabSelect)

CDUIAttributeTabSelect::CDUIAttributeTabSelect()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_TabSelect;
#endif

	return;
}

CDUIAttributeTabSelect::~CDUIAttributeTabSelect(void)
{
}

LPVOID CDUIAttributeTabSelect::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeTabSelect, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

void CDUIAttributeTabSelect::InitSelectItem()
{
	CDUIWnd *pWnd = GetOwnerWnd();
	if (NULL == pWnd) return;
	
	CMMString strDescribe = GetCurSelDescribe();

	RemoveAll();

	tagDuiCombox AttriCombox;
	AttriCombox.vecItem.push_back({ 0, _T("") });
	
	VecDuiControlBase vecControl = pWnd->FindSubControlsByClass(pWnd->GetRootCtrl(), CDUITabLayoutCtrl().GetClass());
	for (auto pControl : vecControl)
	{
		MMInterfaceHelper(CDUITabLayoutCtrl, pControl, pTabCtrl);
		if (NULL == pTabCtrl) continue;
		if (find_if(AttriCombox.vecItem.begin(), AttriCombox.vecItem.end(), [&](tagDuiComboxItem &Item)
		{
			return Item.nItem == (int)pTabCtrl->GetCtrlID();

		}) != AttriCombox.vecItem.end()) continue;

		CMMString strCtrlID = CDUIGlobal::GetInstance()->FindCtrlID(pTabCtrl->GetCtrlID());
		if (strCtrlID.empty()) continue;

		AttriCombox.vecItem.push_back({ (int)pTabCtrl->GetCtrlID(), strCtrlID });
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