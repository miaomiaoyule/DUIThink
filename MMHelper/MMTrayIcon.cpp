#include "StdAfx.h"
#include "MMTrayIcon.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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

RECT CMMTrayIcon::GetTrayIconPos(HWND hWndOwner)
{
	RECT rcPos = {};

	for (int n = 0; n < 2; n++)
	{
		HWND hWndToolBar = NULL;

		if (0 == n)
		{
			HWND hWndShellTray = FindWindow(_T("Shell_TrayWnd"), NULL);
			HWND hWndTrayNotifyWnd = FindWindowEx(hWndShellTray, NULL, _T("TrayNotifyWnd"), NULL);
			HWND hWndSysPager = FindWindowEx(hWndTrayNotifyWnd, NULL, _T("SysPager"), NULL);
			HWND hWndToolbarWindow32 = FindWindowEx(hWndSysPager, NULL, _T("ToolbarWindow32"), NULL);
			hWndToolBar = hWndToolbarWindow32;
		}
		else
		{
			HWND hWnd = FindWindow(_T("NotifyIconOverflowWindow"), NULL);
			hWnd = FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);
			hWndToolBar = hWnd;
		}

		// 获取托盘进程ID
		DWORD dwProcessId = 0;
		GetWindowThreadProcessId(hWndToolBar, &dwProcessId);
		if (dwProcessId == 0)
		{
			cout << "GetWindowThreadProcessId failed:" << GetLastError() << endl;
			return rcPos;
		}

		// 获取托盘进程句柄
		HANDLE hProcess = OpenProcess(
			PROCESS_VM_OPERATION |	// 需要在进程的地址空间上执行操作
			PROCESS_VM_READ |	// 需要使用 ReadProcessMemory 读取进程中的内存
			PROCESS_VM_WRITE,	// 需要在使用 WriteProcessMemory 的进程中写入内存
			FALSE,					// 子进程不继承句柄
			dwProcessId				// 目标进程 PID
		);
		if (hProcess == NULL)
		{
			cout << "OpenProcess failed:" << GetLastError() << endl;
			return rcPos;
		}

		// 在进程虚拟空间中分配内存，用来接收 TBBUTTON 结构体指针
		LPVOID p_tbbutton = VirtualAllocEx(
			hProcess,					// 目标进程句柄
			0,							// 内存起始地址（默认）
			4096,						// 内存大小
			MEM_COMMIT,					// 内存类型（提交）
			PAGE_EXECUTE_READWRITE		// 内存保护属性（可读可写可执行）
		);
		if (p_tbbutton == NULL) 
		{
			cout << "VirtualAllocEx failed:" << GetLastError() << endl;
			return rcPos;
		}

		// 初始化
		DWORD dw_addr_dwData = 0;
		BYTE buff[1024] = { 0 };
		wstring ws_filePath = L"";
		wstring ws_tile = L"";
		HWND h_mainWnd = NULL;
		int i_data_offset = 12;
		int i_str_offset = 18;

		// 判断 x64
		if (CMMVersion::IsWow64())
		{
			i_data_offset += 4;
			i_str_offset += 6;
		}

		// 获取托盘图标个数
		int i_buttons = 0;
		i_buttons = SendMessage(hWndToolBar, TB_BUTTONCOUNT, 0, 0);
		if (i_buttons == 0) 
		{
			cout << "TB_BUTTONCOUNT message failed:" << GetLastError() << endl;
			return rcPos;
		}

		// 遍历托盘
		for (int i = 0; i < i_buttons; i++)
		{
			// 获取 TBBUTTON 结构体指针
			if (!SendMessage(hWndToolBar, TB_GETBUTTON, i, (LPARAM)p_tbbutton))
			{
				cout << "TB_GETBUTTON message failed:" << GetLastError() << endl;
				return rcPos;
			}

			// 读 TBBUTTON.dwData（附加信息）
			if (!ReadProcessMemory(hProcess, (LPVOID)((DWORD)p_tbbutton + i_data_offset), &dw_addr_dwData, 4, 0))
			{
				cout << "ReadProcessMemory failed:" << GetLastError() << endl;
				return rcPos;
			}

			// 读文本
			if (dw_addr_dwData)
			{
				if (!ReadProcessMemory(hProcess, (LPCVOID)dw_addr_dwData, buff, 1024, 0))
				{
					cout << "ReadProcessMemory failed:" << GetLastError() << endl;
					return rcPos;
				}

				h_mainWnd = (HWND)(*((DWORD*)buff));
				ws_filePath = (WCHAR*)buff + i_str_offset;
				ws_tile = (WCHAR*)buff + i_str_offset + MAX_PATH;

				if (h_mainWnd == hWndOwner)
				{
					SendMessage(hWndToolBar, TB_GETITEMRECT, i, (LPARAM)p_tbbutton);
					ReadProcessMemory(hProcess, (LPVOID)((DWORD)p_tbbutton), buff, 1024, 0);
					rcPos = *(RECT*)(buff);

					RECT rcWndToolBar = {};
					GetWindowRect(hWndToolBar, &rcWndToolBar);
					OffsetRect(&rcPos, rcWndToolBar.left, rcWndToolBar.top);

					break;
				}
			}

			// 清理
			dw_addr_dwData = 0;
			h_mainWnd = NULL;
			ws_filePath = L"";
			ws_tile = L"";

			cout << endl;
		}

		VirtualFreeEx(hProcess, p_tbbutton, 0, MEM_RELEASE);
		MMSafeCloseHandle(hProcess)

		if (rcPos.left != rcPos.right && rcPos.top != rcPos.bottom)
		{
			return rcPos;
		}
	}

	return rcPos;
}

//////////////////////////////////////////////////////////////////////////
#endif