#include "StdAfx.h"
#include "DUIComboxCtrl.h"

#define TIME_WAVE_ID					(1989)
#define TIME_WAVE_ELAPSE				(15)

#define COUNT_WAVEOFFSET				(20)
//////////////////////////////////////////////////////////////////////////
class CDUIComboxWnd : public CDUIWnd
{
	MMDeclare_ClassName()
	DuiDeclare_Message_Map()

public:
	CDUIComboxWnd(CDUIComboxCtrl *pOwner);
	~CDUIComboxWnd();

protected:
	CDUIComboxCtrl *					m_pOwner = NULL;
	CDUIWnd *							m_pWndOwner = NULL;
	CDUIListViewCtrl *					m_pComboxView = NULL;

	int									m_nItemHeightNormal = 0;
	int									m_nWaveOffset = 1;
	int									m_nItemToWave = -1;
	std::vector<int>					m_vecItemToNormal;
	std::vector<int>					m_vecItemToWave;

public:
	void Init(CDUIListViewCtrl *pComboxView);
	void UnInit();
	CMMString GetDuiName() const override;

protected:
	void OnFinalMessage() override;
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam) override;
	void OnDuiWndInited(const DuiNotify &Notify);
	void OnDuiItemClick(const DuiNotify &Notify);
	void OnDuiItemMouseEnter(const DuiNotify &Notify);
	void OnDuiItemMouseLeave(const DuiNotify &Notify);
	void OnDuiTimer(const DuiNotify &Notify);
	void OnNotify(const DuiNotify &Notify);
};

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIComboxWnd)

DuiBegin_Message_Map(CDUIComboxWnd, CDUIWnd)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
	Dui_On_Notify(DuiNotify_ItemClick, OnDuiItemClick)
	Dui_On_Notify(DuiNotify_ItemMouseEnter, OnDuiItemMouseEnter)
	Dui_On_Notify(DuiNotify_ItemMouseLeave, OnDuiItemMouseLeave)
	Dui_On_Timer()
DuiEnd_Message_Map()

CDUIComboxWnd::CDUIComboxWnd(CDUIComboxCtrl *pOwner)
	: m_pOwner(pOwner)
{
}

CDUIComboxWnd::~CDUIComboxWnd()
{
	UnInit();

	m_pOwner = NULL;
	m_pWndOwner = NULL;
	m_pComboxView = NULL;

	return;
}

void CDUIComboxWnd::Init(CDUIListViewCtrl *pComboxView)
{
	if (NULL == m_pOwner || NULL == pComboxView || IsWindow(GetWndHandle())) return;

	m_pWndOwner = static_cast<CDUIWnd*>(m_pOwner->GetWndOwner());

	if (NULL == m_pWndOwner)
	{
		assert(false);
		return;
	}

	m_pComboxView = pComboxView;

	//create
	Create(m_pWndOwner->GetWndHandle(), GetClass(), WS_POPUP, WS_EX_TOOLWINDOW);

	//comboxview
	AttachRootCtrl(m_pComboxView);
	SetCaptionHeight(0);
	SetWndInitSize(0, 0);
	SetWndMinSize(0, 0);
	SetWndMaxSize(0, 0);
	m_pOwner->Select(true);

	//dropsize
	CDUIRect rcWnd;
	::GetWindowRect(m_pWndOwner->GetWndHandle(), &rcWnd);
	m_pComboxView->SetPadding(0, 0, 0, 0);
	m_pComboxView->RefreshView();
	CDUISize szTotalRange = m_pComboxView->GetTotalRange();
	CDUISize szDrop = m_pOwner->GetComboxSize();
	CDUIRect rcOwner = m_pOwner->GetAbsoluteRect();
	CDUIRect rcCombox = rcOwner;
	rcCombox.Offset(rcWnd.left, rcWnd.top);

	rcCombox.top = rcCombox.bottom;
	rcCombox.bottom = rcCombox.top + min(szTotalRange.cy, szDrop.cy > 0 ? szDrop.cy : szTotalRange.cy);
	szDrop.cx > 0 ? rcCombox.right = rcCombox.left + szDrop.cx : 0;

	//monitor
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;
	if (rcCombox.bottom > rcWork.bottom)
	{
		rcCombox.Offset(0, -(rcOwner.GetHeight() + rcCombox.GetHeight()));
	}
	if (rcCombox.top < rcWork.top)
	{
		rcCombox.Offset(0, rcOwner.bottom + rcWnd.top - rcCombox.top);
	}

	SetWindowPos(m_hWnd, NULL, rcCombox.left, rcCombox.top, rcCombox.GetWidth(), rcCombox.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);

	// HACK: Don't deselect the parent's caption
	HWND hWndParent = m_hWnd;
	while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);

	return;
}

void CDUIComboxWnd::UnInit()
{
	Close();

	if (m_pOwner)
	{
		m_pOwner->Select(false);
		m_pOwner->Invalidate();
	}
	if (m_pComboxView)
	{
		if (m_nItemHeightNormal > 0)
		{
			for (int i = 0; i < m_pComboxView->GetChildCount(); i++)
			{
				CDUIListItemCtrl *pItem = m_pComboxView->GetChildAt(i);
				if (NULL == pItem) continue;

				pItem->SetFixedHeight(m_nItemHeightNormal);
			}
		}
	}

	DetachRootCtrl();

	return;
}

CMMString CDUIComboxWnd::GetDuiName() const
{
	return _T("");
}

void CDUIComboxWnd::OnFinalMessage()
{
	DetachRootCtrl();

	__super::OnFinalMessage();

	return;
}

LRESULT CDUIComboxWnd::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = __super::OnKillFocus(wParam, lParam);

	UnInit();

	return lRes;
}

void CDUIComboxWnd::OnDuiWndInited(const DuiNotify &Notify)
{
	if (NULL == m_pComboxView || NULL == m_pOwner) return;

	if (m_pComboxView)
	{
		m_pComboxView->EnsureVisible(m_pComboxView->GetCurSel());
	}

	return;
}

void CDUIComboxWnd::OnDuiItemClick(const DuiNotify &Notify)
{
	if (NULL == m_pComboxView
		|| NULL == m_pOwner
		|| m_pComboxView != Notify.pNotifyCtrl) return;

	CDUIListItemCtrl *pItem = m_pComboxView->GetChildAt(Notify.DuiNotifyExtend.ListView.nIndexItem);
	if (NULL == pItem) return;

	if (false == pItem->GetText().empty())
	{
		m_pOwner->SetText(pItem->GetText());
	}

	UnInit();

	return;
}

void CDUIComboxWnd::OnDuiItemMouseEnter(const DuiNotify &Notify)
{
	if (NULL == m_pOwner || m_pOwner->GetWaveHeight() <= 0) return;
	if (NULL == m_pComboxView || m_pComboxView != Notify.pNotifyCtrl) return;

	DuiNotify::tagDuiNotifyExtend NotifyExtend = Notify.DuiNotifyExtend;
	int nIndex = NotifyExtend.ListView.nIndexItem;

	if (m_nItemToWave == nIndex) return;

	//init
	if (-1 == m_nItemToWave)
	{
		CDUIListItemCtrl *pItem = m_pComboxView->GetChildAt(nIndex);
		if (NULL == pItem) return;

		m_nItemHeightNormal = pItem->GetFixedHeight();
		m_nWaveOffset = max(1, (m_pOwner->GetWaveHeight() - pItem->GetFixedHeight()) / COUNT_WAVEOFFSET);
	}

	//to normal
	for (int i = 0; i < m_vecItemToWave.size(); i++)
	{
		m_vecItemToNormal.push_back(m_vecItemToWave[i]);
	}

	//to wave
	m_vecItemToWave.clear();
	m_nItemToWave = nIndex;
	int nIndexPre = m_pComboxView->FindNextIndex(m_nItemToWave, false);
	int nIndexNext = m_pComboxView->FindNextIndex(m_nItemToWave, true);
	-1 != m_nItemToWave ? m_vecItemToWave.push_back(m_nItemToWave) : 0;
	-1 != nIndexPre ? m_vecItemToWave.push_back(nIndexPre) : 0;
	-1 != nIndexNext ? m_vecItemToWave.push_back(nIndexNext) : 0;
	m_vecItemToNormal.erase(std::remove(m_vecItemToNormal.begin(), m_vecItemToNormal.end(), m_nItemToWave), m_vecItemToNormal.end());
	m_vecItemToNormal.erase(std::remove(m_vecItemToNormal.begin(), m_vecItemToNormal.end(), nIndexPre), m_vecItemToNormal.end());
	m_vecItemToNormal.erase(std::remove(m_vecItemToNormal.begin(), m_vecItemToNormal.end(), nIndexNext), m_vecItemToNormal.end());

	m_pComboxView->SetTimer(TIME_WAVE_ID, TIME_WAVE_ELAPSE);

	return;
}

void CDUIComboxWnd::OnDuiItemMouseLeave(const DuiNotify &Notify)
{
	if (NULL == m_pOwner || m_pOwner->GetWaveHeight() <= 0) return;

	return;
}

void CDUIComboxWnd::OnDuiTimer(const DuiNotify &Notify)
{
	if (NULL == m_pOwner || m_pOwner->GetWaveHeight() <= 0) return;
	if (NULL == m_pComboxView || m_pComboxView != Notify.pNotifyCtrl) return;
	if (-1 == m_nItemToWave) return;

	//to normal
	for (int i = m_vecItemToNormal.size() - 1; i >= 0; i--)
	{
		CDUIListItemCtrl *pItem = m_pComboxView->GetChildAt(m_vecItemToNormal[i]);
		if (NULL == pItem) continue;

		int nFixedHeight = pItem->GetFixedHeight() - m_nWaveOffset;
		nFixedHeight = m_nWaveOffset > 0 ? max(m_nItemHeightNormal, nFixedHeight) : min(m_nItemHeightNormal, nFixedHeight);
		pItem->SetFixedHeight(nFixedHeight);

		if (nFixedHeight == m_nItemHeightNormal)
		{
			m_vecItemToNormal.erase(m_vecItemToNormal.begin() + i);
		}
	}

	//to wave
	int nWaveCount = 0;
	int nHeightWave = m_pOwner->GetWaveHeight();
	int nHeightWaveUpDown = m_nItemHeightNormal + (nHeightWave - m_nItemHeightNormal) / 2;
	for (int i = 0; i < m_vecItemToWave.size(); i++)
	{
		CDUIListItemCtrl *pItem = m_pComboxView->GetChildAt(m_vecItemToWave[i]);
		if (NULL == pItem) continue;

		int nFixedHeight = pItem->GetFixedHeight() + m_nWaveOffset;
		int nHeightLimit = (int)m_vecItemToWave[i] == m_nItemToWave ? nHeightWave : nHeightWaveUpDown;
		nFixedHeight = m_nWaveOffset > 0 ? min(nHeightLimit, nFixedHeight) : max(nHeightLimit, nFixedHeight);
		pItem->SetFixedHeight(nFixedHeight);

		if (nFixedHeight == nHeightLimit)
		{
			nWaveCount++;
		}
	}

	if (m_vecItemToNormal.empty() && nWaveCount == m_vecItemToWave.size())
	{
		m_pComboxView->KillTimer(TIME_WAVE_ID);
	}

	return;
}

void CDUIComboxWnd::OnNotify(const DuiNotify &Notify)
{
	__super::OnNotify(Notify);

	if (m_pOwner
		&& m_pWndOwner
		&& (Notify.pNotifyCtrl == m_pComboxView || m_pComboxView->VerifyChild(Notify.pNotifyCtrl)))
	{
		DuiNotify NotifyOwner = Notify;
		NotifyOwner.DuiNotifyExtend.Type = tagDuiNotify::DuiNotifyExtend_Combox;
		NotifyOwner.DuiNotifyExtend.Combox.pComboxCtrl = m_pOwner;
		NotifyOwner.DuiNotifyExtend.Combox.nIndexItem = Notify.DuiNotifyExtend.ListView.nIndexItem;
		NotifyOwner.pNotifyCtrl = Notify.pNotifyCtrl == m_pComboxView ? m_pOwner : NotifyOwner.pNotifyCtrl;
		m_pWndOwner->SendNotify(NotifyOwner);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIComboxCtrl)
MMImplement_ClassName(CDUIComboxCtrl)

CDUIComboxCtrl::CDUIComboxCtrl(void)
{
	m_pComboxWindow = new CDUIComboxWnd(this);
}

CDUIComboxCtrl::~CDUIComboxCtrl(void)
{
	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetBindCtrl(NULL);
	}

	MMSafeDelete(m_pComboxWindow);
	MMSafeDelete(m_pShowListView);
	MMSafeDelete(m_pEditCtrl);

	return;
}

bool CDUIComboxCtrl::RegisterControlListen(IDuiInterface *pIControlListen)
{
	if (NULL == pIControlListen || false == __super::RegisterControlListen(pIControlListen)) return false;

	if (m_pEditCtrl)
	{
		m_pEditCtrl->RegisterControlListen(pIControlListen);
	}

	return true;
}

bool CDUIComboxCtrl::UnRegisterControlListen(IDuiInterface *pIControlListen)
{
	if (NULL == pIControlListen || false == __super::UnRegisterControlListen(pIControlListen)) return false;

	if (m_pEditCtrl)
	{
		m_pEditCtrl->UnRegisterControlListen(pIControlListen);
	}

	return true;
}

bool CDUIComboxCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeEditEnable)
	{
		SetEditEnable(IsEditEnable());

		return true;
	}

	return false;
}

void CDUIComboxCtrl::OnDpiChanged(int nScalePre)
{
	__super::OnDpiChanged(nScalePre);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->OnDpiChanged(nScalePre);
	}

	return;
}

LPVOID CDUIComboxCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIComboxCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIComboxCtrl::GetDescribe() const
{
	return Dui_Ctrl_Combox;
}

CDUIComboxCtrl * CDUIComboxCtrl::Clone(bool bIncludeChild, bool bRefreshCtrlID)
{
	MMInterfaceHelper(CDUIComboxCtrl, __super::Clone(bIncludeChild, bRefreshCtrlID), pComboxClone);
	if (NULL == pComboxClone) return NULL;

	if (m_pShowListView && NULL == pComboxClone->m_pShowListView)
	{
		pComboxClone->m_pShowListView = m_pShowListView->Clone(true, false);
	}

	return pComboxClone;
}

UINT CDUIComboxCtrl::InitCtrlID()
{
	__super::InitCtrlID();

	if (m_pEditCtrl)
	{
		m_pEditCtrl->InitCtrlID();
	}

	return GetCtrlID();
}

void CDUIComboxCtrl::RefreshCtrlID(bool bSelfSingle)
{
	__super::RefreshCtrlID(bSelfSingle);

	if (bSelfSingle) return;

	if (m_pEditCtrl)
	{
		m_pEditCtrl->RefreshCtrlID();
	}

	return;
}

bool CDUIComboxCtrl::SetWndOwner(CDUIWnd *pWndOwner)
{
	if (false == __super::SetWndOwner(pWndOwner)) return false;

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetWndOwner(m_pWndOwner);
	}

	return true;
}

void CDUIComboxCtrl::RefreshView()
{
	__super::RefreshView();

	//edit
	if (m_pEditCtrl && m_pEditCtrl->IsVisible())
	{
		CDUIRect rcTextRange = GetTextRange();
		m_pEditCtrl->OnDuiSize(rcTextRange);
	}

	return;
}

bool CDUIComboxCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	//edit
	if (m_pEditCtrl && m_pEditCtrl->IsVisible())
	{
		m_pEditCtrl->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	return true;
}

void CDUIComboxCtrl::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);

	if (false == IsVisible())
	{
		UnActive();
	}
	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetInternVisible(IsVisible());
	}

	return;
}

bool CDUIComboxCtrl::SetText(LPCTSTR lpszText)
{
	if (false == __super::SetText(lpszText)) return false;

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetText(lpszText);
	}

	return true;
}

void CDUIComboxCtrl::SetTextStyleNormal(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleNormal(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleNormal(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleHot(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleHot(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleHot(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStylePushed(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStylePushed(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStylePushed(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleFocus(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleFocus(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleFocus(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleDisabled(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleDisabled(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleDisabled(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleSelNormal(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleSelNormal(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleNormal(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleSelHot(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleSelHot(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleHot(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleSelPushed(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleSelPushed(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStylePushed(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleSelFocus(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleSelFocus(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleFocus(TextStyle);
	}

	return;
}

void CDUIComboxCtrl::SetTextStyleSelDisabled(const tagDuiTextStyle &TextStyle)
{
	__super::SetTextStyleSelDisabled(TextStyle);

	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetTextStyleDisabled(TextStyle);
	}

	return;
}

bool CDUIComboxCtrl::IsEditEnable()
{
	return m_AttributeEditEnable.GetValue();
}

void CDUIComboxCtrl::SetEditEnable(bool bEnable)
{
#ifndef DUI_DESIGN
	if (bEnable == IsEditEnable()) return;
#endif

	m_AttributeEditEnable.SetValue(bEnable);

	//edit
	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetVisible(IsEditEnable());

		if (IsEditEnable() && false == m_AttributeTextPadding.IsModified())
		{
			tagDuiImageSection ImageSection = GetImageSectionNormal();
			SetTextPadding(ImageSection.rcCorner);
		}
	}

	return;
}

CMMString CDUIComboxCtrl::GetComboxView()
{
	return m_AttributeComboxView.GetCurSelDescribe();
}

void CDUIComboxCtrl::SetComboxView(CMMString strComboxView)
{
	if (strComboxView.empty() || strComboxView == GetComboxView()) return;

	m_AttributeComboxView.SelectItem(strComboxView);

	return;
}

int CDUIComboxCtrl::GetWaveHeight()
{
	return m_AttributeWaveHeight.GetValue();
}

void CDUIComboxCtrl::SetWaveHeight(int nHeight)
{
	if (nHeight < 0 || nHeight == GetWaveHeight()) return;

	m_AttributeWaveHeight.SetValue(nHeight);

	return;
}

SIZE CDUIComboxCtrl::GetComboxSize()
{
	return m_AttributeComboxSize.GetValue();
}

void CDUIComboxCtrl::SetComboxSize(SIZE szDropBox)
{
	if (szDropBox == GetComboxSize()) return;

	m_AttributeComboxSize.SetValue(szDropBox);

	return;
}

int CDUIComboxCtrl::GetCurSel() const
{
	return m_pShowListView ? m_pShowListView->GetCurSel() : -1;
}

bool CDUIComboxCtrl::SelectItem(int nIndex)
{
	if (NULL == m_pShowListView) return false;

	m_pShowListView->SelectItem(nIndex);
	
	CDUIListItemCtrl *pItem = m_pShowListView->GetChildAt(nIndex);
	SetText(pItem ? pItem->GetText() : _T(""));

	return true;
}

int CDUIComboxCtrl::GetChildCount() const
{
	return m_pShowListView ? m_pShowListView->GetChildCount() : 0;
}

bool CDUIComboxCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	return m_pShowListView ? m_pShowListView->InsertChild(pChild, nPos) : false;
}

bool CDUIComboxCtrl::InsertChild(CMMString strText, int nPos)
{
	CDUIListItemCtrl *pItem = new CDUIListItemCtrl();
	pItem->Init();
	pItem->SetText(strText);

	return InsertChild(pItem, nPos);
}

CDUIControlBase * CDUIComboxCtrl::GetChildAt(int nIndex)
{
	return m_pShowListView ? m_pShowListView->GetChildAt(nIndex) : NULL;
}

bool CDUIComboxCtrl::Remove(CDUIControlBase *pControl)
{
	return m_pShowListView ? m_pShowListView->Remove(pControl) : false;
}

bool CDUIComboxCtrl::RemoveAt(int nIndex)
{
	return m_pShowListView ? m_pShowListView->RemoveAt(nIndex) : false;
}

void CDUIComboxCtrl::RemoveAll()
{
	return m_pShowListView ? m_pShowListView->RemoveAll() : NULL;
}

bool CDUIComboxCtrl::Active()
{
	if (NULL == m_pWndOwner) return false;
	if (NULL == m_pComboxWindow || IsWindow(m_pComboxWindow->GetWndHandle())) return false;
	if (false == CDUICheckBoxCtrl::Active()) return false;

	if (NULL == m_pShowListView)
	{
		m_pShowListView = dynamic_cast<CDUIListViewCtrl*>(CDUIGlobal::GetInstance()->LoadDui(GetComboxView(), NULL));

		if (NULL == m_pShowListView)
		{
			MessageBox(m_pWndOwner->GetWndHandle(), _T("please select bind UI of ComboxView£¬it must be listviewctrl"), NULL, NULL);
			return false;
		}
	}

	//combox
	m_pComboxWindow->Init(m_pShowListView);

	DuiNotify Notify = {};
	Notify.NotifyType = DuiNotify_ComboxExpand;
	Notify.DuiNotifyExtend.Type = tagDuiNotify::DuiNotifyExtend_Combox;
	Notify.DuiNotifyExtend.Combox.pComboxCtrl = this;
	Notify.pNotifyCtrl = this;
	m_pWndOwner->SendNotify(Notify);

	return true;
}

bool CDUIComboxCtrl::UnActive()
{
	if (NULL == m_pComboxWindow || false == IsWindow(m_pComboxWindow->GetWndHandle())) return false;

	m_pComboxWindow->UnInit();

	return true;
}

bool CDUIComboxCtrl::IsActive() const
{
	return m_pComboxWindow && IsWindow(m_pComboxWindow->GetWndHandle());
}

void CDUIComboxCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupCombox, _T("Combox"));
	DuiCreateAttribute(m_AttributeEditEnable, _T("EditEnable"), _T(""), m_AttributeGroupCombox);
	DuiCreateAttribute(m_AttributeComboxView, _T("ComboxView"), _T(""), m_AttributeGroupCombox);
	DuiCreateAttribute(m_AttributeWaveHeight, _T("WaveHeight"), _T("Item Height Change As Wave"), m_AttributeGroupCombox);
	DuiCreateAttribute(m_AttributeComboxSize, _T("ComboxSize"), _T(""), m_AttributeGroupCombox);

	return;
}

void CDUIComboxCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupSelect, false);

	//header
	if (NULL == m_pEditCtrl)
	{
		m_pEditCtrl = new CDUIThinkEditCtrl();
		m_pEditCtrl->Init();
		m_pEditCtrl->SetBindCtrl(this);
		m_pEditCtrl->SetVisible(IsEditEnable());

		if (false == m_AttributeTextStyle.IsEmpty())
		{
			m_pEditCtrl->SetTextStyle(m_AttributeTextStyle.GetTextStyle());
		}
		if (false == m_AttributeTextStyleNormal.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleNormal(m_AttributeTextStyleNormal.GetTextStyle());
		}
		if (false == m_AttributeTextStyleHot.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleHot(m_AttributeTextStyleHot.GetTextStyle());
		}
		if (false == m_AttributeTextStylePushed.IsEmpty())
		{
			m_pEditCtrl->SetTextStylePushed(m_AttributeTextStylePushed.GetTextStyle());
		}
		if (false == m_AttributeTextStyleFocus.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleFocus(m_AttributeTextStyleFocus.GetTextStyle());
		}
		if (false == m_AttributeTextStyleDisabled.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleDisabled(m_AttributeTextStyleDisabled.GetTextStyle());
		}
		if (false == m_AttributeTextStyleSelNormal.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleNormal(m_AttributeTextStyleSelNormal.GetTextStyle());
		}
		if (false == m_AttributeTextStyleSelHot.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleHot(m_AttributeTextStyleSelHot.GetTextStyle());
		}
		if (false == m_AttributeTextStyleSelPushed.IsEmpty())
		{
			m_pEditCtrl->SetTextStylePushed(m_AttributeTextStyleSelPushed.GetTextStyle());
		}
		if (false == m_AttributeTextStyleSelFocus.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleFocus(m_AttributeTextStyleSelFocus.GetTextStyle());
		}
		if (false == m_AttributeTextStyleSelDisabled.IsEmpty())
		{
			m_pEditCtrl->SetTextStyleDisabled(m_AttributeTextStyleSelDisabled.GetTextStyle());
		}
	}

	return;
}

void CDUIComboxCtrl::PaintText(HDC hDC)
{
	if (m_pEditCtrl)
	{
		if (m_pEditCtrl->IsVisible()) return;

		SetText(m_pEditCtrl->GetText());
	}

	return __super::PaintText(hDC);
}

bool CDUIComboxCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDown(pt, Msg)) return false;

	Active();

	return true;
}

bool CDUIComboxCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == CDUIButtonCtrl::OnDuiLButtonUp(pt, Msg)) return false;

	return true;
}

LRESULT CDUIComboxCtrl::OnDuiKeyDown(const DuiMessage &Msg)
{
	switch (Msg.chKey)
	{
		case VK_F4:
		{
			Active();

			return 0;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

void CDUIComboxCtrl::SetInternVisible(bool bVisible, bool bTraversal)
{
	__super::SetInternVisible(bVisible, bTraversal);

	if (false == IsVisible())
	{
		UnActive();
	}
	if (m_pEditCtrl)
	{
		m_pEditCtrl->SetInternVisible(IsVisible(), bTraversal);
	}

	return;
}

CDUIControlBase * CDUIComboxCtrl::FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags)
{
#ifndef DUI_DESIGN
	//header
	if (m_pEditCtrl)
	{
		CDUIControlBase *pResult = m_pEditCtrl->FindControl(Proc, pData, uFlags);

		if (pResult && 0 == (uFlags & DuiFind_UpdateTest)) return pResult;
	}
#endif

	return __super::FindControl(Proc, pData, uFlags);
}

void CDUIComboxCtrl::ReapControl()
{
	__super::ReapControl();

	if (m_pEditCtrl)
	{
		m_pEditCtrl->ReapControl();
	}

	return;
}

CDUIAttributeColorSwitch * CDUIComboxCtrl::GetAttributeStatusColor()
{
	if (m_pEditCtrl && m_pEditCtrl->IsVisible())
	{
		if ((m_nControlStatus & ControlStatus_Pushed) || (m_pEditCtrl->m_nControlStatus & ControlStatus_Pushed))
		{
			return &m_AttributeColorPushed;
		}
		else if ((m_nControlStatus & ControlStatus_Hot) || (m_pEditCtrl->m_nControlStatus & ControlStatus_Hot))
		{
			return &m_AttributeColorHot;
		}
		else if (IsFocused() || m_pEditCtrl->IsFocused())
		{
			return &m_AttributeColorFocus;
		}
	}

	return __super::GetAttributeStatusColor();
}

CDUIAttriImageSection * CDUIComboxCtrl::GetAttributeStatusImage()
{
	if (m_pEditCtrl && m_pEditCtrl->IsVisible())
	{
		if ((m_nControlStatus & ControlStatus_Pushed) || (m_pEditCtrl->m_nControlStatus & ControlStatus_Pushed))
		{
			return &m_AttributeImagePushed;
		}
		else if ((m_nControlStatus & ControlStatus_Hot) || (m_pEditCtrl->m_nControlStatus & ControlStatus_Hot))
		{
			return &m_AttributeImageHot;
		}
		else if (IsFocused() || m_pEditCtrl->IsFocused())
		{
			return &m_AttributeImageFocus;
		}
	}

	return __super::GetAttributeStatusImage();
}