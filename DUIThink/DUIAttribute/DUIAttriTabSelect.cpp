#include "StdAfx.h"
#include "DUIAttriTabSelect.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttriTabSelect)
MMImplement_ClassName(CDUIAttriTabSelect)

CDUIAttriTabSelect::CDUIAttriTabSelect()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_TabSelect;
#endif

	return;
}

CDUIAttriTabSelect::~CDUIAttriTabSelect(void)
{
}

LPVOID CDUIAttriTabSelect::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttriTabSelect, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

void CDUIAttriTabSelect::InitSelectItem()
{
	CDUIWndManager *pWndManager = GetOwnerWndManager();
	if (NULL == pWndManager) return;
	
	CMMString strDescribe = GetCurSelDescribe();

	RemoveAll();

	tagDuiCombox AttriCombox;
	AttriCombox.vecItem.push_back({ 0, _T("") });
	
	VecDuiControlBase vecControl = pWndManager->FindSubControlsByClass(pWndManager->GetRootCtrl(), CDUITabLayoutCtrl().GetClass());
	for (auto pControl : vecControl)
	{
		MMInterfaceHelper(CDUITabLayoutCtrl, pControl, pTabCtrl);
		if (NULL == pTabCtrl) continue;

		CMMString strCtrlID = CDUIGlobal::GetInstance()->FindCtrlID(pTabCtrl->GetCtrlID());
		if (strCtrlID.IsEmpty()) continue;

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