#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgQQChat;

//////////////////////////////////////////////////////////////////////////
class CDlgQQFace
	: public CDUIWnd
	, public IDUIControlCallBack
{
	MMDeclare_ClassName()
	DuiDeclare_Message_Map()

public:
	CDlgQQFace(CDlgQQChat *pDlgQQChat, CDUIRect rcEventCtrl);
	~CDlgQQFace();

	//contorl
protected:
	CDUIListViewCtrl *					m_pListViewFaceCtrl = NULL;

	//variant
protected:
	CDlgQQChat *						m_pDlgQQChat = NULL;
	CDUIRect							m_rcEventCtrl;

	//override
protected:
	void OnSize(CDUIControlBase *pControl) override;

	//override
protected:
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	LRESULT OnWndCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	void OnInitDialog() override;

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiItemClickListViewFace(const DuiNotify &Notify);

	//help
protected:
	void InitInfo();
	void PerformAdjustPos();
};

//////////////////////////////////////////////////////////////////////////

