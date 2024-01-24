#ifndef __DUIANIMATECTRL_H__
#define __DUIANIMATECTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAnimateCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAnimateCtrl = { 0x9EFF4728,0x7FBA,0x4923,0x95,0x0C,0x40,0xB0,0x4E,0x33,0x63,0x38 };
class DUITHINK_API CDUIAnimateCtrl
	: public CDUIStaticCtrl
{
	DuiDeclare_CreateControl(CDUIAnimateCtrl)
	MMDeclare_ClassName(CDUIAnimateCtrl)

public:
	CDUIAnimateCtrl(void);
	virtual ~CDUIAnimateCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupAnimation;
	CDUIAttriImageSection				m_AttributeImageAnimate;
	CDUIAttributeSize					m_AttributeSequenceFrameSize;
	CDUIAttributeLong					m_AttributeSequenceFrameSpeed;
	CDUIAttributeBool					m_AttributeAutoPlay;

	//variant
protected:
	tagDuiAnimateImageInfo				m_AnimateImageInfo;
	int									m_nFrameCur = 0;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	void SetVisible(bool bVisible = true) override;
	void SetInternVisible(bool bVisible = true) override;

	//animate
	virtual tagDuiImageSection GetImageSectionAnimate();
	virtual void SetImageSectionAnimate(const tagDuiImageSection &ImageSection);
	virtual bool IsAutoPlay();
	virtual void SetAutoPlay(bool bAutoPlay = true);
	virtual int GetFrameCount();
	virtual CDUISize GetSequenceFrameSize();
	virtual void SetSequenceFrameSize(CDUISize szItem);
	virtual int GetSequenceFrameSpeed();
	virtual void SetSequenceFrameSpeed(int nSpeed);
	virtual void PlayAnimate();
	virtual void PauseAnimate();
	virtual void StopAnimate();

	//message
protected:
	void OnDuiWndManagerAttach() override;
	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;

	//property 
protected:
	void InitProperty() override;
	void InitAttriValue() override;

	//paint
protected:
	void PaintStatusImage(HDC hDC) override;

	//help
protected:
	void ConstructAnimateImageInfo();
	void PlayCurFrame();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif