#ifndef __DUIINSTRUMENTCTRL_H__
#define __DUIINSTRUMENTCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIInstrumentCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIInstrumentCtrl = { 0xBCD93816,0xCD80,0x4F2F,0xA5,0x83,0xBE,0xC2,0x6C,0x23,0x22,0xB5 }; 
class DUITHINK_API CDUIInstrumentCtrl
	: public CDUIContainerCtrl
{
	DuiDeclare_CreateControl(CDUIInstrumentCtrl)
	MMDeclare_ClassName(CDUIInstrumentCtrl)

public:
	CDUIInstrumentCtrl(void);
	~CDUIInstrumentCtrl(void);

	//attribute
protected:

	//variant
protected:

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//message
protected:

	//proprety
protected:
	void InitProperty();
	void InitComplete();

	//paint
protected:

	//help
protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif