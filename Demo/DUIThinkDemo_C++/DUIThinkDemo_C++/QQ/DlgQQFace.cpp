#include "StdAfx.h"
#include "DlgQQFace.h"

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDlgQQFace)

DuiBegin_Message_Map(CDlgQQFace, CDUIWnd)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify_Ctrl(DuiNotify_ItemClick, IDC_ListViewFace, OnDuiItemClickListViewFace)
DuiEnd_Message_Map()

CDlgQQFace::CDlgQQFace(CDlgQQChat *pDlgQQChat, CDUIRect rcEventCtrl)
	: CDUIWnd(_T("DlgQQFace"))
	, m_pDlgQQChat(pDlgQQChat)
	, m_rcEventCtrl(rcEventCtrl)
{

}

CDlgQQFace::~CDlgQQFace()
{

}

void CDlgQQFace::OnSize(CDUIControlBase *pControl)
{
	return;
}

LRESULT CDlgQQFace::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KILLFOCUS:
		{
			if (m_hWnd != (HWND)wParam)
			{
				Close();
			}

			break;
		}
		default:
			break;
	}

	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDlgQQFace::OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	return __super::OnWndCustomMessage(uMsg, wParam, lParam, bHandled);
}

void CDlgQQFace::OnInitDialog()
{
	if (NULL == m_pWndManager) return;

	m_pListViewFaceCtrl = MMInterfaceHelper(CDUIListViewCtrl, m_pWndManager->FindControl(IDC_ListViewFace));

	//init
	InitInfo();

	SetFocus(m_hWnd);

	return;
}

void CDlgQQFace::OnDuiWndInited(const DuiNotify &Notify)
{
	PerformAdjustPos();

	return;
}

void CDlgQQFace::OnDuiItemClickListViewFace(const DuiNotify &Notify)
{
	if (NULL == m_pListViewFaceCtrl
		|| NULL == m_pDlgQQChat) return;

	CDUIListItemCtrl *pItem = m_pListViewFaceCtrl->GetChildAt(Notify.DUINotifyExtend.ListView.nIndexItem);
	if (NULL == pItem) return;

	CDUIAnimateCtrl *pAnimateCtrl = MMInterfaceHelper(CDUIAnimateCtrl, pItem->FindSubControl(IDC_AnimateFace));
	if (NULL == pAnimateCtrl) return;

	m_pDlgQQChat->InsertImage(pAnimateCtrl->GetImageSectionAnimate().vecImageResSwitch.front());

	Close();

	return;
}

void CDlgQQFace::InitInfo()
{
	if (NULL == m_pListViewFaceCtrl) return;

	CDUIListItemCtrl *pItem = m_pListViewFaceCtrl->GetChildAt(0);
	if (NULL == pItem) return;

	pItem = MMInterfaceHelper(CDUIListItemCtrl, pItem->Clone());
	if (NULL == pItem) return;

	m_pListViewFaceCtrl->RemoveAll();
	for (int n = 0; n < 135; n++)
	{
		CDUIListItemCtrl *pItemAdd = MMInterfaceHelper(CDUIListItemCtrl, pItem->Clone());
		if (NULL == pItemAdd) break;

		CDUIAnimateCtrl *pAnimateCtrl = MMInterfaceHelper(CDUIAnimateCtrl, pItemAdd->FindSubControl(IDC_AnimateFace));
		if (NULL == pAnimateCtrl) continue;

		CMMString strImageResName = CMMStrHelp::Format(_T("%d"), n);
		tagDuiImageSection ImageSection;
		ImageSection.vecImageResSwitch.push_back(strImageResName);
		pAnimateCtrl->SetImageSectionAnimate(ImageSection);

		m_pListViewFaceCtrl->InsertChild(pItemAdd);
	}

	MMSafeDelete(pItem);

	return;
}

void CDlgQQFace::PerformAdjustPos()
{
	if (NULL == m_pListViewFaceCtrl) return;

	CDUIRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	rcWnd.Offset(m_rcEventCtrl.left - rcWnd.left, m_rcEventCtrl.top - rcWnd.top);
	rcWnd.Offset(-(rcWnd.GetWidth() - m_rcEventCtrl.GetWidth()) / 2, -m_pListViewFaceCtrl->GetFixedHeight());
	SetWindowPos(m_hWnd, NULL, rcWnd.left, rcWnd.top, NULL, NULL, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

	return;
}