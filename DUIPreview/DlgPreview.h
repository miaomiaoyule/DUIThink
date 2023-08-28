#ifndef __DLGPREVIEW_H__
#define __DLGPREVIEW_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgPreview : public CDUIWnd
{
	DuiDeclare_Message_Map()

public:
	CDlgPreview(CString strDuiName);
	~CDlgPreview();

	//variant
protected:

	//override
protected:
	void OnDuiTimer(const DuiNotify &Notify) override;
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled);
	void OnInitDialog() override;

	//message
protected:
};

//////////////////////////////////////////////////////////////////////////

#endif