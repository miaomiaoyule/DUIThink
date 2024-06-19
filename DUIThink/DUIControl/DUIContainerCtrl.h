#ifndef __DUICONTAINERCTRL_H__
#define __DUICONTAINERCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDTDesignerDoc;
class CDUIGlobal;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIContainerCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIContainerCtrl = { 0x0A5001A1,0xE296,0x44B4,0xB9,0x5E,0xF7,0x11,0x8C,0x03,0x92,0x82 };
class DUITHINK_API CDUIContainerCtrl
	: public CDUIControlBase
{
	friend class CDUIGlobal;
	friend class CDUIXmlPack;
	friend class CDUIControlBase;
	friend class CDUIScrollBarCtrl;
	friend class CDUIWndManager;
	friend class CDUIListHeaderCtrl;
	friend class CDUIListViewCtrl;
	friend class CDTDesignerDoc;
	friend class CDUIWnd;
	friend class CDUILayoutView;
	friend class CDUITreeNodeCtrl;

	DuiDeclare_CreateControl(CDUIContainerCtrl)
	MMDeclare_ClassName(CDUIContainerCtrl)

public:
	CDUIContainerCtrl(void);
	~CDUIContainerCtrl(void);

	//attribute
protected:
	//position
	CDUIAttributeBool					m_AttributeAutoCalcWidth;
	CDUIAttributeBool					m_AttributeAutoCalcHeight;

	//scrollbar
	CDUIAttributeGroup					m_AttributeGroupScroll;
	CDUIAttributeBool					m_AttributeScrollBarSpeedModel = true;
	CDUIAttributeBool					m_AttributeUseHorizScrollBar;
	CDUIAttributeBool					m_AttributeUseVertScrollBar;
	CDUIAttributeBool					m_AttributeHScrollBarPushItemRange = true;
	CDUIAttributeBool					m_AttributeVScrollBarPushItemRange = true;

	//child
	CDUIAttributeGroup					m_AttributeGroupChild;
	CDUIAttributeLong					m_AttributeChildPaddingH;
	CDUIAttributeLong					m_AttributeChildPaddingV;
	CDUIAttributeBool					m_AttributeAutoCalcChildPaddingH;
	CDUIAttributeBool					m_AttributeAutoCalcChildPaddingV;
	CDUIAttributeRect					m_AttributeRangeInset;

	//variant
protected:
	VecDuiControlBase					m_vecChilds;

	CDUIScrollBarCtrl *					m_pHorizScrollBarCtrl;
	CDUIScrollBarCtrl *					m_pVertScrollBarCtrl;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	void OnResourceDelete(CDUIResourceBase *pResourceObj) override;
	void OnResourceSwitch(int nIndexRes) override;
	bool RegisterControlListen(IDUIInterface *pIControlListen) override;
	bool UnRegisterControlListen(IDUIInterface *pIControlListen) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	CDUIContainerCtrl * Clone(bool bIncludeChild = true, bool bRefreshCtrlID = true) override;

	//basic
	UINT InitCtrlID() override;
	void RefreshCtrlID() override;
	bool SetWndManager(CDUIWndManager *pWndManager) override;
	void SetVisible(bool bVisible = true) override;

	//modal
	void SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl) override;

	//layout
	virtual void MovePosition(SIZE szOffset);
	virtual bool IsAutoCalcWidth();
	virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
	virtual bool IsAutoCalcHeight();
	virtual void SetAutoCalcHeight(bool bAutoCalcHeight);
	virtual CDUIRect GetRangeInset();
	virtual void SetRangeInset(CDUIRect rcInset);

	//timer
	bool SetTimer(UINT uTimerID, UINT nElapse) override;
	bool KillTimer(UINT uTimerID) override;
	bool KillTimer() override;

	//animation
	virtual bool IsAnimateDrag();
	virtual void SetAnimateDrag(bool bAnimateDrag);
	virtual bool IsAnimateDraging();
	virtual void OnDragMoveBegin(CDUIControlBase *pDragCtrl);
	virtual void OnDragMoving(CDUIControlBase *pDragCtrl);
	virtual void OnDragMoveEnd(CDUIControlBase *pDragCtrl);

	//scrollbar
	virtual bool IsScrollBarSpeedModel();
	virtual void SetScrollBarSpeedModel(bool bSpeedModel);
	virtual bool IsUseHorizScrollBar();
	virtual void SetUseHorizScrollBar(bool bUseHorizScrollBar);
	virtual bool IsUseVertScrollBar();
	virtual void SetUseVertScrollBar(bool bUseVertScrollBar);
	virtual bool IsHScrollBarPushItemRange();
	virtual void SetHScrollBarPushItemRange(bool bPushItemRange);
	virtual bool IsVScrollBarPushItemRange();
	virtual void SetVScrollBarPushItemRange(bool bPushItemRange);
	virtual SIZE GetScrollPos() const;
	virtual SIZE GetScrollRange() const;
	virtual void SetScrollPos(SIZE szPos, bool bMsg = true);
	virtual void LineUp();
	virtual void LineDown();
	virtual void PageUp();
	virtual void PageDown();
	virtual void HomeUp();
	virtual void EndDown();
	virtual void LineLeft();
	virtual void LineRight();
	virtual void PageLeft();
	virtual void PageRight();
	virtual void HomeLeft();
	virtual void EndRight();
	virtual void EnableScrollBar(bool bEnableHorizontal = true, bool bEnableVertical = true);
	virtual CDUIScrollBarCtrl * GetHorizScrollBar() const override;
	virtual CDUIScrollBarCtrl * GetVertScrollBar() const override;

	//child
	virtual int GetChildCount() const;
	virtual bool VerifyChild(CDUIControlBase *pControl);
	virtual bool InsertChild(CDUIControlBase *pChild, int nPos = -1);
	virtual CDUIControlBase * DetachChild(int nIndex);
	virtual CDUIControlBase * DetachChild(CDUIControlBase *pControl);
	virtual int GetChildIndex(CDUIControlBase *pChild) const;
	virtual bool SetChildIndex(CDUIControlBase *pControl, int nIndex);
	virtual bool SwapChild(CDUIControlBase *pFirstCtrl, CDUIControlBase *pNextCtrl, bool bNeedRefreshView = true);
	virtual bool SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView = true);
	virtual CDUIControlBase * GetChildAt(int nIndex) const;
	virtual int GetChildPaddingH();
	virtual int GetChildPaddingV();
	virtual CDUISize GetChildPadding();
	virtual void SetChildPadding(int nChildPaddingH, int nChildPaddingV);
	virtual bool IsAutoCalcChildPaddingH();
	virtual void SetAutoCalcChildPaddingH(bool bAuto);
	virtual bool IsAutoCalcChildPaddingV();
	virtual void SetAutoCalcChildPaddingV(bool bAuto);
	virtual bool Remove(CDUIControlBase *pControl);
	virtual bool RemoveAt(int nIndex);
	virtual void RemoveAll();
	virtual int TranslateIndex(CDUIPoint pt);
	CDUIControlBase * FindSubControl(UINT uCtrlID);
	CDUIControlBase * FindSubControlThisView(UINT uCtrlID);
	int FindNextIndex(int nIndexCur, bool bPositive = true, UINT uFlags = DuiFind_Visible | DuiFind_Enabled, int nNextCount = 1) const;

	//refresh	 
	void RefreshView() override;
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild = true) override;

	//message
protected:
	void OnDuiSize(CDUIRect rcParentAbs) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//paint
protected:

	//help
protected:
	virtual void RefreshScrollBar();
	virtual void RefreshChildInternVisible(bool bTraversal = false);
	virtual CDUIRect GetLayoutRangeOfScrollBar();
	virtual CDUIRect GetLayoutRangeOfItem();
	virtual void UpdateScrollBar(CDUISize szRange);
	virtual void ScrollChilds(CDUISize szScroll);

	void SetInternVisible(bool bVisible = true, bool bTraversal = false) override;
	CDUIControlBase * FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags) override;
	void ReapControl() override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif