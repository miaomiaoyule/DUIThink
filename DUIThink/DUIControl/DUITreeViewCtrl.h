#ifndef __DUITREEVIEWCTRL_H__
#define __DUITREEVIEWCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUITreeViewCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUITreeViewCtrl = { 0xECC674C4,0xCF39,0x4C5D,0xB4,0xB3,0xA1,0x0B,0x4E,0xF4,0xA2,0x09 };
class DUITHINK_API CDUITreeViewCtrl
	: public CDUIListViewCtrl
	, public IDUIControlCallBack
{
	friend class CDUITreeNodeCtrl;

	DuiDeclare_CreateControl(CDUITreeViewCtrl)
	MMDeclare_ClassName(CDUITreeViewCtrl)

public:
	CDUITreeViewCtrl(void);
	~CDUITreeViewCtrl(void);

	//attribute
protected:
	//tree node
	CDUIAttributeGroup					m_AttributeGroupTreeNode;
	CDUIAttributeBool					m_AttributeExpandEnable = true;
	CDUIAttributeBool					m_AttributeExpandDbClick = true;
	CDUIAttributeBool					m_AttributeExpandIconVisible = true;
	CDUIAttributeBool					m_AttributeSelectSingle;
	CDUIAttributeLong					m_AttributeExpandIconLeftPadding;
	CDUIAttributeLong					m_AttributeExpandIconFixedWidth = 30;
	CDUIAttributeLong					m_AttributeExpandIconFixedHeight = 30;
	CDUIAttributeLong					m_AttributeChildIndent = 17;

	CDUIAttributeGroup					m_AttributeGroupTreeExpandIcon;
	CDUIAttriImageSection				m_AttributeImageExpandIconNormal;
	CDUIAttriImageSection				m_AttributeImageExpandIconHot;
	CDUIAttriImageSection				m_AttributeImageExpandIconPushed;
	CDUIAttriImageSection				m_AttributeImageExpandIconSelNormal;
	CDUIAttriImageSection				m_AttributeImageExpandIconSelHot;
	CDUIAttriImageSection				m_AttributeImageExpandIconSelPushed;

	//variant
protected:
	CDUITreeNodeCtrl *					m_pOwnerNodeCtrl = NULL;
	CDUITreeNodeCtrl *					m_pCurNodeCtrl = NULL;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//override
protected:
	void SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl) override;

	//override
protected:
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;

	void OnClickNode(CDUITreeNodeCtrl *pTreeNode);
	void OnDbClickNode(CDUITreeNodeCtrl *pTreeNode);

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//override
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	bool InsertChild(int nItemModelCount, int nPos = -1) override;
	CDUITreeNodeCtrl * GetChildAt(int nIndex) const override;
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;
	void NeedRefreshView() override;
	void NeedParentRefreshView() override;

	//refresh
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//listview
	void EnsureVisible(int nIndex, bool bCenter = false) override;

	//tree property
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

	//tree operator
	virtual CDUITreeViewCtrl * GetRootView();
	virtual CDUITreeNodeCtrl * GetOwnerNode();
	virtual CDUITreeNodeCtrl * GetRootNode();
	virtual CDUITreeNodeCtrl * GetCurSelNode();
	virtual CDUITreeNodeCtrl * ComparePreNode(CDUITreeNodeCtrl *pFirstNode, CDUITreeNodeCtrl *pNextNode);
	virtual void EnsureVisible(CDUITreeNodeCtrl *pTreeNode, bool bCenter = false);
	virtual void SetCurSelNode(CDUITreeNodeCtrl *pCurNode);
	virtual void SelectTreeNode(CDUITreeNodeCtrl *pFromNode, CDUITreeNodeCtrl *pStopNode);
	virtual void SelectTreeNodeAll(bool bSelect);

	//message
protected:
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:
	virtual void PaintLine(HDC hDC);

	//help
protected:
	virtual void SetOwnerNode(CDUITreeNodeCtrl *pOwnerNode);
	virtual int GetSelectNodeCount();
	CDUITreeNodeCtrl * ProductItemDefault() override;
};

typedef std::vector<CDUITreeViewCtrl*> VecDUITreeViewCtrl;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif