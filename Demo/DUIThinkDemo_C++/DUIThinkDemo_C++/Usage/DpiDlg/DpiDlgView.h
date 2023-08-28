#pragma once

//////////////////////////////////////////////////////////////////////////
class CDpiDlgView : public CDUINotifyPump
{
	DuiDeclare_Message_Map()

public:
	CDpiDlgView();
	~CDpiDlgView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pDpiDlgViewCtrl = NULL;

	CDUIButtonCtrl *					m_pBtnDpiDlg1Ctrl = NULL;
	CDUISliderCtrl *					m_pSliderDpiScaleCtrl = NULL;

	//variant
protected:
	HWND								m_hWndDpiDlg = NULL;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);
	void SetDpiDlgHandle(HWND hWndDpiDlg);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiClickDpiDlg1(const DuiNotify &Notify);
	void OnDuiValueChangedDpiScale(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////

