#include "StdAfx.h"
#include "SwitchSkinView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CSwitchSkinView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Click_Ctrl(IDC_BtnSwitchSkin1, OnDuiClickBtnSwitchSkin1)
DuiEnd_Message_Map()

CSwitchSkinView::CSwitchSkinView()
{

}

CSwitchSkinView::~CSwitchSkinView()
{
	if (m_pSwitchSkinViewCtrl)
	{
		if (m_pSwitchSkinViewCtrl->GetWndManager())
		{
			m_pSwitchSkinViewCtrl->GetWndManager()->RemoveINotify(this);
		}
	}

	return;
}

void CSwitchSkinView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pSwitchSkinViewCtrl = pViewBase;

	if (NULL == m_pSwitchSkinViewCtrl) return;

	if (m_pSwitchSkinViewCtrl->GetWndManager())
	{
		m_pSwitchSkinViewCtrl->GetWndManager()->AddINotify(this);
	}

	//ctrl
	m_pBtnSwitchSkin1Ctrl = MMInterfaceHelper(CDUIButtonCtrl, m_pSwitchSkinViewCtrl->FindSubControl(IDC_BtnSwitchSkin1));

	return;
}

void CSwitchSkinView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

void CSwitchSkinView::OnDuiClickBtnSwitchSkin1(const DuiNotify &Notify)
{
	if (NULL == m_pSwitchSkinViewCtrl || NULL == m_pSwitchSkinViewCtrl->GetWndManager()) return;

	auto pIWndManager = m_pSwitchSkinViewCtrl->GetWndManager();

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

//////////////////////////////////////////////////////////////////////////