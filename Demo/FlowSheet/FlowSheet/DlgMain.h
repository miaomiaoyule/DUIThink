#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgMain
	: public CDUIWnd
	, public IDuiControlCallBack
{
	MMDeclare_ClassName(CDlgMain)
	DuiDeclare_Message_Map()

public:
	CDlgMain();
	virtual ~CDlgMain();

	//control
protected:
	CDUIControlBase *							m_pDragingCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerEditCtrl = NULL;
	CDUIVerticalLayoutCtrl *					m_pVertControlsCtrl = NULL;
	CDUIContainerCtrl *							m_pLinkingCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerStartModelCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerFinishModelCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerInputModelCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerOutputModelCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerOperatorModelCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerJudgeModelCtrl = NULL;
	CDUIRadioBoxCtrl *							m_pRadioBoxStartCtrl = NULL;
	CDUIRadioBoxCtrl *							m_pRadioBoxFinishCtrl = NULL;
	CDUIRadioBoxCtrl *							m_pRadioBoxInputCtrl = NULL;
	CDUIRadioBoxCtrl *							m_pRadioBoxOutputCtrl = NULL;
	CDUIRadioBoxCtrl *							m_pRadioBoxOperatorCtrl = NULL;
	CDUIRadioBoxCtrl *							m_pRadioBoxJudgeCtrl = NULL;

	//variant
protected:
	CDUIPoint									m_ptMouseDown;
	CDUIPoint									m_ptMouseLast;
	bool										m_bDragFromControls = true;
	std::vector<CDUIControlBase*>				m_vecDragCtrl;
	enLinkType									m_LinkingFrom = Link_Top;
	MapLinkData									m_mapLinkData;

	//override
protected:
	void OnFindControl() override;
	void OnInitDialog() override;
	void OnPaint(CDUIControlBase *pControl, HDC hDC) override;
	void OnRelease(CDUIControlBase *pControl) override;

	//message
protected:

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiWndLayout(const DuiNotify &Notify);
	void OnDuiLButtonDown_DragBegin(const DuiNotify &Notify);
	void OnDuiLButtonDown_IDC_ControlLink(const DuiNotify &Notify);
	void OnDuiMouseMove_DragMove(const DuiNotify &Notify);
	void OnDuiMouseMove_IDC_ControlLink(const DuiNotify &Notify);
	void OnDuiLButtonUp_DragEnd(const DuiNotify &Notify);
	void OnDuiLButtonUp_IDC_ControlLink(const DuiNotify &Notify);
	void OnDuiSetFocus_DragCtrl(const DuiNotify &Notify);
	void OnDuiKillFocus_DragCtrl(const DuiNotify &Notify);
	void OnDuiClick_IDC_BtnStart(const DuiNotify &Notify);

	//help
protected:
	void PerformDragBegin(CDUIControlBase *pDragCtrl);
	void PerformDragMove();
	void PerformDragEnd();
	void PerformDrawArrow(HDC hDC, CDUIPoint ptFrom, CDUIPoint ptTarget);
	void PerformExecuteModule(CDUIContainerCtrl *pContainerFromCtrl);
};
