#include "framework.h"
#include "DlgDpiInfo.h"

//////////////////////////////////////////////////////////////////////////
#define Time_FindWnd_ID					(1989)
#define Time_AdjustWndPos_ID			(1990)

#define Time_FindWnd_Elapse				(100)
#define Time_AdjustWndPos_Elapse		(100)

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDlgDpiInfo)
DuiBegin_Message_Map(CDlgDpiInfo, CDUIWnd)
	Dui_On_Notify(DuiNotify_ValueChanged, OnDuiValueChanged)
	Dui_On_Notify(DuiNotify_TextChanged, OnDuiTextChanged)
DuiEnd_Message_Map()

//////////////////////////////////////////////////////////////////////////
CDlgDpiInfo::CDlgDpiInfo()
{

}

CDlgDpiInfo::~CDlgDpiInfo()
{

}

void CDlgDpiInfo::OnDuiTimer(const DuiNotify &Notify)
{

}

LRESULT CDlgDpiInfo::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDlgDpiInfo::OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	SetTimer(m_hWnd, Time_AdjustWndPos_ID, Time_AdjustWndPos_Elapse, NULL);

	return __super::OnCreate(wParam, lParam, bHandled);
}

LRESULT CDlgDpiInfo::OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (wParam)
	{
		case Time_FindWnd_ID:
		{
			PerformAttachWnd(FindWindow(m_strAttachWndClass, NULL));

			return 0;
		}
		case Time_AdjustWndPos_ID:
		{
			PerformAdjustWndPos();

			return 0;
		}
		default:
			break;
	}

	return __super::OnTimer(wParam, lParam, bHandled);
}

void CDlgDpiInfo::OnInitDialog()
{
	CDUIHorizontalLayoutCtrl *pRoot = new CDUIHorizontalLayoutCtrl;
	pRoot->Init();
	pRoot->SetBkColor({ Name_ColorWhite });

	//static
	tagDuiTextStyle TextStyle;
	TextStyle.dwTextStyle |= DT_TOP;
	m_pStaticDpiInfoCtrl = new CDUIStaticCtrl;
	m_pStaticDpiInfoCtrl->Init();
	m_pStaticDpiInfoCtrl->SetTextStyle(TextStyle);
	m_pStaticDpiInfoCtrl->SetAutoCalcWidth(true);
	m_pStaticDpiInfoCtrl->SetText(_T("DpiScale:"));
	pRoot->InsertChild(m_pStaticDpiInfoCtrl);

	//edit
	m_pEditDpiInfoCtrl = new CDUIThinkEditCtrl;
	m_pEditDpiInfoCtrl->Init();
	m_pEditDpiInfoCtrl->SetEditTextType(EditText_NumberInt);
	m_pEditDpiInfoCtrl->SetNumberMinLimit(100);
	m_pEditDpiInfoCtrl->SetNumberMaxLimit(300);
	m_pEditDpiInfoCtrl->SetPaddingL(5);
	m_pEditDpiInfoCtrl->SetBorderRect({ 1,1,1,1 });
	pRoot->InsertChild(m_pEditDpiInfoCtrl);

	m_pWndManager->AttachRootCtrl(pRoot);

	SetWindowPos(m_hWnd, NULL, 0, 0, Size_DlgDpiInfo.cx, Size_DlgDpiInfo.cy, SWP_NOMOVE | SWP_NOACTIVATE);

	return;
}

void CDlgDpiInfo::PerformAttachWndClass(CString strAttachWndClass)
{
	if (false == IsWindow(m_hWnd)) return;

	m_strAttachWndClass = strAttachWndClass;

	SetTimer(m_hWnd, Time_FindWnd_ID, Time_FindWnd_Elapse, NULL);

	PerformAttachWnd(FindWindow(m_strAttachWndClass, NULL));

	return;
}

void CDlgDpiInfo::PerformAttachWnd(HWND hWnd)
{
	if (NULL == m_pEditDpiInfoCtrl) return;

	if (IsWindow(hWnd))
	{
		m_hWndAttach = hWnd;
		m_pEditDpiInfoCtrl->SetText(_T("100"));
		SetWindowLongPtr(m_hWnd, GWLP_HWNDPARENT, (LONG)m_hWndAttach);

		KillTimer(m_hWnd, Time_FindWnd_ID);
	}

	return;
}

void CDlgDpiInfo::OnDuiValueChanged(const DuiNotify &Notify)
{
	return;
}

void CDlgDpiInfo::OnDuiTextChanged(const DuiNotify &Notify)
{
	if (NULL == m_pEditDpiInfoCtrl || Notify.pNotifyCtrl != m_pEditDpiInfoCtrl) return;

	int nScale = _tcstol(m_pEditDpiInfoCtrl->GetText(), NULL, 10);
	CDUIGlobal::GetInstance()->SetScale(nScale);

	return;
}

void CDlgDpiInfo::PerformAdjustWndPos()
{
	if (false == IsWindow(m_hWndAttach) || NULL == m_pWndManager) return;

	MMInterfaceHelper(CDUIContainerCtrl, m_pWndManager->GetRootCtrl(), pRoot);
	if (NULL == pRoot) return;

	CDUIRect rcWndAttach;
	GetWindowRect(m_hWndAttach, &rcWndAttach);

	CDUIRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	rcWnd.left = rcWndAttach.right;
	rcWnd.top = rcWndAttach.top;;
	SetWindowPos(m_hWnd, NULL, rcWnd.left, rcWnd.top, NULL, NULL, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);

	return;
}
