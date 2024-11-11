#include "StdAfx.h"
#include "DUITabLayoutCtrl.h"

DuiImplement_CreateControl(CDUITabLayoutCtrl)
MMImplement_ClassName(CDUITabLayoutCtrl)

CDUITabLayoutCtrl::CDUITabLayoutCtrl(void)
{
	m_nOldSel = -1;

	static tagDuiCombox AttriCombox;
	if (AttriCombox.vecItem.empty())
	{
		AttriCombox.vecItem.push_back({ AnimateSelSwitch_None, _T("None") });
		AttriCombox.vecItem.push_back({ AnimateSelSwitch_H, _T("SelSwitch_H") });
		AttriCombox.vecItem.push_back({ AnimateSelSwitch_V, _T("SelSwitch_V") });
	}

	m_AttributeAnimateSelSwitchType.SetCombox(AttriCombox);
	m_AttributeAnimateSelSwitchType.SelectItem(AnimateSelSwitch_None);

	return;
}

CDUITabLayoutCtrl::~CDUITabLayoutCtrl(void)
{
}

bool CDUITabLayoutCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeSelectIndex)
	{
		SelectItem(m_AttributeSelectIndex.GetValue());

		return true;
	}

	return false;
}

void CDUITabLayoutCtrl::OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	CDUIControlBase *pChildCurSel = GetChildAt(GetCurSel());
	CDUIControlBase *pChildOldSel = GetChildAt(m_nOldSel);
	if (NULL == pChildCurSel || NULL == pChildOldSel) return;

	CDUIRect rcCur = GetAbsoluteRect();
	CDUIRect rcOld = GetAbsoluteRect();

	if (AnimateSelSwitch_H == GetAnimateSelSwitchType())
	{
		bool bPositive = GetCurSel() < m_nOldSel;
		int nOffset = rcCur.GetWidth() / nTotalFrame * nCurFrame;
		rcCur.Offset(bPositive ? -rcCur.GetWidth() : rcCur.GetWidth(), 0);
		rcCur.Offset(bPositive ? nOffset : -nOffset, 0);
		rcOld.Offset(bPositive ? nOffset : -nOffset, 0);
	}
	else
	{
		bool bPositive = GetCurSel() > m_nOldSel;
		int nOffset = rcCur.GetHeight() / nTotalFrame * nCurFrame;
		rcCur.Offset(0, bPositive ? rcCur.GetHeight() : -rcCur.GetHeight());
		rcCur.Offset(0, bPositive ? -nOffset : nOffset);
		rcOld.Offset(0, bPositive ? -nOffset : nOffset);
	}

	pChildCurSel->OnDuiSize(rcCur);
	pChildOldSel->OnDuiSize(rcOld);

	return;
}

void CDUITabLayoutCtrl::OnAnimationStop(int nAnimationID)
{
	CDUIControlBase *pChildOldSel = GetChildAt(m_nOldSel);
	if (NULL == pChildOldSel) return;

	pChildOldSel->SetVisible(false);

	NeedRefreshView();

	return;
}

LPVOID CDUITabLayoutCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUITabLayoutCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUITabLayoutCtrl::GetDescribe() const
{
	return Dui_Ctrl_TabLayout;
}

bool CDUITabLayoutCtrl::SetTimer(UINT uTimerID, UINT nElapse)
{
	return CDUIContainerCtrl::SetTimer(uTimerID, nElapse);
}

bool CDUITabLayoutCtrl::KillTimer(UINT uTimerID)
{
	return CDUIContainerCtrl::KillTimer(uTimerID);
}

bool CDUITabLayoutCtrl::KillTimer()
{
	return CDUIContainerCtrl::KillTimer();
}

void CDUITabLayoutCtrl::RefreshView()
{
	if (IsAnimationRunning(Dui_TimerAnimate_ID)) return;

	return __super::RefreshView();
}

bool CDUITabLayoutCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (false == __super::InsertChild(pChild, nPos)) return false;

	if (m_pHorizScrollBarCtrl != pChild
		&& m_pVertScrollBarCtrl != pChild
		&& IsInitComplete())
	{
		if (-1 == GetCurSel())
		{
			if (pChild->IsVisible())
			{
				Select(GetChildIndex(pChild));
			}
		}
		else
		{
			if (0 <= nPos && nPos <= GetCurSel())
			{
				Select(GetCurSel() + 1);
			}
			if (GetCurSel() != GetChildIndex(pChild))
			{
				pChild->SetVisible(false);
			}
		}
	}

	return true;
}

CDUIControlBase * CDUITabLayoutCtrl::DetachChild(int nIndex)
{
	CDUIControlBase *pSelChild = GetChildAt(GetCurSel());
	CDUIControlBase *pDetachChild = __super::DetachChild(nIndex);

	SetCurSel(-1);
	if (pSelChild == pDetachChild)
	{
		Select(min(GetCurSel(), GetChildCount() - 1));
	}
	else
	{
		Select(GetChildIndex(pSelChild));
	}

	return pDetachChild;
}

bool CDUITabLayoutCtrl::SetChildIndex(CDUIControlBase *pControl, int nIndex)
{
	CDUIControlBase *pSelChild = GetChildAt(GetCurSel());

	if (false == __super::SetChildIndex(pControl, nIndex)) return false;

	SetCurSel(-1);
	Select(GetChildIndex(pSelChild));

	return true;
}

bool CDUITabLayoutCtrl::SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView)
{
	CDUIControlBase *pSelChild = GetChildAt(GetCurSel());
	CDUIControlBase *pFirst = GetChildAt(nIndexFirst);
	CDUIControlBase *pNext = GetChildAt(nIndexNext);

	if (false == __super::SwapChild(nIndexFirst, nIndexNext, bNeedRefreshView)) return false;

	if (pSelChild == pFirst || pSelChild == pNext)
	{
		SetCurSel(-1);
		Select(GetChildIndex(pSelChild));
	}

	return true;
}

bool CDUITabLayoutCtrl::Remove(CDUIControlBase *pControl)
{
	int nIndex = GetChildIndex(pControl);
	if (false == __super::Remove(pControl)) return false;

	if (GetCurSel() == nIndex)
	{
		SetCurSel(-1);
		if (GetChildCount() > 0)
		{
			Select(0);
		}
	}
	else if (GetCurSel() > nIndex)
	{
		Select(GetCurSel() - 1);
	}

	NeedRefreshView();

	return true;
}

bool CDUITabLayoutCtrl::RemoveAt(int nIndex)
{
	return Remove(GetChildAt(nIndex));
}

void CDUITabLayoutCtrl::RemoveAll()
{
	__super::RemoveAll();

	SetCurSel(-1);

	return;
}

bool CDUITabLayoutCtrl::Select(int nIndex)
{
	CDUIControlBase *pChild = GetChildAt(nIndex);
	if (nIndex == GetCurSel() && pChild && pChild->IsVisible()) return true;

	SelectItem(nIndex);

	return true;
}

bool CDUITabLayoutCtrl::Select(CDUIControlBase *pChild)
{
	return Select(GetChildIndex(pChild));
}

int CDUITabLayoutCtrl::GetCurSel()
{
	return m_AttributeSelectIndex.GetValue();
}

enDuiAnimateSelSwithType CDUITabLayoutCtrl::GetAnimateSelSwitchType()
{
	return (enDuiAnimateSelSwithType)m_AttributeAnimateSelSwitchType.GetCurSelItem();
}

void CDUITabLayoutCtrl::SetAnimateSelSwitchType(enDuiAnimateSelSwithType AnimateSelSwitchType)
{
	if (AnimateSelSwitchType == GetAnimateSelSwitchType()) return;

	m_AttributeAnimateSelSwitchType.SelectItem(AnimateSelSwitchType);

	return;
}

void CDUITabLayoutCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupSelect, _T("Select"));
	DuiCreateAttribute(m_AttributeSelectIndex, _T("SelectIndex"), _T(""), m_AttributeGroupSelect);
	DuiCreateAttribute(m_AttributeAnimateSelSwitchType, _T("AnimateSelSwitchType"), _T(""), m_AttributeGroupSelect);

	return;
}

void CDUITabLayoutCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);

	//select
	SelectItem(GetCurSel());

	return;
}

void CDUITabLayoutCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	if (Dui_TimerAnimate_ID == uTimerID)
	{
		OnAnimationElapse(uTimerID);

		return;
	}

	return __super::OnDuiTimer(uTimerID, Msg);
}

void CDUITabLayoutCtrl::SelectItem(int nIndex)
{
	int nCurSel = GetCurSel();
	m_nOldSel = nCurSel;

	//hide
	for (int n = 0; n < GetChildCount(); n++)
	{
		if (n == nIndex) continue;

		CDUIControlBase *pChild = GetChildAt(n);
		if (NULL == pChild) continue;

		pChild->SetVisible(false);
	}

	//show
	CDUIControlBase *pChild = GetChildAt(nIndex);
	if (pChild)
	{
		pChild->SetVisible(true);

		SetCurSel(nIndex);
	}
	else
	{
		SetCurSel(-1);
	}

	//select bind RadioBox
	if (m_pWndOwner)
	{
		CDUIRadioBoxCtrl *pRadioBoxCtrl = m_pWndOwner->GetRadioBoxOfBindTabCtrl(this, GetCurSel());

		if (pRadioBoxCtrl)
		{
			pRadioBoxCtrl->Select(true);
		}
	}

	//animate
	if (AnimateSelSwitch_None != GetAnimateSelSwitchType()
		&& false == GetAbsoluteRect().Empty()
		&& GetChildCount() > 1
		&& -1 != m_nOldSel
		&& m_nOldSel != GetCurSel())
	{
		CDUIControlBase *pChildOldSel = GetChildAt(m_nOldSel);
		if (pChildOldSel)
		{
			pChildOldSel->SetVisible(true);
		}

		StopAnimation();

		StartAnimation();
	}

	return;
}

void CDUITabLayoutCtrl::SetCurSel(int nIndex)
{
	if (nIndex == m_AttributeSelectIndex.GetValue()) return;

	m_AttributeSelectIndex.SetValue(nIndex);

	return;
}
