// DUIPreview.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "DUIPreview.h"

//////////////////////////////////////////////////////////////////////////
CDUIPreview::CDUIPreview()
{
	return;
}

CDUIPreview * CDUIPreview::GetInstance()
{
	static CDUIPreview Obj;
	return &Obj;
}

void CDUIPreview::Init(HINSTANCE hInstance, CString strCommandLine)
{
	//parse
	CMMString strProject, strDui, strParentWnd, strDpiScale, strFrameWnd;
	CMMCommandLine CommandLine(strCommandLine);
	CommandLine.Get(Key_Dui_PreviewProject, strProject);
	CommandLine.Get(Key_Dui_PreviewDuiName, strDui);
	CommandLine.Get(Key_Dui_PreviewParentWnd, strParentWnd);
	CommandLine.Get(Key_Dui_PreviewDpiScale, strDpiScale);
	CommandLine.Get(Key_Dui_PreviewFrameWnd, strFrameWnd);
	HWND hWndParent = (HWND)_ttoi(strParentWnd);
	bool bDpiScale = -1 != strProject.Find(_T("DUIThinkDemo" )) ? 1 == _ttoi(strDpiScale) : true;
	bool bFrameWnd = _T("true") == strFrameWnd;

	//init
	CDUIGlobal::GetInstance()->Init(hInstance);
	CDUIGlobal::GetInstance()->LoadProjectFromFile(strProject);
	CDUIGlobal::GetInstance()->SetDpi(96);

	//wnd
	CDlgListen DlgListen(hWndParent);
	DlgListen.Create(HWND_MESSAGE, _T(""), NULL, NULL);

	CDlgDpiInfo DlgDpiInfo;
	if (bDpiScale)
	{
		DlgDpiInfo.Create(hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	}

	//menu
	if (DuiType_Menu == CDUIGlobal::GetInstance()->GetDuiType(strDui)
		|| DuiType_3DMenu == CDUIGlobal::GetInstance()->GetDuiType(strDui))
	{
		DlgDpiInfo.PerformAttachWndClass(_T("CDUIMenuWnd"));

		CDUIMenu DuiMenu;
		DuiMenu.LoadMenu(strDui);
		DuiMenu.TrackPopupMenu(hWndParent, { -1, -1 });
	}
	else if (DuiType_Calendar == CDUIGlobal::GetInstance()->GetDuiType(strDui))
	{
		DlgDpiInfo.PerformAttachWndClass(_T("CDUICalendarWnd"));

		CDUICalendar DuiCalendar;
		DuiCalendar.LoadCalendar(strDui);
		DuiCalendar.TrackPopupCalendar(hWndParent, { -1, -1 });
	}
	//dlg
	else
	{
		CDlgPreview DlgPreview(strDui);
		DlgPreview.Create(hWndParent, _T("DuiPreView"), bFrameWnd ? DUI_WNDSTYLE_FRAME : DUI_WNDSTYLE_DIALOG, bFrameWnd ? DUI_WNDSTYLE_EX_FRAME : DUI_WNDSTYLE_EX_DIALOG);
		DlgDpiInfo.PerformAttachWnd(DlgPreview);

		DlgPreview.DoModal();
	}

	return;
}

void CDUIPreview::UnInit()
{
	CDUIGlobal::GetInstance()->UnInit();

	return;
}

//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

	//single
	HANDLE MutexHandle = ::CreateMutex(NULL, FALSE, _T("{30FBF490-55C1-45BE-B4A3-C2E77585F6A1}"));
	if (MutexHandle == NULL)
	{
		MessageBox(NULL, _T("Preview Create Fail"), _T("Error"), MB_OK | MB_ICONERROR);
		
		return 0;
	}

	CDUIPreview::GetInstance()->Init(hInstance, lpCmdLine);
	CDUIPreview::GetInstance()->UnInit();

    return 0;
}
