#pragma once

//////////////////////////////////////////////////////////////////////////
class CSvgImageView 
	: public CDUINotifyPump
	, public IDUIPreMessage
{
	DuiDeclare_Message_Map()

public:
	CSvgImageView();
	~CSvgImageView();

	//contorl
protected:
	CDUIContainerCtrl *					m_pSvgImageViewCtrl = NULL;

	CDUIStaticCtrl *					m_pStaticSvgImageCtrl = NULL;
	CDUISliderCtrl *					m_pSliderDpiScaleCtrl = NULL;

	//variant
protected:
	CMMString							m_strSvgImageFile;
	CMMString							m_strTip;

	//override
protected:
	LRESULT OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//method
public:
	void Attach(CDUIContainerCtrl *pViewBase);

	//notify
protected:
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiValueChangedDpiScale(const DuiNotify &Notify);
	void OnDuiVisibleChangedSvgImageView(const DuiNotify &Notify);

	//message
protected:
	void OnWMDropFile(WPARAM wParam, LPARAM lParam);

	//help
protected:
	void ConstructSvgImageBk();
	int GetCurScale();
};

//////////////////////////////////////////////////////////////////////////

