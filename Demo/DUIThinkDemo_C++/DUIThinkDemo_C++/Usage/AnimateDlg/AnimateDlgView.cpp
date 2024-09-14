#include "StdAfx.h"
#include "AnimateDlgView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CAnimateDlgView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify_Ctrl(DuiNotify_ItemClick, IDC_ListViewAnimateDlg, OnDuiItemClickListViewAnimateDlg)
DuiEnd_Message_Map()

CAnimateDlgView::CAnimateDlgView()
{

}

CAnimateDlgView::~CAnimateDlgView()
{
	MMSafeDelete(m_pDlgAnimate);

	if (m_pAnimateDlgViewCtrl)
	{
		if (m_pAnimateDlgViewCtrl->GetWndOwner())
		{
			m_pAnimateDlgViewCtrl->GetWndOwner()->RemoveINotify(this);
		}
	}

	return;
}

void CAnimateDlgView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pAnimateDlgViewCtrl = pViewBase;

	if (NULL == m_pAnimateDlgViewCtrl) return;

	if (m_pAnimateDlgViewCtrl->GetWndOwner())
	{
		m_pAnimateDlgViewCtrl->GetWndOwner()->AddINotify(this);
	}

	//ctrl
	m_pListViewAnimateDlgCtrl = MMInterfaceHelper(CDUIListViewCtrl, m_pAnimateDlgViewCtrl->FindSubControl(IDC_ListViewAnimateDlg));

	return;
}

void CAnimateDlgView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

void CAnimateDlgView::OnDuiItemClickListViewAnimateDlg(const DuiNotify &Notify)
{
	if (NULL == m_pListViewAnimateDlgCtrl 
		|| NULL == m_pListViewAnimateDlgCtrl->GetWndOwner()
		|| m_pListViewAnimateDlgCtrl != Notify.pNotifyCtrl) return;

	MMSafeDelete(m_pDlgAnimate);

	enDuiAnimateWndType AnimateWndType = (enDuiAnimateWndType)(Notify.DuiNotifyExtend.ListView.nIndexItem + 1);
	m_pDlgAnimate = new CDlgDpi(_T("DlgDpi"), NULL, false);

	m_pDlgAnimate->SetAnimateWndType(AnimateWndType);
	m_pDlgAnimate->Create(NULL, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);

	return;
}

//////////////////////////////////////////////////////////////////////////