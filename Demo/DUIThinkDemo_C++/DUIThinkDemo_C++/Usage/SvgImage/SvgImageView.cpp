#include "StdAfx.h"
#include "SvgImageView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CSvgImageView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify_Ctrl(DuiNotify_ValueChanged, IDC_SliderDpiScaleImage, OnDuiValueChangedDpiScale)
	Dui_On_Notify_Ctrl(DuiNotify_VisibleChanged, IDC_VertSvgImageView, OnDuiVisibleChangedSvgImageView)
DuiEnd_Message_Map()

CSvgImageView::CSvgImageView()
{

}

CSvgImageView::~CSvgImageView()
{
	if (m_pSvgImageViewCtrl)
	{
		if (m_pSvgImageViewCtrl->GetWndManager())
		{
			m_pSvgImageViewCtrl->GetWndManager()->RemoveINotify(this);
		}
	}

	CMMSvg::GetInstance()->UnInit();

	return;
}

LRESULT CSvgImageView::OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (uMsg)
	{
		case WM_DROPFILES:
		{
			OnWMDropFile(wParam, lParam);

			break;
		}
		default:
			break;
	}

	return 0;
}

void CSvgImageView::Attach(CDUIContainerCtrl *pViewBase)
{
	m_pSvgImageViewCtrl = pViewBase;

	if (NULL == m_pSvgImageViewCtrl) return;

	if (m_pSvgImageViewCtrl->GetWndManager())
	{
		m_pSvgImageViewCtrl->GetWndManager()->AddINotify(this);
	}

	CMMSvg::GetInstance()->Init();

	//ctrl
	m_pStaticSvgImageCtrl = MMInterfaceHelper(CDUIStaticCtrl, m_pSvgImageViewCtrl->FindSubControl(IDC_StaticSvgImage));
	m_pSliderDpiScaleCtrl = MMInterfaceHelper(CDUISliderCtrl, m_pSvgImageViewCtrl->FindSubControl(IDC_SliderDpiScaleImage));

	//init
	if (NULL == m_pStaticSvgImageCtrl) return;

	m_strTip = m_pStaticSvgImageCtrl->GetText();
	
	DragAcceptFiles(m_pSvgImageViewCtrl->GetWndManager()->GetWndHandle(), m_pSvgImageViewCtrl->IsVisible());

	return;
}

void CSvgImageView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

void CSvgImageView::OnDuiValueChangedDpiScale(const DuiNotify &Notify)
{
	if (NULL == m_pSliderDpiScaleCtrl || Notify.pNotifyCtrl != m_pSliderDpiScaleCtrl) return;

	ConstructSvgImageBk();

	return;
}

void CSvgImageView::OnDuiVisibleChangedSvgImageView(const DuiNotify &Notify)
{
	if (NULL == m_pSvgImageViewCtrl 
		|| NULL == m_pSvgImageViewCtrl->GetWndManager()
		|| m_pSvgImageViewCtrl != Notify.pNotifyCtrl) return;

	DragAcceptFiles(m_pSvgImageViewCtrl->GetWndManager()->GetWndHandle(), m_pSvgImageViewCtrl->IsVisible());

	return;
}

void CSvgImageView::OnWMDropFile(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pSvgImageViewCtrl
		|| NULL == m_pStaticSvgImageCtrl
		|| NULL == m_pSvgImageViewCtrl->GetWndManager()->GetWndHandle()) return;

	//activate us first !
	SetActiveWindow(m_pSvgImageViewCtrl->GetWndManager()->GetWndHandle());

	HDROP hDropInfo = (HDROP)wParam;

	UINT nFilesCount = DragQueryFile(hDropInfo, INFINITE, NULL, 0);
	for (UINT i = 0; i < nFilesCount; i++)
	{
		TCHAR szFile[MAX_PATH] = {};
		int pathLen = DragQueryFile(hDropInfo, i, szFile, MAX_PATH);

		m_strSvgImageFile = szFile;
	}

	DragFinish(hDropInfo);

	//svgimage
	ConstructSvgImageBk();

	return;
}

void CSvgImageView::ConstructSvgImageBk()
{
	if (NULL == m_pStaticSvgImageCtrl) return;

	CMMString strExt;
	CMMFile::ParseFileName(m_strSvgImageFile, CMMString(), strExt);
	strExt.MakeLower();
	if (_T("svg") == strExt)
	{
		HBITMAP hBitmap = NULL;
		LPBYTE pBits = NULL;
		int nWidth = 0, nHeight = 0;
		CMMSvg::GetInstance()->ParseImage(m_strSvgImageFile, GetCurScale(), hBitmap, nWidth, nHeight, &pBits);

		m_pStaticSvgImageCtrl->SetCustomBmpBack(hBitmap, CustomBack_Center | CustomBack_VCenter | CustomBack_Scale);
		m_pStaticSvgImageCtrl->SetText(_T(""));

		return;
	}

	m_pStaticSvgImageCtrl->SetText(m_strTip);

	return;
}

int CSvgImageView::GetCurScale()
{
	if (NULL == m_pSliderDpiScaleCtrl) return 100;

	return 100 + m_pSliderDpiScaleCtrl->GetCurValue();
}

//////////////////////////////////////////////////////////////////////////