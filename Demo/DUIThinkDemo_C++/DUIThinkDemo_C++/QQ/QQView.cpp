#include "StdAfx.h"
#include "QQView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CQQView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Click_Ctrl(IDC_BtnQQChat, OnDuiClickBtnQQChat)
DuiEnd_Message_Map()

CQQView::CQQView()
{

}

CQQView::~CQQView()
{
	if (m_pQQViewCtrl)
	{
		if (m_pQQViewCtrl->GetWndOwner())
		{
			m_pQQViewCtrl->GetWndOwner()->RemoveINotify(this);
		}
	}

	return;
}

void CQQView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pQQViewCtrl = pViewBase;

	if (NULL == m_pQQViewCtrl) return;

	if (m_pQQViewCtrl->GetWndOwner())
	{
		m_pQQViewCtrl->GetWndOwner()->AddINotify(this);
	}

	//ctrl
	m_pBtnQQChatCtrl = MMInterfaceHelper(CDUIButtonCtrl, m_pQQViewCtrl->FindSubControl(IDC_BtnQQChat));

	return;
}

void CQQView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

void CQQView::OnDuiClickBtnQQChat(const DuiNotify &Notify)
{
	if (NULL == m_pQQViewCtrl 
		|| NULL == m_pQQViewCtrl->GetWndOwner()) return;

	CDlgQQChat DlgQQChat;
	DlgQQChat.Create(m_pQQViewCtrl->GetWndOwner()->GetWndHandle(), _T("DlgQQChat"), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	DlgQQChat.DoModal();

	return;
}

//////////////////////////////////////////////////////////////////////////