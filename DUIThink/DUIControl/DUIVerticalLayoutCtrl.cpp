#include "StdAfx.h"
#include "DUIVerticalLayoutCtrl.h"

DuiImplement_CreateControl(CDUIVerticalLayoutCtrl)
MMImplement_ClassName(CDUIVerticalLayoutCtrl)

CDUIVerticalLayoutCtrl::CDUIVerticalLayoutCtrl(void)
{
}

CDUIVerticalLayoutCtrl::~CDUIVerticalLayoutCtrl(void)
{
	if (m_pWndManager)
	{
		m_pWndManager->RemoveINotify(m_pAnimateDrag);
	}

	MMSafeDelete(m_pAnimateDrag);

	return;
}

LPVOID CDUIVerticalLayoutCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIVerticalLayoutCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIVerticalLayoutCtrl::GetDescribe() const
{
	return Dui_Ctrl_VerticalLayout;
}

UINT CDUIVerticalLayoutCtrl::GetControlFlags()
{
	return __super::GetControlFlags();
}

void CDUIVerticalLayoutCtrl::SetAnimateDrag(bool bAnimateDrag)
{
	__super::SetAnimateDrag(bAnimateDrag);

	if (IsAnimateDrag() && NULL == m_pAnimateDrag)
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_V);
	}
	if (m_pWndManager)
	{
		if (IsAnimateDrag())
		{
			m_pWndManager->AddINotify(m_pAnimateDrag);
		}
		else
		{
			m_pWndManager->RemoveINotify(m_pAnimateDrag);
		}
	}

	return;
}

bool CDUIVerticalLayoutCtrl::IsAnimateDraging()
{
	return m_pAnimateDrag && m_pAnimateDrag->IsAnimateDraging();
}

void CDUIVerticalLayoutCtrl::OnDragMoveBegin(CDUIControlBase *pDragCtrl)
{
	return m_pAnimateDrag ? m_pAnimateDrag->OnDragMoveBegin(pDragCtrl) : NULL;
}

void CDUIVerticalLayoutCtrl::OnDragMoving(CDUIControlBase *pDragCtrl)
{
	return m_pAnimateDrag ? m_pAnimateDrag->OnDragMoving(pDragCtrl) : NULL;
}

void CDUIVerticalLayoutCtrl::OnDragMoveEnd(CDUIControlBase *pDragCtrl)
{
	return m_pAnimateDrag ? m_pAnimateDrag->OnDragMoveEnd(pDragCtrl) : NULL;
}

int CDUIVerticalLayoutCtrl::TranslateIndex(CDUIPoint pt)
{
	int nTargetIndex = -1;
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || pChild->IsFloat()) continue;

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		if (pt.x <= rcChild.right && pt.y <= rcChild.bottom) return nIndex;
	}

	return nTargetIndex;
}

void CDUIVerticalLayoutCtrl::RefreshView()
{
	RefreshScrollBar();

	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcRangeLayout = GetLayoutRangeOfItem();
	CDUIRect rcRangeView = rcRangeLayout;
	
	rcRangeLayout.Offset(-szScrollPos.cx, -szScrollPos.cy);
	CDUIRect rcMax = rcRangeLayout;

	//variant
	int nChildPaddingV = GetChildPaddingV();
	int nDynamicCount = 0;
	int nFixedHeightTotal = 0;
	VecDuiControlBase vecControlTop, vecControlBottom;

	//adjust
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || false == pChild->IsVisible()) continue;

		//float
		if (pChild->IsFloat())
		{
			pChild->OnDuiSize(rcRangeLayout);

			continue;
		}

		//bottom
		if (VertAlign_Bottom == pChild->GetVertAlignType())
		{
			vecControlBottom.insert(vecControlBottom.begin(), pChild);
		}
		//top
		else
		{
			vecControlTop.push_back(pChild);
		}

		CDUIRect rcPadding = pChild->GetPadding();
		nFixedHeightTotal += rcPadding.top;
		nFixedHeightTotal += rcPadding.bottom;

		if (nIndex > 0)
		{
			nFixedHeightTotal += nChildPaddingV;
		}

		int nFixedHeight = pChild->GetFixedHeight();
		if (nFixedHeight <= 0)
		{
			nDynamicCount++;
			continue;
		}

		nFixedHeightTotal += nFixedHeight;
	}

	//dynamicheight
	int nDynamicHeight = 0, nDynamicSurplus = 0;
	if (nDynamicCount > 0)
	{
		int nDynamicHeightTotal = rcRangeLayout.GetHeight() - nFixedHeightTotal;
		nDynamicHeightTotal = max(nDynamicHeightTotal, 0);
		nDynamicHeight = nDynamicHeightTotal / nDynamicCount;
		nDynamicSurplus = nDynamicHeightTotal % nDynamicCount;
	}

	//adjust layout
	if (nFixedHeightTotal > rcRangeLayout.GetHeight())
	{
		rcRangeLayout.bottom += (nFixedHeightTotal - rcRangeLayout.GetHeight());
	}

	//layout top
	CDUIRect rcModal;
	int nTileY = rcRangeLayout.top;
	for (int nIndex = 0; nIndex < vecControlTop.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlTop[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileY += nChildPaddingV;
		}

		int nFixedHeight = pChild->GetFixedHeight();
		if (nFixedHeight <= 0)
		{
			rcModal = CDUIRect(rcRangeLayout.left, nTileY, rcRangeLayout.right, nTileY);
			nTileY += nDynamicHeight + pChild->GetPadding().top + pChild->GetPadding().bottom;

			if (nDynamicSurplus > 0 && IsAutoCalcChildPaddingV())
			{
				nTileY++;
				rcModal.Offset(0, 1);
				nDynamicSurplus--;
			}

			rcModal.bottom = nTileY;
		}
		else
		{
			rcModal = CDUIRect(rcRangeLayout.left, nTileY, rcRangeLayout.right, nTileY);
			nTileY += nFixedHeight + pChild->GetPadding().top + pChild->GetPadding().bottom;
			rcModal.bottom = nTileY;
		}

		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	//layout bottom
	nTileY = rcRangeLayout.bottom;
	for (int nIndex = 0; nIndex < vecControlBottom.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlBottom[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileY -= nChildPaddingV;
		}

		int nFixedHeight = pChild->GetFixedHeight();
		if (nFixedHeight <= 0)
		{
			rcModal = CDUIRect(rcRangeLayout.left, nTileY, rcRangeLayout.right, nTileY);
			nTileY -= nDynamicHeight + pChild->GetPadding().top + pChild->GetPadding().bottom;

			if (nDynamicSurplus > 0 && IsAutoCalcChildPaddingV())
			{
				nTileY--;
				rcModal.Offset(0, -1);
				nDynamicSurplus--;
			}

			rcModal.top = nTileY;
		}
		else
		{
			rcModal = CDUIRect(rcRangeLayout.left, nTileY, rcRangeLayout.right, nTileY);
			nTileY -= nFixedHeight + pChild->GetPadding().top + pChild->GetPadding().bottom;
			rcModal.top = nTileY;
		}

		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	UpdateScrollBar(CDUISize(rcMax.GetWidth() - rcRangeView.GetWidth(), rcMax.GetHeight() - rcRangeView.GetHeight()));

	CDUIControlBase::RefreshView();

	//autoheight
	if (IsAutoCalcHeight()
		&& nFixedHeightTotal != GetFixedHeight()
		&& DuiDpiScaleBackCtrl(nFixedHeightTotal) != DuiDpiScaleBackCtrl(GetFixedHeight()))
	{
		SetFixedHeight(DuiDpiScaleBackCtrl(nFixedHeightTotal));
	}

	return;
}

bool CDUIVerticalLayoutCtrl::DoPaint(HDC hDC, bool bGenerateBmp /* = false */)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	//darg topmost
	if (m_pAnimateDrag && m_pAnimateDrag->GetDragingCtrl())
	{
		m_pAnimateDrag->GetDragingCtrl()->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	return true;
}

void CDUIVerticalLayoutCtrl::InitProperty()
{
	__super::InitProperty();

	//pos
	DuiCreateAttribute(m_AttributeAutoCalcHeight, _T("AutoCalcHeight"), _T(""), m_AttributeGroupPosition);

	//child
	DuiCreateAttribute(m_AttributeChildPaddingV, _T("ChildPaddingV"), _T(""), m_AttributeGroupChild);
	DuiCreateAttribute(m_AttributeAutoCalcChildPaddingV, _T("AutoCalcChildPaddingV"), _T("For TileV"), m_AttributeGroupChild);

	return;
}

void CDUIVerticalLayoutCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	return;
}

void CDUIVerticalLayoutCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAnimateDrag, true);
	DuiInitAttriVisible(m_AttributeGroupChild, true);
	DuiInitAttriVisible(m_AttributeChildPaddingH, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, true);
	DuiInitAttriVisible(m_AttributeAutoCalcChildPaddingH, false);

	if (IsAnimateDrag())
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_V);
	}

	return;
}

void CDUIVerticalLayoutCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (m_pWndManager && m_pAnimateDrag)
	{
		m_pWndManager->AddINotify(m_pAnimateDrag);
	}
	
	return;
}

void CDUIVerticalLayoutCtrl::OnDuiWndManagerDetach()
{
	if (m_pWndManager)
	{
		m_pWndManager->RemoveINotify(m_pAnimateDrag);
	}

	return __super::OnDuiWndManagerDetach();
}

void CDUIVerticalLayoutCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	if (Dui_TimerAnimate_ID == uTimerID)
	{
		if (m_pAnimateDrag)
		{
			m_pAnimateDrag->OnAnimationElapse(uTimerID);
		}

		return;
	}

	return __super::OnDuiTimer(uTimerID, Msg);
}

void CDUIVerticalLayoutCtrl::ScrollChilds(CDUISize szScroll)
{
	return __super::ScrollChilds(szScroll);
}