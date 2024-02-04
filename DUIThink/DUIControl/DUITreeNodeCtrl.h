#ifndef __DUITREENODECTRL_H__
#define __DUITREENODECTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUITreeNodeCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUITreeNodeCtrl = { 0x6A8110CA,0xA91C,0x43A8,0x97,0x88,0xAF,0xE8,0xAB,0xD2,0x82,0x66 };
class DUITHINK_API CDUITreeNodeCtrl 
	: public CDUIListItemCtrl
{
	friend class CDUITreeViewCtrl;

	DuiDeclare_CreateControl(CDUITreeNodeCtrl)
	MMDeclare_ClassName(CDUITreeNodeCtrl)

public:
	CDUITreeNodeCtrl(void);
	~CDUITreeNodeCtrl(void);

	//attribute
protected:

	//variant
protected:
	CDUIVerticalLayoutCtrl *			m_pVertContainerCtrl = NULL;
	CDUIHorizontalLayoutCtrl *			m_pHorizContainerCtrl = NULL;
	CDUICheckBoxCtrl *					m_pCheckExpandCtrl = NULL;
	CDUITreeViewCtrl *					m_pTreeViewCtrl = NULL;

	bool								m_bSelectChild = true;
	bool								m_bSelectParent = true;

	//override
protected:
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnRelease(CDUIControlBase *pControl) override;
	void OnPaint(CDUIControlBase *pControl, HDC hDC) override;
	void OnSize(CDUIControlBase *pControl) override;
	void OnVisibleChanged(CDUIControlBase *pControl) override;

	void OnSelectChangedExpand();
	void OnSelectChangedSelect();
	void OnVisibleChangedExpand();
	void OnVisibleChangedSelect();

	//override
protected:
	void SetOwner(CDUIListViewCtrl *pOwner) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void NeedParentRefreshView() override;

	//override
	CDUITreeNodeCtrl * GetPrevSiblingCtrl() override;
	CDUITreeNodeCtrl * GetNextSiblingCtrl() override;
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;
	void RefreshView() override;
	void RefreshSubItem() override;

	//listitem
	void EnsureVisible(bool bCenter = false) override;
	bool IsSelected() const override;
	bool Select(bool bSelect = true) override;
	void SetSelectIconVisible(bool bVisible) override;
	void SetSelectIconLeftPadding(int nLeft) override;
	void SetSelectIconFixedWidth(int nWidth) override;
	void SetSelectIconFixedHeight(int nHeight) override;
	void SetImageSectionSelIconNormal(const tagDuiImageSection &ImageSection) override;
	void SetImageSectionSelIconHot(const tagDuiImageSection &ImageSection) override;
	void SetImageSectionSelIconPushed(const tagDuiImageSection &ImageSection) override;
	void SetImageSectionSelIconSelNormal(const tagDuiImageSection &ImageSection) override;
	void SetImageSectionSelIconSelHot(const tagDuiImageSection &ImageSection) override;
	void SetImageSectionSelIconSelPushed(const tagDuiImageSection &ImageSection) override;
	void PerformEditText() override;

	//treenode property
	virtual bool IsExpandEnable();
	virtual void SetExpandEnable(bool bEnable);
	virtual bool IsExpandDbClick();
	virtual void SetExpandDbClick(bool bDbClick);
	virtual bool IsExpandIconVisible();
	virtual void SetExpandIconVisible(bool bVisible);
	virtual bool IsSelectSingle();
	virtual void SetSelectSingle(bool bSingle);
	virtual int GetExpandIconLeftPadding();
	virtual void SetExpandIconLeftPadding(int nLeft);
	virtual int GetExpandIconFixedWidth();
	virtual void SetExpandIconFixedWidth(int nWidth);
	virtual int GetExpandIconFixedHeight();
	virtual void SetExpandIconFixedHeight(int nHeight);
	virtual int GetChildIndent();
	virtual void SetChildIndent(int nIndent);
	virtual tagDuiImageSection GetImageSectionExpandIconNormal();
	virtual tagDuiImageSection GetImageSectionExpandIconHot();
	virtual tagDuiImageSection GetImageSectionExpandIconPushed();
	virtual tagDuiImageSection GetImageSectionExpandIconSelNormal();
	virtual tagDuiImageSection GetImageSectionExpandIconSelHot();
	virtual tagDuiImageSection GetImageSectionExpandIconSelPushed();
	virtual void SetImageSectionExpandIconNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionExpandIconHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionExpandIconPushed(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionExpandIconSelNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionExpandIconSelHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionExpandIconSelPushed(const tagDuiImageSection &ImageSection);

	//treenode operator
	virtual int GetLevel();
	virtual bool IsExpanded() const;
	virtual bool Expand(bool bExpand = true);
	virtual CDUICheckBoxCtrl * GetExpandIconCtrl();
	virtual int GetChildNodeIndex(CDUITreeNodeCtrl *pTreeNode);
	virtual int GetChildNodeCount();
	virtual int GetSelectNodeCount();
	virtual bool InsertChildNode(int nItemModelCount, int nPos = -1);
	virtual bool InsertChildNode(CDUIControlBase *pTreeNode, int nPos = -1);
	virtual bool RemoveChildNode(CDUIControlBase *pTreeNode);
	virtual bool RemoveChildNodeAt(int nIndex);
	virtual void RemoveChildNodeAll();
	virtual CDUITreeViewCtrl * GetOwnerView();
	virtual CDUITreeViewCtrl * GetTreeView();
	virtual CDUITreeViewCtrl * GetRootView();
	virtual CDUITreeNodeCtrl * GetRootNode();
	virtual CDUITreeNodeCtrl * GetParentNode();
	virtual CDUITreeNodeCtrl * GetOwnerNode(int nLevel);
	virtual CDUITreeNodeCtrl * GetChildNode(int nIndex);
	virtual CDUITreeNodeCtrl * GetLastNode();
	virtual CDUITreeNodeCtrl * GetCurSelNode();
	virtual void SelectChildNode(bool bSelect);

	//message
protected:
	bool OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//property
protected:
	void InitComplete() override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild = true) override;

	//paint
protected:
	void PaintLine(HDC hDC) override;
	void PaintLineOnListType(HDC hDC) override;

	//help
protected:
	CDUIRect GetTextRange() override;
	CDUIRect GetBackRange() override;
	tagDuiListInfo GetItemStyleInfo() override;
	CDUIControlBase * FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags) override;
	void SendNotify(enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0, CMMString strTextOld = _T("")) override;
	void PerformItemMouseDown(bool bLeft, const CDUIPoint &pt) override;
	virtual bool SelectTreeNode(CDUITreeNodeCtrl *pFromNode, CDUITreeNodeCtrl *pStopNode, bool &bFindFrom);
	virtual void SelectTreeNodeAll(bool bSelect);
	virtual void OnSelectChangedNode(bool bSelect);
	virtual void CalcNodeHeight();
	virtual void ConstructTreeView();
	virtual void OnChildNodeChanged(bool bAdd);
};

typedef std::vector<CDUITreeNodeCtrl*> VecDUITreeNodeCtrl;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif