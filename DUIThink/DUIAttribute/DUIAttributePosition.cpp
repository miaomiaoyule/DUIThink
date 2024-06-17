#include "StdAfx.h"
#include "DUIAttributePosition.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributePosition)
MMImplement_ClassName(CDUIAttributePosition)

CDUIAttributePosition::CDUIAttributePosition()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Position;
#endif

	return;
}

CDUIAttributePosition::~CDUIAttributePosition(void)
{
	return;
}

LPVOID CDUIAttributePosition::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributePosition, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

bool CDUIAttributePosition::SetOwner(CDUIPropertyObject *pOwner)
{
	if (false == __super::SetOwner(pOwner)) return false;

	m_pOwnerCtrl = MMInterfaceHelper(CDUIControlBase, m_pOwner);

	return true;
}

tagDuiPosition CDUIAttributePosition::GetPosition()
{
	return CDUIGlobal::GetInstance()->GetAttriPosition(m_uValueHash);
}

void CDUIAttributePosition::SetPosition(const tagDuiPosition &Position)
{
	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriPosition(Position);

	NotifyChange();

	return;
}

void CDUIAttributePosition::SetFloat(bool bFloat)
{
	if (bFloat == IsFloat()) return;

	tagDuiPosition Position = GetPosition();
	Position.bFloat = bFloat;
	SetPosition(Position);

	NotifyChange();

	return;
}

bool CDUIAttributePosition::IsFloat()
{
	tagDuiPosition Position = GetPosition();

	return Position.bFloat;
}

enDuiHorizAlignType CDUIAttributePosition::GetHorizAlignType()
{
	tagDuiPosition Position = GetPosition();

	return Position.HorizPosition.HorizAlignType;
}

void CDUIAttributePosition::SetHorizAlignType(enDuiHorizAlignType AlignType)
{
	if (AlignType == GetHorizAlignType()) return;

	tagDuiPosition Position = GetPosition();
	Position.HorizPosition.HorizAlignType = AlignType;
	SetPosition(Position);

	NotifyChange();

	return;
}

int	CDUIAttributePosition::GetLeftAlignValue()
{
	if (NULL == m_pOwnerCtrl) return 0;

	tagDuiPosition Position = GetPosition();

	return m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.HorizPosition.nLeftAlignValue) : Position.HorizPosition.nLeftAlignValue;
}

void CDUIAttributePosition::SetLeftAlignValue(int nOffset)
{
	tagDuiPosition Position = GetPosition();
	if (nOffset == Position.HorizPosition.nLeftAlignValue) return;

	Position.HorizPosition.nLeftAlignValue = nOffset;

	SetPosition(Position);

	NotifyChange();

	return;
}

int	CDUIAttributePosition::GetRightAlignValue()
{
	if (NULL == m_pOwnerCtrl) return 0;

	tagDuiPosition Position = GetPosition();

	return m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.HorizPosition.nRightAlignValue) : Position.HorizPosition.nRightAlignValue;
}

void CDUIAttributePosition::SetRightAlignValue(int nOffset)
{
	tagDuiPosition Position = GetPosition();
	if (nOffset == Position.HorizPosition.nRightAlignValue) return;

	Position.HorizPosition.nRightAlignValue = nOffset;

	SetPosition(Position);

	NotifyChange();

	return;
}

int CDUIAttributePosition::GetFixedWidth()
{
	if (NULL == m_pOwnerCtrl) return 0;

	tagDuiPosition Position = GetPosition();

	return DuiDpiScaleAttri(Position.HorizPosition.nFixedWidth);
}

void CDUIAttributePosition::SetFixedWidth(int nWidth)
{
	tagDuiPosition Position = GetPosition();
	if (nWidth == Position.HorizPosition.nFixedWidth) return;

	Position.HorizPosition.nFixedWidth = nWidth;

	SetPosition(Position);

	NotifyChange();

	return;
}

enDuiVertAlignType CDUIAttributePosition::GetVertAlignType()
{
	tagDuiPosition Position = GetPosition();

	return Position.VertPosition.VertAlignType;
}

void CDUIAttributePosition::SetVertAlignType(enDuiVertAlignType AlignType)
{
	if (AlignType == GetVertAlignType()) return;

	tagDuiPosition Position = GetPosition();
	Position.VertPosition.VertAlignType = AlignType;
	SetPosition(Position);

	NotifyChange();

	return;
}

int	CDUIAttributePosition::GetTopAlignValue()
{
	if (NULL == m_pOwnerCtrl) return 0;

	tagDuiPosition Position = GetPosition();

	return m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.VertPosition.nTopAlignValue) : Position.VertPosition.nTopAlignValue;
}

void CDUIAttributePosition::SetTopAlignValue(int nOffset)
{
	tagDuiPosition Position = GetPosition();
	if (nOffset == Position.VertPosition.nTopAlignValue) return;

	Position.VertPosition.nTopAlignValue = nOffset;

	SetPosition(Position);

	NotifyChange();

	return;
}

int CDUIAttributePosition::GetBottomAlignValue()
{
	if (NULL == m_pOwnerCtrl) return 0;

	tagDuiPosition Position = GetPosition();

	return m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.VertPosition.nBottomAlignValue) : Position.VertPosition.nBottomAlignValue;
}

void CDUIAttributePosition::SetBottomAlignValue(int nOffset)
{
	tagDuiPosition Position = GetPosition();
	if (nOffset == Position.VertPosition.nBottomAlignValue) return;

	Position.VertPosition.nBottomAlignValue = nOffset;

	SetPosition(Position);

	NotifyChange();

	return;
}

int CDUIAttributePosition::GetFixedHeight()
{
	if (NULL == m_pOwnerCtrl) return 0;

	tagDuiPosition Position = GetPosition();

	return DuiDpiScaleAttri(Position.VertPosition.nFixedHeight);
}

void CDUIAttributePosition::SetFixedHeight(int nHeight)
{
	tagDuiPosition Position = GetPosition();
	if (nHeight == Position.VertPosition.nFixedHeight) return;

	Position.VertPosition.nFixedHeight = nHeight;

	SetPosition(Position);

	NotifyChange();

	return;
}

bool CDUIAttributePosition::SetPositionFromAbsolute(const CDUIRect &rcAbsolute)
{
	if (NULL == m_pOwnerCtrl || (m_pOwnerCtrl->GetAbsoluteRect() == rcAbsolute)) return true;

	CDUIContainerCtrl *pParentCtrl = m_pOwnerCtrl->GetParent();
	CDUIRect rcModalParentAbs = pParentCtrl ? pParentCtrl->GetAbsoluteRect() : m_pOwnerCtrl->GetModalParentRect();

	//Ïà¶Ô×ø±ê
	CDUIRect rcRelative;
	rcRelative.left = rcAbsolute.left - rcModalParentAbs.left;
	rcRelative.top = rcAbsolute.top - rcModalParentAbs.top;
	rcRelative.right = rcRelative.left + rcAbsolute.GetWidth();
	rcRelative.bottom = rcRelative.top + rcAbsolute.GetHeight();

	rcRelative.CheckRect();
	if (rcRelative.Empty()) return false;

	//float
	m_pOwnerCtrl->SetFloat(true);
	m_pOwnerCtrl->SetPaddingL(0);
	m_pOwnerCtrl->SetPaddingT(0);

	//right
	tagDuiPosition Position = GetPosition();
	Position.HorizPosition.nFixedWidth = rcRelative.GetWidth();

	//left
	switch (Position.HorizPosition.HorizAlignType)
	{
		case HorizAlign_Left:
		{
			Position.HorizPosition.nLeftAlignValue = rcRelative.left;

			break;
		}
		case HorizAlign_Center:
		{
			Position.HorizPosition.nLeftAlignValue = rcRelative.left - (rcModalParentAbs.GetWidth() / 2 - Position.HorizPosition.nFixedWidth / 2);

			break;
		}
		case HorizAlign_Right:
		{
			Position.HorizPosition.nLeftAlignValue = rcModalParentAbs.GetWidth() - rcRelative.left;

			break;
		}
		default:
		{
			break;
		}
	}

	//bottom
	Position.VertPosition.nFixedHeight = rcRelative.GetHeight();

	//top
	switch (Position.VertPosition.VertAlignType)
	{
		case VertAlign_Top:
		{
			Position.VertPosition.nTopAlignValue = rcRelative.top;

			break;
		}
		case VertAlign_Bottom:
		{
			Position.VertPosition.nTopAlignValue = rcModalParentAbs.GetHeight() - rcRelative.top;

			break;
		}
		case VertAlign_Middle:
		{
			Position.VertPosition.nTopAlignValue = rcRelative.top - (rcModalParentAbs.GetHeight() / 2 - Position.VertPosition.nFixedHeight / 2);

			break;
		}
		default:
		{
			break;
		}
	}

	SetPosition(Position);

	m_pOwnerCtrl->m_rcAbsolute = GetAbsoluteRect(rcModalParentAbs);

	return true;
}

CDUIRect CDUIAttributePosition::GetAbsoluteRect(const CDUIRect &rcParentAbs)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return {};

	//layout
	tagDuiPosition Position = GetPosition();
	int nLeftAlignValue = m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.HorizPosition.nLeftAlignValue) : Position.HorizPosition.nLeftAlignValue;
	int nRightAlignValue = m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.HorizPosition.nRightAlignValue) : Position.HorizPosition.nRightAlignValue;
	int nTopAlignValue = m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.VertPosition.nTopAlignValue) : Position.VertPosition.nTopAlignValue;
	int nBottomAlignValue = m_pOwnerCtrl->IsDpiPadding() ? DuiDpiScaleAttri(Position.VertPosition.nBottomAlignValue) : Position.VertPosition.nBottomAlignValue;
	int nFixedWidth = DuiDpiScaleAttri(Position.HorizPosition.nFixedWidth);
	nFixedWidth = min(m_pOwnerCtrl->GetMaxWidth(), nFixedWidth);
	nFixedWidth = max(m_pOwnerCtrl->GetMinWidth(), nFixedWidth);
	int nFixedHeight = DuiDpiScaleAttri(Position.VertPosition.nFixedHeight);
	nFixedHeight = min(m_pOwnerCtrl->GetMaxHeight(), nFixedHeight);
	nFixedHeight = max(m_pOwnerCtrl->GetMinHeight(), nFixedHeight);
	CDUIRect rcRelative;

	//left
	switch (Position.HorizPosition.HorizAlignType)
	{
		case HorizAlign_Left:
		{
			rcRelative.left = nLeftAlignValue;

			break;
		}
		case HorizAlign_Right:
		{
			rcRelative.left = nFixedWidth > 0 ? rcParentAbs.GetWidth() - nRightAlignValue - nFixedWidth : 0;

			break;
		}
		case HorizAlign_Center:
		{
			if (nFixedWidth > 0)
			{
				rcRelative.left = (rcParentAbs.GetWidth() - nRightAlignValue) / 2 - nFixedWidth / 2 + nLeftAlignValue;
			}
			else
			{
				rcRelative.left = (rcParentAbs.GetWidth() - nRightAlignValue) / 2 + nLeftAlignValue;
			}

			break;
		}
		default:
		{
			break;
		}
	}

	//right
	if (nFixedWidth <= 0)
	{
		rcRelative.right = rcParentAbs.GetWidth() - nRightAlignValue;
	}
	else
	{
		rcRelative.right = rcRelative.left + nFixedWidth;
	}

	//top
	switch (Position.VertPosition.VertAlignType)
	{
		case VertAlign_Top:
		{
			rcRelative.top = nTopAlignValue;

			break;
		}
		case VertAlign_Bottom:
		{
			rcRelative.top = nFixedHeight ? rcParentAbs.GetHeight() - nBottomAlignValue - nFixedHeight : 0;

			break;
		}
		case VertAlign_Middle:
		{
			if (nFixedHeight > 0)
			{
				rcRelative.top = (rcParentAbs.GetHeight() - nBottomAlignValue) / 2 - nFixedHeight / 2 + nTopAlignValue;
			}
			else
			{
				rcRelative.top = (rcParentAbs.GetHeight() - nBottomAlignValue) / 2 + nTopAlignValue;
			}

			break;
		}
		default:
		{
			break;
		}
	}

	//bottom
	if (nFixedHeight <= 0)
	{
		rcRelative.bottom = rcParentAbs.GetHeight() - nBottomAlignValue;
	}
	else
	{
		rcRelative.bottom = rcRelative.top + nFixedHeight;
	}

	rcRelative.CheckRect();
	rcRelative.Offset(CDUISize(rcParentAbs.left, rcParentAbs.top));

	return rcRelative;
}

LPCTSTR CDUIAttributePosition::ConvertHorizAlignTypeToString(enDuiHorizAlignType AlignType)
{
	if (HorizAlign_Left == AlignType)
	{
		return Dui_Name_HorizAlignLeft;
	}
	if (HorizAlign_Right == AlignType)
	{
		return Dui_Name_HorizAlignRight;
	}
	if (HorizAlign_Center == AlignType)
	{
		return Dui_Name_HorizAlignCenter;
	}

	return NULL;
}

LPCTSTR CDUIAttributePosition::ConvertVertAlignTypeToString(enDuiVertAlignType AlignType)
{
	if (VertAlign_Top == AlignType)
	{
		return Dui_Name_VertAlignTop;
	}
	if (VertAlign_Bottom == AlignType)
	{
		return Dui_Name_VertAlignBottom;
	}
	if (VertAlign_Middle == AlignType)
	{
		return Dui_Name_VertAlignMiddle;
	}

	return NULL;
}

enDuiHorizAlignType CDUIAttributePosition::ConvertStringToHorizAlignType(CMMString strAlignType)
{
	if (Dui_Name_HorizAlignLeft == strAlignType)
	{
		return HorizAlign_Left;
	}
	if (Dui_Name_HorizAlignRight == strAlignType)
	{
		return HorizAlign_Right;
	}
	if (Dui_Name_HorizAlignCenter == strAlignType)
	{
		return HorizAlign_Center;
	}

	return HorizAlign_Left;
}

enDuiVertAlignType CDUIAttributePosition::ConvertStringToVertAlignType(CMMString strAlignType)
{
	if (Dui_Name_VertAlignTop == strAlignType)
	{
		return VertAlign_Top;
	}
	if (Dui_Name_VertAlignBottom == strAlignType)
	{
		return VertAlign_Bottom;
	}
	if (Dui_Name_VertAlignMiddle == strAlignType)
	{
		return VertAlign_Middle;
	}

	return VertAlign_Top;
}

bool CDUIAttributePosition::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
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

uint32_t CDUIAttributePosition::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttributePosition::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}