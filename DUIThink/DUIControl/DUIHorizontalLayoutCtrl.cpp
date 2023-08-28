#include "StdAfx.h"
#include "DUIHorizontalLayoutCtrl.h"

DuiImplement_CreateControl(CDUIHorizontalLayoutCtrl)
MMImplement_ClassName(CDUIHorizontalLayoutCtrl)

CDUIHorizontalLayoutCtrl::CDUIHorizontalLayoutCtrl(void)
{
}

CDUIHorizontalLayoutCtrl::~CDUIHorizontalLayoutCtrl(void)
{
	if (m_pWndManager)
	{
		m_pWndManager->RemoveINotify(m_pAnimateDrag);
	}

	MMSafeDelete(m_pAnimateDrag);

	return;
}

LPVOID CDUIHorizontalLayoutCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIHorizontalLayoutCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIHorizontalLayoutCtrl::GetDescribe() const
{
	return Dui_Ctrl_HorizontalLayout;
}

void CDUIHorizontalLayoutCtrl::SetAnimateDrag(bool bAnimateDrag)
{
	__super::SetAnimateDrag(bAnimateDrag);

	if (IsAnimateDrag() && NULL == m_pAnimateDrag)
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_H);
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

bool CDUIHorizontalLayoutCtrl::IsAnimateDraging()
{
	return m_pAnimateDrag && m_pAnimateDrag->IsAnimateDraging();
}

void CDUIHorizontalLayoutCtrl::OnDragMoveBegin(CDUIControlBase *pDragCtrl)
{
	return m_pAnimateDrag ? m_pAnimateDrag->OnDragMoveBegin(pDragCtrl) : NULL;
}

void CDUIHorizontalLayoutCtrl::OnDragMoving(CDUIControlBase *pDragCtrl)
{
	return m_pAnimateDrag ? m_pAnimateDrag->OnDragMoving(pDragCtrl) : NULL;
}

void CDUIHorizontalLayoutCtrl::OnDragMoveEnd(CDUIControlBase *pDragCtrl)
{
	return m_pAnimateDrag ? m_pAnimateDrag->OnDragMoveEnd(pDragCtrl) : NULL;
}

int CDUIHorizontalLayoutCtrl::TranslateIndex(CDUIPoint pt)
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

void CDUIHorizontalLayoutCtrl::RefreshView()
{
	RefreshScrollBar();

	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcRangeLayout = GetLayoutRangeOfItem();
	CDUIRect rcRangeView = rcRangeLayout;

	rcRangeLayout.Offset(-szScrollPos.cx, -szScrollPos.cy);
	CDUIRect rcMax = rcRangeLayout;

	//variant
	int nChildPaddingH = GetChildPaddingH();
	int nDynamicCount = 0;
	int nFixedWidthTotal = 0;
	VecDuiControlBase vecControlLeft, vecControlRight;

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

		//right
		if (HorizAlign_Right == pChild->GetHorizAlignType())
		{
			vecControlRight.insert(vecControlRight.begin(), pChild);
		}
		//left
		else
		{
			vecControlLeft.push_back(pChild);
		}

		//width
		CDUIRect rcPadding = pChild->GetPadding();
		nFixedWidthTotal += rcPadding.left;
		nFixedWidthTotal += rcPadding.right;

		if (nIndex > 0)
		{
			nFixedWidthTotal += nChildPaddingH;
		}

		int nFixedWidth = pChild->GetFixedWidth();
		if (nFixedWidth <= 0)
		{
			nDynamicCount++;
			continue;
		}

		nFixedWidthTotal += nFixedWidth;
	}

	//dynamicwidth
	int nDynamicWidth = 0, nDynamicSurplus = 0;
	if (nDynamicCount > 0)
	{
		int nDynamicWidthTotal = rcRangeLayout.GetWidth() - nFixedWidthTotal;
		nDynamicWidthTotal = max(nDynamicWidthTotal, 0);
		nDynamicWidth = nDynamicWidthTotal / nDynamicCount;
		nDynamicSurplus = nDynamicWidthTotal % nDynamicCount;
	}

	//adjust layout
	if (nFixedWidthTotal > rcRangeLayout.GetWidth())
	{
		rcRangeLayout.right += (nFixedWidthTotal - rcRangeLayout.GetWidth());
	}

	//layout left
	CDUIRect rcModal;
	int nTileX = rcRangeLayout.left;
	for (int nIndex = 0; nIndex < vecControlLeft.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlLeft[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileX += nChildPaddingH;
		}

		int nFixedWidth = pChild->GetFixedWidth();
		if (nFixedWidth <= 0)
		{
			rcModal = CDUIRect(nTileX, rcRangeLayout.top, nTileX, rcRangeLayout.bottom);
			nTileX += nDynamicWidth + pChild->GetPadding().left + pChild->GetPadding().right;

			if (nDynamicSurplus > 0 && IsAutoCalcChildPaddingH())
			{
				nTileX++;
				rcModal.Offset(1, 0);
				nDynamicSurplus--;
			}

			rcModal.right = nTileX;
		}
		else
		{
			rcModal = CDUIRect(nTileX, rcRangeLayout.top, nTileX, rcRangeLayout.bottom);
			nTileX += nFixedWidth + pChild->GetPadding().left + pChild->GetPadding().right;
			rcModal.right = nTileX;
		}
		
		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	//layout right
	nTileX = rcRangeLayout.right;
	for (int nIndex = 0; nIndex < vecControlRight.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlRight[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileX -= nChildPaddingH;
		}

		int nFixedWidth = pChild->GetFixedWidth();
		if (nFixedWidth <= 0)
		{
			rcModal = CDUIRect(nTileX, rcRangeLayout.top, nTileX, rcRangeLayout.bottom);
			nTileX -= nDynamicWidth + pChild->GetPadding().left + pChild->GetPadding().right;

			if (nDynamicSurplus > 0 && IsAutoCalcChildPaddingH())
			{
				nTileX--;
				rcModal.Offset(-1, 0);
				nDynamicSurplus--;
			}

			rcModal.left = nTileX;
		}
		else
		{
			rcModal = CDUIRect(nTileX, rcRangeLayout.top, nTileX, rcRangeLayout.bottom);
			nTileX -= nFixedWidth + pChild->GetPadding().left + pChild->GetPadding().right;
			rcModal.left = nTileX;
		}

		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	//scrollbar
	UpdateScrollBar(CDUISize(rcMax.GetWidth() - rcRangeView.GetWidth(), rcMax.GetHeight() - rcRangeView.GetHeight()));

	CDUIControlBase::RefreshView();

	//autowidth
	if (IsAutoCalcWidth()
		&& nFixedWidthTotal != GetFixedWidth()
		&& DuiDpiScaleBackCtrl(nFixedWidthTotal) != DuiDpiScaleBackCtrl(GetFixedWidth()))
	{
		SetFixedWidth(DuiDpiScaleBackCtrl(nFixedWidthTotal));
	}

	return;
}

bool CDUIHorizontalLayoutCtrl::DoPaint(HDC hDC, bool bGenerateBmp /* = false */)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	//darg topmost
	if (m_pAnimateDrag && m_pAnimateDrag->GetDragingCtrl())
	{
		m_pAnimateDrag->GetDragingCtrl()->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	return true;
}

void CDUIHorizontalLayoutCtrl::InitProperty()
{
	__super::InitProperty();

	//pos
	DuiCreateAttribute(m_AttributeAutoCalcWidth, _T("AutoCalcWidth"), _T(""), m_AttributeGroupPosition);

	//child
	DuiCreateAttribute(m_AttributeChildPaddingH, _T("ChildPaddingH"), _T(""), m_AttributeGroupChild);
	DuiCreateAttribute(m_AttributeAutoCalcChildPaddingH, _T("AutoCalcChildPaddingH"), _T("For TileH"), m_AttributeGroupChild);

	return;
}

void CDUIHorizontalLayoutCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	return;
}

void CDUIHorizontalLayoutCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAnimateDrag, true);
	DuiInitAttriVisible(m_AttributeGroupChild, true);
	DuiInitAttriVisible(m_AttributeChildPaddingV, false);
	DuiInitAttriVisible(m_AttributeAutoCalcWidth, true);
	DuiInitAttriVisible(m_AttributeAutoCalcChildPaddingV, false);

	if (IsAnimateDrag())
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_H);
	}

	return;
}

void CDUIHorizontalLayoutCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (m_pWndManager && m_pAnimateDrag)
	{
		m_pWndManager->AddINotify(m_pAnimateDrag);
	}

	return;
}

void CDUIHorizontalLayoutCtrl::OnDuiWndManagerDetach()
{
	if (m_pWndManager)
	{
		m_pWndManager->RemoveINotify(m_pAnimateDrag);
	}

	return __super::OnDuiWndManagerDetach();
}

void CDUIHorizontalLayoutCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
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

void CDUIHorizontalLayoutCtrl::ScrollChilds(CDUISize szScroll)
{
	return __super::ScrollChilds(szScroll);
}
