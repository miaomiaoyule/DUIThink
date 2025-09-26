// DUIThinkDemo_C++.cpp : 定义应用程序的入口点。
//

#include "StdAfx.h"
#include "DUIThinkDemo_C++.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DUITHINKDEMOC, szWindowClass, MAX_LOADSTRING);

	CDUIGlobal::GetInstance()->Init(hInstance);
	//CDUIGlobal::GetInstance()->SetDpi(CMMDpi::GetDpiOfMainMonitor());
	CDUIGlobal::GetInstance()->LoadProjectFromFile(_T("../DUIThinkDemo.DuiProj/DUIThinkDemo.DuiProj"));
	CMMDpi::SetProcessDPIAwareness(enMMPROCESS_DPI_AWARENESS::MMPROCESS_PER_MONITOR_DPI_AWARE);

	//cmd line
	CMMCommandLine CmdLine(lpCmdLine);
	CMMString strValue;

	do
	{
		//dpi dlg
		CmdLine.Get(Name_DpiDlgKey, strValue);
		if (false == strValue.empty())
		{
			//have
			HWND hWndDpiDlg = FindWindow(_T("CDlgDpi"), NULL);
			if (hWndDpiDlg) break;

			//create
			CMMString strDuiName = strValue;

			CmdLine.Get(Name_ParentWndKey, strValue);
			HWND hWndParent = (HWND)(_tcstoul(strValue, NULL, 10));

			CDlgDpi DlgDpi(strDuiName, hWndParent);
			DlgDpi.Create(hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
			DlgDpi.DoBlock();

			break;
		}

		//normal demo
		CDlgDemo DlgDemo(_T("DlgMain"));
		DlgDemo.Create(NULL, _T("DUIThinkDemo"), DUI_WNDSTYLE_FRAME, DUI_WNDSTYLE_EX_FRAME);
		DlgDemo.DoModal();

	} while (false);

	CDUIGlobal::GetInstance()->UnInit();

	return 0;
}