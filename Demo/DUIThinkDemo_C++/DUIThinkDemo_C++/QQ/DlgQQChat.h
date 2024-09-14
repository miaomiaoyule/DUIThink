#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgQQChat
	: public CDUIWnd
	, public IDuiControlCallBack
{
	MMDeclare_ClassName()
	DuiDeclare_Message_Map()

public:
	CDlgQQChat();
	~CDlgQQChat();

	//contorl
protected:
	CDUIListViewCtrl *					m_pListViewMessageCtrl = NULL;
	CDUIThinkEditCtrl *					m_pEditSendCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnSendCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnFaceCtrl = NULL;

	//variant
protected:
	CDlgQQFace *						m_pDlgQQFace = NULL;

	//override
protected:
	void OnSize(CDUIControlBase *pControl) override;

	//override
protected:
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog() override;

	//method
public:
	void InsertImage(CMMString strImageResName);

	//notify
protected:
	void OnDuiClickBtnSend(const DuiNotify &Notify);
	void OnDuiClickBtnFace(const DuiNotify &Notify);
	void OnDuiClickFont(const DuiNotify& Notify);

	//help
protected:
	void ConstructMessageItem(CDUIListItemCtrl *pItem, VecDuiRichTextItem vecRichTextItem, bool bOther);
};

//////////////////////////////////////////////////////////////////////////

