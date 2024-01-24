#ifndef __DUIWATERCTRL_H__
#define __DUIWATERCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWaterCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWaterCtrl = { 0x5AC6F49B,0x9E94,0x48BB,0x9C,0x4F,0x53,0xAA,0xFA,0x80,0x6E,0xD8 }; 
class DUITHINK_API CDUIWaterCtrl
	: public CDUIControlBase
{
	DuiDeclare_CreateControl(CDUIWaterCtrl)
	MMDeclare_ClassName(CDUIWaterCtrl)

public:
	CDUIWaterCtrl(void);
	virtual ~CDUIWaterCtrl(void);

	//attribute
protected:

	//variant
protected:
	//water
	tagDuiRippleBitmap *				m_pRippleBmp = NULL;

	//interface
protected:

	//method 
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	void RefreshView() override;
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//message 
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg);
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg);

	//paint
protected:
	void PaintWater(HDC hDC);

	//water help
private:
	void InitWater();
	void UnInitWater();
	void RippleSpread();
	void DropStone(int nX, int nY);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif