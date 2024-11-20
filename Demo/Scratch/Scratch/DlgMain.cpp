#include "framework.h"
#include "DlgMain.h"

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDlgMain)
DuiBegin_Message_Map(CDlgMain, CDUIWnd)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify(DuiNotify_WndLayout, OnDuiWndLayout)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizMove, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizRotateRight, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizRotateLeft, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizSayTime, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizSay, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizSoundPlus, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizSoundSet, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_HorizEventRun, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizMove, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizRotateRight, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizRotateLeft, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizSayTime, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizSay, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizSoundPlus, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizSoundSet, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_HorizEventRun, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizMove, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizRotateRight, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizRotateLeft, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizSayTime, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizSay, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizSoundPlus, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizSoundSet, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_HorizEventRun, OnDuiLButtonUp_DragEnd)
DuiEnd_Message_Map()

CDlgMain::CDlgMain()
	: CDUIWnd(_T("DlgMain"))
{
	return;
}

CDlgMain::~CDlgMain()
{
	return;
}

void CDlgMain::OnFindControl()
{
	Dui_DDX_Control(CDUIHorizontalLayoutCtrl, m_pHorizMoveCtrl, IDC_HorizMove);
	Dui_DDX_Control(CDUIContainerCtrl, m_pContainerEditCtrl, IDC_ContainerEdit);
	Dui_DDX_Control(CDUIVerticalLayoutCtrl, m_pVertControlsCtrl, IDC_VertControls);

	return;
}

void CDlgMain::OnInitDialog()
{
	return;
}

void CDlgMain::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

void CDlgMain::OnDuiWndLayout(const DuiNotify &Notify)
{
	return;
}

void CDlgMain::OnDuiLButtonDown_DragBegin(const DuiNotify &Notify)
{
	PerformDragBegin(Notify.pNotifyCtrl);

	return;
}

void CDlgMain::OnDuiMouseMove_DragMove(const DuiNotify &Notify)
{
	PerformDragMove();

	return;
}

void CDlgMain::OnDuiLButtonUp_DragEnd(const DuiNotify &Notify)
{
	PerformDragEnd();

	if (m_pContainerEditCtrl->VerifyChild(Notify.pNotifyCtrl)
		&& m_ptMouseDown == GetMousePosLast())
	{
		CMMString strCtrlID = CMMStrHelp::Format(_T("Click CtrlID:%d"), Notify.pNotifyCtrl->GetCtrlID());
		MessageBox(m_hWnd, strCtrlID, NULL, NULL);
	}

	return;
}

void CDlgMain::PerformDragBegin(CDUIControlBase *pDragCtrl)
{
	if (0 == (CDUIWnd::MapKeyState() & MK_LBUTTON)) return;

	//end
	PerformDragEnd();

	//info
	m_bDragFromControls = m_pVertControlsCtrl->VerifyChild(pDragCtrl);

	//clone
	if (m_bDragFromControls)
	{
		m_pDragingCtrl = pDragCtrl->Clone(true, false);
		m_pDragingCtrl->SetAbsoluteRect(pDragCtrl->GetAbsoluteRect());
		m_pRootCtrl->InsertChild(m_pDragingCtrl);
	}
	else
	{
		m_pDragingCtrl = pDragCtrl;
	}

	//pos
	m_ptMouseDown = GetMousePosLast();
	m_ptMouseLast = m_ptMouseDown;

	//magnet
	m_MagnetBox.SceneInit(MagnetType_Left | MagnetType_Right | MagnetType_Top | MagnetType_Bottom, 12);
	for (int n = 0; n < m_pContainerEditCtrl->GetChildCount(); n++)
	{
		CDUIControlBase *pChild = m_pContainerEditCtrl->GetChildAt(n);
		if (NULL == pChild || pChild == m_pDragingCtrl) continue;

		CDUIRect rcPos = pChild->GetAbsoluteRect();
		rcPos = PerformScaleBox(rcPos);
		m_MagnetBox.PushBox(n, rcPos);
	}

	CDUIRect rcPos = m_pDragingCtrl->GetAbsoluteRect();
	rcPos = PerformScaleBox(rcPos);
	m_MagnetBox.PushBox(-1, rcPos);

	//other drag
	if (false == m_bDragFromControls)
	{
		m_vecDragCtrl.clear();
		PerformDragAttachNearest(-1);
	}

	return;
}

void CDlgMain::PerformDragMove()
{
	if (NULL == m_pDragingCtrl) return;

	//move pos
	CDUIPoint ptMouse = GetMousePosLast();
	CDUIPoint ptOffset = { ptMouse.x - m_ptMouseLast.x, ptMouse.y - m_ptMouseLast.y };
	m_ptMouseLast = ptMouse;

	//magnet
	CDUIRect rcPos = m_MagnetBox.MoveBox(-1, ptOffset);

	//pos
	CDUIRect rcDragingCtrl = m_pDragingCtrl->GetAbsoluteRect();
	rcPos = PerformRestoreBox(rcPos);
	rcPos.right = rcPos.left + rcDragingCtrl.GetWidth();
	m_pDragingCtrl->SetAbsoluteRect(rcPos);

	//other pos
	ptOffset = { rcPos.left - rcDragingCtrl.left, rcPos.top - rcDragingCtrl.top };
	for (auto pDragCtrl : m_vecDragCtrl)
	{
		CDUIRect rcDragCtrl = pDragCtrl->GetAbsoluteRect();
		rcDragCtrl.Offset(ptOffset.x, ptOffset.y);
		pDragCtrl->SetAbsoluteRect(rcDragCtrl);

		//update magnet info
		rcDragCtrl = PerformScaleBox(rcDragCtrl);
		m_MagnetBox.PushBox(m_pContainerEditCtrl->GetChildIndex(pDragCtrl), rcDragCtrl);
	}

	return;
}

void CDlgMain::PerformDragEnd()
{
	//add control
	if (m_pDragingCtrl && m_bDragFromControls)
	{
		CDUIRect rcPos = m_pDragingCtrl->GetAbsoluteRect();
		CDUIControlBase *pDragCtrl = m_pDragingCtrl->Clone(true, false);
		m_pContainerEditCtrl->InsertChild(pDragCtrl);
		m_pContainerEditCtrl->RefreshView();
		pDragCtrl->SetAbsoluteRect(rcPos);

		m_pRootCtrl->Remove(m_pDragingCtrl);
	}

	//info
	m_pDragingCtrl = NULL;
	m_vecDragCtrl.clear();

	return;
}

void CDlgMain::PerformDragAttachNearest(int nIndexNearest)
{
	CMMMagnetBox::tagFindMagnetBox FindMagnetBox = m_MagnetBox.GetNearestMagnetBox(nIndexNearest);
	if (-1 != FindMagnetBox.rcBoxBottom.nIndex)
	{
		CDUIControlBase *pChild = m_pContainerEditCtrl->GetChildAt(FindMagnetBox.rcBoxBottom.nIndex);
		if (find(m_vecDragCtrl.begin(), m_vecDragCtrl.end(), pChild) == m_vecDragCtrl.end())
		{
			m_vecDragCtrl.push_back(pChild);

			PerformDragAttachNearest(FindMagnetBox.rcBoxBottom.nIndex);
		}
	}

	return;
}

CDUIRect CDlgMain::PerformScaleBox(CDUIRect rcBox)
{
	rcBox.left += 25;
	rcBox.right = rcBox.left;
	rcBox.bottom -= 12;

	return rcBox;
}

CDUIRect CDlgMain::PerformRestoreBox(CDUIRect rcBox)
{
	rcBox.left -= 25;
	rcBox.bottom += 12;

	return rcBox;
}