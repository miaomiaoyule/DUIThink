#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_TRAYICON_H__
#define __MM_TRAYICON_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMTrayIcon
{
public:
	CMMTrayIcon(void);
	virtual ~CMMTrayIcon(void);

private:
	HINSTANCE							m_hInstance = NULL;
	HWND								m_hWnd = NULL;
	UINT								m_uMessage = 0;
	HICON								m_hIcon = NULL;
	NOTIFYICONDATA						m_TrayData = {};
	bool								m_bCreated = false;
	bool								m_bVisible = true;

public:
	bool CreateTrayIcon(HWND hWnd, HINSTANCE hInstance, UINT uIconResourceID, CMMString strToolTip, UINT uTrayIconMsg);
	void DeleteTrayIcon();
	bool SetTooltip(CMMString strToolTip);
	CMMString GetTooltip() const;
	bool ShowNotifyBubble(CMMString strInfo, CMMString strInfoTitle);

	bool IsCreated() { return m_bCreated; }
	bool IsVisible() { return IsCreated() && m_bVisible; }
	bool SetIcon(HICON hIcon);
	bool SetIcon(CMMString strIconFile);
	bool SetIcon(UINT uIconResourceID);
	HICON GetIcon() const;
	void ShowIcon(bool bShow);

	//static
public:
	static RECT GetTrayIconPos(HWND hWndOwner);
};

//////////////////////////////////////////////////////////////////////////

#endif 
#endif

