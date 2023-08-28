#ifndef __DLGLISTEN_H__
#define __DLGLISTEN_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgListen : public CDUIWnd
{
	DuiDeclare_Message_Map()

public:
	CDlgListen(HWND hWndOwner);
	~CDlgListen();

	//variant
protected:
	HWND								m_hWndOwner = NULL;

	//override
protected:
	void OnDuiTimer(const DuiNotify &Notify) override;
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled);

	//message
protected:
};

//////////////////////////////////////////////////////////////////////////

#endif