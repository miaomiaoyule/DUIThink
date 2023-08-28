#include "StdAfx.h"
#include "DpiDlgView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CDpiDlgView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Click_Ctrl(IDC_BtnDpiDlg1, OnDuiClickDpiDlg1)
	Dui_On_Notify_Ctrl(DuiNotify_ValueChanged, IDC_SliderDpiScale, OnDuiValueChangedDpiScale)
DuiEnd_Message_Map()

CDpiDlgView::CDpiDlgView()
{

}

CDpiDlgView::~CDpiDlgView()
{
	if (m_pDpiDlgViewCtrl)
	{
		if (m_pDpiDlgViewCtrl->GetWndManager())
		{
			m_pDpiDlgViewCtrl->GetWndManager()->RemoveINotify(this);
		}
	}

	return;
}

void CDpiDlgView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pDpiDlgViewCtrl = pViewBase;

	if (NULL == m_pDpiDlgViewCtrl) return;

	if (m_pDpiDlgViewCtrl->GetWndManager())
	{
		m_pDpiDlgViewCtrl->GetWndManager()->AddINotify(this);
	}

	//ctrl
	m_pBtnDpiDlg1Ctrl = MMInterfaceHelper(CDUIButtonCtrl, m_pDpiDlgViewCtrl->FindSubControl(IDC_BtnDpiDlg1));
	m_pSliderDpiScaleCtrl = MMInterfaceHelper(CDUISliderCtrl, m_pDpiDlgViewCtrl->FindSubControl(IDC_SliderDpiScale));

	return;
}

void CDpiDlgView::SetDpiDlgHandle(HWND hWndDpiDlg)
{
	m_hWndDpiDlg = hWndDpiDlg;

	return;
}

void CDpiDlgView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

void CDpiDlgView::OnDuiClickDpiDlg1(const DuiNotify &Notify)
{
	if (NULL == m_pDpiDlgViewCtrl || NULL == m_pDpiDlgViewCtrl->GetWndManager()) return;

	auto pIWndManager = m_pDpiDlgViewCtrl->GetWndManager();

	CMMString strCmdLine;
	strCmdLine = strCmdLine + _T(" /") + Name_DpiDlgKey + _T(" DlgDpi");
	strCmdLine = strCmdLine + _T(" /") + Name_ParentWndKey + CMMStrHelp::Format(_T(" %u"), pIWndManager->GetWndHandle());
	
	TCHAR szFile[MAX_PATH] = {};
	GetModuleFileName(GetModuleHandle(NULL), szFile, MMCountString(szFile));
	CMMString strPath;
	CMMFile::ParseFilePathName(szFile, strPath, CMMString());
	ShellExecute(NULL, NULL, szFile, strCmdLine, strPath, SW_HIDE);

	return;
}

void CDpiDlgView::OnDuiValueChangedDpiScale(const DuiNotify &Notify)
{
	if (NULL == m_pSliderDpiScaleCtrl || Notify.pNotifyCtrl != m_pSliderDpiScaleCtrl) return;

	int nScale = 100 + m_pSliderDpiScaleCtrl->GetCurValue();
	PostMessage(m_hWndDpiDlg, WM_DEMO_DPISCALE, (WPARAM)nScale, NULL);

	return;
}

//////////////////////////////////////////////////////////////////////////