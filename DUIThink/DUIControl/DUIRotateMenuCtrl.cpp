#include "StdAfx.h"
#include "DUIRotateMenuCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIRotateMenuItemCtrl)
MMImplement_ClassName(CDUIRotateMenuItemCtrl)

CDUIRotateMenuItemCtrl::CDUIRotateMenuItemCtrl()
{

}

CDUIRotateMenuItemCtrl::~CDUIRotateMenuItemCtrl()
{

}

LPVOID CDUIRotateMenuItemCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIRotateMenuItemCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIRotateMenuItemCtrl::GetDescribe() const
{
	return Dui_Ctrl_RotateMenuItem;
}

bool CDUIRotateMenuItemCtrl::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == CDUIListItemCtrl::OnDuiMouseEnter(pt, Msg)) return false;

	return true;
}

void CDUIRotateMenuItemCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	return CDUIListItemCtrl::OnDuiMouseLeave(pt, Msg);
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIRotateMenuCtrl)
MMImplement_ClassName(CDUIRotateMenuCtrl)

DuiBegin_Message_Map(CDUIRotateMenuCtrl, CDUINotifyPump)
Dui_On_Notify(DuiNotify_ItemButtonDown, OnDuiItemButtonDown)
Dui_On_Notify(DuiNotify_ItemMouseMove, OnDuiItemMouseMove)
DuiEnd_Message_Map()

CDUIRotateMenuCtrl::CDUIRotateMenuCtrl(void)
{
}

CDUIRotateMenuCtrl::~CDUIRotateMenuCtrl(void)
{
	if (m_pWndManager)
	{
		m_pWndManager->RemoveINotify(this);
	}

	return;
}

bool CDUIRotateMenuCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeItemSizeMax
		|| pAttributeObj == &m_AttributeItemSizeMin)
	{
		return true;
	}
	if (pAttributeObj == &m_AttributeAngleSpaceMin)
	{
		SetAngleSpaceMin(GetAngleSpaceMin());

		return true;
	}

	return false;
}

LPVOID CDUIRotateMenuCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIRotateMenuCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIRotateMenuCtrl::GetDescribe() const
{
	return Dui_Ctrl_RotateMenu;
}

bool CDUIRotateMenuCtrl::SetWndManager(CDUIWndManager *pWndManager)
{
	if (m_pWndManager)
	{
		m_pWndManager->RemoveINotify(this);
	}

	if (false == __super::SetWndManager(pWndManager)) return false;

	if (m_pWndManager)
	{
		m_pWndManager->AddINotify(this);
	}

	return true;
}

void CDUIRotateMenuCtrl::RefreshView()
{
	if (GetChildCount() > 0)
	{
		//angle
		int nAngleStart = GetAngleStart(), nAngleFinish = GetAngleFinish();
		int nAngleRange = nAngleFinish - nAngleStart;
		int nAngleMid = nAngleStart + nAngleRange / 2;
		m_nAngleSpace = nAngleRange / max(1, GetChildCount() - 1);
		m_nAngleSpace = max(m_nAngleSpace, GetAngleSpaceMin());
		m_nAngleScrollRange = max(nAngleRange, (GetChildCount() - 1) * m_nAngleSpace);
		m_nAngleScrollCur = min(m_nAngleScrollCur, m_nAngleScrollRange);

		//radius
		m_szRadius.cx = (GetWidth() - GetItemSizeMax().cx) / 2;
		m_szRadius.cx = max(0, m_szRadius.cx);
		m_szRadius.cy = (GetHeight() - GetItemSizeMax().cy) / 2;
		m_szRadius.cy = max(0, m_szRadius.cy);

		//size
		CDUISize szItemMax = GetItemSizeMax(), szItemMin = GetItemSizeMin();
		SizeF szSizeSpace = { (szItemMax.cx - szItemMin.cx) / (nAngleRange / 2.0f), (szItemMax.cy - szItemMin.cy) / (nAngleRange / 2.0f) };

		//pos
		for (int n = 0; n < GetChildCount(); n++)
		{
			CDUIListItemCtrl *pListItem = GetChildAt(n);
			if (NULL == pListItem) continue;

			float fAngle = GetChildAngle(n);
			fAngle = max(nAngleStart, min(fAngle, nAngleFinish));
			int nAngleOffset = abs(nAngleMid - fAngle);

			//size
			CDUISize szItem(szItemMax.cx - szSizeSpace.Width * nAngleOffset, szItemMax.cy - szSizeSpace.Height * nAngleOffset);

			//point
			CDUIPoint ptCenter(GetWidth() / 2, GetHeight() / 2);
			float fArcLeft = (fAngle) / 180 * PI;
			PointF ptItem;
			ptItem.X = cos(fArcLeft) * m_szRadius.cx + ptCenter.x;
			ptItem.Y = sin(fArcLeft) * m_szRadius.cy + ptCenter.y;
			ptItem.X -= szItem.cx / 2;
			ptItem.Y -= szItem.cy / 2;

			pListItem->SetFloat(true);
			pListItem->SetPaddingL(ptItem.X);
			pListItem->SetPaddingT(ptItem.Y);
			pListItem->SetFixedWidth(szItem.cx);
			pListItem->SetFixedHeight(szItem.cy);
		}
	}

	return __super::RefreshView();
}

bool CDUIRotateMenuCtrl::DoPaint(HDC hDC, bool bGenerateBmp/* = false*/)
{
	if (false == CDUIControlBase::DoPaint(hDC, bGenerateBmp)) return false;

	//find mid
	int nAngleStart = GetAngleStart(), nAngleFinish = GetAngleFinish();
	int nAngleRange = nAngleFinish - nAngleStart;
	int nAngleMid = nAngleStart + nAngleRange / 2;
	int nIndexMid = -1;
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	IntersectRect(&rcItemRange, &rcItemRange, &m_rcPaint);
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIRotateMenuItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;
	
		float fAngle = GetChildAngle(n);
		if (IsRotateClockWise() ? fAngle > nAngleMid : fAngle < nAngleMid)
		{
			nIndexMid = n;

			break;
		}

		pItem->OnDraw(hDC, rcItemRange, bGenerateBmp);
	}
	if (-1 != nIndexMid)
	{
		for (int n = GetChildCount() - 1; n >= nIndexMid; n--)
		{
			CDUIRotateMenuItemCtrl *pItem = GetChildAt(n);
			if (NULL == pItem || false == pItem->IsVisible()) continue;

			pItem->OnDraw(hDC, rcItemRange, bGenerateBmp);
		}
	}

	return true;
}

bool CDUIRotateMenuCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (MMInterfaceHelper(CDUIRotateMenuItemCtrl, pChild)
		|| MMInterfaceHelper(CDUIScrollBarCtrl, pChild))
	{
		bool bRes = __super::InsertChild(pChild, nPos);

		return bRes;
	}

	return false;
}

CDUIRotateMenuItemCtrl * CDUIRotateMenuCtrl::GetChildAt(int nIndex) const
{
	return static_cast<CDUIRotateMenuItemCtrl *>(CDUIContainerCtrl::GetChildAt(nIndex));
}

SIZE CDUIRotateMenuCtrl::GetScrollPos() const
{
	return { m_nAngleScrollCur, 0 };
}

SIZE CDUIRotateMenuCtrl::GetScrollRange() const
{
	return { m_nAngleScrollRange, 0 };
}

void CDUIRotateMenuCtrl::SetScrollPos(SIZE szPos, bool bMsg/* = true*/)
{
	ScrollChilds(szPos);

	return;
}

void CDUIRotateMenuCtrl::EnsureVisible(int nIndex, bool bCenter)
{
	if (nIndex < 0 || nIndex >= GetChildCount()) return;

	int nAngleStart = GetAngleStart(), nAngleFinish = GetAngleFinish();
	int nAngleRange = nAngleFinish - nAngleStart;
	int nAngleMid = nAngleStart + nAngleRange / 2;
	int nAngleOffset = 0;

	do
	{
		float fAngle = GetChildAngle(nIndex);
		if (bCenter)
		{
			nAngleOffset = nAngleMid - fAngle;
			nAngleOffset *= IsRotateClockWise() ? 1 : -1;

			break;
		}
		if (nAngleStart <= fAngle && fAngle <= nAngleFinish)
		{
			break;
		}

		nAngleOffset = fAngle < nAngleStart ? nAngleStart - fAngle : nAngleFinish - fAngle;
		nAngleOffset *= IsRotateClockWise() ? 1 : -1;

	} while (false);
	
	ScrollChilds(CDUISize(nAngleOffset, 0));

	NeedRefreshView();

	return;
}

CDUISize CDUIRotateMenuCtrl::GetItemSizeMin()
{
	return m_AttributeItemSizeMin.GetValue();
}

void CDUIRotateMenuCtrl::SetItemSizeMin(CDUISize szItem)
{
	if (szItem == GetItemSizeMin()) return;

	m_AttributeItemSizeMin.SetValue(szItem);

	NeedRefreshView();

	return;
}

CDUISize CDUIRotateMenuCtrl::GetItemSizeMax()
{
	return m_AttributeItemSizeMax.GetValue();
}

void CDUIRotateMenuCtrl::SetItemSizeMax(CDUISize szItem)
{
	if (szItem == GetItemSizeMax()) return;

	m_AttributeItemSizeMax.SetValue(szItem);

	NeedRefreshView();

	return;
}

int CDUIRotateMenuCtrl::GetAngleStart()
{
	return m_AttributeAngleStart.GetValue();
}

void CDUIRotateMenuCtrl::SetAngleStart(int nAngle)
{
	if (nAngle == GetAngleStart()) return;

	m_AttributeAngleStart.SetValue(nAngle);

	NeedRefreshView();

	return;
}

int CDUIRotateMenuCtrl::GetAngleFinish()
{
	return m_AttributeAngleFinish.GetValue();
}

void CDUIRotateMenuCtrl::SetAngleFinish(int nAngle)
{
	if (nAngle == GetAngleFinish()) return;

	m_AttributeAngleFinish.SetValue(nAngle);

	NeedRefreshView();

	return;
}

int CDUIRotateMenuCtrl::GetAngleSpaceMin()
{
	return m_AttributeAngleSpaceMin.GetValue();
}

void CDUIRotateMenuCtrl::SetAngleSpaceMin(int nAngle)
{
	if (nAngle == GetAngleSpaceMin()) return;

	m_AttributeAngleSpaceMin.SetValue(nAngle);

	NeedRefreshView();

	return;
}

int CDUIRotateMenuCtrl::GetAngleSpace()
{
	return m_nAngleSpace;
}

bool CDUIRotateMenuCtrl::IsRotateClockWise()
{
	return m_AttributeRotateClockWise.GetValue();
}

void CDUIRotateMenuCtrl::SetRotateClockWise(bool bClockWise)
{
	if (bClockWise == IsRotateClockWise()) return;

	m_AttributeRotateClockWise.SetValue(bClockWise);

	NeedRefreshView();

	return;
}

float CDUIRotateMenuCtrl::GetChildAngle(int nIndex)
{
	CDUIRotateMenuItemCtrl *pChild = GetChildAt(nIndex);
	if (NULL == pChild) return 0;

	if (m_nAngleSpace <= 0) return 0;
	int nAngleStart = GetAngleStart(), nAngleFinish = GetAngleFinish();
	int nAngleRange = nAngleFinish - nAngleStart;
	int nAngleMid = nAngleStart + nAngleRange / 2;

	//angle
	int nIndexFirst = m_nAngleScrollCur / m_nAngleSpace;
	int nAngleSurplus = m_nAngleScrollCur % m_nAngleSpace;
	int nAngleFirst = IsRotateClockWise() ? nAngleStart : nAngleFinish;
	
	float fAngle = nAngleFirst + (nIndex - nIndexFirst) * m_nAngleSpace - nAngleSurplus;
	IsRotateClockWise() ? fAngle : fAngle = nAngleFirst - (nIndex - nIndexFirst) * m_nAngleSpace + nAngleSurplus;

	return fAngle;
}

bool CDUIRotateMenuCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_ptMouseDown = pt;
	m_ptLastMousePos = pt;

	return __super::OnDuiLButtonDown(pt, Msg);
}

bool CDUIRotateMenuCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (m_pWndManager && m_pWndManager->GetCaptureControl() == this)
	{
		ScrollChilds(pt);
	}

	return __super::OnDuiMouseMove(pt, Msg);
}

bool CDUIRotateMenuCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	switch ((int)(short)HIWORD(Msg.wParam))
	{
		case WHEEL_DELTA:
		{
			ScrollChildsByWheelUp(Dui_ScrollSpeed_Normal);

			break;
		}
		case -WHEEL_DELTA:
		{
			ScrollChildsByWheelDown(Dui_ScrollSpeed_Normal);

			break;
		}
	}

	CDUIControlBase::OnDuiMouseWheel(pt, Msg);

	return true;
}

void CDUIRotateMenuCtrl::InitProperty()
{
	__super::InitProperty();

	//modify name
	DuiModifyAttriName(m_AttributeGroupListView, _T("RotateMenu"), DuiResVersion_3);

	//create
	DuiCreateAttribute(m_AttributeAngleStart, _T("AngleStart"), _T("start from right border"), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeAngleFinish, _T("AngleFinish"), _T("round is 360"), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeAngleSpaceMin, _T("AngleSpaceMin"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeRotateClockWise, _T("RotateClockWise"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeItemSizeMin, _T("ItemSizeMin"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeItemSizeMax, _T("ItemSizeMax"), _T(""), m_AttributeGroupListView);

	return;
}

void CDUIRotateMenuCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);
	DuiInitAttriVisible(m_AttributeAnimateDrag, false);
	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeGroupChild, false);
	DuiInitAttriVisible(m_AttributeGroupListType, false);
	DuiInitAttriVisible(m_AttributeGroupTileType, false);
	DuiInitAttriVisible(m_AttributeGroupLine, false);
	DuiInitAttriVisible(m_AttributeImageMouseDragSel, false);
	DuiInitAttriVisible(m_AttributeItemModel, true);
	DuiInitAttriVisible(m_AttributeColorItemStatusHot, true);
	DuiInitAttriVisible(m_AttributeColorItemStatusSelHot, true);
	DuiInitAttriVisible(m_AttributeImageItemStatusHot, true);
	DuiInitAttriVisible(m_AttributeImageItemStatusSelHot, true);

	MMSafeDelete(m_pListHeader);

	EnableScrollBar(false, false);

	return;
}

void CDUIRotateMenuCtrl::PaintBkColor(HDC hDC)
{
	__super::PaintBkColor(hDC);

	return;
}

void CDUIRotateMenuCtrl::OnDuiItemButtonDown(const DuiNotify &Notify)
{
	if (Notify.uCtrlID != GetCtrlID()) return;

	m_ptMouseDown = Notify.ptMouse;
	m_ptLastMousePos = Notify.ptMouse;

	return;
}

void CDUIRotateMenuCtrl::OnDuiItemMouseMove(const DuiNotify &Notify)
{
	if (Notify.uCtrlID != GetCtrlID() || NULL == m_pWndManager) return;

	DuiNotify::tagDuiNotifyExtend NotifyExtend = Notify.DuiNotifyExtend;
	int nIndex = NotifyExtend.ListView.nIndexItem;

	CDUIListItemCtrl *pListItem = GetChildAt(nIndex);
	if (NULL == pListItem) return;

	if (pListItem == m_pWndManager->GetCaptureControl()
		|| pListItem->VerifyChild(m_pWndManager->GetCaptureControl()))
	{
		ScrollChilds(Notify.ptMouse);
	}

	return;
}

CDUIListItemCtrl * CDUIRotateMenuCtrl::ProductItemDefault()
{
	CDUIRotateMenuItemCtrl *pItem = new CDUIRotateMenuItemCtrl();
	pItem->Init();

	return pItem;
}

void CDUIRotateMenuCtrl::ScrollChildsByWheelUp(int nOffset)
{
	int nDirection = GetAngleFinish() > 90 ? 1 : -1;
	nDirection *= (IsRotateClockWise() ? 1 : -1);
	ScrollChilds(SIZE{ nOffset * nDirection, 0 });

	return;
}

void CDUIRotateMenuCtrl::ScrollChildsByWheelDown(int nOffset)
{
	int nDirection = GetAngleFinish() > 90 ? 1 : -1;
	nDirection *= (IsRotateClockWise() ? -1 : 1);
	ScrollChilds(SIZE{ nOffset * nDirection, 0 });

	return;
}

void CDUIRotateMenuCtrl::ScrollChilds(CDUIPoint ptMouse)
{
	CDUIPoint ptCenter(GetAbsoluteRect().left + GetAbsoluteRect().GetWidth() / 2, GetAbsoluteRect().top + GetAbsoluteRect().GetHeight() / 2);
	int nMarkX = m_ptLastMousePos.y <= ptCenter.y ? 1 : -1;
	int nMarkY = m_ptLastMousePos.x <= ptCenter.x ? -1 : 1;
	IsRotateClockWise() ? nMarkX *= -1 : nMarkX;
	IsRotateClockWise() ? nMarkY *= -1 : nMarkY;

	CDUISize szScroll(nMarkX * (ptMouse.x - m_ptLastMousePos.x), nMarkY * (ptMouse.y - m_ptLastMousePos.y));
	ScrollChilds(szScroll);

	m_ptLastMousePos = ptMouse;

	return;
}

void CDUIRotateMenuCtrl::ScrollChilds(CDUISize szScroll)
{
	m_nAngleScrollCur += szScroll.cx;
	m_nAngleScrollCur = max(m_nAngleScrollCur, 0);
	m_nAngleScrollCur = min(m_nAngleScrollCur, m_nAngleScrollRange);

	NeedRefreshView();

	return;
}

//////////////////////////////////////////////////////////////////////////