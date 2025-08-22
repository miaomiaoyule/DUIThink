#include "StdAfx.h"
#include "DUITreeViewCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUITreeViewCtrl)
MMImplement_ClassName(CDUITreeViewCtrl)

CDUITreeViewCtrl::CDUITreeViewCtrl(void)
{
	m_AttributeItemModel.SetDuiType(DuiType_ModelTreeNode);

	return;
}

CDUITreeViewCtrl::~CDUITreeViewCtrl(void)
{
	UnRegisterControlCallBack(this);

	return;
}

bool CDUITreeViewCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeExpandEnable)
	{
		SetExpandEnable(IsExpandEnable());

		return true;
	}
	if (pAttributeObj == &m_AttributeExpandDbClick)
	{
		SetExpandDbClick(IsExpandDbClick());

		return true;
	}
	if (pAttributeObj == &m_AttributeExpandIconVisible)
	{
		SetExpandIconVisible(IsExpandIconVisible());

		return true;
	}
	if (pAttributeObj == &m_AttributeExpandIconLeftPadding)
	{
		SetExpandIconLeftPadding(GetExpandIconLeftPadding());

		return true;
	}
	if (pAttributeObj == &m_AttributeExpandIconFixedWidth)
	{
		SetExpandIconFixedWidth(GetExpandIconFixedWidth());

		return true;
	}
	if (pAttributeObj == &m_AttributeExpandIconFixedHeight)
	{
		SetExpandIconFixedHeight(GetExpandIconFixedHeight());

		return true;
	}
	if (pAttributeObj == &m_AttributeChildIndent)
	{
		SetChildIndent(GetChildIndent());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageExpandIconNormal)
	{
		SetImageSectionExpandIconNormal(GetImageSectionExpandIconNormal());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageExpandIconHot)
	{
		SetImageSectionExpandIconHot(GetImageSectionExpandIconHot());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageExpandIconPushed)
	{
		SetImageSectionExpandIconPushed(GetImageSectionExpandIconPushed());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageExpandIconSelNormal)
	{
		SetImageSectionExpandIconSelNormal(GetImageSectionExpandIconSelNormal());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageExpandIconSelHot)
	{
		SetImageSectionExpandIconSelHot(GetImageSectionExpandIconSelHot());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageExpandIconSelPushed)
	{
		SetImageSectionExpandIconSelPushed(GetImageSectionExpandIconSelPushed());

		return true;
	}

	return false;
}

void CDUITreeViewCtrl::SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl)
{
	return;
}

void CDUITreeViewCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	//view
	if (pControl == this)
	{
		if (DuiNotify::DuiNotifyExtend_None == Notify.DuiNotifyExtend.Type) return;

		switch (Notify.NotifyType)
		{
			case DuiNotify_ItemClick:
			{
				OnClickNode(Notify.DuiNotifyExtend.TreeView.pTreeNode);

				break;
			}
			case DuiNotify_ItemDbClick:
			{
				OnDbClickNode(Notify.DuiNotifyExtend.TreeView.pTreeNode);

				break;
			}
		}

		return;
	}

	return __super::OnNotify(pControl, Notify);
}

void CDUITreeViewCtrl::OnClickNode(CDUITreeNodeCtrl *pTreeNode)
{
	if (NULL == pTreeNode) return;

	if (pTreeNode->IsExpandEnable() && false == IsExpandDbClick())
	{
		pTreeNode->Expand(!pTreeNode->IsExpanded());
	}

	return;
}

void CDUITreeViewCtrl::OnDbClickNode(CDUITreeNodeCtrl *pTreeNode)
{
	if (NULL == pTreeNode) return;

	if (pTreeNode->IsExpandEnable() && IsExpandDbClick())
	{
		pTreeNode->Expand(!pTreeNode->IsExpanded());
	}

	return;
}

LPVOID CDUITreeViewCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUITreeViewCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUITreeViewCtrl::GetDescribe() const
{
	return Dui_Ctrl_TreeView;
}

bool CDUITreeViewCtrl::InsertChild(CDUIControlBase *pChild, int nPos/* = -1*/)
{
	MMInterfaceHelper(CDUITreeNodeCtrl, pChild, pTreeNode);
	if (pTreeNode)
	{
		if (false == __super::InsertChild(pChild, nPos)) return false;
		
		do
		{
			if (false == IsInitComplete()) break;

			CDUITreeViewCtrl *pTreeView = pTreeNode->GetTreeView();
			if (pTreeView)
			{
				pTreeView->SetChildPadding(GetChildPadding().cx, GetChildPadding().cy);
				pTreeView->SetSwitchListItemHeight(GetSwitchListItemHeight());
				pTreeView->SetItemStatusColorResSwitchNormal(GetItemStatusColorResSwitchNormal());
				pTreeView->SetItemStatusColorResSwitchHot(GetItemStatusColorResSwitchHot());
				pTreeView->SetItemStatusColorResSwitchSelNormal(GetItemStatusColorResSwitchSelNormal());
				pTreeView->SetItemStatusColorResSwitchSelHot(GetItemStatusColorResSwitchSelHot());
				pTreeView->SetItemStatusImageSectionNormal(GetItemStatusImageSectionNormal());
				pTreeView->SetItemStatusImageSectionHot(GetItemStatusImageSectionHot());
				pTreeView->SetItemStatusImageSectionSelNormal(GetItemStatusImageSectionSelNormal());
				pTreeView->SetItemStatusImageSectionSelHot(GetItemStatusImageSectionSelHot());
			}
			{
				pTreeNode->SetExpandEnable(IsExpandEnable());
				pTreeNode->SetExpandDbClick(IsExpandDbClick());
				pTreeNode->SetExpandIconVisible(IsExpandIconVisible());
				pTreeNode->SetExpandIconLeftPadding(GetExpandIconLeftPadding());
				pTreeNode->SetExpandIconFixedWidth(GetExpandIconFixedWidth());
				pTreeNode->SetExpandIconFixedHeight(GetExpandIconFixedHeight());
				pTreeNode->SetChildIndent(GetChildIndent());
				pTreeNode->SetImageSectionExpandIconNormal(GetImageSectionExpandIconNormal());
				pTreeNode->SetImageSectionExpandIconHot(GetImageSectionExpandIconHot());
				pTreeNode->SetImageSectionExpandIconPushed(GetImageSectionExpandIconPushed());
				pTreeNode->SetImageSectionExpandIconSelNormal(GetImageSectionExpandIconSelNormal());
				pTreeNode->SetImageSectionExpandIconSelHot(GetImageSectionExpandIconSelHot());
				pTreeNode->SetImageSectionExpandIconSelPushed(GetImageSectionExpandIconSelPushed());
			}

		} while (false);
		if (m_pOwnerNodeCtrl)
		{
			m_pOwnerNodeCtrl->OnChildNodeChanged(true);
		}

		return true;
	}
	if ((NULL == m_pHorizScrollBarCtrl || NULL == m_pVertScrollBarCtrl)
		&& MMInterfaceHelper(CDUIScrollBarCtrl, pChild))
	{
		return __super::InsertChild(pChild, nPos);
	}
	if (NULL == m_pListHeader && MMInterfaceHelper(CDUIListHeaderCtrl, pChild))
	{
		return __super::InsertChild(pChild, nPos);
	}

	return false;
}

bool CDUITreeViewCtrl::InsertChild(int nItemModelCount, int nPos/* = -1*/)
{
	if (nItemModelCount <= 0) return false;

	for (int n = 0; n < nItemModelCount; n++)
	{
		CDUIControlBase *pItemInfo = CDUIGlobal::GetInstance()->LoadDui(GetItemModel(), NULL);
		CDUITreeNodeCtrl *pTreeNode = ProductItemDefault();
		if (NULL == pTreeNode)
		{
			assert(false);
			return false;
		}

		pTreeNode->InsertChild(pItemInfo);

		if (false == InsertChild(pTreeNode, nPos))
		{
			assert(false);
			return false;
		}
	}

	return true;
}

CDUITreeNodeCtrl * CDUITreeViewCtrl::GetChildAt(int nIndex) const
{
	return static_cast<CDUITreeNodeCtrl*>(CDUIContainerCtrl::GetChildAt(nIndex));
}

bool CDUITreeViewCtrl::Remove(CDUIControlBase *pControl)
{
	if (__super::Remove(pControl)) return true;

	MMInterfaceHelper(CDUITreeNodeCtrl, pControl, pTreeNode);
	if (pTreeNode)
	{
		CDUITreeNodeCtrl *pParentNode = pTreeNode->GetParentNode();

		return pParentNode ? pParentNode->RemoveChildNode(pTreeNode) : false;
	}

	return false;
}

bool CDUITreeViewCtrl::RemoveAt(int nIndex)
{
	CDUITreeViewCtrl *pRootView = GetRootView();
	if (NULL == pRootView) return false;

	CDUITreeNodeCtrl *pTreeNode = GetChildAt(nIndex);
	if (NULL == pTreeNode) return false;

	//cur sel
	if (pRootView->GetCurSelNode() == pTreeNode || pTreeNode->VerifyChild(pRootView->GetCurSelNode()))
	{
		pRootView->SetCurSelNode(NULL);
	}

	bool bRes = __super::RemoveAt(nIndex);

	//expand icon
	if (m_pOwnerNodeCtrl)
	{
		m_pOwnerNodeCtrl->OnChildNodeChanged(false);
	}

	return bRes;
}

void CDUITreeViewCtrl::RemoveAll()
{
	__super::RemoveAll();

	//expand icon
	if (m_pOwnerNodeCtrl)
	{
		m_pOwnerNodeCtrl->OnChildNodeChanged(false);
	}

	return;
}

void CDUITreeViewCtrl::NeedRefreshView()
{
	if (GetOwnerNode())
	{
		GetOwnerNode()->NeedRefreshView();

		return;
	}

	return __super::NeedRefreshView();
}

void CDUITreeViewCtrl::NeedParentRefreshView()
{
	if (GetOwnerNode())
	{
		GetOwnerNode()->NeedRefreshView();

		return;
	}

	return __super::NeedParentRefreshView();
}

bool CDUITreeViewCtrl::DoPaint(HDC hDC, bool bGenerateBmp/* = false*/)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	PaintLine(hDC);

	return true;
}

bool CDUITreeViewCtrl::IsExpandEnable()
{
	return m_AttributeExpandEnable.GetValue();
}

void CDUITreeViewCtrl::SetExpandEnable(bool bEnable)
{
#ifndef DUI_DESIGN
	if (bEnable == IsExpandEnable()) return;
#endif

	m_AttributeExpandEnable.SetValue(bEnable);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetExpandEnable(IsExpandEnable());
	}

	return;
}

bool CDUITreeViewCtrl::IsExpandDbClick()
{
	return m_AttributeExpandDbClick.GetValue();
}

void CDUITreeViewCtrl::SetExpandDbClick(bool bDbClick)
{
#ifndef DUI_DESIGN
	if (bDbClick == IsExpandDbClick()) return;
#endif

	m_AttributeExpandDbClick.SetValue(bDbClick);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetExpandDbClick(IsExpandDbClick());
	}

	return;
}

bool CDUITreeViewCtrl::IsExpandIconVisible()
{
	return m_AttributeExpandIconVisible.GetValue();
}

void CDUITreeViewCtrl::SetExpandIconVisible(bool bVisible)
{
#ifndef DUI_DESIGN
	if (bVisible == IsExpandIconVisible()) return;
#endif

	m_AttributeExpandIconVisible.SetValue(bVisible);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetExpandIconVisible(IsExpandIconVisible() && pTreeNode->GetChildNodeCount() > 0);
	}

	return;
}

bool CDUITreeViewCtrl::IsSelectSingle()
{
	return m_AttributeSelectSingle.GetValue();
}

void CDUITreeViewCtrl::SetSelectSingle(bool bSingle)
{
	if (bSingle == IsSelectSingle()) return;

	m_AttributeSelectSingle.SetValue(bSingle);

	return;
}

int CDUITreeViewCtrl::GetExpandIconLeftPadding()
{
	return m_AttributeExpandIconLeftPadding.GetValue();
}

void CDUITreeViewCtrl::SetExpandIconLeftPadding(int nLeft)
{
#ifndef DUI_DESIGN
	if (nLeft == GetExpandIconLeftPadding()) return;
#endif

	m_AttributeExpandIconLeftPadding.SetValue(nLeft);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetExpandIconLeftPadding(GetExpandIconLeftPadding());
	}

	return;
}

int CDUITreeViewCtrl::GetExpandIconFixedWidth()
{
	return m_AttributeExpandIconFixedWidth.GetValue();
}

void CDUITreeViewCtrl::SetExpandIconFixedWidth(int nWidth)
{
#ifndef DUI_DESIGN
	if (nWidth == GetExpandIconFixedWidth()) return;
#endif

	m_AttributeExpandIconFixedWidth.SetValue(nWidth);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		pTreeNode->SetExpandIconFixedWidth(GetExpandIconFixedWidth());
	}

	return;
}

int CDUITreeViewCtrl::GetExpandIconFixedHeight()
{
	return m_AttributeExpandIconFixedHeight.GetValue();
}

void CDUITreeViewCtrl::SetExpandIconFixedHeight(int nHeight)
{
#ifndef DUI_DESIGN
	if (nHeight == GetExpandIconFixedHeight()) return;
#endif

	m_AttributeExpandIconFixedHeight.SetValue(nHeight);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetExpandIconFixedHeight(GetExpandIconFixedHeight());
	}

	return;
}

int CDUITreeViewCtrl::GetChildIndent()
{
	return m_AttributeChildIndent.GetValue();
}

void CDUITreeViewCtrl::SetChildIndent(int nIndent)
{
#ifndef DUI_DESIGN
	if (nIndent == GetChildIndent()) return;
#endif

	m_AttributeChildIndent.SetValue(nIndent);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetChildIndent(GetChildIndent());
	}

	return;
}

CDUITreeViewCtrl * CDUITreeViewCtrl::GetRootView()
{
	CDUITreeNodeCtrl *pRootNode = GetRootNode();

	return pRootNode ? MMStaticPtr(CDUITreeViewCtrl, pRootNode->GetRootView()) : NULL;
}

CDUITreeNodeCtrl * CDUITreeViewCtrl::GetOwnerNode()
{
	return m_pOwnerNodeCtrl;
}

CDUITreeNodeCtrl * CDUITreeViewCtrl::GetRootNode()
{
	if (m_pOwnerNodeCtrl)
	{
		return m_pOwnerNodeCtrl->GetRootNode();
	}

	return GetChildAt(0);
}

CDUITreeNodeCtrl * CDUITreeViewCtrl::GetCurSelNode()
{
	CDUITreeViewCtrl *pRootView = GetRootView();

	return pRootView ? pRootView->m_pCurNodeCtrl : NULL;
}

CDUITreeNodeCtrl * CDUITreeViewCtrl::ComparePreNode(CDUITreeNodeCtrl *pFirstNode, CDUITreeNodeCtrl *pNextNode)
{
	if (NULL == pFirstNode || NULL == pNextNode) return NULL;

	CDUITreeNodeCtrl *pFirstNodeComp = pFirstNode;
	CDUITreeNodeCtrl *pNextNodeComp = pNextNode;
	while (pFirstNodeComp && pNextNodeComp)
	{
		//same level
		if (pFirstNodeComp->GetParent() == pNextNodeComp->GetParent())
		{
			return MMStaticPtr(CDUITreeNodeCtrl, pFirstNodeComp->GetIndex() < pNextNodeComp->GetIndex() ? pFirstNode : pNextNode);
		}

		//unsame level
		int nLevelFirst = pFirstNodeComp->GetLevel();
		int nLevelNext = pNextNodeComp->GetLevel();
		int nLevel = min(nLevelFirst, nLevelNext);
		nLevel == nLevelFirst && nLevel == nLevelNext ? nLevel-- : nLevel;
		pFirstNodeComp = nLevelFirst > nLevel ? pFirstNodeComp->GetOwnerNode(nLevel) : pFirstNodeComp;
		pNextNodeComp = nLevelNext > nLevel ? pNextNodeComp->GetOwnerNode(nLevel) : pNextNodeComp;

		if (pFirstNodeComp == pNextNodeComp)
		{
			return MMStaticPtr(CDUITreeNodeCtrl, nLevelFirst < nLevelNext ? pFirstNode : pNextNode);
		}
	}

	return NULL;
}

void CDUITreeViewCtrl::EnsureVisible(CDUITreeNodeCtrl *pTreeNode, bool bCenter)
{
	if (NULL == pTreeNode) return;

	CDUITreeViewCtrl *pRootView = GetRootView();
	if (pRootView != this)
	{
		pRootView->EnsureVisible(pTreeNode, bCenter);

		return;
	}

	if (NULL == m_pVertScrollBarCtrl || false == m_pVertScrollBarCtrl->IsVisible()) return;

	CDUIRect rcItem = pTreeNode->GetAbsoluteRect();
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	if (bCenter)
	{
		rcItemRange.top = rcItemRange.top + rcItemRange.GetHeight() / 2 - GetSwitchListItemHeight() / 2;
		rcItemRange.bottom = rcItemRange.top + GetSwitchListItemHeight();
	}

	//visible
	CDUIRect rcTemp;
	if (IntersectRect(&rcTemp, &rcItem, &rcItemRange)) return;

	//scrollpos
	if (rcItem.bottom > rcItemRange.bottom)
	{
		SIZE szScrollPos = GetScrollPos();
		szScrollPos.cy += (rcItem.bottom - rcItemRange.bottom);
		SetScrollPos(szScrollPos);
	}
	if (rcItem.top < rcItemRange.top)
	{
		SIZE szScrollPos = GetScrollPos();
		szScrollPos.cy -= (rcItemRange.top - rcItem.top);
		SetScrollPos(szScrollPos);
	}

	return;
}

void CDUITreeViewCtrl::SetCurSelNode(CDUITreeNodeCtrl *pCurNode)
{
	CDUITreeViewCtrl *pRootView = GetRootView();
	if (pRootView != this)
	{
		pRootView->SetCurSelNode(pCurNode);

		return;
	}

	m_pCurNodeCtrl = MMStaticPtr(CDUITreeNodeCtrl, pCurNode);

	//select
	if (m_pCurNodeCtrl)
	{
		m_pCurNodeCtrl->Select(true);
	}

	return;
}

void CDUITreeViewCtrl::SelectTreeNode(CDUITreeNodeCtrl *pFromNode, CDUITreeNodeCtrl *pStopNode)
{
	if (NULL == pFromNode || NULL == pStopNode) return;

	CDUITreeNodeCtrl *pFirstNode = ComparePreNode(pFromNode, pStopNode);
	CDUITreeNodeCtrl *pNextNode = pFirstNode == pFromNode ? pStopNode : pFromNode;
	if (NULL == pFirstNode || NULL == pNextNode) return;

	bool bFindFrom = false;
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) return;

		if (false == pTreeNode->SelectTreeNode(pFirstNode, pNextNode, bFindFrom)) break;
	}

	return;
}

void CDUITreeViewCtrl::SelectTreeNodeAll(bool bSelect)
{
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) return;

		pTreeNode->SelectTreeNodeAll(bSelect);
	}

	return;
}

tagDuiImageSection CDUITreeViewCtrl::GetImageSectionExpandIconNormal()
{
	return m_AttributeImageExpandIconNormal.GetImageSection();
}

tagDuiImageSection CDUITreeViewCtrl::GetImageSectionExpandIconHot()
{
	return m_AttributeImageExpandIconHot.GetImageSection();
}

tagDuiImageSection CDUITreeViewCtrl::GetImageSectionExpandIconPushed()
{
	return m_AttributeImageExpandIconPushed.GetImageSection();
}

tagDuiImageSection CDUITreeViewCtrl::GetImageSectionExpandIconSelNormal()
{
	return m_AttributeImageExpandIconSelNormal.GetImageSection();
}

tagDuiImageSection CDUITreeViewCtrl::GetImageSectionExpandIconSelHot()
{
	return m_AttributeImageExpandIconSelHot.GetImageSection();
}

tagDuiImageSection CDUITreeViewCtrl::GetImageSectionExpandIconSelPushed()
{
	return m_AttributeImageExpandIconSelPushed.GetImageSection();
}

void CDUITreeViewCtrl::SetImageSectionExpandIconNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageExpandIconNormal.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetImageSectionExpandIconNormal(GetImageSectionExpandIconNormal());
	}

	Invalidate();

	return;
}

void CDUITreeViewCtrl::SetImageSectionExpandIconHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageExpandIconHot.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetImageSectionExpandIconHot(GetImageSectionExpandIconHot());
	}

	Invalidate();

	return;
}

void CDUITreeViewCtrl::SetImageSectionExpandIconPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageExpandIconPushed.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetImageSectionExpandIconPushed(GetImageSectionExpandIconPushed());
	}

	Invalidate();

	return;
}

void CDUITreeViewCtrl::SetImageSectionExpandIconSelNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageExpandIconSelNormal.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetImageSectionExpandIconSelNormal(GetImageSectionExpandIconSelNormal());
	}

	Invalidate();

	return;
}

void CDUITreeViewCtrl::SetImageSectionExpandIconSelHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageExpandIconSelHot.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetImageSectionExpandIconSelHot(GetImageSectionExpandIconSelHot());
	}

	Invalidate();

	return;
}

void CDUITreeViewCtrl::SetImageSectionExpandIconSelPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageExpandIconSelPushed.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode) continue;

		pTreeNode->SetImageSectionExpandIconSelPushed(GetImageSectionExpandIconSelPushed());
	}

	Invalidate();

	return;
}

void CDUITreeViewCtrl::EnsureVisible(int nIndex, bool bCenter)
{
	CDUITreeViewCtrl *pRootView = GetRootView();
	if (pRootView != this)
	{
		pRootView->EnsureVisible(GetChildAt(nIndex), bCenter);

		return;
	}

	EnsureVisible(GetChildAt(nIndex), bCenter);

	return;
}

bool CDUITreeViewCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseWheel(pt, Msg);

	CDUITreeViewCtrl *pRootView = GetRootView();
	if (pRootView != this)
	{
		pRootView->OnDuiMouseWheel(pt, Msg);
	}

	return true;
}

void CDUITreeViewCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupTreeNode, _T("TreeNode"));
	DuiCreateAttribute(m_AttributeExpandEnable, _T("ExpandEnable"), _T(""), m_AttributeGroupTreeNode);
	DuiCreateAttribute(m_AttributeExpandDbClick, _T("ExpandDbClick"), _T(""), m_AttributeGroupTreeNode);
	DuiCreateAttribute(m_AttributeExpandIconVisible, _T("ExpandIconVisible"), _T(""), m_AttributeGroupTreeNode);
	DuiCreateAttribute(m_AttributeSelectSingle, _T("SelectSingle"), _T(""), m_AttributeGroupTreeNode);
	DuiCreateAttribute(m_AttributeExpandIconLeftPadding, _T("ExpandIconLeftPadding"), _T(""), m_AttributeGroupTreeNode);
	DuiCreateAttribute(m_AttributeExpandIconFixedWidth, _T("ExpandIconFixedWidth"), _T(""), m_AttributeGroupTreeNode);
	DuiCreateAttribute(m_AttributeExpandIconFixedHeight, _T("ExpandIconFixedHeight"), _T(""), m_AttributeGroupTreeNode);
	DuiCreateAttribute(m_AttributeChildIndent, _T("ChildIndent"), _T(""), m_AttributeGroupTreeNode);

	DuiCreateGroupAttribute(m_AttributeGroupTreeExpandIcon, _T("TreeExpandIcon"));
	DuiCreateAttribute(m_AttributeImageExpandIconNormal, _T("ExpandIconNormal"), _T(""), m_AttributeGroupTreeExpandIcon);
	DuiCreateAttribute(m_AttributeImageExpandIconHot, _T("ExpandIconHot"), _T(""), m_AttributeGroupTreeExpandIcon);
	DuiCreateAttribute(m_AttributeImageExpandIconPushed, _T("ExpandIconPushed"), _T(""), m_AttributeGroupTreeExpandIcon);
	DuiCreateAttribute(m_AttributeImageExpandIconSelNormal, _T("ExpandIconSelNormal"), _T(""), m_AttributeGroupTreeExpandIcon);
	DuiCreateAttribute(m_AttributeImageExpandIconSelHot, _T("ExpandIconSelHot"), _T(""), m_AttributeGroupTreeExpandIcon);
	DuiCreateAttribute(m_AttributeImageExpandIconSelPushed, _T("ExpandIconSelPushed"), _T(""), m_AttributeGroupTreeExpandIcon);

	//value
	DuiInitAttriColorRes(m_AttributeColorItemStatusHot, );
	DuiInitAttriColorRes(m_AttributeColorItemStatusSelHot, );
	DuiInitAttriValue(m_AttributeUseHorizScrollBar, false);
	DuiInitAttriValue(m_AttributeUseVertScrollBar, false);
	DuiInitAttriValue(m_AttributeMultiSel, true);
	DuiInitAttriValue(m_AttributeSelectIconVisible, true);

	return;
}

void CDUITreeViewCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeListViewType, false);
	DuiInitAttriVisible(m_AttributeGroupTileType, false);

	RegisterControlCallBack(this);

	return;
}

void CDUITreeViewCtrl::PaintLine(HDC hDC)
{
	if (GetRootView() != this) return;

	if (IsShowRowLine() || IsShowColumnLine())
	{
		CDUIRect rcLayout = GetLayoutRangeOfItem();
		if (false == IntersectRect(&rcLayout, &m_rcPaint, &rcLayout)) return;

		CDUIRenderClip Clip;
		Clip.GenerateClip(hDC, rcLayout);

		CDUIRect rcTemp;
		for (int n = 0; n < GetChildCount(); n++)
		{
			CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
			if (NULL == pTreeNode || false == pTreeNode->IsVisible()) continue;

			CDUIRect rcNode = pTreeNode->GetAbsoluteRect();
			if (false == IntersectRect(&rcTemp, &rcNode, &rcLayout)) continue;

			pTreeNode->PaintLineOnListType(hDC);
		}
	}

	return;
}

void CDUITreeViewCtrl::SetOwnerNode(CDUITreeNodeCtrl *pOwnerNode)
{
	m_pOwnerNodeCtrl = pOwnerNode;

	if (m_pOwnerNodeCtrl && m_pListHeader)
	{
		SetUseListHeader(false);

		MMSafeDelete(m_pListHeader);
	}

	return;
}

int CDUITreeViewCtrl::GetSelectNodeCount()
{
	int nCount = 0;
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildAt(n);
		if (NULL == pTreeNode || false == pTreeNode->IsSelected()) continue;

		nCount++;
	}

	return nCount;
}

CDUITreeNodeCtrl * CDUITreeViewCtrl::ProductItemDefault()
{
	CDUITreeNodeCtrl *pItem = new CDUITreeNodeCtrl();
	pItem->Init();

	return pItem;
}