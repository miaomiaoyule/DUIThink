#include "StdAfx.h"
#include "DUIHotKeyCtrl.h"

//////////////////////////////////////////////////////////////////////////
class CDUIHotKeyWnd : public CDUIWnd
{
	MMDeclare_ClassName()

public:
	CDUIHotKeyWnd(CDUIHotKeyCtrl *pOwner);
	~CDUIHotKeyWnd();

	//variant
protected:
	CDUIHotKeyCtrl *					m_pOwner = NULL;
	CDUIWndManager *					m_pWndManagerOwner = NULL;

public:
	void Init();
	void UnInit();
	void AdjustWndSize(bool bCreate);

	LPCTSTR GetSuperClassName() const override;

protected:
	void OnFinalMessage() override;
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

public:
	void GetHotKey(BYTE &cbVKeyCode, BYTE &cbModifierFlag) const;
	WORD GetHotKey() const;
	void SetHotKey(BYTE cbVKeyCode, BYTE cbModifierFlag);
	void SetHotKey(WORD wHotKey);
};

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIHotKeyWnd)

CDUIHotKeyWnd::CDUIHotKeyWnd(CDUIHotKeyCtrl *pOwner)
	: m_pOwner(pOwner)
{

}

CDUIHotKeyWnd::~CDUIHotKeyWnd()
{
	m_pOwner = NULL;

	UnInit();

	return;
}

void CDUIHotKeyWnd::Init()
{
	if (NULL == m_pOwner || IsWindow(GetHWND())) return;

	m_pWndManagerOwner = static_cast<CDUIWndManager*>(m_pOwner->GetWndManager());

	if (NULL == m_pWndManagerOwner)
	{
		assert(false);
		return;
	}

	//wndstyle
	UINT uWndStyle = 0;
	CDUIRect rcBorderLine = m_pOwner->GetBorderLine();
	CDUIAttributeTextStyle *pAttribute = m_pOwner->GetAttributeTextStyleActive();
	tagDuiTextStyle TextStyle = pAttribute ? pAttribute->GetTextStyle() : tagDuiTextStyle();

	uWndStyle = DUI_WNDSTYLE_CHILD | ES_AUTOHSCROLL;
	(TextStyle.dwTextStyle & DT_LEFT) ? uWndStyle |= ES_LEFT : NULL;
	(TextStyle.dwTextStyle & DT_CENTER) ? uWndStyle |= ES_CENTER : NULL;
	(TextStyle.dwTextStyle & DT_RIGHT) ? uWndStyle |= ES_RIGHT : NULL;

	Create(m_pWndManagerOwner->GetWndHandle(), _T(""), uWndStyle, WS_EX_WINDOWEDGE | WS_EX_TRANSPARENT);

	if (false == IsWindow(m_hWnd))
	{
		assert(false);
		return;
	}

	AdjustWndSize(true);

	//text
	HFONT hFont = pAttribute ? pAttribute->GetFont() : NULL;
	hFont ? SetWindowFont(m_hWnd, hFont, TRUE) : NULL;
	Edit_SetReadOnly(m_hWnd, true);
	SetHotKey(m_pOwner->GetHotKey());
	SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
	int nSize = GetWindowTextLength(m_hWnd);
	if (m_pOwner->IsAutoSelAll())
	{
		if (nSize == 0) nSize = 1;
		Edit_SetSel(m_hWnd, 0, nSize);
	}
	else
	{
		Edit_SetSel(m_hWnd, nSize, nSize);
	}

	//show
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);

	return;
}

void CDUIHotKeyWnd::UnInit()
{
	Close();

	return;
}

void CDUIHotKeyWnd::AdjustWndSize(bool bCreate)
{
	if (NULL == m_pOwner || NULL == m_pWndManagerOwner) return;

	CDUIRect rcPos = m_pOwner->GetTextRange();

	//parent
	CDUIControlBase *pParent = m_pOwner;
	RECT rcParent;
	while (pParent = pParent->GetParent())
	{
		rcParent = pParent->GetAbsoluteRect();
		::IntersectRect(&rcPos, &rcPos, &rcParent);
	}

	//popup edit
	if (GetWindowLong(m_hWnd, GWL_STYLE) & WS_POPUP)
	{
		CDUIRect rcWnd;
		::GetWindowRect(m_pWndManagerOwner->GetWndHandle(), &rcWnd);
		rcPos.Offset(rcWnd.left, rcWnd.top);
	}

	SetWindowPos(m_hWnd, NULL, rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE);

	CDUIRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	::InvalidateRect(m_hWnd, &rcClient, TRUE);

	return;
}

LPCTSTR CDUIHotKeyWnd::GetSuperClassName() const
{
	return HOTKEY_CLASS;
}

void CDUIHotKeyWnd::OnFinalMessage()
{
	__super::OnFinalMessage();

	if (m_pOwner && m_pWndManagerOwner)
	{
		if (m_pWndManagerOwner->GetFocusControl() == m_pOwner)
		{
			m_pWndManagerOwner->SetFocusControl(NULL);
		}

		m_pOwner->Invalidate();
	}

	return;
}

LRESULT CDUIHotKeyWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	UnInit();

	return 0;
}

LRESULT CDUIHotKeyWnd::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = false;
	switch (uMsg)
	{
		case WM_KILLFOCUS:
		{
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);

			break;
		}
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			bHandled = true;

			lRes = __super::OnOldWndProc(uMsg, wParam, lParam);

			if (VK_RETURN == TCHAR(wParam))
			{
				m_pWndManagerOwner->SendNotify(m_pOwner, DuiNotify_Return);
			}

			//text
			m_pOwner->SetHotKey(GetHotKey());

			break;
		}
		case WM_ERASEBKGND:
		{
			bHandled = true;

			break;
		}
		case WM_PAINT:
		{
			bHandled = true;

			::HideCaret(m_hWnd);

			PAINTSTRUCT ps = {};
			HDC hDC = ::BeginPaint(m_hWnd, &ps);
			Gdiplus::Graphics Gp(hDC);

			CDUIRect rcWnd;
			::GetWindowRect(m_hWnd, &rcWnd);
			MMScreenToClient(rcWnd, m_pWndManagerOwner->GetWndHandle());
			do
			{
				if (rcWnd.IsEmpty()) break;

				HBITMAP hBmpBk = CDUIRenderEngine::GenerateBitmap(m_pWndManagerOwner, m_pWndManagerOwner->GetRootCtrl(), rcWnd);
				if (NULL == hBmpBk) break;

				Bitmap *pBmp = CDUIRenderEngine::GetAlphaBitmap(hBmpBk);
				if (NULL == pBmp) break;

				CDUIRect rcClient;
				GetClientRect(m_hWnd, &rcClient);
				Gp.DrawImage(pBmp, rcClient.left, rcClient.top, rcClient.GetWidth(), rcClient.GetHeight());

				MMSafeDelete(pBmp);
				MMSafeDeleteObject(hBmpBk);

			} while (false);

			::EndPaint(m_hWnd, &ps);

			break;
		}
	}

	if (bHandled) return lRes;
	return __super::OnOldWndProc(uMsg, wParam, lParam);
}

void CDUIHotKeyWnd::GetHotKey(BYTE &cbVKeyCode, BYTE &cbModifierFlag) const
{
	WORD dwHotKey = GetHotKey();
	cbVKeyCode = LOBYTE(dwHotKey);
	cbModifierFlag = HIBYTE(dwHotKey);

	return;
}

WORD CDUIHotKeyWnd::GetHotKey() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (WORD)(::SendMessage(m_hWnd, HKM_GETHOTKEY, 0, 0L));
}

void CDUIHotKeyWnd::SetHotKey(BYTE cbVKeyCode, BYTE cbModifierFlag)
{
	ASSERT(::IsWindow(m_hWnd));
	SetHotKey(MAKEWORD(cbVKeyCode, cbModifierFlag));

	return;
}

void CDUIHotKeyWnd::SetHotKey(WORD wHotKey)
{
	ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, HKM_SETHOTKEY, wHotKey, 0L);

	return;
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIHotKeyCtrl)
MMImplement_ClassName(CDUIHotKeyCtrl)

CDUIHotKeyCtrl::CDUIHotKeyCtrl(void)
{
	m_pHotKeyWindow = new CDUIHotKeyWnd(this);

	return;
}

CDUIHotKeyCtrl::~CDUIHotKeyCtrl(void)
{
	MMSafeDelete(m_pHotKeyWindow);

	if (m_pWndManager)
	{
		m_pWndManager->RemovePreMessagePtr(this);
	}

	return;
}

LRESULT CDUIHotKeyCtrl::OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
	{
		if (false == IsFocused()
			|| NULL == m_pHotKeyWindow
			|| false == IsWindow(*m_pHotKeyWindow)) return 0;

		::SendMessage(*m_pHotKeyWindow, uMsg, wParam, lParam);

		bHandled = true;
	}

	return 0;
}

bool CDUIHotKeyCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeHotKey)
	{
		SetHotKey(GetHotKey());

		return true;
	}

	return false;
}

LPVOID CDUIHotKeyCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIHotKeyCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIHotKeyCtrl::GetDescribe() const
{
	return Dui_Ctrl_HotKey;
}

void CDUIHotKeyCtrl::SetVisible(bool bVisible/* = true*/)
{
	__super::SetVisible(bVisible);

	if (false == IsVisible() && m_pHotKeyWindow)
	{
		m_pHotKeyWindow->UnInit();
	}

	return;
}

void CDUIHotKeyCtrl::SetEnabled(bool bEnable/* = true*/)
{
	CDUIStaticCtrl::SetEnabled(bEnable);

	if (false == IsEnabled() && m_pHotKeyWindow)
	{
		m_pHotKeyWindow->UnInit();
	}

	return;
}

void CDUIHotKeyCtrl::RefreshView()
{
	__super::RefreshView();

	//adjust
	if (m_pHotKeyWindow && m_pHotKeyWindow->GetHWND())
	{
		m_pHotKeyWindow->AdjustWndSize(false);
	}

	return;
}

void CDUIHotKeyCtrl::GetHotKey(BYTE &cbVKeyCode, BYTE &cbModifierFlag)
{
	return m_AttributeHotKey.GetValue(cbVKeyCode, cbModifierFlag);
}

WORD CDUIHotKeyCtrl::GetHotKey()
{
	return m_AttributeHotKey.GetValue();
}

void CDUIHotKeyCtrl::SetHotKey(BYTE cbVKeyCode, BYTE cbModifierFlag)
{
	return SetHotKey(MAKEWORD(cbVKeyCode, cbModifierFlag));
}

void CDUIHotKeyCtrl::SetHotKey(WORD wHotKey)
{
#ifndef DUI_DESIGN
	if (wHotKey == GetHotKey()) return;
#endif

	m_AttributeHotKey.SetValue(wHotKey);

	SetText(GetHotKeyName());

	return;
}

CMMString CDUIHotKeyCtrl::GetKeyName(UINT uVKeyCode, UINT uMapType, bool bDistinguishLeftRight)
{
	UINT uScanCode = ::MapVirtualKeyEx(uVKeyCode, uMapType, ::GetKeyboardLayout(0));

	switch (uVKeyCode)
	{
		// Keys which are "extended" (except for Return which is Numeric Enter as extended)
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_NEXT: // Page down
		case VK_PRIOR: // Page up
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			uScanCode |= 0x100; // Add extended bit
	}

	if (bDistinguishLeftRight)
		uScanCode |= 0x01000000L;

	TCHAR szStr[MAX_PATH] = {};
	::GetKeyNameText(uScanCode << 16, szStr, MAX_PATH);

	return szStr;
}

CMMString CDUIHotKeyCtrl::GetHotKeyName()
{
	CMMString strKeyName;
	const TCHAR szPlus[] = _T(" + ");

	BYTE cbVKeyCode = 0, cbModifierFlag = 0;
	GetHotKey(cbVKeyCode, cbModifierFlag);

	if (cbVKeyCode != 0 || cbModifierFlag != 0)
	{
		if (cbModifierFlag & HOTKEYF_CONTROL)
		{
			strKeyName += GetKeyName(VK_CONTROL);
			strKeyName += szPlus;
		}
		if (cbModifierFlag & HOTKEYF_SHIFT)
		{
			strKeyName += GetKeyName(VK_SHIFT);
			strKeyName += szPlus;
		}
		if (cbModifierFlag & HOTKEYF_ALT)
		{
			strKeyName += GetKeyName(VK_MENU);
			strKeyName += szPlus;
		}

		strKeyName += GetKeyName(cbVKeyCode, MAPVK_VK_TO_VSC, cbModifierFlag & HOTKEYF_EXT);

		return strKeyName;
	}

	return _T("无");
}

bool CDUIHotKeyCtrl::IsAutoSelAll()
{
	return m_AttributeAutoSelAll.GetValue();
}

void CDUIHotKeyCtrl::SetAutoSelAll(bool bAutoSelAll)
{
	if (bAutoSelAll == IsAutoSelAll()) return;

	m_AttributeAutoSelAll.SetValue(bAutoSelAll);

	return;
}

bool CDUIHotKeyCtrl::OnDuiSetFocus()
{
	if (false == __super::OnDuiSetFocus()) return false;
	if (m_pHotKeyWindow && m_pHotKeyWindow->GetHWND()) return true;

	m_pHotKeyWindow->Init();

	Invalidate();

	return true;
}

bool CDUIHotKeyCtrl::OnDuiKillFocus()
{
	if (false == __super::OnDuiKillFocus()) return false;

	if (m_pHotKeyWindow)
	{
		m_pHotKeyWindow->UnInit();
	}

	Invalidate();

	return true;
}

void CDUIHotKeyCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (NULL == m_pWndManager) return;

	m_pWndManager->AddPreMessagePtr(this);

	return;
}

void CDUIHotKeyCtrl::OnDuiWndManagerDetach()
{
	__super::OnDuiWndManagerAttach();

	if (NULL == m_pWndManager) return;

	m_pWndManager->RemovePreMessagePtr(this);

	return;
}

void CDUIHotKeyCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupHotKey, _T("HotKey"));
	DuiCreateAttribute(m_AttributeAutoSelAll, _T("AutoSelAll"), _T(""), m_AttributeGroupHotKey);
	DuiCreateAttribute(m_AttributeHotKey, _T("HotKey"), _T(""), m_AttributeGroupHotKey);

	return;
}

void CDUIHotKeyCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	return;
}

void CDUIHotKeyCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupAnimation, false);
	DuiInitAttriVisible(m_AttributeGroupText, false);

	return;
}

void CDUIHotKeyCtrl::SetInternVisible(bool bVisible/* = true*/)
{
	__super::SetInternVisible(bVisible);

	if (false == IsVisible() && m_pHotKeyWindow)
	{
		m_pHotKeyWindow->UnInit();
	}

	return;
}

BYTE CDUIHotKeyCtrl::HotkeyToMod(BYTE cbModifierFlag)
{
	//某些快捷键例如Left Right Up Down等设置好后，GetHotKey 
	//得到的cbModifierFlag会多一个HOTKEYF_EXT,RegisterHotKey不需要，这里需要去掉
	cbModifierFlag &= ~HOTKEYF_EXT;

	//shift
	if (cbModifierFlag & HOTKEYF_SHIFT)
	{
		cbModifierFlag &= ~HOTKEYF_SHIFT;
		cbModifierFlag |= MOD_SHIFT;
	}

	//alt
	if (cbModifierFlag & HOTKEYF_ALT)
	{
		cbModifierFlag &= ~HOTKEYF_ALT;
		cbModifierFlag |= MOD_ALT;
	}

	//ctrl
	if (cbModifierFlag & HOTKEYF_CONTROL)
	{
		cbModifierFlag &= ~HOTKEYF_CONTROL;
		cbModifierFlag |= MOD_CONTROL;
	}

	return cbModifierFlag;
}

BYTE CDUIHotKeyCtrl::ModToHotkey(BYTE cbModifierFlag)
{
	//shift
	if (cbModifierFlag & MOD_SHIFT)
	{
		cbModifierFlag &= ~MOD_SHIFT;
		cbModifierFlag |= HOTKEYF_SHIFT;
	}

	//alt
	if (cbModifierFlag & MOD_ALT)
	{
		cbModifierFlag &= ~MOD_ALT;
		cbModifierFlag |= HOTKEYF_ALT;
	}

	//ctrl
	if (cbModifierFlag & MOD_CONTROL)
	{
		cbModifierFlag &= ~MOD_CONTROL;
		cbModifierFlag |= HOTKEYF_CONTROL;
	}

	return cbModifierFlag;
}