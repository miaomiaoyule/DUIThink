#ifndef __DUISLIDERCTRL_H__
#define __DUISLIDERCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUISliderCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUISliderCtrl = { 0x7E46B557,0x965E,0x4C3F,0xAD,0xEA,0x11,0x86,0x34,0x7A,0x58,0x88 };
class DUITHINK_API CDUISliderCtrl
	: public CDUIProgressCtrl
{
	DuiDeclare_CreateControl(CDUISliderCtrl)
	MMDeclare_ClassName(CDUISliderCtrl)

public:
	CDUISliderCtrl(void);
	~CDUISliderCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupThumbVert;
	CDUIAttributeLong					m_AttributeThumbVertInsetLeft;
	CDUIAttributeLong					m_AttributeThumbVertInsetRight;
	CDUIAttributeLong					m_AttributeThumbVertFixedHeight = 30;

	CDUIAttributeGroup					m_AttributeGroupThumbHoriz;
	CDUIAttributeLong					m_AttributeThumbHorizInsetTop;
	CDUIAttributeLong					m_AttributeThumbHorizInsetBottom;
	CDUIAttributeLong					m_AttributeThumbHorizFixedWidth = 30;

	CDUIAttributeGroup					m_AttributeGroupThumbStatus;
	CDUIAttributeColorSwitch			m_AttributeColorThumbNormal;
	CDUIAttributeColorSwitch			m_AttributeColorThumbHot;
	CDUIAttributeColorSwitch			m_AttributeColorThumbPushed;
	CDUIAttriImageSection				m_AttributeImageThumbNormal;
	CDUIAttriImageSection				m_AttributeImageThumbHot;
	CDUIAttriImageSection				m_AttributeImageThumbPushed;

	//variant
protected:
	CDUIPoint							m_ptLButtonDown;
	CDUIPoint							m_ptThumbLButtonDown;
	CDUIRect							m_rcCurThumb;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	virtual void SetThumbSize(CDUISize szThumb);

	virtual int GetThumbVertInsetLeft();
	virtual int GetThumbVertInsetRight();
	virtual int GetThumbVertFixedHeight();
	virtual int GetThumbHorizInsetTop();
	virtual int GetThumbHorizInsetBottom();
	virtual int GetThumbHorizFixedWidth();
	virtual void SetThumbVertInsetLeft(int nLeft);
	virtual void SetThumbVertInsetRight(int nRight);
	virtual void SetThumbVertFixedHeight(int nHeight);
	virtual void SetThumbHorizInsetTop(int nTop);
	virtual void SetThumbHorizInsetBottom(int nBottom);
	virtual void SetThumbHorizFixedWidth(int nWidth);

	virtual vector<CMMString> GetColorResSwitchThumbNormal();
	virtual vector<CMMString> GetColorResSwitchThumbHot();
	virtual vector<CMMString> GetColorResSwitchThumbPushed();
	virtual void SetColorResSwitchThumbNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchThumbHot(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchThumbPushed(const vector<CMMString> &vecResSwitch);

	virtual tagDuiImageSection GetImageSectionThumbNormal();
	virtual tagDuiImageSection GetImageSectionThumbHot();
	virtual tagDuiImageSection GetImageSectionThumbPushed();
	virtual void SetImageSectionThumbNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionThumbHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionThumbPushed(const tagDuiImageSection &ImageSection);

	//property
protected:
	void InitProperty() override;

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

	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;
	LRESULT OnDuiKeyDown(const DuiMessage &Msg) override;

	//help
protected:
	//内部布局
	virtual void CalcSubPos();

	//滑块位置
	virtual void CalcThumbPos();
	virtual void CalcThumbPosHoriz();
	virtual void CalcThumbPosVert();

	//坐标计算
	virtual void CalcCurValueFromPt(const CDUIPoint &pt);
	virtual void CalcCurValueFromPtHoriz(const CDUIPoint &pt);
	virtual void CalcCurValueFromPtVert(const CDUIPoint &pt);

	//滑块坐标
	virtual void CalcThumbPosOnMouseDown(const CDUIPoint &pt);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif