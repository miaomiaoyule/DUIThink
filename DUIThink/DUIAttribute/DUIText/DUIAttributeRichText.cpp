#include "StdAfx.h"
#include "DUIAttributeRichText.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeRichText)
MMImplement_ClassName(CDUIAttributeRichText)

CDUIAttributeRichText::CDUIAttributeRichText()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_RichText;
#endif

	return;
}

CDUIAttributeRichText::~CDUIAttributeRichText(void)
{
	return;
}

LPVOID CDUIAttributeRichText::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeRichText, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

void CDUIAttributeRichText::Draw(HDC hDC, CDUIRect &rcPaint, bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, int nLineSpace, bool bShadow)
{
	if (empty() || NULL == m_pOwner) return;

	CDUIRenderEngine::DrawRichText(hDC, rcPaint, GetRichText(), bGdiplusRender, RenderType, nLineSpace);

	return;
}

bool CDUIAttributeRichText::empty()
{
	tagDuiRichText RichText = GetRichText();

	return RichText.vecRichTextItem.empty();
}

void CDUIAttributeRichText::Clear()
{
	SetRichText(tagDuiRichText());

	m_bModified = false;

	return;
}

tagDuiRichText CDUIAttributeRichText::GetRichText()
{
	return CDUIGlobal::GetInstance()->GetAttriRichText(m_uValueHash);
}

void CDUIAttributeRichText::SetRichText(const tagDuiRichText &RichText)
{
	if (RichText == GetRichText()) return;

	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriRichText(RichText);

	NotifyChange();

	return;
}

void CDUIAttributeRichText::SetTextStyle(DWORD dwTextStyle)
{
	tagDuiRichText RichText = GetRichText();

	if (dwTextStyle == RichText) return;

	RichText.dwTextStyle = dwTextStyle;
	SetRichText(RichText);

	NotifyChange();

	return;
}

VecDuiRichTextItem CDUIAttributeRichText::GetRichTextItem()
{
	tagDuiRichText RichText = GetRichText();

	return RichText.vecRichTextItem;
}

void CDUIAttributeRichText::SetRichTextItem(const VecDuiRichTextItem &vecRichTextItem)
{
	tagDuiRichText RichText = GetRichText();
	RichText.vecRichTextItem = vecRichTextItem;
	SetRichText(RichText);

	NotifyChange();

	return;
}

void CDUIAttributeRichText::AddRichTextItem(const tagDuiRichTextItem &RichTextItem)
{
	tagDuiRichText RichText = GetRichText();
	RichText.vecRichTextItem.push_back(RichTextItem);
	SetRichText(RichText);

	NotifyChange();

	return;
}

int CDUIAttributeRichText::GetLineLimit()
{
	tagDuiRichText RichText = GetRichText();

	return RichText.nLineLimit;
}

void CDUIAttributeRichText::SetLineLimit(int nLineLimit)
{
	tagDuiRichText RichText = GetRichText();

	if (RichText.nLineLimit == nLineLimit) return;

	RichText.nLineLimit = nLineLimit;
	SetRichText(RichText);

	NotifyChange();

	return;
}

bool CDUIAttributeRichText::IsAlignLeft()
{
	tagDuiRichText RichText = GetRichText();

	return (0 != (RichText.dwTextStyle & DT_LEFT) || (false == IsAlignRight() && false == IsAlignCenter()));
}

void CDUIAttributeRichText::SetAlignLeft(bool bAlignLeft)
{
	if (IsAlignLeft() == bAlignLeft) return;

	tagDuiRichText RichText = GetRichText();

	if (bAlignLeft)
	{
		RichText.dwTextStyle &= ~(DT_CENTER | DT_RIGHT);
		RichText.dwTextStyle |= DT_LEFT;
	}
	else
	{
		RichText.dwTextStyle &= ~DT_LEFT;
	}

	SetRichText(RichText);

	NotifyChange();

	return;
}

bool CDUIAttributeRichText::IsAlignCenter()
{
	tagDuiRichText RichText = GetRichText();

	return 0 != (RichText.dwTextStyle & DT_CENTER);
}

void CDUIAttributeRichText::SetAlignCenter(bool bAlignCenter)
{
	if (IsAlignCenter() == bAlignCenter) return;

	tagDuiRichText RichText = GetRichText();

	if (bAlignCenter)
	{
		RichText.dwTextStyle &= ~(DT_LEFT | DT_RIGHT);
		RichText.dwTextStyle |= DT_CENTER;
	}
	else
	{
		RichText.dwTextStyle &= ~DT_CENTER;
	}

	SetRichText(RichText);

	NotifyChange();

	return;
}

bool CDUIAttributeRichText::IsAlignRight()
{
	tagDuiRichText RichText = GetRichText();

	return 0 != (RichText.dwTextStyle & DT_RIGHT);
}

void CDUIAttributeRichText::SetAlignRight(bool bAlignRight)
{
	if (IsAlignRight() == bAlignRight) return;

	tagDuiRichText RichText = GetRichText();

	if (bAlignRight)
	{
		RichText.dwTextStyle &= ~(DT_LEFT | DT_CENTER);
		RichText.dwTextStyle |= DT_RIGHT;
	}
	else
	{
		RichText.dwTextStyle &= ~DT_RIGHT;
	}

	SetRichText(RichText);

	NotifyChange();

	return;
}

bool CDUIAttributeRichText::IsVAlignTop()
{
	tagDuiRichText RichText = GetRichText();

	return (0 != (RichText.dwTextStyle & DT_TOP) || (false == IsVAlignBottom() && false == IsVAlignCenter()));
}

void CDUIAttributeRichText::SetVAlignTop(bool bVAlignTop)
{
	if (IsVAlignTop() == bVAlignTop) return;

	tagDuiRichText RichText = GetRichText();

	if (bVAlignTop)
	{
		RichText.dwTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_WORDBREAK);
		RichText.dwTextStyle |= DT_TOP;
	}
	else
	{
		RichText.dwTextStyle &= ~DT_TOP;
	}

	SetRichText(RichText);

	NotifyChange();

	return;
}

bool CDUIAttributeRichText::IsVAlignCenter()
{
	tagDuiRichText RichText = GetRichText();

	return 0 != (RichText.dwTextStyle & DT_VCENTER);
}

void CDUIAttributeRichText::SetVAlignCenter(bool bVAlignCenter)
{
	if (IsVAlignCenter() == bVAlignCenter) return;

	tagDuiRichText RichText = GetRichText();

	if (bVAlignCenter)
	{
		RichText.dwTextStyle &= ~(DT_TOP | DT_BOTTOM | DT_WORDBREAK);
		RichText.dwTextStyle |= DT_VCENTER;
	}
	else
	{
		RichText.dwTextStyle &= ~DT_VCENTER;
	}

	SetRichText(RichText);

	NotifyChange();

	return;
}

bool CDUIAttributeRichText::IsVAlignBottom()
{
	tagDuiRichText RichText = GetRichText();

	return 0 != (RichText.dwTextStyle & DT_BOTTOM);
}

void CDUIAttributeRichText::SetVAlignBottom(bool bVAlignBottom)
{
	if (IsVAlignBottom() == bVAlignBottom) return;

	tagDuiRichText RichText = GetRichText();

	if (bVAlignBottom)
	{
		RichText.dwTextStyle &= ~(DT_TOP | DT_VCENTER | DT_WORDBREAK);
		RichText.dwTextStyle |= DT_BOTTOM;
	}
	else
	{
		RichText.dwTextStyle &= ~DT_BOTTOM;
	}

	SetRichText(RichText);

	NotifyChange();

	return;
}

//ÊÇ·ñ»»ÐÐ
bool CDUIAttributeRichText::IsWordBreak()
{
	tagDuiRichText RichText = GetRichText();

	return 0 != (RichText.dwTextStyle & DT_WORDBREAK);
}

void CDUIAttributeRichText::SetWordBreak(bool bWordBreak)
{
	if (IsWordBreak() == bWordBreak) return;

	tagDuiRichText RichText = GetRichText();

	if (bWordBreak)
	{
		RichText.dwTextStyle &= ~DT_SINGLELINE;
		RichText.dwTextStyle |= DT_WORDBREAK | DT_EDITCONTROL;
	}
	else
	{
		RichText.dwTextStyle &= ~DT_WORDBREAK & ~DT_EDITCONTROL;
		RichText.dwTextStyle |= DT_SINGLELINE;
	}

	SetRichText(RichText);

	NotifyChange();

	return;
}

//ÊÇ·ñ...
bool CDUIAttributeRichText::IsEndEllipsis()
{
	tagDuiRichText RichText = GetRichText();

	return 0 != (RichText.dwTextStyle & DT_END_ELLIPSIS);
}

void CDUIAttributeRichText::SetEndEllipsis(bool bEndEllipsis)
{
	if (IsEndEllipsis() == bEndEllipsis) return;

	tagDuiRichText RichText = GetRichText();
	if (bEndEllipsis) RichText.dwTextStyle |= DT_END_ELLIPSIS;
	else RichText.dwTextStyle &= ~DT_END_ELLIPSIS;

	SetRichText(RichText);

	NotifyChange();

	return;
}

CDUISize CDUIAttributeRichText::MeasureString()
{
	if (NULL == m_pOwner) return {};

	MMInterfaceHelper(CDUIControlBase, m_pOwner, pOwnerCtrl);
	if (NULL == pOwnerCtrl) return {};

	CDUIWnd *pWndManager = m_pOwner->GetWndOwner();
	if (NULL == pWndManager) return {};

	CDUIRect rcText = { 0, 0, 9999, 9999 };
	tagDuiRichText RichText = GetRichText();
	RichText.dwTextStyle |= DT_CALCRECT;
	CDUIRenderEngine::DrawRichText(pWndManager->GetWndDC(), rcText, RichText, pWndManager->IsGdiplusRenderText(), pWndManager->GetGdiplusRenderTextType(), 0, pOwnerCtrl->IsShadowText());

	return { rcText.GetWidth(), rcText.GetHeight() };
}

bool CDUIAttributeRichText::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
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

uint32_t CDUIAttributeRichText::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttributeRichText::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}

void CDUIAttributeRichText::NotifyChange()
{
#ifndef DUI_DESIGN
	return;
#endif

	if (NULL == m_pOwner || false == m_pOwner->IsInitComplete()) return;

	__super::NotifyChange();

	tagDuiRichText RichText = GetRichText();
	m_bModified = RichText.vecRichTextItem.size() > 0
		|| (DT_SINGLELINE | DT_END_ELLIPSIS) != RichText.dwTextStyle;

	return;
}