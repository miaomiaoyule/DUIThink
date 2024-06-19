#ifndef __DUIPROGRESSCTRL_H__
#define __DUIPROGRESSCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIProgressCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIProgressCtrl = { 0x67F028D8,0xA87F,0x40F8,0xB6,0x86,0xC2,0x24,0x85,0x98,0x52,0xE4 };
class DUITHINK_API CDUIProgressCtrl
	: public CDUIStaticCtrl
{
	DuiDeclare_CreateControl(CDUIProgressCtrl)
	MMDeclare_ClassName(CDUIProgressCtrl)

public:
	CDUIProgressCtrl(void);
	~CDUIProgressCtrl(void);

	//attribute
protected:
	//progress
	CDUIAttributeGroup					m_AttributeGroupProgressType;
	CDUIAttributeCombox					m_AttributeProgressType;
	CDUIAttributeCombox					m_AttributeProgressMoveType;

	//value
	CDUIAttributeGroup					m_AttributeGroupValue;
	CDUIAttributeLong					m_AttributeMaxValue = 100;
	CDUIAttributeLong					m_AttributeCurValue;

	//groove
	CDUIAttributeGroup					m_AttributeGroupGroove;
	CDUIAttributeColorSwitch			m_AttributeColorGrooveNormal;
	CDUIAttributeColorSwitch			m_AttributeColorGrooveHot;
	CDUIAttriImageSection				m_AttributeImageGrooveNormal;
	CDUIAttriImageSection				m_AttributeImageGrooveHot;
	CDUIAttributeRect					m_AttributeGrooveInsetToBorder;

	//slip
	CDUIAttributeGroup					m_AttributeGroupSlip;
	CDUIAttributeColorSwitch			m_AttributeColorSlipNormal;
	CDUIAttributeColorSwitch			m_AttributeColorSlipHot;
	CDUIAttriImageSection				m_AttributeImageSlipNormal;
	CDUIAttriImageSection				m_AttributeImageSlipHot;
	CDUIAttributeRect					m_AttributeSlipInsetToGroove;

	//variant
protected:
	CDUIRect							m_rcCurSlip;								//当前滑条
	CDUIRect							m_rcGroove;									//滑槽
	CDUIRect							m_rcSlip;									//滑条

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void RefreshView() override;

	//value
	virtual int GetMaxValue();
	virtual void SetMaxValue(int nMaxValue);
	virtual int GetCurValue();
	virtual void SetCurValue(int nCurValue);
	virtual double GetValueScale();

	//color
	virtual vector<CMMString> GetColorResSwitchGrooveNormal();
	virtual vector<CMMString> GetColorResSwitchGrooveHot();
	virtual vector<CMMString> GetColorResSwitchSlipNormal();
	virtual vector<CMMString> GetColorResSwitchSlipHot();
	virtual void SetColorResSwitchGrooveNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchGrooveHot(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchSlipNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchSlipHot(const vector<CMMString> &vecResSwitch);

	//image
	virtual tagDuiImageSection GetImageSectionGrooveNormal();
	virtual tagDuiImageSection GetImageSectionGrooveHot();
	virtual tagDuiImageSection GetImageSectionSlipNormal();
	virtual tagDuiImageSection GetImageSectionSlipHot();
	virtual void SetImageSectionGrooveNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionGrooveHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSlipNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSlipHot(const tagDuiImageSection &ImageSection);

	//rect
	virtual virtual CDUIRect GetGrooveRect();
	virtual virtual CDUIRect GetSlipRect();

	//progress
	virtual virtual enDuiProgressType GetProgressType();
	virtual virtual void SetProgressType(enDuiProgressType ProgressType);
	virtual virtual enDuiProgressMoveType GetProgressMoveType();
	virtual virtual void SetProgressMoveType(enDuiProgressMoveType ProgressMoveType);

	//message
protected:
	bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:
	void PaintStatusColor(HDC hDC) override;
	void PaintStatusImage(HDC hDC) override;

	//help
protected:
	virtual void CalcSubPos();
	virtual void CalcSubRect();

	//slip
	virtual void CalcSlipPos();
	virtual void CalcHorizSlipPos();
	virtual void CalcVertSlipPos();
	virtual void CalcGrooveAndSlipRect();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif