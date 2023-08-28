#include "framework.h"
#include "DlgPreview.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CDlgPreview, CDUIWnd)
DuiEnd_Message_Map()

//////////////////////////////////////////////////////////////////////////
CDlgPreview::CDlgPreview(CString strDuiName)
	: CDUIWnd(strDuiName)
{

}

CDlgPreview::~CDlgPreview()
{

}

void CDlgPreview::OnDuiTimer(const DuiNotify &Notify)
{

}

LRESULT CDlgPreview::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DROPFILES:
		{
			SetActiveWindow(m_hWnd);      // activate us first !

			HDROP hDropInfo = (HDROP)wParam;

			CString strFile;
			UINT nFilesCount = DragQueryFile(hDropInfo, INFINITE, NULL, 0);
			for (UINT i = 0; i < nFilesCount; i++)
			{
				int pathLen = DragQueryFile(hDropInfo, i, strFile.GetBuffer(MAX_PATH), MAX_PATH);
				strFile.ReleaseBuffer(pathLen);		
			}

			DragFinish(hDropInfo);

			break;
		}
	}

	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDlgPreview::OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	return __super::OnCreate(wParam, lParam, bHandled);
}

LRESULT CDlgPreview::OnTimer(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (wParam)
	{
		default:
			break;
	}

	return __super::OnTimer(wParam, lParam, bHandled);
}

void CDlgPreview::OnInitDialog()
{
	__super::OnInitDialog();

	DragAcceptFiles(m_hWnd, TRUE);
	
	CenterWindow();

	return;
}
