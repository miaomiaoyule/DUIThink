#ifndef __DUIHORZLAYOUTCTRL_H__
#define __DUIHORZLAYOUTCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIHorizontalLayoutCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIHorizontalLayoutCtrl = { 0xC8636BBC,0x7A02,0x4774,0xA4,0x1C,0xE8,0xE4,0x50,0xFD,0x82,0x95 };
class DUITHINK_API CDUIHorizontalLayoutCtrl
	: public CDUIContainerCtrl
{
	DuiDeclare_CreateControl(CDUIHorizontalLayoutCtrl)
	MMDeclare_ClassName(CDUIHorizontalLayoutCtrl)

public:
	CDUIHorizontalLayoutCtrl(void);
	~CDUIHorizontalLayoutCtrl(void);

	//attribute
protected:
	
	//variant
protected:
	CDUIAnimationDrag *					m_pAnimateDrag = NULL;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//basic
	void SetAnimateDrag(bool bAnimateDrag) override;
	bool IsAnimateDraging() override;
	void OnDragMoveBegin(CDUIControlBase *pDragCtrl) override;
	void OnDragMoving(CDUIControlBase *pDragCtrl) override;
	void OnDragMoveEnd(CDUIControlBase *pDragCtrl) override;

	//child
	int TranslateIndex(CDUIPoint pt) override;

	//refresh
	void RefreshView() override;
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//message
protected:
	void OnDuiWndManagerAttach() override;
	void OnDuiWndManagerDetach() override;
	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;

	//help
protected:
	void ScrollChilds(CDUISize szScroll) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif