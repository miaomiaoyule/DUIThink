#include "StdAfx.h"
#include "MMTrayIcon.h"

//////////////////////////////////////////////////////////////////////////
CMMTrayIcon::CMMTrayIcon(void)
{
}

CMMTrayIcon::~CMMTrayIcon(void) 
{
	DeleteTrayIcon();

	return;
}

bool CMMTrayIcon::CreateTrayIcon(HWND hWnd, HINSTANCE hInstance, UINT uIconResourceID, CMMString strToolTip, UINT uTrayIconMsg)
{
	if (NULL == hWnd || NULL == hInstance || uIconResourceID <= 0) return false;
	
	//delete
	DeleteTrayIcon();

	//icon
	m_hInstance = hInstance;
	m_uMessage = uTrayIconMsg;
	m_hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(uIconResourceID));
	if (NULL == m_hIcon)
	{
		assert(false);
		return false;
	}

	//create
	m_TrayData = {};
	m_TrayData.cbSize = sizeof(NOTIFYICONDATA);
	m_TrayData.hWnd = hWnd;
	m_TrayData.uID = uIconResourceID;
	m_TrayData.hIcon = m_hIcon;
	m_TrayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_TrayData.uCallbackMessage = m_uMessage;
	lstrcpyn(m_TrayData.szTip, strToolTip, MMCountString(m_TrayData.szTip));

	m_bCreated = Shell_NotifyIcon(NIM_ADD, &m_TrayData);

	return m_bCreated;
}

void CMMTrayIcon::DeleteTrayIcon() 
{
	Shell_NotifyIcon(NIM_DELETE, &m_TrayData);
	m_TrayData = {};
	m_bCreated = false;
	m_bVisible = false;
	m_hWnd = NULL;
	m_uMessage = 0;
	if (m_hIcon)
	{
		::DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}

	return;
}

bool CMMTrayIcon::SetTooltip(CMMString strToolTip)
{
	if (false == IsCreated()) return false;

	ZeroMemory(m_TrayData.szTip, sizeof(m_TrayData.szTip));
	lstrcpyn(m_TrayData.szTip, strToolTip, MMCountString(m_TrayData.szTip));

	m_TrayData.uFlags = NIF_TIP;
	
	return Shell_NotifyIcon(NIM_MODIFY, &m_TrayData) == TRUE;
}

CMMString CMMTrayIcon::GetTooltip() const
{
	return m_TrayData.szTip;
}

bool CMMTrayIcon::ShowNotifyBubble(CMMString strInfo, CMMString strInfoTitle)
{
	if (false == IsCreated()) return false;
	
	ZeroMemory(m_TrayData.szInfo, sizeof(m_TrayData.szInfo));
	lstrcpyn(m_TrayData.szInfo, strInfo, MMCountString(m_TrayData.szInfo));
	
	ZeroMemory(m_TrayData.szInfoTitle, sizeof(m_TrayData.szInfoTitle));
	lstrcpyn(m_TrayData.szInfoTitle, strInfoTitle, MMCountString(m_TrayData.szInfoTitle));
	
	m_TrayData.uFlags = NIF_INFO;
	m_TrayData.dwInfoFlags = NIIF_INFO;
	m_TrayData.uTimeout = 30 * 1000;  //最大30s
	
	return Shell_NotifyIcon(NIM_MODIFY, &m_TrayData) == TRUE;
}

bool CMMTrayIcon::SetIcon(HICON hIcon) 
{
	if (false == IsCreated() || NULL == hIcon) return false;

	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}

	m_hIcon = hIcon;

	m_TrayData.uFlags = NIF_ICON;
	m_TrayData.hIcon = m_hIcon;

	return Shell_NotifyIcon(NIM_MODIFY, &m_TrayData) == TRUE;
}

bool CMMTrayIcon::SetIcon(CMMString strIconFile)
{
	HICON hIcon = LoadIcon(m_hInstance, strIconFile);
	return SetIcon(hIcon);
}

bool CMMTrayIcon::SetIcon(UINT uIconResourceID)
{
	HICON hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(uIconResourceID));
	return SetIcon(hIcon);
}

HICON CMMTrayIcon::GetIcon() const
{
	return m_hIcon;
}

void CMMTrayIcon::ShowIcon(bool bShow) 
{
	if (false == IsCreated()) return;

	m_bVisible = bShow;
	m_TrayData.uFlags = NIF_ICON;
	m_TrayData.hIcon = bShow ? m_hIcon: NULL;

	Shell_NotifyIcon(NIM_MODIFY, &m_TrayData);

	return;
}
