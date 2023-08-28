#include "StdAfx.h"
#include "ProgressView.h"

//////////////////////////////////////////////////////////////////////////
#define Timer_ProgressValue_ID			(1989)
#define Timer_ProgressValue_Elapse		(300)

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CProgressView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify(DuiNotify_Timer, OnDuiTimer)
DuiEnd_Message_Map()

CProgressView::CProgressView()
{

}

CProgressView::~CProgressView()
{
	if (m_pProgressViewCtrl)
	{
		if (m_pProgressViewCtrl->GetWndManager())
		{
			m_pProgressViewCtrl->GetWndManager()->RemoveINotify(this);
		}
	}

	return;
}

void CProgressView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pProgressViewCtrl = pViewBase;

	if (NULL == m_pProgressViewCtrl) return;

	if (m_pProgressViewCtrl->GetWndManager())
	{
		m_pProgressViewCtrl->GetWndManager()->AddINotify(this);
	}

	//horiz
	m_pHProgress1Ctrl = MMInterfaceHelper(CDUIProgressCtrl, m_pProgressViewCtrl->FindSubControl(IDC_ProgressCtrl_100242));
	m_pHProgress2Ctrl = MMInterfaceHelper(CDUIProgressCtrl, m_pProgressViewCtrl->FindSubControl(IDC_ProgressCtrl_100244));

	//vert
	m_pVProgress1Ctrl = MMInterfaceHelper(CDUIProgressCtrl, m_pProgressViewCtrl->FindSubControl(IDC_ProgressCtrl_100246));
	m_pVProgress2Ctrl = MMInterfaceHelper(CDUIProgressCtrl, m_pProgressViewCtrl->FindSubControl(IDC_ProgressCtrl_100247));

	m_pProgressViewCtrl->SetTimer(Timer_ProgressValue_ID, Timer_ProgressValue_Elapse);

	return;
}

void CProgressView::OnDuiWndInited(const DuiNotify &Notify)
{
	if (NULL == m_pProgressViewCtrl) return;

	return;
}

void CProgressView::OnDuiTimer(const DuiNotify &Notify)
{
	if (NULL == m_pProgressViewCtrl || m_pProgressViewCtrl != Notify.pNotifyCtrl) return;

	//horiz
	if (m_pHProgress1Ctrl)
	{
		m_pHProgress1Ctrl->SetCurValue((m_pHProgress1Ctrl->GetCurValue() + 10) % m_pHProgress1Ctrl->GetMaxValue());
	}
	if (m_pHProgress2Ctrl)
	{
		m_pHProgress2Ctrl->SetCurValue((m_pHProgress2Ctrl->GetCurValue() + 10) % m_pHProgress2Ctrl->GetMaxValue());
	}

	//vert
	if (m_pVProgress1Ctrl)
	{
		m_pVProgress1Ctrl->SetCurValue((m_pVProgress1Ctrl->GetCurValue() + 10) % m_pVProgress1Ctrl->GetMaxValue());
	}
	if (m_pVProgress2Ctrl)
	{
		m_pVProgress2Ctrl->SetCurValue((m_pVProgress2Ctrl->GetCurValue() + 10) % m_pVProgress2Ctrl->GetMaxValue());
	}

	return;
}

//////////////////////////////////////////////////////////////////////////