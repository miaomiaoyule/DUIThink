#include "StdAfx.h"
#include "DUIAttributeTextStyle.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeTextStyle)
MMImplement_ClassName(CDUIAttributeTextStyle)

CDUIAttributeTextStyle::CDUIAttributeTextStyle(DWORD dwTextStyle)
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_TextStyle;
#endif

	SetTextStyle(dwTextStyle);

	return;
}

CDUIAttributeTextStyle::~CDUIAttributeTextStyle(void)
{
	return;
}

LPVOID CDUIAttributeTextStyle::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeTextStyle, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeTextStyle & CDUIAttributeTextStyle::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	return *this;
}

void CDUIAttributeTextStyle::Draw(HDC hDC, CDUIRect &rcPaint, LPCTSTR lpszText, bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, bool bShadow)
{
	if (MMInvalidString(lpszText) || NULL == m_pOwner) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	CDUIRenderEngine::DrawText(hDC, GetFont(), rcPaint, lpszText, GetTextColor(), TextStyle.dwTextStyle, bGdiplusRender, RenderType, true, bShadow);

	return;
}

bool CDUIAttributeTextStyle::empty()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return TextStyle.vecFontResSwitch.empty() && false == IsModified();
}

void CDUIAttributeTextStyle::Clear()
{
	SetTextStyle(tagDuiTextStyle());

	m_bModified = false;

	return;
}

tagDuiTextStyle CDUIAttributeTextStyle::GetTextStyle()
{
	return CDUIGlobal::GetInstance()->GetAttriTextStyle(m_uValueHash);
}

void CDUIAttributeTextStyle::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyle()) return;

	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriTextStyle(TextStyle);

	NotifyChange();

	return;
}

void CDUIAttributeTextStyle::SetTextStyle(DWORD dwTextStyle)
{
	tagDuiTextStyle TextStyle = GetTextStyle();
	if (TextStyle.dwTextStyle == dwTextStyle) return;

	TextStyle.dwTextStyle = dwTextStyle;
	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

HFONT CDUIAttributeTextStyle::GetFont()
{
	if (NULL == m_pOwner) return NULL;

	CDUIFontBase *pFontBaseCur = GetFontBaseCur();
	if (NULL == pFontBaseCur)
	{
		CDUIFontBase *pFontBase = CDUIGlobal::GetInstance()->GetFontResDefault();

		return pFontBase ? pFontBase->GetHandle() : NULL;
	}

	return pFontBaseCur->GetHandle();
}

int CDUIAttributeTextStyle::GetFontSize()
{
	CDUIFontBase *pFontBaseCur = GetFontBaseCur();
	if (NULL == pFontBaseCur)
	{
		CDUIFontBase *pFontBase = CDUIGlobal::GetInstance()->GetFontResDefault();

		return pFontBase ? pFontBase->GetSize() : 0;
	}

	return pFontBaseCur->GetSize();
}

LPCTSTR CDUIAttributeTextStyle::GetFontResName()
{
	CDUIFontBase *pFontBaseCur = GetFontBaseCur();
	if (NULL == pFontBaseCur)
	{
		CDUIFontBase *pFontBase = CDUIGlobal::GetInstance()->GetFontResDefault();

		return pFontBase ? pFontBase->GetFontName() : _T("");
	}

	return pFontBaseCur->GetResourceName();
}

vector<CMMString> CDUIAttributeTextStyle::GetFontResSwitch()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return TextStyle.vecFontResSwitch;
}

void CDUIAttributeTextStyle::SetFontResSwitch(const vector<CMMString> &vecFontResSwitch)
{
	if (vecFontResSwitch == GetFontResSwitch()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();
	TextStyle.vecFontResSwitch = vecFontResSwitch;
	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

void CDUIAttributeTextStyle::SetFontResSwitch(const CMMString &strResSwitch)
{
	return SetFontResSwitch(CMMStrHelp::ParseStrFromString(strResSwitch, _T(";")));
}

LPCTSTR CDUIAttributeTextStyle::GetColorResName()
{
	CDUIColorBase *pColorBaseCur = GetColorBaseCur();

	return pColorBaseCur ? pColorBaseCur->GetResourceName() : _T("");
}

ARGB CDUIAttributeTextStyle::GetTextColor()
{
	CDUIColorBase *pColorBaseCur = GetColorBaseCur();

	return pColorBaseCur ? pColorBaseCur->GetColor() : 0xff000000;
}

vector<CMMString> CDUIAttributeTextStyle::GetColorResSwitch()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return TextStyle.vecColorResSwitch;
}

void CDUIAttributeTextStyle::SetColorResSwitch(const vector<CMMString> &vecResSwitch)
{
	if (vecResSwitch == GetColorResSwitch()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();
	TextStyle.vecColorResSwitch = vecResSwitch;
	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

void CDUIAttributeTextStyle::SetColorResSwitch(const CMMString &strColorResSwitch)
{
	return SetColorResSwitch(CMMStrHelp::ParseStrFromString(strColorResSwitch, _T(";")));
}

bool CDUIAttributeTextStyle::IsAlignLeft()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return (0 != (TextStyle.dwTextStyle & DT_LEFT) || (0 == (TextStyle.dwTextStyle & DT_RIGHT) && 0 == (TextStyle.dwTextStyle & DT_CENTER)));
}

void CDUIAttributeTextStyle::SetAlignLeft(bool bAlignLeft)
{
	if (bAlignLeft == IsAlignLeft()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bAlignLeft)
	{
		TextStyle.dwTextStyle &= ~(DT_CENTER | DT_RIGHT);
		TextStyle.dwTextStyle |= DT_LEFT;
	}
	else
	{
		TextStyle.dwTextStyle &= ~DT_LEFT;
	}

	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

bool CDUIAttributeTextStyle::IsAlignCenter()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return 0 != (TextStyle.dwTextStyle & DT_CENTER);
}

void CDUIAttributeTextStyle::SetAlignCenter(bool bAlignCenter)
{
	if (bAlignCenter == IsAlignCenter()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bAlignCenter)
	{
		TextStyle.dwTextStyle &= ~(DT_LEFT | DT_RIGHT);
		TextStyle.dwTextStyle |= DT_CENTER;
	}
	else
	{
		TextStyle.dwTextStyle &= ~DT_CENTER;
	}

	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

bool CDUIAttributeTextStyle::IsAlignRight()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return 0 != (TextStyle.dwTextStyle & DT_RIGHT);
}

void CDUIAttributeTextStyle::SetAlignRight(bool bAlignRight)
{
	if (bAlignRight == IsAlignRight()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bAlignRight)
	{
		TextStyle.dwTextStyle &= ~(DT_LEFT | DT_CENTER);
		TextStyle.dwTextStyle |= DT_RIGHT;
	}
	else
	{
		TextStyle.dwTextStyle &= ~DT_RIGHT;
	}

	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

bool CDUIAttributeTextStyle::IsVAlignTop()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return (0 != (TextStyle.dwTextStyle & DT_TOP) || (0 == (TextStyle.dwTextStyle & DT_BOTTOM) && 0 == (TextStyle.dwTextStyle & DT_VCENTER)));
}

void CDUIAttributeTextStyle::SetVAlignTop(bool bVAlignTop)
{
	if (bVAlignTop == IsVAlignTop()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bVAlignTop)
	{
		TextStyle.dwTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_WORDBREAK);
		TextStyle.dwTextStyle |= DT_TOP;
	}
	else
	{
		TextStyle.dwTextStyle &= ~DT_TOP;
	}

	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

bool CDUIAttributeTextStyle::IsVAlignCenter()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return 0 != (TextStyle.dwTextStyle & DT_VCENTER);
}

void CDUIAttributeTextStyle::SetVAlignCenter(bool bVAlignCenter)
{
	if (bVAlignCenter == IsVAlignCenter()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bVAlignCenter)
	{
		TextStyle.dwTextStyle &= ~(DT_TOP | DT_BOTTOM | DT_WORDBREAK);
		TextStyle.dwTextStyle |= DT_VCENTER;
	}
	else
	{
		TextStyle.dwTextStyle &= ~DT_VCENTER;
	}

	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

bool CDUIAttributeTextStyle::IsVAlignBottom()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return 0 != (TextStyle.dwTextStyle & DT_BOTTOM);
}

void CDUIAttributeTextStyle::SetVAlignBottom(bool bVAlignBottom)
{
	if (bVAlignBottom == IsVAlignBottom()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bVAlignBottom)
	{
		TextStyle.dwTextStyle &= ~(DT_TOP | DT_VCENTER | DT_WORDBREAK);
		TextStyle.dwTextStyle |= DT_BOTTOM;
	}
	else
	{
		TextStyle.dwTextStyle &= ~DT_BOTTOM;
	}

	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

bool CDUIAttributeTextStyle::IsWordBreak()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return 0 != (TextStyle.dwTextStyle & DT_WORDBREAK);
}

void CDUIAttributeTextStyle::SetWordBreak(bool bWordBreak)
{
	if (bWordBreak == IsWordBreak()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bWordBreak)
	{
		TextStyle.dwTextStyle &= ~DT_SINGLELINE;
		TextStyle.dwTextStyle |= DT_WORDBREAK | DT_EDITCONTROL;
	}
	else
	{
		TextStyle.dwTextStyle &= ~DT_WORDBREAK & ~DT_EDITCONTROL;
		TextStyle.dwTextStyle |= DT_SINGLELINE;
	}

	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

bool CDUIAttributeTextStyle::IsEndEllipsis()
{
	tagDuiTextStyle TextStyle = GetTextStyle();

	return 0 != (TextStyle.dwTextStyle & DT_END_ELLIPSIS);
}

void CDUIAttributeTextStyle::SetEndEllipsis(bool bEndEllipsis)
{
	if (bEndEllipsis == IsEndEllipsis()) return;

	tagDuiTextStyle TextStyle = GetTextStyle();

	if (bEndEllipsis) TextStyle.dwTextStyle |= DT_END_ELLIPSIS;
	else TextStyle.dwTextStyle &= ~DT_END_ELLIPSIS;
	SetTextStyle(TextStyle);

	NotifyChange();

	return;
}

CDUISize CDUIAttributeTextStyle::MeasureString(LPCTSTR lpszText)
{
	if (MMInvalidString(lpszText) || NULL == m_pOwner) return {};

	MMInterfaceHelper(CDUIControlBase, m_pOwner, pOwnerCtrl);
	if (NULL == pOwnerCtrl) return {};

	CDUIWndManager *pWndManager = pOwnerCtrl->GetWndManager();
	if (NULL == pWndManager) return {};

	tagDuiTextStyle TextStyle = GetTextStyle();

	//single line
	if (false == IsWordBreak())
	{
		CDUIRect rcText = { 0, 0, 9999, GetFontSize() + 8 };
		CDUIRenderEngine::DrawText(pWndManager->GetWndDC(), GetFont(), rcText, lpszText, 0, DT_CALCRECT | TextStyle.dwTextStyle, pWndManager->IsGdiplusRenderText(), pWndManager->GetGdiplusRenderTextType(), true, pOwnerCtrl->IsShadowText());

		return { rcText.GetWidth(), rcText.GetHeight() };
	}

	//multi line
	CDUIRect rcText = { 0, 0, pOwnerCtrl->GetWidth(), 9999 };
	CDUIRenderEngine::DrawText(pWndManager->GetWndDC(), GetFont(), rcText, lpszText, 0, DT_CALCRECT | TextStyle.dwTextStyle, pWndManager->IsGdiplusRenderText(), pWndManager->GetGdiplusRenderTextType(), true, pOwnerCtrl->IsShadowText());

	return { rcText.GetWidth(), rcText.GetHeight() };
}

bool CDUIAttributeTextStyle::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriObjValueID))
	{
		m_uValueHash = strtoul(lpszValue, NULL, 10);

#ifdef DUI_DESIGN
		CDUIGlobal::GetInstance()->OnAttriValueIDRead(GetAttributeType(), GetValueID());
#endif

		return true;
	}

	return false;
}

uint32_t CDUIAttributeTextStyle::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttributeTextStyle::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}

void CDUIAttributeTextStyle::NotifyChange()
{
	if (NULL == m_pOwner || false == m_pOwner->IsInitComplete()) return;

	__super::NotifyChange();

	tagDuiTextStyle TextStyle = GetTextStyle();
	m_bModified = TextStyle.vecFontResSwitch.size() > 0
		|| TextStyle.vecColorResSwitch.size() > 0
		|| (DT_SINGLELINE | DT_END_ELLIPSIS) != TextStyle.dwTextStyle;

	return;
}

CDUIFontBase * CDUIAttributeTextStyle::GetFontBaseCur()
{
	tagDuiTextStyle TextStyle = GetTextStyle();
	int nIndexRes = CDUIGlobal::GetInstance()->GetSwitchResIndex();
	nIndexRes = min(nIndexRes, (int)TextStyle.vecFontResSwitch.size() - 1);
	if (nIndexRes < TextStyle.vecFontResSwitch.size())
	{
		return CDUIGlobal::GetInstance()->GetFontResource(TextStyle.vecFontResSwitch[nIndexRes]);
	}

	return NULL;
}

CDUIColorBase * CDUIAttributeTextStyle::GetColorBaseCur()
{
	tagDuiTextStyle TextStyle = GetTextStyle();
	int nIndexRes = CDUIGlobal::GetInstance()->GetSwitchResIndex();
	nIndexRes = min(nIndexRes, (int)TextStyle.vecColorResSwitch.size() - 1);
	if (nIndexRes < TextStyle.vecColorResSwitch.size())
	{
		return CDUIGlobal::GetInstance()->GetColorResource(TextStyle.vecColorResSwitch[nIndexRes]);
	}

	return NULL;
}