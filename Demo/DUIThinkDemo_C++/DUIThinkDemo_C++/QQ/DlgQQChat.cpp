#include "StdAfx.h"
#include "DlgQQChat.h"

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDlgQQChat)

DuiBegin_Message_Map(CDlgQQChat, CDUIWnd)
	Dui_On_Click_Ctrl(IDC_BtnSend, OnDuiClickBtnSend)
	Dui_On_Click_Ctrl(IDC_BtnFace, OnDuiClickBtnFace)
	Dui_On_Click_Ctrl(IDC_ButtonCtrl_100463, OnDuiClickFont)
DuiEnd_Message_Map()

CDlgQQChat::CDlgQQChat()
	: CDUIWnd(_T("DlgQQChat"))
{

}

CDlgQQChat::~CDlgQQChat()
{
	MMSafeDelete(m_pDlgQQFace);

	return;
}

void CDlgQQChat::OnSize(CDUIControlBase *pControl)
{
	if (IDC_VertOther == pControl->GetCtrlID()
		|| IDC_VertSelf == pControl->GetCtrlID())
	{
		CDUIListItemCtrl *pItem = MMInterfaceHelper(CDUIListItemCtrl, pControl->GetParent());
		if (NULL == pItem) return;

		pItem->SetFixedHeight(pControl->GetFixedHeight());

		return;
	}

	return;
}

LRESULT CDlgQQChat::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDlgQQChat::OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	return __super::OnWndCustomMessage(uMsg, wParam, lParam, bHandled);
}

void CDlgQQChat::OnInitDialog()
{
	if (NULL == m_pWndManager) return;

	m_pListViewMessageCtrl = MMInterfaceHelper(CDUIListViewCtrl, m_pWndManager->FindControl(IDC_ListViewMessage));
	m_pEditSendCtrl = MMInterfaceHelper(CDUIThinkEditCtrl, m_pWndManager->FindControl(IDC_EditSend));
	m_pBtnSendCtrl = MMInterfaceHelper(CDUIButtonCtrl, m_pWndManager->FindControl(IDC_BtnSend));
	m_pBtnFaceCtrl = MMInterfaceHelper(CDUIButtonCtrl, m_pWndManager->FindControl(IDC_BtnFace));

	m_pListViewMessageCtrl->RemoveAll();
	m_pEditSendCtrl->SetRichEdit(true);

	CenterWindow();

	return;
}

void CDlgQQChat::InsertImage(CMMString strImageResName)
{
	if (NULL == m_pEditSendCtrl) return;

	m_pEditSendCtrl->SetReplaceSel(_T(""), strImageResName);

	return;
}

void CDlgQQChat::OnDuiClickBtnSend(const DuiNotify &Notify)
{
	if (NULL == m_pListViewMessageCtrl
		|| NULL == m_pEditSendCtrl) return;

	//message
	VecDuiRichTextItem vecRichTextItem = m_pEditSendCtrl->GetRichTextItem();
	m_pEditSendCtrl->SetText(_T(""));

	//other
	{
		CDUIListItemCtrl *pItem = MMInterfaceHelper(CDUIListItemCtrl, CDUIGlobal::GetInstance()->LoadDui(_T("QQChatItem")));
		if (NULL == pItem) return;

		ConstructMessageItem(pItem, vecRichTextItem, true);

		m_pListViewMessageCtrl->InsertChild(pItem);
	}

	//self
	{
		CDUIListItemCtrl *pItem = MMInterfaceHelper(CDUIListItemCtrl, CDUIGlobal::GetInstance()->LoadDui(_T("QQChatItem")));
		if (NULL == pItem) return;

		ConstructMessageItem(pItem, vecRichTextItem, false);

		m_pListViewMessageCtrl->InsertChild(pItem);
	}

	return;
}

void CDlgQQChat::OnDuiClickBtnFace(const DuiNotify &Notify)
{
	if (NULL == m_pBtnFaceCtrl) return;

	//pos
	CDUIRect rcFaceCtrl = m_pBtnFaceCtrl->GetAbsoluteRect();
	MMClientToScreen(rcFaceCtrl, m_hWnd);

	MMSafeDelete(m_pDlgQQFace);
	m_pDlgQQFace = new CDlgQQFace(this, rcFaceCtrl);
	m_pDlgQQFace->Create(m_hWnd, _T("DlgQQFace"), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);

	return;
}

void CDlgQQChat::OnDuiClickFont(const DuiNotify& Notify)
{
	if (NULL == m_pEditSendCtrl) return;

	LOGFONT LogFont = {};
	CHOOSEFONT FontInfo = {};
	FontInfo.lStructSize = sizeof(FontInfo);
	FontInfo.hwndOwner = m_hWnd;
	FontInfo.lpLogFont = &LogFont;
	if (ChooseFont(&FontInfo))
	{
		CDUIFontBase *pFontBase = new CDUIFontBase(CMMService::ProductGUID(), LogFont.lfFaceName, FontInfo.iPointSize / 10, LogFont.lfWeight, LogFont.lfItalic, LogFont.lfUnderline, LogFont.lfStrikeOut);
		if (false == CDUIGlobal::GetInstance()->AddResource(pFontBase))
		{
			MMSafeDelete(pFontBase);

			return;
		}

		tagDuiTextStyle TextStyle = m_pEditSendCtrl->GetTextStyle();
		TextStyle.vecFontResSwitch = { pFontBase->GetResourceName() };
		m_pEditSendCtrl->SetTextStyle(TextStyle);
	}

	return;
}

void CDlgQQChat::ConstructMessageItem(CDUIListItemCtrl *pItem, VecDuiRichTextItem vecRichTextItem, bool bOther)
{
	if (NULL == pItem) return;

	CDUIVerticalLayoutCtrl *pVertOtherCtrl = MMInterfaceHelper(CDUIVerticalLayoutCtrl, pItem->FindSubControl(IDC_VertOther));
	CDUIVerticalLayoutCtrl *pVertSelfCtrl = MMInterfaceHelper(CDUIVerticalLayoutCtrl, pItem->FindSubControl(IDC_VertSelf));
	if (NULL == pVertOtherCtrl || NULL == pVertSelfCtrl) return;

	pVertOtherCtrl->SetVisible(bOther);
	pVertOtherCtrl->RegisterControlCallBack(this);
	pVertSelfCtrl->SetVisible(false == bOther);
	pVertSelfCtrl->RegisterControlCallBack(this);

	//other
	if (bOther)
	{	
		CDUIThinkEditCtrl *pEditMessageOtherCtrl = MMInterfaceHelper(CDUIThinkEditCtrl, pVertOtherCtrl->FindSubControl(IDC_EditMessageOther));
		if (NULL == pEditMessageOtherCtrl) return;

		pEditMessageOtherCtrl->SetRichEdit(true);
		pEditMessageOtherCtrl->SetReadOnly(true);
		pEditMessageOtherCtrl->SetRichTextItem(vecRichTextItem);

		return;
	}

	//self
	CDUIThinkEditCtrl *pEditMessageSelfCtrl = MMInterfaceHelper(CDUIThinkEditCtrl, pVertSelfCtrl->FindSubControl(IDC_EditMessageSelf));
	if (NULL == pEditMessageSelfCtrl) return;

	pEditMessageSelfCtrl->SetRichEdit(true);
	pEditMessageSelfCtrl->SetReadOnly(true);
	pEditMessageSelfCtrl->SetRichTextItem(vecRichTextItem);

	return;
}