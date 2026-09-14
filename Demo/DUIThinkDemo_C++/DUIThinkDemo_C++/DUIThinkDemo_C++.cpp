// DUIThinkDemo_C++.cpp : 定义应用程序的入口点。
//

#include "StdAfx.h"
#include "DUIThinkDemo_C++.h"
#include <string>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

//////////////////////////////////////////////////////////////////////////
#if defined(DuiPlatform_SDL)

#ifndef DUITHINK_DEMO_PROJ_FILE
#define DUITHINK_DEMO_PROJ_FILE "DUIThinkDemo_C++.DuiProj/DUIThinkDemo_C++.DuiProj"
#endif

int main(int argc, char *argv[])
{
	if (false == CDUIGlobal::GetInstance()->Init(NULL)) return -1;

	const CMMString strProj = CA2CT(std::string(DUITHINK_DEMO_PROJ_FILE));
	if (false == CDUIGlobal::GetInstance()->LoadProjectFromFile(strProj))
	{
		CDUIGlobal::GetInstance()->UnInit();
		return -2;
	}

	CMMDpi::SetProcessDPIAwareness(enMMPROCESS_DPI_AWARENESS::MMPROCESS_PER_MONITOR_DPI_AWARE);

	CMMString strCmdLine;
	for (int i = 1; i < argc; ++i)
	{
		if (i > 1) strCmdLine += _T(" ");
		strCmdLine += CA2CT(std::string(argv[i]));
	}

	CMMCommandLine CmdLine(strCmdLine);
	CMMString strValue;

	do
	{
		CmdLine.Get(Name_DpiDlgKey, strValue);
		if (false == strValue.empty())
		{
			HWND hWndDpiDlg = FindWindow(_T("CDlgDpi"), NULL);
			if (hWndDpiDlg) break;

			CMMString strDuiName = strValue;
			CmdLine.Get(Name_ParentWndKey, strValue);
			HWND hWndParent = (HWND)(_tcstoul(strValue, NULL, 10));

			CDlgDpi DlgDpi(strDuiName, hWndParent);
			DlgDpi.Create(hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
			DlgDpi.DoBlock();
			break;
		}

		CDlgDemo DlgDemo;
		if (NULL == DlgDemo.Create(NULL, _T("DUIThinkDemo"), DUI_WNDSTYLE_FRAME, DUI_WNDSTYLE_EX_FRAME))
		{
			CDUIGlobal::GetInstance()->UnInit();
			return -3;
		}
		DlgDemo.DoModal();
	} while (false);

	CDUIGlobal::GetInstance()->UnInit();
	return 0;
}

#else

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	CDUIGlobal::GetInstance()->Init(hInstance);
	//CDUIGlobal::GetInstance()->SetDpi(CMMDpi::GetDpiOfMainMonitor());
	CDUIGlobal::GetInstance()->LoadProjectFromFile(_T("../DUIThinkDemo_C++.DuiProj/DUIThinkDemo_C++.DuiProj"));
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
		CDlgDemo DlgDemo;
		DlgDemo.Create(NULL, _T("DUIThinkDemo"), DUI_WNDSTYLE_FRAME, DUI_WNDSTYLE_EX_FRAME);
		DlgDemo.DoModal();

	} while (false);

	CDUIGlobal::GetInstance()->UnInit();

	return 0;
}

#endif
