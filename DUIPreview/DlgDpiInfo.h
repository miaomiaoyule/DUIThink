#ifndef __DLGDPIINFO_H__
#define __DLGDPIINFO_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgDpiInfo : public CDUIWnd
{
	MMDeclare_ClassName()
	DuiDeclare_Message_Map()

public:
	CDlgDpiInfo();
	~CDlgDpiInfo();

	//variant
protected:
	CMMString							m_strAttachWndClass;
	HWND								m_hWndAttach = NULL;

	//control
protected:
	CDUIStaticCtrl *					m_pStaticDpiInfoCtrl = NULL;
	CDUIThinkEditCtrl *					m_pEditDpiInfoCtrl = NULL;

	//override
protected:
	void OnDuiTimer(const DuiNotify &Notify) override;
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled);
	void OnInitDialog() override;

	//method
public:
	void PerformAttachWndClass(CString strAttachWndClass);
	void PerformAttachWnd(HWND hWnd);

	//message
protected:
	void OnDuiValueChanged(const DuiNotify &Notify);
	void OnDuiTextChanged(const DuiNotify &Notify);

	//help
protected:
	void PerformAdjustWndPos();
};

//////////////////////////////////////////////////////////////////////////

#endif