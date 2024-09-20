#ifndef __DLGTEST_H__
#define __DLGTEST_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgDemo : public CDUIWnd
{
	DuiDeclare_Message_Map()

public:
	CDlgDemo(LPCTSTR lpszDuiName);
	~CDlgDemo();

	//control
protected:
	//control
	CDUIContainerCtrl *					m_pTabViewControls = NULL;

	//control view
	CStaticView							m_StaticView;
	CCheckBoxView						m_CheckBoxView;
	CRadioBoxView						m_RadioBoxView;
	CProgressView						m_ProgressView;
	CComboxView							m_ComboxView;

	//usage view
	CDpiDlgView							m_DpiDlgView;
	CSvgImageView						m_SvgImageView;
	CSwitchSkinView						m_SwitchSkinView;
	CAnimateDlgView						m_AnimateDlgView;

	//qq view
	CQQView								m_QQView;

	//variant
protected:
	//tray
	CMMTrayIcon							m_TrayIcon;
	bool								m_bShowTrayIcon = true;

	//dialog
protected:
	CDlgChatTip *						m_pDlgChatTip = NULL;

	//override
protected:
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnFindControl() override;
	void OnInitDialog() override;

	//notify
protected:
	void OnTimerTabViewControls(const DuiNotify &Notify);

	//message
protected:
	void OnWMDemoDpiWnd(WPARAM wParam, LPARAM lParam);
	void OnWMDemoTrayIcon(WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////

#endif