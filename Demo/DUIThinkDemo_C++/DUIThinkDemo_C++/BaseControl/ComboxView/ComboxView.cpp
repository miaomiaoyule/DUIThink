#include "StdAfx.h"
#include "ComboxView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CComboxView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify_Ctrl(DuiNotify_ItemSelected, IDC_ComboxCtrl_100280, OnDuiItemSelectedCombox1)
	Dui_On_Notify_Ctrl(DuiNotify_ItemSelected, IDC_ComboxCtrl_100282, OnDuiItemSelectedCombox2)
	Dui_On_Notify_Ctrl(DuiNotify_ItemSelected, IDC_ComboxCtrl_100293, OnDuiItemSelectedCombox3)
	Dui_On_Notify_Ctrl(DuiNotify_ItemSelected, IDC_ComboxCtrl_100299, OnDuiItemSelectedCombox4)
	Dui_On_Click_Ctrl(IDC_ButtonCtrl_100292, OnDuiClickBtnRemoveCombox2)
DuiEnd_Message_Map()

CComboxView::CComboxView()
{

}

CComboxView::~CComboxView()
{
	if (m_pComboxViewCtrl)
	{
		if (m_pComboxViewCtrl->GetWndOwner())
		{
			m_pComboxViewCtrl->GetWndOwner()->RemoveINotify(this);
		}
	}

	return;
}

void CComboxView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pComboxViewCtrl = pViewBase;
	
	if (NULL == m_pComboxViewCtrl) return;

	if (m_pComboxViewCtrl->GetWndOwner())
	{
		m_pComboxViewCtrl->GetWndOwner()->AddINotify(this);
	}

	//ctrl
	m_pCombox1Ctrl = MMInterfaceHelper(CDUIComboxCtrl, m_pComboxViewCtrl->FindSubControl(IDC_ComboxCtrl_100280));
	m_pCombox2Ctrl = MMInterfaceHelper(CDUIComboxCtrl, m_pComboxViewCtrl->FindSubControl(IDC_ComboxCtrl_100282));
	m_pCombox3Ctrl = MMInterfaceHelper(CDUIComboxCtrl, m_pComboxViewCtrl->FindSubControl(IDC_ComboxCtrl_100293));
	m_pCombox4Ctrl = MMInterfaceHelper(CDUIComboxCtrl, m_pComboxViewCtrl->FindSubControl(IDC_ComboxCtrl_100299));

	return;
}

void CComboxView::OnDuiWndInited(const DuiNotify &Notify)
{
	if (NULL == m_pComboxViewCtrl) return;

	return;
}

void CComboxView::OnDuiItemSelectedCombox1(const DuiNotify &Notify)
{
	if (NULL == m_pCombox1Ctrl 
		|| m_pCombox1Ctrl != Notify.pNotifyCtrl) return;

	CDUIListItemCtrl *pItem = MMInterfaceHelper(CDUIListItemCtrl, m_pCombox1Ctrl->GetChildAt(Notify.DuiNotifyExtend.Combox.nIndexItem));
	if (NULL == pItem) return;

	return;
}

void CComboxView::OnDuiItemSelectedCombox2(const DuiNotify &Notify)
{
	if (NULL == m_pCombox2Ctrl
		|| m_pCombox2Ctrl != Notify.pNotifyCtrl) return;

	CDUIListItemCtrl *pItem = MMInterfaceHelper(CDUIListItemCtrl, m_pCombox2Ctrl->GetChildAt(Notify.DuiNotifyExtend.Combox.nIndexItem));
	if (NULL == pItem) return;

	MMInterfaceHelper(CDUIStaticCtrl, pItem->FindSubControl(IDC_StaticCtrl_100290), pStaticItemTextCtrl);
	if (NULL == pStaticItemTextCtrl) return;

	m_pCombox2Ctrl->SetText(pStaticItemTextCtrl->GetText());

	return;
}

void CComboxView::OnDuiItemSelectedCombox3(const DuiNotify &Notify)
{
	if (NULL == m_pCombox3Ctrl
		|| m_pCombox3Ctrl != Notify.pNotifyCtrl) return;

	CDUIListItemCtrl *pItem = MMInterfaceHelper(CDUIListItemCtrl, m_pCombox3Ctrl->GetChildAt(Notify.DuiNotifyExtend.Combox.nIndexItem));
	if (NULL == pItem) return;

	return;
}

void CComboxView::OnDuiItemSelectedCombox4(const DuiNotify &Notify)
{
	if (NULL == m_pCombox4Ctrl
		|| NULL == m_pCombox4Ctrl->GetParent()
		|| m_pCombox4Ctrl != Notify.pNotifyCtrl) return;

	CDUIListItemCtrl *pItem = MMInterfaceHelper(CDUIListItemCtrl, m_pCombox4Ctrl->GetChildAt(Notify.DuiNotifyExtend.Combox.nIndexItem));
	if (NULL == pItem) return;

	m_pCombox4Ctrl->SetForeImageSection(pItem->GetForeImageSection());

	return;
}

void CComboxView::OnDuiClickBtnRemoveCombox2(const DuiNotify &Notify)
{
	if (NULL == m_pCombox2Ctrl
		|| Notify.DuiNotifyExtend.Combox.pComboxCtrl != m_pCombox2Ctrl) return;

	for (int n = 0; n < m_pCombox2Ctrl->GetChildCount(); n++)
	{
		MMInterfaceHelper(CDUIListItemCtrl, m_pCombox2Ctrl->GetChildAt(n), pItem);
		if (NULL == pItem) continue;

		if (false == pItem->VerifyChild(Notify.pNotifyCtrl)) continue;

		m_pCombox2Ctrl->RemoveAt(n);

		break;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////