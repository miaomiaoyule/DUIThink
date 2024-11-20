#pragma once

//////////////////////////////////////////////////////////////////////////
class CDlgMain
	: public CDUIWnd
{
	MMDeclare_ClassName(CDlgMain)
	DuiDeclare_Message_Map()

public:
	CDlgMain();
	virtual ~CDlgMain();

	//control
protected:
	CDUIControlBase *							m_pDragingCtrl = NULL;
	CDUIHorizontalLayoutCtrl *					m_pHorizMoveCtrl = NULL;
	CDUIContainerCtrl *							m_pContainerEditCtrl = NULL;
	CDUIVerticalLayoutCtrl *					m_pVertControlsCtrl = NULL;

	//variant
protected:
	CDUIPoint									m_ptMouseDown;
	CDUIPoint									m_ptMouseLast;
	bool										m_bDragFromControls = true;
	CMMMagnetBox								m_MagnetBox;
	std::vector<CDUIControlBase*>				m_vecDragCtrl;

	//override
protected:
	void OnFindControl() override;
	void OnInitDialog() override;

	//message
protected:

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiWndLayout(const DuiNotify &Notify);
	void OnDuiLButtonDown_DragBegin(const DuiNotify &Notify);
	void OnDuiMouseMove_DragMove(const DuiNotify &Notify);
	void OnDuiLButtonUp_DragEnd(const DuiNotify &Notify);

	//help
protected:
	void PerformDragBegin(CDUIControlBase *pDragCtrl);
	void PerformDragMove();
	void PerformDragEnd();
	void PerformDragAttachNearest(int nIndexNearest);
	CDUIRect PerformScaleBox(CDUIRect rcBox);
	CDUIRect PerformRestoreBox(CDUIRect rcBox);
};
