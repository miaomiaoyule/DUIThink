#include "StdAfx.h"
#include "DUIEditCtrl.h"

//////////////////////////////////////////////////////////////////////////
class CDUIEditWnd : public CDUIWnd
{
	MMDeclare_ClassName()

public:
	CDUIEditWnd(CDUIEditCtrl *pOwner);
	~CDUIEditWnd();

protected:
	CDUIEditCtrl *						m_pOwner = NULL;
	CDUIWndManager *					m_pWndManagerOwner = NULL;
	HBRUSH								m_hBkBrush = NULL;

public:
	void Init();
	void UnInit();
	void AdjustWndSize(bool bCreate);

	LPCTSTR GetSuperClassName() const override;

protected:
	void OnFinalMessage() override;
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void ShowBalloonTip(_In_z_ LPCWSTR lpszTitle, _In_z_ LPCWSTR lpszText, _In_ INT ttiIcon)
	{
		ASSERT(::IsWindow(m_hWnd));
		EDITBALLOONTIP bt;
		::ZeroMemory(&bt, sizeof(EDITBALLOONTIP));
		bt.cbStruct = sizeof(EDITBALLOONTIP);
		bt.pszTitle = lpszTitle;
		bt.pszText = lpszText;
		bt.ttiIcon = ttiIcon;
		Edit_ShowBalloonTip(m_hWnd, &bt);

		return;
	}

	void HideBalloontip()
	{
		ASSERT(::IsWindow(m_hWnd));
		Edit_HideBalloonTip(m_hWnd);

		return;
	}
};

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIEditWnd)

CDUIEditWnd::CDUIEditWnd(CDUIEditCtrl *pOwner)
	: m_pOwner(pOwner)
	, m_hBkBrush(NULL)
{
}

CDUIEditWnd::~CDUIEditWnd()
{
	m_pOwner = NULL;

	UnInit();

	MMSafeDeleteObject(m_hBkBrush);

	return;
}

void CDUIEditWnd::Init()
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

	uWndStyle = WS_CHILD | ES_AUTOHSCROLL | WS_VISIBLE;
	m_pWndManagerOwner->IsWndLayered() || m_pOwner->IsDesktopEdit() ? uWndStyle |= WS_POPUP : NULL;
	rcBorderLine.left || rcBorderLine.top || rcBorderLine.right || rcBorderLine.bottom ? uWndStyle |= WS_BORDER : NULL;
	if (TextStyle.IsWordBreak())
	{
		uWndStyle |= (ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN);
		uWndStyle &= ~ES_AUTOHSCROLL;
	}

	m_pOwner->IsNumberOnly() ? uWndStyle |= ES_NUMBER : NULL;
	m_pOwner->IsPasswordMode() ? uWndStyle |= ES_PASSWORD : NULL;
	m_pOwner->IsReadOnly() ? uWndStyle |= ES_READONLY : NULL;
	(TextStyle.dwTextStyle & DT_LEFT) ? uWndStyle |= ES_LEFT : NULL;
	(TextStyle.dwTextStyle & DT_CENTER) ? uWndStyle |= ES_CENTER : NULL;
	(TextStyle.dwTextStyle & DT_RIGHT) ? uWndStyle |= ES_RIGHT : NULL;

	Create(m_pWndManagerOwner->GetWndHandle(), _T(""), uWndStyle, 0);

	if (false == IsWindow(m_hWnd))
	{
		assert(false);
		return;
	}
	if (m_pOwner->IsDesktopEdit())
	{
		SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	AdjustWndSize(true);

	//text
	HFONT hFont = pAttribute ? pAttribute->GetFont() : NULL;
	hFont ? SetWindowFont(m_hWnd, hFont, TRUE) : NULL;
	m_pOwner->IsPasswordMode() ? Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar()) : NULL;
	Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
	Edit_SetText(m_hWnd, m_pOwner->GetText());
	Edit_SetModify(m_hWnd, FALSE);
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

void CDUIEditWnd::UnInit()
{
	Close();

	return;
}

void CDUIEditWnd::AdjustWndSize(bool bCreate)
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

	//caret pos
	if (false == bCreate)
	{
		int nPos = LOWORD(Edit_GetSel(m_hWnd));
		Edit_SetSel(m_hWnd, 0, 0);
		Edit_SetSel(m_hWnd, nPos, nPos);
	}

	CDUIRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	::InvalidateRect(m_hWnd, &rcClient, TRUE);

	return;
}

LPCTSTR CDUIEditWnd::GetSuperClassName() const
{
	return WC_EDIT;
}

void CDUIEditWnd::OnFinalMessage()
{
	__super::OnFinalMessage();

	if (m_pOwner && m_pWndManagerOwner)
	{
		m_pWndManagerOwner->SendNotify(m_pOwner, DuiNotify_Edited);

		if (m_pWndManagerOwner->GetFocusControl() == m_pOwner)
		{
			m_pWndManagerOwner->SetFocusControl(NULL);
		}

		m_pOwner->OnEditDestroy();
		m_pOwner->Invalidate();
	}

	MMSafeDeleteObject(m_hBkBrush);

	return;
}

LRESULT CDUIEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

	UnInit();

	return lRes;
}

LRESULT CDUIEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, bool &/*bHandled*/)
{
	if (NULL == m_pOwner || NULL == m_pWndManagerOwner) return 0;

	//copy text
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	CMMString strText(_T('\0'), cchLen);
	::GetWindowText(m_hWnd, (LPTSTR)strText.c_str(), cchLen);

	//limit
	if (m_pOwner->IsNumberOnly())
	{
		int nNumSrc = _ttol(strText);
		int nNumAdjust = min(max(nNumSrc, m_pOwner->GetNumberMinLimit()), m_pOwner->GetNumberMaxLimit());
		if (nNumSrc != nNumAdjust)
		{
			Edit_SetText(GetHWND(), CMMStrHelp::Format(_T("%d"), nNumAdjust));

			return 0;
		}
	}

	ShowCaret(GetHWND());

	m_pOwner->SetText(strText);

	//AdjustWndSize(false);

	return 0;
}

LRESULT CDUIEditWnd::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pOwner || NULL == m_pWndManagerOwner) return __super::OnOldWndProc(uMsg, wParam, lParam);

	LRESULT lRes = 0;
	bool bHandled = false;
	switch (uMsg)
	{
		case WM_CREATE:
		{
			break;
		}
		case WM_PRINT:
		{
			break;
		}
		case WM_KILLFOCUS:
		{
			bHandled = true;
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
			break;
		}
		case OCM_COMMAND:
		{
			bHandled = true;
			if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
			{
				lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			}
			else if (EN_UPDATE == GET_WM_COMMAND_CMD(wParam, lParam)
				|| EN_VSCROLL == GET_WM_COMMAND_CMD(wParam, lParam)
				|| EN_HSCROLL == GET_WM_COMMAND_CMD(wParam, lParam))
			{
				CDUIRect rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
			}

			break;
		}
		case WM_KEYDOWN:
		{
			if (VK_RETURN == TCHAR(wParam))
			{
				bHandled = true;
				m_pWndManagerOwner->SendNotify(m_pOwner, DuiNotify_Return);
			}
			else if (VK_TAB == TCHAR(wParam))
			{
				bHandled = true;
			}

			break;
		}
		case OCM__BASE + WM_CTLCOLOREDIT:
		case OCM__BASE + WM_CTLCOLORSTATIC:
		{
			DWORD dwTextColor = m_pOwner->GetTextColorFocus();
			0 == dwTextColor ? dwTextColor = m_pOwner->GetTextColor() : 0;
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

			::SetBkMode((HDC)wParam, TRANSPARENT);
			MMSafeDeleteObject(m_hBkBrush);

			CDUIRect rcWnd;
			::GetWindowRect(m_hWnd, &rcWnd);
			MMScreenToClient(rcWnd, m_pWndManagerOwner->GetWndHandle());
			if (false == rcWnd.empty())
			{
				HBITMAP hBmpEditBk = CDUIRenderEngine::GenerateBitmap(m_pWndManagerOwner, m_pWndManagerOwner->GetRootCtrl(), rcWnd);
				if (hBmpEditBk)
				{
					m_hBkBrush = ::CreatePatternBrush(hBmpEditBk);
					MMSafeDeleteObject(hBmpEditBk);
				}
			}

			return (LRESULT)m_hBkBrush;
		}
		case WM_CHAR:
		{
			/*if (m_pOwner->IsFileNameInputLimit())
			{
				switch (wParam)
				{
					case VK_RETURN:
					case VK_TAB:
					case 0x0A:
					case 0x0B:
						::SetFocus(m_pOwner->GetManager()->GetPaintWindow());
						return 0L;
					case '\\':
					case '/':
					case ':':
					case '*':
					case '?':
					case '"':
					case '>':
					case '<':
					case '|':
						ShowBalloonTip(NULL, FILERENAME_TIP, TTI_NONE);
						return 0L;
					default:
						bHandled = FALSE;
						break;
				}
			}*/
			break;
		}
	}

	if (bHandled) return lRes;
	return OnOldWndProc(uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIEditCtrl)
MMImplement_ClassName(CDUIEditCtrl)

CDUIEditCtrl::CDUIEditCtrl(void)
{
	m_pEditWindow = new CDUIEditWnd(this);

	return;
}

CDUIEditCtrl::~CDUIEditCtrl(void)
{
	MMSafeDelete(m_pEditWindow);

	return;
}

LPVOID CDUIEditCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIEditCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIEditCtrl::GetDescribe() const
{
	return Dui_Ctrl_Edit;
}

void CDUIEditCtrl::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);

	if (false == IsVisible() && m_pEditWindow)
	{
		m_pEditWindow->UnInit();
	}

	return;
}

void CDUIEditCtrl::SetEnabled(bool bEnable)
{
	__super::SetEnabled(bEnable);

	if (false == IsEnabled() && m_pEditWindow)
	{
		m_pEditWindow->UnInit();
	}

	return;
}

bool CDUIEditCtrl::SetText(LPCTSTR lpszText)
{
	if (false == __super::SetText(lpszText)) return false;

	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		int nLen = ::Edit_GetTextLength(m_pEditWindow->GetHWND()) + 1;

		std::vector<TCHAR> vecEditText;
		vecEditText.resize(nLen);
		Edit_GetText(m_pEditWindow->GetHWND(), vecEditText.data(), vecEditText.size());

		if (0 != lstrcmp(vecEditText.data(), lpszText))
		{
			Edit_SetText(m_pEditWindow->GetHWND(), GetText());
		}
	}

	Invalidate();

	return true;
}

void CDUIEditCtrl::RefreshView()
{
	__super::RefreshView();

	//adjust
	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		m_pEditWindow->AdjustWndSize(false);
	}

	return;
}

void CDUIEditCtrl::SetSel(long nStartChar, long nEndChar)
{
	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		Edit_SetSel(m_pEditWindow->GetHWND(), nStartChar, nEndChar);
	}

	return;
}

void CDUIEditCtrl::SetSelAll()
{
	SetSel(0, -1);

	return;
}

void CDUIEditCtrl::SetReplaceSel(LPCTSTR lpszReplace)
{
	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		Edit_ReplaceSel(m_pEditWindow->GetHWND(), lpszReplace);
	}

	return;
}

bool CDUIEditCtrl::IsReadOnly()
{
	return m_AttributeReadOnly.GetValue();
}

void CDUIEditCtrl::SetReadOnly(bool bReadOnly)
{
	if (bReadOnly == m_AttributeReadOnly.GetValue()) return;

	m_AttributeReadOnly.SetValue(bReadOnly);

	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		Edit_SetReadOnly(m_pEditWindow->GetHWND(), IsReadOnly());
	}

	Invalidate();

	return;
}

bool CDUIEditCtrl::IsNumberOnly()
{
	return m_AttributeNumberOnly.GetValue();
}

void CDUIEditCtrl::SetNumberOnly(bool bNumberOnly)
{
	if (bNumberOnly == m_AttributeNumberOnly.GetValue()) return;

	m_AttributeNumberOnly.SetValue(bNumberOnly);

	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		LONG styleValue = ::GetWindowLong(m_pEditWindow->GetHWND(), GWL_STYLE);
		IsNumberOnly() ? styleValue |= ES_NUMBER : styleValue &= ~ES_NUMBER;
		::SetWindowLong(m_pEditWindow->GetHWND(), GWL_STYLE, styleValue);
	}

	Invalidate();

	return;
}

bool CDUIEditCtrl::IsPasswordMode()
{
	return m_AttributePasswordMode.GetValue();
}

void CDUIEditCtrl::SetPasswordMode(bool bPasswordMode)
{
	if (bPasswordMode == m_AttributePasswordMode.GetValue()) return;

	m_AttributePasswordMode.SetValue(bPasswordMode);

	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		LONG styleValue = ::GetWindowLong(m_pEditWindow->GetHWND(), GWL_STYLE);
		IsPasswordMode() ? styleValue |= ES_PASSWORD : styleValue &= ~ES_PASSWORD;
		::SetWindowLong(m_pEditWindow->GetHWND(), GWL_STYLE, styleValue);
	}

	Invalidate();

	return;
}

bool CDUIEditCtrl::IsAutoSelAll()
{
	return m_AttributeAutoSelAll.GetValue();
}

void CDUIEditCtrl::SetAutoSelAll(bool bAutoSelAll)
{
	if (bAutoSelAll == IsAutoSelAll()) return;

	m_AttributeAutoSelAll.SetValue(bAutoSelAll);

	return;
}

bool CDUIEditCtrl::IsDesktopEdit()
{
	return m_AttributeDesktopEdit.GetValue();
}

void CDUIEditCtrl::SetDesktopEdit(bool bDesktopEdit)
{
	if (bDesktopEdit == IsDesktopEdit()) return;

	m_AttributeDesktopEdit.SetValue(bDesktopEdit);

	return;
}

UINT CDUIEditCtrl::GetMaxChar()
{
	return m_AttributeMaxChar.GetValue();
}

void CDUIEditCtrl::SetMaxChar(UINT uMax)
{
	if (uMax == m_AttributeMaxChar.GetValue()) return;

	m_AttributeMaxChar.SetValue(uMax);

	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		Edit_LimitText(m_pEditWindow->GetHWND(), GetMaxChar());
	}

	return;
}

int CDUIEditCtrl::GetNumberMinLimit()
{
	return m_AttributeNumberMinLimit.GetValue();
}

void CDUIEditCtrl::SetNumberMinLimit(int nMinNum)
{
	if (nMinNum == GetNumberMinLimit()) return;

	m_AttributeNumberMinLimit.SetValue(nMinNum);

	return;
}

int CDUIEditCtrl::GetNumberMaxLimit()
{
	return m_AttributeNumberMaxLimit.GetValue();
}

void CDUIEditCtrl::SetNumberMaxLimit(int nMaxNum)
{
	if (nMaxNum == GetNumberMaxLimit()) return;

	m_AttributeNumberMaxLimit.SetValue(nMaxNum);

	return;
}

TCHAR CDUIEditCtrl::GetPasswordChar()
{
	return *m_AttributePasswordChar.GetValue();
}

void CDUIEditCtrl::SetPasswordChar(TCHAR chPasswordChar)
{
	if (chPasswordChar == GetPasswordChar()) return;

	m_AttributePasswordChar.SetValue(CMMString(chPasswordChar));

	if (m_pEditWindow && m_pEditWindow->GetHWND())
	{
		Edit_SetPasswordChar(m_pEditWindow->GetHWND(), GetPasswordChar());
	}

	Invalidate();

	return;
}

CMMString CDUIEditCtrl::GetTipText()
{
	return m_AttributeTipText.GetValue();
}

void CDUIEditCtrl::SetTipText(LPCTSTR lpszTipText)
{
	if (0 == lstrcmp(lpszTipText, m_AttributeTipText.GetValue())) return;

	m_AttributeTipText.SetValue(lpszTipText);

	Invalidate();

	return;
}

tagDuiTextStyle CDUIEditCtrl::GetTextStyleTipTextNormal()
{
	return m_AttributeTextStyleTipTextNormal.GetTextStyle();
}

ARGB CDUIEditCtrl::GetTextColorTipTextNormal()
{
	return m_AttributeTextStyleTipTextNormal.GetTextColor();
}

void CDUIEditCtrl::SetTextStyleTipTextNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleTipTextNormal.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

bool CDUIEditCtrl::OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (GetCursor())
	{
		::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(GetCursor())));
	}
	else
	{
		::SetCursor(::LoadCursor(nullptr, IDC_IBEAM));
	}

	return true;
}

bool CDUIEditCtrl::OnDuiSetFocus()
{
	if (false == __super::OnDuiSetFocus()) return false;
	if (m_pEditWindow && m_pEditWindow->GetHWND()) return true;

	m_pEditWindow->Init();

	Invalidate();

	return true;
}

bool CDUIEditCtrl::OnDuiKillFocus()
{
	if (false == __super::OnDuiKillFocus()) return false;

	if (m_pEditWindow)
	{
		m_pEditWindow->UnInit();
	}

	Invalidate();

	return true;
}

void CDUIEditCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupEdit, _T("Edit"));
	DuiCreateAttribute(m_AttributeReadOnly, _T("ReadOnly"), _T("ReadOnly"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeNumberOnly, _T("NumberOnly"), _T("NumberOnly"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributePasswordMode, _T("PasswordMode"), _T("PasswordMode"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeAutoSelAll, _T("AutoSelAll"), _T("AutoSelAll"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeDesktopEdit, _T("DesktopEdit"), _T("Edit For DesktopWnd"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeMaxChar, _T("MaxChar"), _T("Max Char Count"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeNumberMinLimit, _T("NumberMinLimit"), _T("NumberMinLimit"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeNumberMaxLimit, _T("NumberMaxLimit"), _T("NumberMaxLimit"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributePasswordChar, _T("PasswordChar"), _T("PasswordChar"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeTipText, _T("TipText"), _T("Tip Info For Empty Edit"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeTextStyleTipTextNormal, _T("TextStyleTipTextNormal"), _T(""), m_AttributeGroupEdit);

	return;
}

void CDUIEditCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	DuiInitAttriValue(m_AttributeMaxChar, 255);
	DuiInitAttriValue(m_AttributeNumberMinLimit, 0x80000000);
	DuiInitAttriValue(m_AttributeNumberMaxLimit, 0x7fffffff);
	DuiInitAttriValue(m_AttributePasswordChar, _T("*"));

	if (false == m_AttributeCursor.IsModified())
	{
		m_AttributeCursor.SelectItem(Cursor_IBeam);
	}

	return;
}

void CDUIEditCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pWndManager) return;

	//text
	CMMString strText = GetText(), strTipText = GetTipText();
	CMMString strTextDraw = strText.empty() ? strTipText : strText;
	if (strTextDraw.empty()
		|| (m_pEditWindow && IsWindow(m_pEditWindow->GetHWND()) && false == strText.empty())) return;

	//textstyle
	CDUIAttributeTextStyle *pAttribute = NULL;
	if (strText.empty())
	{
		pAttribute = &m_AttributeTextStyleTipTextNormal;
	}
	else if (false == IsEnabled())
	{
		pAttribute = &m_AttributeTextStyleDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeTextStylePushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeTextStyleHot;
	}
	else if (IsFocused())
	{
		pAttribute = &m_AttributeTextStyleFocus;
	}
	else
	{
		pAttribute = &m_AttributeTextStyleNormal;
	}

	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyleNormal : pAttribute;
	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyle : pAttribute;
	if (NULL == pAttribute) return;

	//range
	CDUIRect rcRange = GetTextRange();

	//password
	if (IsPasswordMode() && false == strText.empty())
	{
		int n = 0;
		while (strTextDraw[n])
		{
			strTextDraw.SetAt(n++, GetPasswordChar());
		}
	}

	pAttribute->Draw(hDC, rcRange, strTextDraw, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), IsShadowText());

	return;
}

void CDUIEditCtrl::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);

	if (false == IsVisible() && m_pEditWindow)
	{
		m_pEditWindow->UnInit();
	}

	return;
}

void CDUIEditCtrl::OnEditDestroy()
{
	Invalidate();

	return;
}