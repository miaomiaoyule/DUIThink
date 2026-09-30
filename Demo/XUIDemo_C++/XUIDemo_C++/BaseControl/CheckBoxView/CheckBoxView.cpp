#include "StdAfx.h"
#include "CheckBoxView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CCheckBoxView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CCheckBoxView::CCheckBoxView()
{

}

CCheckBoxView::~CCheckBoxView()
{
	if (m_pCheckBoxViewCtrl)
	{
		if (m_pCheckBoxViewCtrl->GetWndOwner())
		{
			m_pCheckBoxViewCtrl->GetWndOwner()->RemoveINotify(this);
		}
	}

	return;
}

void CCheckBoxView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pCheckBoxViewCtrl = pViewBase;

	if (NULL == m_pCheckBoxViewCtrl) return;

	if (m_pCheckBoxViewCtrl->GetWndOwner())
	{
		m_pCheckBoxViewCtrl->GetWndOwner()->AddINotify(this);
	}

	//ctrl
	m_pCheckBox1Ctrl = MMInterfaceHelper(CDUICheckBoxCtrl, m_pCheckBoxViewCtrl->FindSubControl(IDC_CheckBoxCtrl_100153));
	m_pCheckBox2Ctrl = MMInterfaceHelper(CDUICheckBoxCtrl, m_pCheckBoxViewCtrl->FindSubControl(IDC_CheckBoxCtrl_100155));

	m_pCheckBox1Ctrl->SetShortcut(50);
	m_pCheckBox2Ctrl->SetShortcut(51);

	return;
}

void CCheckBoxView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////