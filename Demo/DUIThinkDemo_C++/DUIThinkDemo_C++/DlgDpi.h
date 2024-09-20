#ifndef __DLGDPI_H__
#define __DLGDPI_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgDpi : public CDUIWnd
{
	MMDeclare_ClassName()
	DuiDeclare_Message_Map()

public:
	CDlgDpi(LPCTSTR lpszDuiName, HWND hWndParent, bool bNeedSwitchRes = true);
	~CDlgDpi();

	//control
protected:
	CDUIButtonCtrl *					m_pBtnSwitchResCtrl = NULL;

	//variant
protected:
	HWND								m_hWndParent = NULL;
	bool								m_bNeedSwitchRes = true;

	//override
protected:
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog() override;

	//notify
protected:
	void OnDuiTimer(const DuiNotify &Notify) override;
	void OnDuiClickBtnSwitchSkin(const DuiNotify &Notify);

	//message
protected:
	void OnWMDemoDpiScale(WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////

#endif