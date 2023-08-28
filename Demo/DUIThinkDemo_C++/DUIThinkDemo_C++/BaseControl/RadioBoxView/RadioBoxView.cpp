#include "StdAfx.h"
#include "RadioBoxView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CRadioBoxView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CRadioBoxView::CRadioBoxView()
{

}

CRadioBoxView::~CRadioBoxView()
{
	if (m_pRadioBoxViewCtrl)
	{
		if (m_pRadioBoxViewCtrl->GetWndManager())
		{
			m_pRadioBoxViewCtrl->GetWndManager()->RemoveINotify(this);
		}
	}

	return;
}

void CRadioBoxView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pRadioBoxViewCtrl = pViewBase;
	
	if (NULL == m_pRadioBoxViewCtrl) return;

	if (m_pRadioBoxViewCtrl->GetWndManager())
	{
		m_pRadioBoxViewCtrl->GetWndManager()->AddINotify(this);
	}

	//ctrl
	m_pRadioBox1Ctrl = MMInterfaceHelper(CDUIRadioBoxCtrl, m_pRadioBoxViewCtrl->FindSubControl(IDC_RadioBoxCtrl_100198));
	m_pRadioBox2Ctrl = MMInterfaceHelper(CDUIRadioBoxCtrl, m_pRadioBoxViewCtrl->FindSubControl(IDC_RadioBoxCtrl_100199));

	m_pRadioBox1Ctrl->SetShortcut(52);
	m_pRadioBox2Ctrl->SetShortcut(53);

	return;
}

void CRadioBoxView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////