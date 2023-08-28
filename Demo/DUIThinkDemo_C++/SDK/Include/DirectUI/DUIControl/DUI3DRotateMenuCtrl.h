#ifndef __DUI3DROTATEMENUCTRL_H__
#define __DUI3DROTATEMENUCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUI3DRotateMenuItemCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUI3DRotateMenuItemCtrl = { 0xB413C6A8,0x0F06,0x4FF8,0x9C,0x8B,0xFF,0x5E,0xE5,0x7A,0xD0,0xAD };
class DUITHINK_API CDUI3DRotateMenuItemCtrl : public CDUIMenuItemCtrl
{
	DuiDeclare_CreateControl(CDUI3DRotateMenuItemCtrl)
		MMDeclare_ClassName(CDUI3DRotateMenuItemCtrl)

public:
	CDUI3DRotateMenuItemCtrl(void);
	~CDUI3DRotateMenuItemCtrl(void);

	//method
public:
	//override
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	CMMString GetDescribe() const;

	//message
protected:
	bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg);
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg);
};

//////////////////////////////////////////////////////////////////////////
#define VER_CDUI3DRotateMenuCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUI3DRotateMenuCtrl = { 0x46422DE5,0x3874,0x475E,0x92,0x64,0x32,0x2C,0x5C,0x7D,0xB1,0xE0 };
class DUITHINK_API CDUI3DRotateMenuCtrl
	: public CDUIMenuCtrl
	, public CDUINotifyPump
{
	DuiDeclare_CreateControl(CDUI3DRotateMenuCtrl)
	MMDeclare_ClassName(CDUI3DRotateMenuCtrl)
	DuiDeclare_Message_Map()

public:
	CDUI3DRotateMenuCtrl(void);
	~CDUI3DRotateMenuCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroup3DRotateMenu;
	CDUIAttributeSize					m_AttributeItemSizeMin;
	CDUIAttributeSize					m_AttributeItemSizeMax;
	CDUIAttributeLong					m_AttributeAngleStart;
	CDUIAttributeLong					m_AttributeAngleFinish;
	CDUIAttributeLong					m_AttributeAngleSpaceMin;
	CDUIAttributeBool					m_AttributeRotateClockWise;

	//variant
protected:
	bool								m_bFirstLayout = true;
	int									m_nAngleSpace = 0;
	int									m_nAngleScrollCur = 0;
	int									m_nAngleScrollRange = 0;
	CDUISize							m_szRadius;
	CDUIPoint							m_ptMouseDown;
	CDUIPoint							m_ptLastMousePos;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//basic
	bool SetWndManager(CDUIWndManager *pWndManager) override;

	//refresh
	void RefreshView() override;

	//scroll
	virtual void EnsureVisible(int nIndex);

	//3drotate menu
	virtual CDUISize GetItemSizeMin();
	virtual void SetItemSizeMin(CDUISize szItem);
	virtual CDUISize GetItemSizeMax();
	virtual void SetItemSizeMax(CDUISize szItem);
	virtual int GetAngleStart();
	virtual void SetAngleStart(int nAngle);
	virtual int GetAngleFinish();
	virtual void SetAngleFinish(int nAngle);
	virtual int GetAngleSpaceMin();
	virtual void SetAngleSpaceMin(int nAngle);
	virtual bool IsRotateClockWise();
	virtual void SetRotateClockWise(bool bClockWise);

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//proprety
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//paint
protected:
	void PaintBkColor(HDC hDC) override;

	//notify
protected:
	void OnDuiItemButtonDown(const DuiNotify &Notify);
	void OnDuiItemMouseMove(const DuiNotify &Notify);

	//help
protected:
	CDUIListItemCtrl * ProductItemDefault();
	void ScrollChildsByWheelUp(int nOffset);
	void ScrollChildsByWheelDown(int nOffset);
	void ScrollChilds(CDUIPoint ptMouse);
	void ScrollChilds(CDUISize szScroll);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
