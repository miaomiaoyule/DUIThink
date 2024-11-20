// FlowSheet.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "FlowSheet.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    CDUIGlobal::GetInstance()->Init(hInstance);
    CDUIGlobal::GetInstance()->SetDpi(96);
#ifdef _DEBUG
    CDUIGlobal::GetInstance()->LoadProjectFromFile(_T("../FlowSheet.DuiProj/FlowSheet.DuiProj"));
#else
    CDUIGlobal::GetInstance()->LoadProjectFromFile(_T("../FlowSheet.DuiProj/FlowSheet.DuiProj"));
#endif

    CDlgMain DlgMain;
    DlgMain.Create(NULL, _T("FlowSheet"), DUI_WNDSTYLE_FRAME, DUI_WNDSTYLE_EX_FRAME);
    DlgMain.DoModal();

    CDUIGlobal::GetInstance()->UnInit();

    return 0;
}
