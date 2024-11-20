#include "framework.h"
#include "DlgMain.h"

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDlgMain)
DuiBegin_Message_Map(CDlgMain, CDUIWnd)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify(DuiNotify_WndLayout, OnDuiWndLayout)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_RadioBoxStart, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_RadioBoxFinish, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_RadioBoxInput, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_RadioBoxOutput, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_RadioBoxOperator, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_RadioBoxJudge, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ContainerStart, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ContainerFinish, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ContainerInput, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ContainerOutput, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ContainerOperator, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ContainerJudge, OnDuiLButtonDown_DragBegin)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ControlLinkLeft, OnDuiLButtonDown_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ControlLinkTop, OnDuiLButtonDown_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ControlLinkRight, OnDuiLButtonDown_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonDown, IDC_ControlLinkBottom, OnDuiLButtonDown_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_RadioBoxStart, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_RadioBoxFinish, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_RadioBoxInput, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_RadioBoxOutput, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_RadioBoxOperator, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_RadioBoxJudge, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ContainerStart, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ContainerFinish, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ContainerInput, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ContainerOutput, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ContainerOperator, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ContainerJudge, OnDuiMouseMove_DragMove)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ControlLinkLeft, OnDuiMouseMove_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ControlLinkTop, OnDuiMouseMove_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ControlLinkRight, OnDuiMouseMove_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_MouseMove, IDC_ControlLinkBottom, OnDuiMouseMove_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_RadioBoxStart, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_RadioBoxFinish, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_RadioBoxInput, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_RadioBoxOutput, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_RadioBoxOperator, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_RadioBoxJudge, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ContainerStart, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ContainerFinish, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ContainerInput, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ContainerOutput, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ContainerOperator, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ContainerJudge, OnDuiLButtonUp_DragEnd)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ControlLinkLeft, OnDuiLButtonUp_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ControlLinkTop, OnDuiLButtonUp_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ControlLinkRight, OnDuiLButtonUp_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_LButtonUp, IDC_ControlLinkBottom, OnDuiLButtonUp_IDC_ControlLink)
	Dui_On_Notify_Ctrl(DuiNotify_SetFocus, IDC_ContainerStart, OnDuiSetFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_SetFocus, IDC_ContainerFinish, OnDuiSetFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_SetFocus, IDC_ContainerInput, OnDuiSetFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_SetFocus, IDC_ContainerOutput, OnDuiSetFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_SetFocus, IDC_ContainerOperator, OnDuiSetFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_SetFocus, IDC_ContainerJudge, OnDuiSetFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_KillFocus, IDC_ContainerStart, OnDuiKillFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_KillFocus, IDC_ContainerFinish, OnDuiKillFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_KillFocus, IDC_ContainerInput, OnDuiKillFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_KillFocus, IDC_ContainerOutput, OnDuiKillFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_KillFocus, IDC_ContainerOperator, OnDuiKillFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_KillFocus, IDC_ContainerJudge, OnDuiKillFocus_DragCtrl)
	Dui_On_Notify_Ctrl(DuiNotify_Click, IDC_BtnStart, OnDuiClick_IDC_BtnStart)
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
	Dui_DDX_Control(CDUIContainerCtrl, m_pContainerEditCtrl, IDC_ContainerEdit);
	Dui_DDX_Control(CDUIVerticalLayoutCtrl, m_pVertControlsCtrl, IDC_VertControls);
	Dui_DDX_Control(CDUIRadioBoxCtrl, m_pRadioBoxStartCtrl, IDC_RadioBoxStart);
	Dui_DDX_Control(CDUIRadioBoxCtrl, m_pRadioBoxFinishCtrl, IDC_RadioBoxFinish);
	Dui_DDX_Control(CDUIRadioBoxCtrl, m_pRadioBoxInputCtrl, IDC_RadioBoxInput);
	Dui_DDX_Control(CDUIRadioBoxCtrl, m_pRadioBoxOutputCtrl, IDC_RadioBoxOutput);
	Dui_DDX_Control(CDUIRadioBoxCtrl, m_pRadioBoxOperatorCtrl, IDC_RadioBoxOperator);
	Dui_DDX_Control(CDUIRadioBoxCtrl, m_pRadioBoxJudgeCtrl, IDC_RadioBoxJudge);

	return;
}

void CDlgMain::OnInitDialog()
{
	CDUIContainerCtrl *pContainerCtrl = NULL;
	Dui_DDX_Control(CDUIContainerCtrl, pContainerCtrl, IDC_ContainerStart);
	m_pContainerStartModelCtrl = pContainerCtrl->Clone(true, false);

	Dui_DDX_Control(CDUIContainerCtrl, pContainerCtrl, IDC_ContainerFinish);
	m_pContainerFinishModelCtrl = pContainerCtrl->Clone(true, false);

	Dui_DDX_Control(CDUIContainerCtrl, pContainerCtrl, IDC_ContainerInput);
	m_pContainerInputModelCtrl = pContainerCtrl->Clone(true, false);

	Dui_DDX_Control(CDUIContainerCtrl, pContainerCtrl, IDC_ContainerOutput);
	m_pContainerOutputModelCtrl = pContainerCtrl->Clone(true, false);

	Dui_DDX_Control(CDUIContainerCtrl, pContainerCtrl, IDC_ContainerOperator);
	m_pContainerOperatorModelCtrl = pContainerCtrl->Clone(true, false);

	Dui_DDX_Control(CDUIContainerCtrl, pContainerCtrl, IDC_ContainerJudge);
	m_pContainerJudgeModelCtrl = pContainerCtrl->Clone(true, false);

	m_pContainerEditCtrl->RemoveAll();
	m_pContainerEditCtrl->RegisterControlCallBack(this);

	return;
}

void CDlgMain::OnPaint(CDUIControlBase *pControl, HDC hDC)
{
	if (pControl == m_pContainerEditCtrl)
	{
		//linked
		auto mapLinkData = m_mapLinkData;
		for (auto LinkDataItem : mapLinkData)
		{
			CDUIContainerCtrl *pContainerFromCtrl = LinkDataItem.first;

			//verify from
			if (-1 == m_pContainerEditCtrl->GetChildIndex(pContainerFromCtrl))
			{
				m_mapLinkData.erase(pContainerFromCtrl);

				continue;
			}

			//link target
			auto LinkData = LinkDataItem.second;
			for (int n = 0; n < 4; n++)
			{
				CDUIContainerCtrl *pContainerTargetCtrl = NULL;
				CDUIControlBase *pLinkFromCtrl = NULL;
				tagLinkTarget LinkTarget = {};

				if (0 == n)
				{
					LinkTarget = LinkData.LinkLeft;
					pContainerTargetCtrl = LinkTarget.pLinkCtrl;
					pLinkFromCtrl = pContainerFromCtrl->FindSubControl(IDC_ControlLinkLeft);
				}
				else if (1 == n)
				{
					LinkTarget = LinkData.LinkTop;
					pContainerTargetCtrl = LinkTarget.pLinkCtrl;
					pLinkFromCtrl = pContainerFromCtrl->FindSubControl(IDC_ControlLinkTop);
				}
				else if (2 == n)
				{
					LinkTarget = LinkData.LinkRight;
					pContainerTargetCtrl = LinkTarget.pLinkCtrl;
					pLinkFromCtrl = pContainerFromCtrl->FindSubControl(IDC_ControlLinkRight);
				}
				else if (3 == n)
				{
					LinkTarget = LinkData.LinkBottom;
					pContainerTargetCtrl = LinkTarget.pLinkCtrl;
					pLinkFromCtrl = pContainerFromCtrl->FindSubControl(IDC_ControlLinkBottom);
				}
				if (NULL == pLinkFromCtrl)
				{
					continue;
				}
				if (-1 == m_pContainerEditCtrl->GetChildIndex(pContainerTargetCtrl))
				{
					m_mapLinkData.erase(pContainerTargetCtrl);

					continue;
				}

				//verify target
				CDUIControlBase *pLinkTargetCtrl = NULL;
				if (Link_Left == LinkTarget.LinkType)
				{
					pLinkTargetCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkLeft);
				}
				else if (Link_Top == LinkTarget.LinkType)
				{
					pLinkTargetCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkTop);
				}
				else if (Link_Right == LinkTarget.LinkType)
				{
					pLinkTargetCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkRight);
				}
				else if (Link_Bottom == LinkTarget.LinkType)
				{
					pLinkTargetCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkBottom);
				}
				if (NULL == pLinkTargetCtrl)
				{
					m_mapLinkData.erase(pContainerTargetCtrl);

					continue;
				}

				CDUIRect rcLinkFrom = pLinkFromCtrl->GetAbsoluteRect();
				CDUIRect rcLinkTarget = pLinkTargetCtrl->GetAbsoluteRect();
				CDUIPoint ptFrom(rcLinkFrom.left + rcLinkFrom.GetWidth() / 2, rcLinkFrom.top + rcLinkFrom.GetHeight() / 2);
				CDUIPoint ptTarget(rcLinkTarget.left + rcLinkTarget.GetWidth() / 2, Link_Top == LinkTarget.LinkType ? rcLinkTarget.top : rcLinkTarget.bottom);

				PerformDrawArrow(hDC, ptFrom, ptTarget);
			}
		}

		//linking
		if (m_pLinkingCtrl)
		{
			CDUIControlBase *pLinkFromCtrl = NULL;
			if (Link_Left == m_LinkingFrom)
			{
				pLinkFromCtrl = m_pLinkingCtrl->FindSubControl(IDC_ControlLinkLeft);
			}
			else if (Link_Top == m_LinkingFrom)
			{
				pLinkFromCtrl = m_pLinkingCtrl->FindSubControl(IDC_ControlLinkTop);
			}
			else if (Link_Right == m_LinkingFrom)
			{
				pLinkFromCtrl = m_pLinkingCtrl->FindSubControl(IDC_ControlLinkRight);
			}
			else if (Link_Bottom == m_LinkingFrom)
			{
				pLinkFromCtrl = m_pLinkingCtrl->FindSubControl(IDC_ControlLinkBottom);
			}
			if (pLinkFromCtrl)
			{
				CDUIRect rcLinkFrom = pLinkFromCtrl->GetAbsoluteRect();
				CDUIPoint ptFrom(rcLinkFrom.left + rcLinkFrom.GetWidth() / 2, rcLinkFrom.top + rcLinkFrom.GetHeight() / 2);
				CDUIPoint ptTarget = GetMousePosLast();

				PerformDrawArrow(hDC, ptFrom, ptTarget);
			}
		}

		return;
	}

	return;
}

void CDlgMain::OnRelease(CDUIControlBase *pControl)
{
	m_mapLinkData.erase(MMDynamicPtr(CDUIContainerCtrl, pControl));

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

void CDlgMain::OnDuiLButtonDown_IDC_ControlLink(const DuiNotify &Notify)
{
	MMInterfaceHelper(CDUIContainerCtrl, Notify.pNotifyCtrl->GetParent(), pContainerFromCtrl);
	if (NULL == pContainerFromCtrl) return;

	m_pLinkingCtrl = pContainerFromCtrl;
	if (IDC_ControlLinkLeft == Notify.uCtrlID)
	{
		m_LinkingFrom = Link_Left;
	}
	else if (IDC_ControlLinkTop == Notify.uCtrlID)
	{
		m_LinkingFrom = Link_Top;
	}
	else if (IDC_ControlLinkRight == Notify.uCtrlID)
	{
		m_LinkingFrom = Link_Right;
	}
	else if (IDC_ControlLinkBottom == Notify.uCtrlID)
	{
		m_LinkingFrom = Link_Bottom;
	}
	
	m_pLinkingCtrl->RegisterControlCallBack(this);

	return;
}

void CDlgMain::OnDuiMouseMove_DragMove(const DuiNotify &Notify)
{
	PerformDragMove();

	return;
}

void CDlgMain::OnDuiMouseMove_IDC_ControlLink(const DuiNotify &Notify)
{
	if (NULL == m_pLinkingCtrl) return;

	m_pContainerEditCtrl->Invalidate();

	return;
}

void CDlgMain::OnDuiLButtonUp_DragEnd(const DuiNotify &Notify)
{
	PerformDragEnd();

	if (m_pContainerEditCtrl->VerifyChild(Notify.pNotifyCtrl)
		&& m_ptMouseDown == GetMousePosLast())
	{
		CMMString strCtrlID = CMMStrHelp::Format(_T("Click CtrlID:%d"), Notify.pNotifyCtrl->GetCtrlID());
		//MessageBox(m_hWnd, strCtrlID, NULL, NULL);
	}

	return;
}

void CDlgMain::OnDuiLButtonUp_IDC_ControlLink(const DuiNotify &Notify)
{
	do
	{
		if (NULL == m_pLinkingCtrl) break;

		//target container
		CDUIControlBase *pLinkTargetCtrl = FindControl(Notify.ptMouse);
		if (NULL == pLinkTargetCtrl) break;

		CDUIContainerCtrl *pContainerTargetCtrl = MMDynamicPtr(CDUIContainerCtrl, pLinkTargetCtrl->GetParent());
		if (NULL == pContainerTargetCtrl
			|| pContainerTargetCtrl == m_pLinkingCtrl
			|| -1 == m_pContainerEditCtrl->GetChildIndex(pContainerTargetCtrl)) break;

		//target ctrl
		CDUIControlBase *pLinkLeftCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkLeft);
		CDUIControlBase *pLinkTopCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkTop);
		CDUIControlBase *pLinkRightCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkRight);
		CDUIControlBase *pLinkBottomCtrl = pContainerTargetCtrl->FindSubControl(IDC_ControlLinkBottom);
		CDUIRect rcLinkLeft = pLinkLeftCtrl ? pLinkLeftCtrl->GetAbsoluteRect() : CDUIRect();
		CDUIRect rcLinkTop = pLinkTopCtrl ? pLinkTopCtrl->GetAbsoluteRect() : CDUIRect();
		CDUIRect rcLinkRight = pLinkRightCtrl ? pLinkRightCtrl->GetAbsoluteRect() : CDUIRect();
		CDUIRect rcLinkBottom = pLinkBottomCtrl ? pLinkBottomCtrl->GetAbsoluteRect() : CDUIRect();

		//target type
		enLinkType LinkTarget = Link_Left;
		if (rcLinkLeft.PtInRect(Notify.ptMouse))
		{
			LinkTarget = Link_Left;
		}
		else if (rcLinkTop.PtInRect(Notify.ptMouse))
		{
			LinkTarget = Link_Top;
		}
		else if (rcLinkRight.PtInRect(Notify.ptMouse))
		{
			LinkTarget = Link_Right;
		}
		else if (rcLinkBottom.PtInRect(Notify.ptMouse))
		{
			LinkTarget = Link_Bottom;
		}

		if (Link_Left == m_LinkingFrom)
		{
			m_mapLinkData[m_pLinkingCtrl].LinkLeft.pLinkCtrl = pContainerTargetCtrl;
			m_mapLinkData[m_pLinkingCtrl].LinkLeft.LinkType = LinkTarget;
		}
		else if (Link_Top == m_LinkingFrom)
		{
			m_mapLinkData[m_pLinkingCtrl].LinkTop.pLinkCtrl = pContainerTargetCtrl;
			m_mapLinkData[m_pLinkingCtrl].LinkTop.LinkType = LinkTarget;
		}
		else if (Link_Right == m_LinkingFrom)
		{
			m_mapLinkData[m_pLinkingCtrl].LinkRight.pLinkCtrl = pContainerTargetCtrl;
			m_mapLinkData[m_pLinkingCtrl].LinkRight.LinkType = LinkTarget;
		}
		else if (Link_Bottom == m_LinkingFrom)
		{
			m_mapLinkData[m_pLinkingCtrl].LinkBottom.pLinkCtrl = pContainerTargetCtrl;
			m_mapLinkData[m_pLinkingCtrl].LinkBottom.LinkType = LinkTarget;
		}

	} while (false);
	
	m_pLinkingCtrl = NULL;
	m_LinkingFrom = Link_Top;

	Invalidate();

	return;
}

void CDlgMain::OnDuiSetFocus_DragCtrl(const DuiNotify &Notify)
{
	if (-1 == m_pContainerEditCtrl->GetChildIndex(Notify.pNotifyCtrl)) return;

	MMInterfaceHelper(CDUIContainerCtrl, Notify.pNotifyCtrl, pContainerDragCtrl);
	if (NULL == pContainerDragCtrl) return;

	MMInterfaceHelper(CDUIContainerCtrl, pContainerDragCtrl->FindSubControl(IDC_ContainerSign), pContainerSignCtrl);
	if (NULL == pContainerSignCtrl) return;

	pContainerSignCtrl->SetVisible(true);

	return;
}

void CDlgMain::OnDuiKillFocus_DragCtrl(const DuiNotify &Notify)
{
	if (-1 == m_pContainerEditCtrl->GetChildIndex(Notify.pNotifyCtrl)) return;

	MMInterfaceHelper(CDUIContainerCtrl, Notify.pNotifyCtrl, pContainerDragCtrl);
	if (NULL == pContainerDragCtrl) return;

	MMInterfaceHelper(CDUIContainerCtrl, pContainerDragCtrl->FindSubControl(IDC_ContainerSign), pContainerSignCtrl);
	if (NULL == pContainerSignCtrl) return;

	pContainerSignCtrl->SetVisible(false);

	return;
}

void CDlgMain::OnDuiClick_IDC_BtnStart(const DuiNotify &Notify)
{
	if (NULL == m_pContainerEditCtrl) return;

	CDUIContainerCtrl *pContainerStartCtrl = MMDynamicPtr(CDUIContainerCtrl, m_pContainerEditCtrl->FindSubControl(IDC_ContainerStart));
	if (NULL == pContainerStartCtrl);

	PerformExecuteModule(pContainerStartCtrl);

	return;
}

void CDlgMain::PerformDragBegin(CDUIControlBase *pDragCtrl)
{
	if (0 == (CDUIWnd::MapKeyState() & MK_LBUTTON)) return;

	//ptTarget
	PerformDragEnd();

	//info
	m_bDragFromControls = m_pVertControlsCtrl->VerifyChild(pDragCtrl);

	//clone
	if (m_bDragFromControls)
	{
		if (m_pRadioBoxStartCtrl == pDragCtrl)
		{
			m_pDragingCtrl = m_pContainerStartModelCtrl->Clone(true, false);
		}
		else if (m_pRadioBoxFinishCtrl == pDragCtrl)
		{
			m_pDragingCtrl = m_pContainerFinishModelCtrl->Clone(true, false);
		}
		else if (m_pRadioBoxInputCtrl == pDragCtrl)
		{
			m_pDragingCtrl = m_pContainerInputModelCtrl->Clone(true, false);
		}
		else if (m_pRadioBoxOutputCtrl == pDragCtrl)
		{
			m_pDragingCtrl = m_pContainerOutputModelCtrl->Clone(true, false);
		}
		else if (m_pRadioBoxOperatorCtrl == pDragCtrl)
		{
			m_pDragingCtrl = m_pContainerOperatorModelCtrl->Clone(true, false);
		}
		else if (m_pRadioBoxJudgeCtrl == pDragCtrl)
		{
			m_pDragingCtrl = m_pContainerJudgeModelCtrl->Clone(true, false);
		}
		
		CDUIRect rcPos = pDragCtrl->GetAbsoluteRect();
		rcPos.right = rcPos.left + m_pDragingCtrl->GetFixedWidth();
		rcPos.bottom = rcPos.top + m_pDragingCtrl->GetFixedHeight();
		m_pDragingCtrl->SetAbsoluteRect(rcPos);
		m_pRootCtrl->InsertChild(m_pDragingCtrl);
	}
	else
	{
		m_pDragingCtrl = pDragCtrl;
	}

	//pos
	m_ptMouseDown = GetMousePosLast();
	m_ptMouseLast = m_ptMouseDown;

	//other drag
	if (false == m_bDragFromControls)
	{
		m_vecDragCtrl.clear();
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

	//pos
	CDUIRect rcPos = m_pDragingCtrl->GetAbsoluteRect();
	rcPos.Offset(ptOffset.x, ptOffset.y);
	m_pDragingCtrl->SetAbsoluteRect(rcPos);

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
		pDragCtrl->RegisterControlCallBack(this);
		pDragCtrl->SetFocus();

		m_pRootCtrl->Remove(m_pDragingCtrl);
	}

	//info
	m_pDragingCtrl = NULL;
	m_vecDragCtrl.clear();

	return;
}

void CDlgMain::PerformDrawArrow(HDC hDC, CDUIPoint ptFrom, CDUIPoint ptTarget)
{
	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetCompositingQuality(CompositingQuality::CompositingQualityHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::PointF start(ptFrom.x, ptFrom.y);
	Gdiplus::PointF end(ptTarget.x, ptTarget.y);

	// 绘制主线
	Pen pen(Color(255, 0, 0, 0), 2); // 红色箭头，宽度为5
	Gp.DrawLine(&pen, start, end);

	// 计算箭头的方向
	float angle = atan2(end.Y - start.Y, end.X - start.X);
	float arrowLength = 10.0f; // 箭头的长度
	float arrowAngle = 30.0f; // 箭头的角度

	// 计算箭头的两个部分
	PointF arrowPoint1(end.X - arrowLength * cos(angle - arrowAngle * M_PI / 180),
		end.Y - arrowLength * sin(angle - arrowAngle * M_PI / 180));

	PointF arrowPoint2(end.X - arrowLength * cos(angle + arrowAngle * M_PI / 180),
		end.Y - arrowLength * sin(angle + arrowAngle * M_PI / 180));

	// 绘制箭头的两个部分
	Gp.DrawLine(&pen, end, arrowPoint1);
	Gp.DrawLine(&pen, end, arrowPoint2);

	return;
}

void CDlgMain::PerformExecuteModule(CDUIContainerCtrl *pContainerFromCtrl)
{
	if (NULL == pContainerFromCtrl);

	auto FindIt = m_mapLinkData.find(pContainerFromCtrl);
	if (FindIt == m_mapLinkData.end()) return;

	auto vecFindCtrl = FindSubControlsByClass(pContainerFromCtrl, CDUIStaticCtrl().GetClass());
	if (vecFindCtrl.empty()) return;

	CDUIStaticCtrl *pStaticTextCtrl = MMDynamicPtr(CDUIStaticCtrl, vecFindCtrl.front());
	if (NULL == pStaticTextCtrl) return;

	MessageBox(m_hWnd, pStaticTextCtrl->GetText(), _T("执行到"), NULL);

	auto LinkData = m_mapLinkData[pContainerFromCtrl];
	PerformExecuteModule(LinkData.LinkLeft.pLinkCtrl);
	PerformExecuteModule(LinkData.LinkTop.pLinkCtrl);
	PerformExecuteModule(LinkData.LinkRight.pLinkCtrl);
	PerformExecuteModule(LinkData.LinkBottom.pLinkCtrl);

	return;
}