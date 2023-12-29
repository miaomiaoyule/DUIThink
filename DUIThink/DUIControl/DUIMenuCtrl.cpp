#include "StdAfx.h"
#include "DUIMenuCtrl.h"

//////////////////////////////////////////////////////////////////////////
#define Default_LineMenu_Height			(6)
#define	Default_LineMenu_Color			(0xFFBCBFC4)	
#define Size_MenuNormal					(150)

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIMenuWnd)

DuiBegin_Message_Map(CDUIMenuWnd, CDUIWnd)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CDUIMenuWnd::CDUIMenuWnd()
{
	return;
}

CDUIMenuWnd::CDUIMenuWnd(CDUIMenuItemCtrl *pOwner, CMMString strDuiName)
	: CDUIWnd(strDuiName)
	, m_pOwner(pOwner)
{
	CDUIControlBase *pRootCtrl = CDUIGlobal::GetInstance()->LoadDui(GetDuiName(), m_pWndManager);
	m_pShowMenuView = MMInterfaceHelper(CDUIMenuCtrl, pRootCtrl);

	if (m_pShowMenuView)
	{
		m_pShowMenuView->RegisterControlCallBack(this);
	}

	return;
}

CDUIMenuWnd::~CDUIMenuWnd()
{
	UnInit();

	if (m_pShowMenuView)
	{
		m_pShowMenuView->UnRegisterControlCallBack(this);
	}

	m_pOwner = NULL;
	m_pWndManagerOwner = NULL;

	MMSafeDelete(m_pShowMenuView);

	return;
}

void CDUIMenuWnd::Init(HWND hWndParent)
{
	assert(m_pShowMenuView);
	if (IsWindow(m_hWnd) || NULL == m_pShowMenuView) return;

	//init view
	if (m_pShowMenuView && NULL == MMInterfaceHelper(CDUIRotateMenuCtrl, m_pShowMenuView))
	{
		m_pShowMenuView->UnSelectAllItems();
	}
	if (m_pOwner)
	{
		m_pWndManagerOwner = static_cast<CDUIWndManager*>(m_pOwner->GetWndManager());
	}

	Create(hWndParent, _T("DuiMenuWnd"), WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, m_ptTrack.x, m_ptTrack.y);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	
	//focus
	m_pShowMenuView->SetFocus();

	return;
}

void CDUIMenuWnd::Init(HWND hWndParent, CDUIPoint ptTrack)
{
	m_ptTrack = ptTrack;

	Init(hWndParent);

	return;
}

void CDUIMenuWnd::UnInit()
{
	//save
#ifdef DUI_DESIGN
	if (m_pWndManager->GetRootCtrl())
	{
		CDUIGlobal::GetInstance()->SaveDui(GetDuiName(), MMDynamicPtr(CDUIWndManager, m_pWndManager));
	}
#endif

	//unexpand view
	if (m_pShowMenuView)
	{
		for (int i = 0; i < m_pShowMenuView->GetMenuItemCount(); i++)
		{
			CDUIMenuItemCtrl *pMenuItem = m_pShowMenuView->GetMenuItem(i);
			if (NULL == pMenuItem) continue;

			pMenuItem->Expand(false);
		}
	}

	Close();

	m_pWndManager->DetachRootCtrl();

	return;
}

CDUIMenuCtrl * CDUIMenuWnd::GetMenuView()
{
	return m_pShowMenuView;
}

void CDUIMenuWnd::SetMenuView(CDUIMenuCtrl *pMenuView)
{
	if (NULL == pMenuView || pMenuView == GetMenuView()) return;

	m_pShowMenuView = pMenuView;

	return;
}

CDUIMenuItemCtrl * CDUIMenuWnd::GetOwner()
{
	return m_pOwner;
}

void CDUIMenuWnd::OnSize(CDUIControlBase *pControl)
{
	if (pControl == m_pShowMenuView)
	{
	}

	return;
}

LRESULT CDUIMenuWnd::OnCreate(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	m_pWndManager->DetachRootCtrl();

	__super::OnCreate(wParam, lParam, bHandled);

	//init view
	m_pWndManager->AttachRootCtrl(m_pShowMenuView);

	return 0;
}

LRESULT CDUIMenuWnd::OnKillFocus(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	__super::OnKillFocus(wParam, lParam, bHandled);

#ifdef DUI_DESIGN
	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONULL);
	if (NULL == hMonitor) return 0;
#endif

	if (NULL == m_pOwner)
	{
		HWND hWndFocus = (HWND)wParam;
		while (hWndFocus)
		{
			if (hWndFocus == m_hWnd) return 0;

			hWndFocus = GetParent(hWndFocus);
		}

		UnInit();
	}

	return 0;
}

LRESULT CDUIMenuWnd::OnKeyDown(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	__super::OnKeyDown(wParam, lParam, bHandled);

	if (VK_ESCAPE == wParam
		|| (VK_LEFT == wParam && m_pOwner))
	{
		Close();
	}

	return 0;
}

LRESULT CDUIMenuWnd::OnClose(WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	__super::OnClose(wParam, lParam, bHandled);

	if (m_pOwner)
	{
		m_pOwner->SetFocus();
	}

	return 0;
}

LRESULT CDUIMenuWnd::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DUIRESIZEMENU:
		{
			OnWMDuiResizeMenu(wParam, lParam);

			return 0;
		}
	}

	return  __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDUIMenuWnd::OnWMDuiResizeMenu(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pShowMenuView) return 0;

	MMInterfaceHelper(CDUIRotateMenuCtrl, m_pShowMenuView, pRotateMenu);

	//wnd size
	CDUIRect rcWnd;
	CDUISize szRange = m_pShowMenuView->GetTotalRange();
	CDUIRect rcInset = m_pShowMenuView->GetRangeInset();
	szRange.cx <= 0 ? szRange.cx = Size_MenuNormal : szRange.cx += rcInset.left + rcInset.right;
	szRange.cy <= 0 ? szRange.cy = Size_MenuNormal : szRange.cy += rcInset.top + rcInset.bottom;
	pRotateMenu ? szRange.cx = rcWnd.GetWidth() : szRange.cx;
	pRotateMenu ? szRange.cy = rcWnd.GetHeight() : szRange.cy;
	GetWindowRect(m_pWndManager->GetWndHandle(), &rcWnd);

	if (rcWnd.GetWidth() != szRange.cx
		|| rcWnd.GetHeight() != szRange.cy)
	{
		SetWindowPos(m_pWndManager->GetWndHandle(), NULL, 0, 0, szRange.cx, szRange.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
	}

	//wnd pos
	if (NULL == m_pOwner)
	{
		ResizeMenu();
	}
	else
	{
		ResizeSubMenu();
	}

	return 0;
}

void CDUIMenuWnd::OnDuiWndInited(const DuiNotify &Notify)
{
	if (NULL == m_pShowMenuView) return;

	m_pShowMenuView->EnsureVisible(m_pShowMenuView->GetCurSel());

	return;
}

void CDUIMenuWnd::ResizeMenu()
{
	if (NULL == m_pShowMenuView) return;

	CDUIRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;

	//adjust pt
	if (-1 == m_ptTrack.x && -1 == m_ptTrack.y)
	{
		CDUIRect rcWndParent;
		GetWindowRect(::GetParent(m_hWnd), &rcWndParent);

		if (rcWndParent.IsEmpty()) rcWndParent = rcWork;
		m_ptTrack.x = rcWndParent.left + rcWndParent.GetWidth() / 2 - rcWnd.GetWidth() / 2;
		m_ptTrack.y = rcWndParent.top + rcWndParent.GetHeight() / 2 - rcWnd.GetHeight() / 2;
	}

	rcWnd.Offset(m_ptTrack.x - rcWnd.left, m_ptTrack.y - rcWnd.top);
	if (rcWnd.right > rcWork.right) rcWnd.Offset(rcWork.right - rcWnd.right, 0);
	if (rcWnd.bottom > rcWork.bottom) rcWnd.Offset(0, -rcWnd.GetHeight());

	MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), FALSE);

	return;
}

void CDUIMenuWnd::ResizeSubMenu()
{
	if (NULL == m_pShowMenuView) return;

	CDUIRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;

	CDUIRect rcWndOwner;
	GetWindowRect(m_pWndManagerOwner->GetWndHandle(), &rcWndOwner);

	CDUIRect rcOwner = m_pOwner->GetAbsoluteRect();
	CDUIPoint ptOwner(rcOwner.right, rcOwner.top);
	ClientToScreen(m_pWndManagerOwner->GetWndHandle(), &ptOwner);

	rcWnd.Offset(rcWndOwner.right - rcWnd.left, ptOwner.y - rcWnd.top);
	if (rcWnd.right > rcWork.right) rcWnd.Offset(-(rcWndOwner.GetWidth() + rcWnd.GetWidth()), 0);
	if (rcWnd.bottom > rcWork.bottom) rcWnd.Offset(0, -rcWnd.GetHeight());
	
	MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), FALSE);

	return;
}
//////////////////////////////////////////////////////////////////////////

DuiImplement_CreateControl(CDUIMenuItemCtrl)
MMImplement_ClassName(CDUIMenuItemCtrl)

CDUIMenuItemCtrl::CDUIMenuItemCtrl(void)
{
	return;
}

CDUIMenuItemCtrl::~CDUIMenuItemCtrl(void)
{
	MMSafeDelete(m_pExpandMenuWnd);

	return;
}

bool CDUIMenuItemCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeHasExpandMenu)
	{
		InitExpandMenu(true);

		return true;
	}
	if (pAttributeObj == &m_AttributeLineMenu)
	{
		SetLineMenu(IsLineMenu());

		return true;
	}

	return false;
}

bool CDUIMenuItemCtrl::IsSelected() const
{
	return __super::IsSelected();
}

bool CDUIMenuItemCtrl::Select(bool bSelect /* = true */)
{
	if (false == __super::Select(bSelect)) return false;

	if (IsSelected())
	{
		SetFocus();
		ActiveExpandMenu();
	}
	else
	{
		UnActiveExpandMenu();
	}

	return true;
}

LPVOID CDUIMenuItemCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIMenuItemCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIMenuItemCtrl::GetDescribe() const
{
	return Dui_Ctrl_MenuItem;
}

bool CDUIMenuItemCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	if (IsLineMenu())
	{
		PaintLineMenu(hDC);
	}
	else
	{
		PaintIconChecked(hDC);
		PaintIconNormal(hDC);
		PaintIconExpand(hDC);
	}

	return true;
}

void CDUIMenuItemCtrl::RefreshView()
{
	__super::RefreshView();

	CalcItemRect();

	return;
}

bool CDUIMenuItemCtrl::IsExpanded() const
{
	return IsSelected() && m_pExpandMenuWnd && IsWindow(m_pExpandMenuWnd->GetHWND());
}

bool CDUIMenuItemCtrl::Expand(bool bExpand)
{
	return Select(bExpand);
}

bool CDUIMenuItemCtrl::CheckMenu(bool bCheck/* = true*/)
{
	if (NULL == m_pOwner) return false;
	if (m_bCheckMenu == bCheck) return true;

	m_bCheckMenu = bCheck;

	if (IsChecked() && m_pOwner)
	{
		MMInterfaceHelper(CDUIMenuCtrl, MMStaticPtr(CDUIControlBase, m_pOwner), pMenuCtrl);
		if (NULL == pMenuCtrl) return false;

		pMenuCtrl->CheckMenu(GetIndex());
	}

	Invalidate();

	return true;
}

bool CDUIMenuItemCtrl::IsChecked()
{
	return m_bCheckMenu;
}

CDUIMenuCtrl * CDUIMenuItemCtrl::GetExpandMenuView() const
{
	if (NULL == m_pExpandMenuWnd) return NULL;

	return m_pExpandMenuWnd->GetMenuView();
}

CDUIMenuItemCtrl * CDUIMenuItemCtrl::GetExpandMenuItemFromIndex(int nIndex) const
{
	MMInterfaceHelper(CDUIMenuCtrl, GetExpandMenuView(), pMenuView);
	if (NULL == pMenuView) return NULL;

	return pMenuView->GetMenuItem(nIndex);
}

CDUIMenuItemCtrl * CDUIMenuItemCtrl::GetExpandMenuItemFromID(UINT uCtrlID) const
{
	MMInterfaceHelper(CDUIMenuCtrl, GetExpandMenuView(), pMenuView);
	if (NULL == pMenuView) return NULL;

	for (int i = 0; i < pMenuView->GetChildCount(); i++)
	{
		CDUIMenuItemCtrl *pMenuItem = pMenuView->GetMenuItem(i);
		if (NULL == pMenuItem) continue;

		if (pMenuItem->GetCtrlID() == uCtrlID) return pMenuItem;
	}

	return NULL;
}

bool CDUIMenuItemCtrl::IsLineMenu()
{
	return m_AttributeLineMenu.GetValue();
}

void CDUIMenuItemCtrl::SetLineMenu(bool bLineMenu)
{
#ifndef DUI_DESIGN
	if (bLineMenu == IsLineMenu()) return;
#endif

	m_AttributeLineMenu.SetValue(bLineMenu);

	if (IsLineMenu())
	{
		SetFixedHeight(Default_LineMenu_Height);
		SetEnabled(false);
		NeedParentRefreshView();

		return;
	}

	SetEnabled(true);

	return;
}

void CDUIMenuItemCtrl::SetLineColorResSwitch(const CMMString &strResSwitch)
{
	m_AttributeLineColor.SetColorResSwitch(CMMStrHelp::ParseStrFromString(strResSwitch, ';'));

	Invalidate();

	return;
}

CMMString CDUIMenuItemCtrl::GetMenuGroupID()
{
	return m_AttributeGroupID.GetValue();
}

void CDUIMenuItemCtrl::SetMenuGroupID(CMMString strGroupID)
{
	if (strGroupID == m_AttributeGroupID.GetValue()) return;

	m_AttributeGroupID.SetValue(strGroupID);

	return;
}

void CDUIMenuItemCtrl::SetIconCheckedResSwitch(const CMMString &strResSwitch)
{
	m_AttributeIconChecked.SetImageResSwitch(strResSwitch);

	NeedRefreshView();

	return;
}

void CDUIMenuItemCtrl::SetIconNormalResSwitch(const CMMString &strResSwitch)
{
	m_AttributeIconNormal.SetImageResSwitch(strResSwitch);

	NeedRefreshView();

	return;
}

void CDUIMenuItemCtrl::SetIconExpandResSwitch(const CMMString &strResSwitch)
{
	m_AttributeIconExpand.SetImageResSwitch(strResSwitch);

	NeedRefreshView();

	return;
}

int CDUIMenuItemCtrl::GetIconCheckedLeftPadding()
{
	return m_AttributeIconCheckedLeftPadding.GetValue();
}

void CDUIMenuItemCtrl::SetIconCheckedLeftPadding(int nX)
{
	if (nX == m_AttributeIconCheckedLeftPadding.GetValue()) return;

	m_AttributeIconCheckedLeftPadding.SetValue(nX);

	NeedRefreshView();

	return;
}

int CDUIMenuItemCtrl::GetIconNormalLeftPadding()
{
	return m_AttributeIconNormalLeftPadding.GetValue();
}

void CDUIMenuItemCtrl::SetIconNormalLeftPadding(int nX)
{
	if (nX == m_AttributeIconNormalLeftPadding.GetValue()) return;

	m_AttributeIconNormalLeftPadding.SetValue(nX);

	NeedRefreshView();

	return;
}

int CDUIMenuItemCtrl::GetIconExpandRightPadding()
{
	return m_AttributeIconExpandRightPadding.GetValue();
}

void CDUIMenuItemCtrl::SetIconExpandRightPadding(int nX)
{
	if (nX == m_AttributeIconExpandRightPadding.GetValue()) return;

	m_AttributeIconExpandRightPadding.SetValue(nX);

	NeedRefreshView();

	return;
}

bool CDUIMenuItemCtrl::IsHasExpandMenu()
{
	return m_AttributeHasExpandMenu.GetValue();
}

void CDUIMenuItemCtrl::SetHasExpandMenu(bool bHasExpandMenu)
{
	if (bHasExpandMenu == IsHasExpandMenu()) return;

	m_AttributeHasExpandMenu.SetValue(bHasExpandMenu);

	InitExpandMenu();

	return;
}

bool CDUIMenuItemCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_nControlStatus & ControlStatus_Pushed);

	if (false == __super::OnDuiLButtonUp(pt, Msg)) return false;
	if (false == bClick) return false;
	if (false == GetMenuGroupID().IsEmpty())
	{
		CheckMenu(true);
	}
	else
	{
		CheckMenu(!IsChecked());
	}

	MMInterfaceHelper(CDUIMenuCtrl, GetExpandMenuView(), pMenuView);
	if (NULL == pMenuView
		|| pMenuView->GetChildCount() <= 0
		|| NULL == m_pExpandMenuWnd
		|| false == IsWindow(m_pExpandMenuWnd->GetHWND()))
	{
		g_DuiMenuCmd.uMenuID = GetCtrlID();
		g_DuiMenuCmd.uMenuTag = GetTag();
		g_DuiMenuCmd.strMenuUserData = GetUserData();
		g_DuiMenuCmd.strMenuText = GetText();

		if (g_pDuiMenuWndRoot)
		{
			g_pDuiMenuWndRoot->UnInit();
		}
	}

	return true;
}

bool CDUIMenuItemCtrl::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseEnter(pt, Msg)) return false;

	Select();

	return true;
}

void CDUIMenuItemCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseLeave(pt, Msg);

	if (false == IsExpanded())
	{
		Select(false);
	}

	return;
}

LRESULT CDUIMenuItemCtrl::OnDuiKeyDown(const DuiMessage &Msg)
{
	LRESULT lRes = __super::OnDuiKeyDown(Msg);

	if (VK_RIGHT == Msg.chKey)
	{
		Active();

		return lRes;
	}

	return lRes;
}

void CDUIMenuItemCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeMenuGroup, _T("Menu"));
	DuiCreateAttribute(m_AttributeLineMenu, _T("LineMenu"), _T("线型菜单"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeLineColor, _T("LineColor"), _T("线颜色"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeGroupID, _T("GroupID"), _T("Group ID"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeIconChecked, _T("IconChecked"), _T("IconChecked"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeIconCheckedLeftPadding, _T("IconCheckedLPadding"), _T("IconCheckedLPadding"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeIconNormal, _T("IconNormal"), _T("IconNormal"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeIconNormalLeftPadding, _T("IconNormalLPadding"), _T("IconNormalLPadding"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeIconExpand, _T("IconExpand"), _T("IconExpand"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeIconExpandRightPadding, _T("IconExpandRPadding"), _T("IconExpandRPadding"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeHasExpandMenu, _T("HasExpandMenu"), _T("HasExpandMenu"), m_AttributeMenuGroup);
	DuiCreateAttribute(m_AttributeExpandViewDuiName, _T("ExpandViewDuiName"), _T("ExpandViewDuiName"), m_AttributeMenuGroup);

	return;
}

void CDUIMenuItemCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	return;
}

void CDUIMenuItemCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAnimateDrag, false);
	DuiInitAttriVisible(m_AttributeExpandViewDuiName, false);
	DuiInitAttriVisible(m_AttributeGroupScroll, false);

	EnableScrollBar(false, false);

	LoadExpandMenu();

	return;
}

void CDUIMenuItemCtrl::PaintLineMenu(HDC hDC)
{
	m_AttributeLineColor.FillRect(hDC, m_rcLineMenu, IsColorHSL());

	return;
}

void CDUIMenuItemCtrl::PaintIconChecked(HDC hDC)
{
	if (false == IsChecked()) return;

	m_AttributeIconChecked.Draw(hDC, m_rcIconChecked, m_rcPaint);

	return;
}

void CDUIMenuItemCtrl::PaintIconNormal(HDC hDC)
{
	m_AttributeIconNormal.Draw(hDC, m_rcIconNormal, m_rcPaint);

	return;
}

void CDUIMenuItemCtrl::PaintIconExpand(HDC hDC)
{
	m_AttributeIconExpand.Draw(hDC, m_rcIconExpand, m_rcPaint);

	return;
}

CDUIRect CDUIMenuItemCtrl::GetTextRange()
{
	return m_rcItemText;
}

void CDUIMenuItemCtrl::ActiveExpandMenu()
{
	if (NULL == m_pWndManager || false == IsHasExpandMenu()) return;
	if (NULL == m_pExpandMenuWnd || IsWindow(m_pExpandMenuWnd->GetHWND())) return;

	m_pExpandMenuWnd->Init(m_pWndManager->GetWndHandle());

	return;
}

void CDUIMenuItemCtrl::UnActiveExpandMenu()
{
	if (NULL == m_pExpandMenuWnd || false == IsWindow(m_pExpandMenuWnd->GetHWND())) return;

	m_pExpandMenuWnd->UnInit();

	return;
}

void CDUIMenuItemCtrl::LoadExpandMenu()
{
	if (MMInvalidString(m_AttributeExpandViewDuiName.GetValue())) return;

	MMSafeDelete(m_pExpandMenuWnd);
	m_pExpandMenuWnd = new CDUIMenuWnd(this, m_AttributeExpandViewDuiName.GetValue());

	return;
}

void CDUIMenuItemCtrl::InitExpandMenu(bool bActive)
{
	CMMString strName = m_AttributeExpandViewDuiName.GetValue();

	//delete
	if (false == IsHasExpandMenu())
	{
		MMSafeDelete(m_pExpandMenuWnd);
		m_AttributeExpandViewDuiName.SetValue(_T(""));
		CDUIGlobal::GetInstance()->RemoveDui(strName);

		return;
	}

	//new expand view
	CDUIGlobal::GetInstance()->RemoveDui(strName);
	strName = CDUIGlobal::GetInstance()->CreateMenu(true);
	m_AttributeExpandViewDuiName.SetValue(strName);

	MMSafeDelete(m_pExpandMenuWnd);
	m_pExpandMenuWnd = new CDUIMenuWnd(this, strName);

	if (bActive)
	{
		Active();
	}

	return;
}

void CDUIMenuItemCtrl::CalcItemRect()
{
	if (NULL == m_pOwner) return;

	tagDuiListInfo ListInfo = m_pOwner->GetListInfo();

	m_rcLineMenu = m_rcAbsolute;
	m_rcLineMenu.top = m_rcLineMenu.top + m_rcLineMenu.GetHeight() / 2 - 1;
	m_rcLineMenu.bottom = m_rcLineMenu.top + 1;

	CDUIRect rcSourceChecked = m_AttributeIconChecked.GetSource();
	CDUIRect rcSourceNormal = m_AttributeIconNormal.GetSource();
	CDUIRect rcSourceExpand = m_AttributeIconExpand.GetSource();
	if (false == rcSourceChecked.IsEmpty())
	{
		m_rcIconChecked = GetAbsoluteRect();
		m_rcIconChecked.left += GetIconCheckedLeftPadding();
		m_rcIconChecked.right = m_rcIconChecked.left + rcSourceChecked.GetWidth();
		m_rcIconChecked.top += (m_rcIconChecked.GetHeight() - rcSourceChecked.GetHeight()) / 2;
		m_rcIconChecked.bottom = m_rcIconChecked.top + rcSourceChecked.GetHeight();
	}
	if (false == rcSourceNormal.IsEmpty())
	{
		m_rcIconNormal = GetAbsoluteRect();
		false == m_rcIconChecked.IsEmpty() ? m_rcIconNormal.left = m_rcIconChecked.right : 0;
		m_rcIconNormal.left += GetIconNormalLeftPadding();
		m_rcIconNormal.right = m_rcIconNormal.left + rcSourceNormal.GetWidth();
		m_rcIconNormal.top += (m_rcIconNormal.GetHeight() - rcSourceNormal.GetHeight()) / 2;
		m_rcIconNormal.bottom = m_rcIconNormal.top + rcSourceNormal.GetHeight();
	}
	if (false == rcSourceExpand.IsEmpty())
	{
		m_rcIconExpand = GetAbsoluteRect();
		m_rcIconExpand.right -= GetIconExpandRightPadding();
		m_rcIconExpand.left = m_rcIconExpand.right - rcSourceExpand.GetWidth();
		m_rcIconExpand.top += (m_rcIconExpand.GetHeight() - rcSourceExpand.GetHeight()) / 2;
		m_rcIconExpand.bottom = m_rcIconExpand.top + rcSourceExpand.GetHeight();
	}

	m_rcItemText = GetAbsoluteRect();
	if (false == m_rcIconExpand.IsEmpty())
	{
		m_rcItemText.right = m_rcIconExpand.left;
	}
	if (false == m_rcIconNormal.IsEmpty())
	{
		m_rcItemText.left = m_rcIconNormal.right;
	}
	else if (false == m_rcIconChecked.IsEmpty())
	{
		m_rcItemText.left = m_rcIconChecked.right;
	}

	CDUIRect rcTextPadding = m_pOwner->GetItemTextPadding();
	m_rcItemText.left += rcTextPadding.left;
	m_rcItemText.right -= rcTextPadding.right;
	m_rcItemText.top += rcTextPadding.top;
	m_rcItemText.bottom -= rcTextPadding.bottom;

	rcTextPadding = GetTextPadding();
	m_rcItemText.left += rcTextPadding.left;
	m_rcItemText.right -= rcTextPadding.right;
	m_rcItemText.top += rcTextPadding.top;
	m_rcItemText.bottom -= rcTextPadding.bottom;
	m_rcItemText.CheckRect();

	return;
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIMenuCtrl)
MMImplement_ClassName(CDUIMenuCtrl)

CDUIMenuCtrl::CDUIMenuCtrl(void)
{
}

CDUIMenuCtrl::~CDUIMenuCtrl(void)
{
}

bool CDUIMenuCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (pAttributeObj == &m_AttributeColorItemStatusNormal)
	{
		SetItemStatusColorResSwitchSelNormal(GetItemStatusColorResSwitchNormal());
	}
	else if (pAttributeObj == &m_AttributeColorItemStatusHot)
	{
		SetItemStatusColorResSwitchSelHot(GetItemStatusColorResSwitchHot());
	}
	else if (pAttributeObj == &m_AttributeImageItemStatusNormal)
	{
		SetItemStatusImageSectionSelNormal(GetItemStatusImageSectionNormal());
	}
	else if (pAttributeObj == &m_AttributeImageItemStatusHot)
	{
		SetItemStatusImageSectionSelHot(GetItemStatusImageSectionHot());
	}
	else if (pAttributeObj == &m_AttributeItemTextStyleNormal)
	{
		SetItemTextStyleSelNormal(GetItemTextStyleNormal());
	}
	else if (pAttributeObj == &m_AttributeItemTextStyleHot)
	{
		SetItemTextStyleSelHot(GetItemTextStyleHot());
	}

	if (__super::OnAttributeChange(pAttributeObj)) return true;

	return false;
}

LPVOID CDUIMenuCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIMenuCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIMenuCtrl::GetDescribe() const
{
	return Dui_Ctrl_Menu;
}

int CDUIMenuCtrl::GetMenuItemCount() const
{
	return GetChildCount();
}

bool CDUIMenuCtrl::InsertMenuItem(CDUIMenuItemCtrl *pMenuItem, int nPos)
{
	return InsertChild(pMenuItem, nPos);
}

CDUIMenuItemCtrl * CDUIMenuCtrl::GetMenuItem(int nIndex) const
{
	return GetChildAt(nIndex);
}

void CDUIMenuCtrl::CheckMenu(int nIndex)
{
	CDUIMenuItemCtrl *pMenuItem = GetChildAt(nIndex);
	if (NULL == pMenuItem) return;
	if (false == pMenuItem->CheckMenu()) return;
	if (false == pMenuItem->IsChecked() || pMenuItem->GetMenuGroupID().IsEmpty()) return;

	for (int i = 0; i < GetChildCount(); i++)
	{
		if (i == nIndex) continue;

		CDUIMenuItemCtrl *pItem = GetChildAt(i);
		if (NULL == pItem || 0 != lstrcmp(pItem->GetMenuGroupID(), pMenuItem->GetMenuGroupID())) continue;

		pItem->CheckMenu(false);
	}

	return;
}

void CDUIMenuCtrl::RefreshView()
{
	__super::RefreshView();

	if (NULL == m_pWndManager || false == IsWindow(m_pWndManager->GetWndHandle())) return;

	PostMessage(m_pWndManager->GetWndHandle(), WM_DUIRESIZEMENU, NULL, NULL);

	return;
}

bool CDUIMenuCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (NULL == pChild || NULL == MMInterfaceHelper(CDUIMenuItemCtrl, pChild)) return false;

	return __super::InsertChild(pChild, nPos);
}

CDUIMenuItemCtrl * CDUIMenuCtrl::GetChildAt(int nIndex) const
{
	CDUIListItemCtrl *pItem = __super::GetChildAt(nIndex);
	if (NULL == pItem) return NULL;

	return MMInterfaceHelper(CDUIMenuItemCtrl, MMStaticPtr(CDUIControlBase, pItem));
}

void CDUIMenuCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);
	DuiInitAttriVisible(m_AttributeAnimateDrag, false);
	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeChildPaddingH, false);
	DuiInitAttriVisible(m_AttributeUseListHeader, false);
	DuiInitAttriVisible(m_AttributeListViewType, false);
	DuiInitAttriVisible(m_AttributeMultiSel, false);
	DuiInitAttriVisible(m_AttributeScrollSelect, false);
	DuiInitAttriVisible(m_AttributeItemModel, false);
	DuiInitAttriVisible(m_AttributeGroupTileType, false);
	DuiInitAttriVisible(m_AttributeGroupLine, false);
	DuiInitAttriVisible(m_AttributeColorItemStatusSelNormal, false);
	DuiInitAttriVisible(m_AttributeColorItemStatusSelHot, false);
	DuiInitAttriVisible(m_AttributeImageItemStatusSelNormal, false);
	DuiInitAttriVisible(m_AttributeImageItemStatusSelHot, false);
	DuiInitAttriVisible(m_AttributeItemTextStyleSelNormal, false);
	DuiInitAttriVisible(m_AttributeItemTextStyleSelHot, false);
	DuiInitAttriVisible(m_AttributeImageMouseDragSel, false);
	DuiInitAttriVisible(m_AttributeSelectIconVisible, false);
	DuiInitAttriVisible(m_AttributeSelectDbClick, false);
	DuiInitAttriVisible(m_AttributeSelectIconLeftPadding, false);
	DuiInitAttriVisible(m_AttributeSelectIconFixedWidth, false);
	DuiInitAttriVisible(m_AttributeSelectIconFixedHeight, false);
	DuiInitAttriVisible(m_AttributeImageSelectIconNormal, false);
	DuiInitAttriVisible(m_AttributeImageSelectIconHot, false);
	DuiInitAttriVisible(m_AttributeImageSelectIconPushed, false);
	DuiInitAttriVisible(m_AttributeImageSelectIconSelNormal, false);
	DuiInitAttriVisible(m_AttributeImageSelectIconSelHot, false);
	DuiInitAttriVisible(m_AttributeImageSelectIconSelPushed, false);
	DuiModifyAttriDescribe(m_AttributeItemTextPadding, _T("Item Text Padding"));
	DuiModifyAttriDescribe(m_AttributeItemTextStyleNormal, _T("Item Text Style"));
	DuiModifyAttriDescribe(m_AttributeItemTextStyleHot, _T("Item Text Style"));
	DuiModifyAttriDescribe(m_AttributeItemTextStyleSelNormal, _T("Item Text Style"));
	DuiModifyAttriDescribe(m_AttributeItemTextStyleSelHot, _T("Item Text Style"));
	DuiModifyAttriDescribe(m_AttributeItemTextStyleDisabled, _T("Item Text Style"));

	return;
}

CDUIListItemCtrl * CDUIMenuCtrl::ProductItemDefault()
{
	CDUIMenuItemCtrl *pItem = new CDUIMenuItemCtrl();
	pItem->Init();

	return pItem;
}

//////////////////////////////////////////////////////////////////////////
CDUIMenu::CDUIMenu()
{
}

CDUIMenu::~CDUIMenu()
{
	DestroyMenu();

	return;
}

void CDUIMenu::LoadMenu(LPCTSTR lpszMenu)
{
	DestroyMenu();

	g_pDuiMenuWndRoot = new CDUIMenuWnd(NULL, lpszMenu);

	return;
}

void CDUIMenu::DestroyMenu()
{
	MMSafeDelete(g_pDuiMenuWndRoot);

	return;
}

CDUIMenuCtrl * CDUIMenu::GetRootMenu()
{
	if (NULL == g_pDuiMenuWndRoot) return NULL;

	return g_pDuiMenuWndRoot->GetMenuView();
}

tagDuiMenuCmd CDUIMenu::TrackPopupMenu(HWND hWndParent, CDUIPoint pt)
{
	g_DuiMenuCmd = {};

	if (NULL == g_pDuiMenuWndRoot) return g_DuiMenuCmd;

	g_pDuiMenuWndRoot->Init(hWndParent, pt);
	g_pDuiMenuWndRoot->DoBlock();
	DestroyMenu();

	return g_DuiMenuCmd;
}
//////////////////////////////////////////////////////////////////////////