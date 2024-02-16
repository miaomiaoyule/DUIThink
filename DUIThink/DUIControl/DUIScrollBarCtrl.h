#ifndef __DUISCROLLBARCTRL_H__
#define __DUISCROLLBARCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIContainerCtrl;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIScrollBarCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIScrollBarCtrl = { 0xE2E6839C,0xBEB5,0x4F40,0xAB,0x9F,0xF3,0xC0,0xD9,0xD1,0xFC,0x73 };
class DUITHINK_API CDUIScrollBarCtrl
	: public CDUISliderCtrl
{
	friend class CDUIContainerCtrl;

	DuiDeclare_CreateControl(CDUIScrollBarCtrl)
	MMDeclare_ClassName(CDUIScrollBarCtrl)

public:
	CDUIScrollBarCtrl(void);
	~CDUIScrollBarCtrl(void);

	//attrubute
protected:
	CDUIAttributeLong					m_AttributeScrollSpeed;

	//btn pos
	CDUIAttributeGroup					m_AttributeGroupUpDownBtnVert;
	CDUIAttributeLong					m_AttributeUpDownBtnVertInsetLeft;
	CDUIAttributeLong					m_AttributeUpDownBtnVertInsetRight;
	CDUIAttributeLong					m_AttributeUpDownBtnVertFixedHeight;

	CDUIAttributeGroup					m_AttributeGroupUpDownBtnHoriz;
	CDUIAttributeLong					m_AttributeUpDownBtnHorizInsetTop;
	CDUIAttributeLong					m_AttributeUpDownBtnHorizInsetBottom;
	CDUIAttributeLong					m_AttributeUpDownBtnHorizFixedWidth;

	//btn status
	CDUIAttributeGroup					m_AttributeGroupUpBtnStatus;
	CDUIAttributeColorSwitch			m_AttributeColorUpBtnNormal;
	CDUIAttributeColorSwitch			m_AttributeColorUpBtnHot;
	CDUIAttributeColorSwitch			m_AttributeColorUpBtnPushed;
	CDUIAttriImageSection				m_AttributeImageUpBtnNormal;
	CDUIAttriImageSection				m_AttributeImageUpBtnHot;
	CDUIAttriImageSection				m_AttributeImageUpBtnPushed;

	CDUIAttributeGroup					m_AttributeGroupDownBtnStatus;
	CDUIAttributeColorSwitch			m_AttributeColorDownBtnNormal;
	CDUIAttributeColorSwitch			m_AttributeColorDownBtnHot;
	CDUIAttributeColorSwitch			m_AttributeColorDownBtnPushed;
	CDUIAttriImageSection				m_AttributeImageDownBtnNormal;
	CDUIAttriImageSection				m_AttributeImageDownBtnHot;
	CDUIAttriImageSection				m_AttributeImageDownBtnPushed;

	//variant
protected:
	CDUIRect							m_rcUpBtn;
	CDUIRect							m_rcDownBtn;

	int									m_nUpBtnStatus;
	int									m_nDownBtnStatus;

	CDUIPoint							m_ptOnUpDownBtn;

	LONG								delay_deltaY_;
	DWORD								delay_number_;
	DWORD								delay_left_;

	CDUIContainerCtrl *					m_pOwnerCtrl = NULL;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//owner
	virtual void SetOwnerCtrl(CDUIContainerCtrl *pOwnerCtrl);

	//value
	void SetMaxValue(int nMaxValue) override;
	void SetCurValue(int nCurValue) override;

	//scroll
	virtual int GetScrollSpeed();

	//btn pos
	virtual int GetUpDownBtnVertInsetLeft();
	virtual int GetUpDownBtnVertInsetRight();
	virtual int GetUpDownBtnVertFixedHeight();
	virtual int GetUpDownBtnHorizInsetTop();
	virtual int GetUpDownBtnHorizInsetBottom();
	virtual int GetUpDownBtnHorizFixedWidth();
	virtual void SetUpDownBtnVertInsetLeft(int nLeft);
	virtual void SetUpDownBtnVertInsetRight(int nRight);
	virtual void SetUpDownBtnVertFixedHeight(int nHeight);
	virtual void SetUpDownBtnHorizInsetTop(int nTop);
	virtual void SetUpDownBtnHorizInsetBottom(int nBottom);
	virtual void SetUpDownBtnHorizFixedWidth(int nWidth);

	//upbtn status
	virtual vector<CMMString> GetColorResSwitchUpBtnNormal();
	virtual vector<CMMString> GetColorResSwitchUpBtnHot();
	virtual vector<CMMString> GetColorResSwitchUpBtnPushed();
	virtual void SetColorResSwitchUpBtnNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchUpBtnHot(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchUpBtnPushed(const vector<CMMString> &vecResSwitch);

	virtual tagDuiImageSection GetImageSectionUpBtnNormal();
	virtual tagDuiImageSection GetImageSectionUpBtnHot();
	virtual tagDuiImageSection GetImageSectionUpBtnPushed();
	virtual void SetImageSectionUpBtnNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionUpBtnHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionUpBtnPushed(const tagDuiImageSection &ImageSection);

	//downbtn status
	virtual vector<CMMString> GetColorResSwitchDownBtnNormal();
	virtual vector<CMMString> GetColorResSwitchDownBtnHot();
	virtual vector<CMMString> GetColorResSwitchDownBtnPushed();
	virtual void SetColorResSwitchDownBtnNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchDownBtnHot(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchDownBtnPushed(const vector<CMMString> &vecResSwitch);

	virtual tagDuiImageSection GetImageSectionDownBtnNormal();
	virtual tagDuiImageSection GetImageSectionDownBtnHot();
	virtual tagDuiImageSection GetImageSectionDownBtnPushed();
	virtual void SetImageSectionDownBtnNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionDownBtnHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionDownBtnPushed(const tagDuiImageSection &ImageSection);

	//progress
protected:
	void SetProgressType(enDuiProgressType ProgressType) override;
	void SetProgressMoveType(enDuiProgressMoveType ProgressMoveType) override;

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//paint
protected:
	void PaintStatusColor(HDC hDC) override;
	void PaintStatusImage(HDC hDC) override;

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;

	//help
protected:
	virtual void CalcSubRect();

	virtual void CalcUpDownBtnRect();
	virtual void CalcUpDownBtnRectHoriz();
	virtual void CalcUpDownBtnRectVert();

	virtual void CalcSlipPos();
	virtual void CalcGrooveAndSlipRect();
	virtual void CalcScrollBarThumbSize();

	//help
protected:
	void OnTimerScrollUpDownBtn(CDUIPoint ptCurMouse);
	void OnTimerScrollMouseWheel();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif