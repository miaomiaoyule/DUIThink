#ifndef __DUIROTATEMENUCTRL_H__
#define __DUIROTATEMENUCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIRotateMenuItemCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIRotateMenuItemCtrl = { 0xB413C6A8,0x0F06,0x4FF8,0x9C,0x8B,0xFF,0x5E,0xE5,0x7A,0xD0,0xAD };
class DUITHINK_API CDUIRotateMenuItemCtrl : public CDUIMenuItemCtrl
{
	DuiDeclare_CreateControl(CDUIRotateMenuItemCtrl)
		MMDeclare_ClassName(CDUIRotateMenuItemCtrl)

public:
	CDUIRotateMenuItemCtrl(void);
	~CDUIRotateMenuItemCtrl(void);

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
#define VER_CDUIRotateMenuCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIRotateMenuCtrl = { 0x46422DE5,0x3874,0x475E,0x92,0x64,0x32,0x2C,0x5C,0x7D,0xB1,0xE0 };
class DUITHINK_API CDUIRotateMenuCtrl
	: public CDUIMenuCtrl
	, public CDUINotifyPump
{
	DuiDeclare_CreateControl(CDUIRotateMenuCtrl)
	MMDeclare_ClassName(CDUIRotateMenuCtrl)
	DuiDeclare_Message_Map()

public:
	CDUIRotateMenuCtrl(void);
	~CDUIRotateMenuCtrl(void);

	//attribute
protected:
	CDUIAttributeLong					m_AttributeAngleStart;
	CDUIAttributeLong					m_AttributeAngleFinish;
	CDUIAttributeLong					m_AttributeAngleSpaceMin;
	CDUIAttributeBool					m_AttributeRotateClockWise;
	CDUIAttributeSize					m_AttributeItemSizeMin;
	CDUIAttributeSize					m_AttributeItemSizeMax;

	//variant
protected:
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
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIRotateMenuItemCtrl * GetChildAt(int nIndex) const override;

	//scroll
	SIZE GetScrollPos() const override;
	SIZE GetScrollRange() const override;
	void SetScrollPos(SIZE szPos, bool bMsg = true) override;
	void EnsureVisible(int nIndex, bool bCenter = true) override;

	//rotate menu
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
	virtual int GetAngleSpace();
	virtual bool IsRotateClockWise();
	virtual void SetRotateClockWise(bool bClockWise);
	virtual float GetChildAngle(int nIndex);

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
