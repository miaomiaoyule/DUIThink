#ifndef __DUIVERTLAYOUTCTRL_H__
#define __DUIVERTLAYOUTCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIVerticalLayoutCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIVerticalLayoutCtrl = { 0x53C742CF,0x879E,0x47D3,0xAD,0x86,0xC6,0x44,0x24,0xE5,0xC6,0x77 };
class DUITHINK_API CDUIVerticalLayoutCtrl 
	: public CDUIContainerCtrl
{
	DuiDeclare_CreateControl(CDUIVerticalLayoutCtrl)
	MMDeclare_ClassName(CDUIVerticalLayoutCtrl)

public:
	CDUIVerticalLayoutCtrl(void);
	~CDUIVerticalLayoutCtrl(void);

	//attribute
protected:
	//animate
	CDUIAttributeGroup					m_AttributeGroupAnimation;
	CDUIAttributeBool					m_AttributeAnimateDrag;

	//variant
protected:
	CDUIAnimationDrag *					m_pAnimateDrag = NULL;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	CMMString GetDescribe() const;
	UINT GetControlFlags() ;

	//basic
	bool IsAnimateDrag();
	void SetAnimateDrag(bool bAnimateDrag) override;
	bool IsAnimateDraging() override;
	void OnDragMoveBegin(CDUIControlBase *pDragCtrl) override;
	void OnDragMoving(CDUIControlBase *pDragCtrl) override;
	void OnDragMoveEnd(CDUIControlBase *pDragCtrl) override;

	//child
	int TranslateIndex(CDUIPoint pt) override;

	//refresh
	void RefreshView();
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//property
protected:
	void InitProperty();
	void InitComplete();

	//message
protected:
	void OnDuiWndManagerAttach() override;
	void OnDuiWndManagerDetach() override;
	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;

	//help
protected:
	void ScrollChilds(CDUISize szScroll);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif