#ifndef __DUIPROGRESSCIRCLECTRL_H__
#define __DUIPROGRESSCIRCLECTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIProgressCircleCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIProgressCircleCtrl = { 0x5A0F773C,0x40CB,0x46DC,0xB9,0xC6,0xE8,0x2C,0xD6,0x60,0x4C,0xC2 };
class DUITHINK_API CDUIProgressCircleCtrl
	: public CDUIProgressCtrl
{
	DuiDeclare_CreateControl(CDUIProgressCircleCtrl)
	MMDeclare_ClassName(CDUIProgressCircleCtrl)

public:
	CDUIProgressCircleCtrl(void);
	~CDUIProgressCircleCtrl(void);

	//attribute
protected:
	//slip
	CDUIAttributeLong					m_AttributeSlipWide = 1;

	//variant
protected:

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	virtual int GetSlipWide();
	virtual void SetSlipWide(int nWide);

	//property
protected:
	void InitProperty() override;

	//paint
protected:
	void PaintStatusColor(HDC hDC) override;
	void PaintStatusImage(HDC hDC) override;

	//help
protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif