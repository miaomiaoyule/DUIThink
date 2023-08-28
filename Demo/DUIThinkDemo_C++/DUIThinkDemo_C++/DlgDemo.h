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

	//variant
protected:
	//control
	CStaticView							m_StaticView;
	CCheckBoxView						m_CheckBoxView;
	CRadioBoxView						m_RadioBoxView;
	CProgressView						m_ProgressView;
	CComboxView							m_ComboxView;

	//usage
	CDpiDlgView							m_DpiDlgView;
	CSvgImageView						m_SvgImageView;
	CSwitchSkinView						m_SwitchSkinView;
	CAnimateDlgView						m_AnimateDlgView;

	//qq
	CQQView								m_QQView;

	//override
protected:
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	void OnInitDialog() override;

	//message
protected:
	void OnWMDemoDpiWnd(WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////

#endif