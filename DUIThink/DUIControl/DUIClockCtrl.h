#ifndef __DUICLOCKCTRL_H__
#define __DUICLOCKCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIClockCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIClockCtrl = { 0x5C22007F,0x8A0B,0x44F2,0x9B,0x83,0x0B,0xDA,0x4A,0x80,0x5A,0x50 }; 
class DUITHINK_API CDUIClockCtrl
	: public CDUIContainerCtrl
{
	DuiDeclare_CreateControl(CDUIClockCtrl)
	MMDeclare_ClassName(CDUIClockCtrl)

public:
	CDUIClockCtrl(void);
	~CDUIClockCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupClock;
	CDUIAttriImageSection				m_AttributeImageClockDial;
	CDUIAttriImageSection				m_AttributeImageClockHour;
	CDUIAttriImageSection				m_AttributeImageClockMinute;
	CDUIAttriImageSection				m_AttributeImageClockSecond;
	CDUIAttriImageSection				m_AttributeImageClockCenterDot;

	//variant
protected:

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//clock
	virtual tagDuiImageSection GetImageSectionClockDial();
	virtual tagDuiImageSection GetImageSectionClockHour();
	virtual tagDuiImageSection GetImageSectionClockMinute();
	virtual tagDuiImageSection GetImageSectionClockSecond();
	virtual tagDuiImageSection GetImageSectionClockCenterDot();
	virtual void SetImageSectionClockDial(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionClockHour(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionClockMinute(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionClockSecond(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionClockCenterDot(const tagDuiImageSection &ImageSection);

	//message
protected:
	void OnDuiWndManagerAttach() override;
	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;

	//proprety
protected:
	void InitProperty();
	void InitComplete();

	//paint
protected:
	void PaintBkImage(HDC hDC) override;

	//help
protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif