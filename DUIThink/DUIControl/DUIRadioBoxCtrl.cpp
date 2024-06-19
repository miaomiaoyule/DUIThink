#include "StdAfx.h"
#include "DUIRadioBoxCtrl.h"

DuiImplement_CreateControl(CDUIRadioBoxCtrl)
MMImplement_ClassName(CDUIRadioBoxCtrl)

CDUIRadioBoxCtrl::CDUIRadioBoxCtrl(void)
{
}

CDUIRadioBoxCtrl::~CDUIRadioBoxCtrl(void)
{
	if (GetWndManager())
	{
		GetWndManager()->RemoveRadioBoxFromGroup(this);
	}

	return;
}

bool CDUIRadioBoxCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeGroupName)
	{
		SetGroupName(GetGroupName());

		return true;
	}

	return false;
}

LPVOID CDUIRadioBoxCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIRadioBoxCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIRadioBoxCtrl::GetDescribe() const
{
	return Dui_Ctrl_RadioBox;
}

bool CDUIRadioBoxCtrl::Active()
{
	if (false == CDUIButtonCtrl::Active()) return false;

	Select(true);

	return true;
}

void CDUIRadioBoxCtrl::Select(bool bSelect, bool bNotify)
{
#ifndef DUI_DESIGN
	if (bSelect == IsSelected()) return;
#endif

	if (bSelect)
	{
		PerformUnSelOther();
	}

	__super::Select(bSelect, bNotify);

	PerformActiveBindTabPage();

	Invalidate();

	return;
}

CMMString CDUIRadioBoxCtrl::GetGroupName()
{
	return m_AttributeGroupName.GetValue();
}

void CDUIRadioBoxCtrl::SetGroupName(LPCTSTR lpszGroup)
{
	if (GetWndManager())
	{
		GetWndManager()->RemoveRadioBoxFromGroup(this);
	}

	m_AttributeGroupName.SetValue(lpszGroup);

	if (GetWndManager())
	{
		GetWndManager()->AddRadioBoxToGroup(this);
	}

	return;
}

CDUITabLayoutCtrl * CDUIRadioBoxCtrl::GetBindTabCtrl()
{
	if (NULL == GetWndManager()) return NULL;

	UINT uBindTabCtrlID = GetBindTabCtrlID();

	CDUIControlBase *pControl = GetWndManager()->FindControl(uBindTabCtrlID);
	if (NULL == pControl) return NULL;

	return MMInterfaceHelper(CDUITabLayoutCtrl, pControl);
}

void CDUIRadioBoxCtrl::SetBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl)
{
	if (NULL == pTabCtrl || pTabCtrl == GetBindTabCtrl()) return;

	SetBindTabCtrlID(pTabCtrl->GetCtrlID());

	return;
}

UINT CDUIRadioBoxCtrl::GetBindTabCtrlID()
{
	return m_AttributeBindTabCtrlID.GetCurSelItem();
}

void CDUIRadioBoxCtrl::SetBindTabCtrlID(UINT uCtrlID)
{
	m_AttributeBindTabCtrlID.SelectItem(uCtrlID);

	PerformActiveBindTabPage();

	return;
}

long CDUIRadioBoxCtrl::GetBindTabCtrlIndex()
{
	return m_AttributeBindTabCtrlIndex.GetValue();
}

void CDUIRadioBoxCtrl::SetBindTabCtrlIndex(int nIndex)
{
	if (nIndex == GetBindTabCtrlIndex()) return;

	m_AttributeBindTabCtrlIndex.SetValue(nIndex);

	PerformActiveBindTabPage();

	return;
}

void CDUIRadioBoxCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupRadio, _T("Radio"));
	DuiCreateAttribute(m_AttributeGroupName, _T("GroupName"), _T(""), m_AttributeGroupRadio);
	DuiCreateAttribute(m_AttributeBindTabCtrlID, _T("BindTabCtrlID"), _T(""), m_AttributeGroupRadio);
	DuiCreateAttribute(m_AttributeBindTabCtrlIndex, _T("BindTabCtrlIndex"), _T(""), m_AttributeGroupRadio);

	return;
}

bool CDUIRadioBoxCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_nControlStatus & ControlStatus_Pushed);

	CDUIButtonCtrl::OnDuiLButtonUp(pt, Msg);

	if (bClick)
	{
		if (false == IsSelected())
		{
			Select(true);
		}
	}

	Invalidate();

	return true;
}

void CDUIRadioBoxCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (GetWndManager())
	{
		GetWndManager()->AddRadioBoxToGroup(this);
	}

	return;
}

void CDUIRadioBoxCtrl::OnDuiWndManagerDetach()
{
	__super::OnDuiWndManagerDetach();

	if (GetWndManager())
	{
		GetWndManager()->RemoveRadioBoxFromGroup(this);
	}

	return;
}

void CDUIRadioBoxCtrl::PerformActiveBindTabPage()
{
	if (false == IsSelected() || NULL == GetWndManager()) return;

	CDUITabLayoutCtrl *pBindTabCtrl = GetBindTabCtrl();
	if (pBindTabCtrl == NULL) return;

	long lBindTabCtrlIndex = GetBindTabCtrlIndex();

	//如果没有绑定下标则以组内下标
	if (lBindTabCtrlIndex < 0)
	{
		auto vecRadioBox = GetWndManager()->GetRadioBoxGroup(GetGroupName());
		for (int nIndex = 0; nIndex < vecRadioBox.size(); nIndex++)
		{
			if (vecRadioBox[nIndex] == this)
			{
				lBindTabCtrlIndex = nIndex;

				SetBindTabCtrlIndex(lBindTabCtrlIndex);

				break;
			}
		}
	}

	if (lBindTabCtrlIndex < 0) return;

	//激活Tab页
	pBindTabCtrl->Select(lBindTabCtrlIndex);

	return;
}

void CDUIRadioBoxCtrl::PerformUnSelOther()
{
	if (NULL == m_pWndManager) return;

	//unselect other
	auto vecRadioBox = m_pWndManager->GetRadioBoxGroup(GetGroupName());
	for (int nIndex = 0; nIndex < vecRadioBox.size(); nIndex++)
	{
		CDUIRadioBoxCtrl *pRadioBoxCtrl = vecRadioBox[nIndex];
		if (NULL == pRadioBoxCtrl || pRadioBoxCtrl == this) continue;

		pRadioBoxCtrl->Select(false);
	}

	return;
}