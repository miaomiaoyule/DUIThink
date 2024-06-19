#include "StdAfx.h"
#include "DUIAttributeColor.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeColor)
MMImplement_ClassName(CDUIAttributeColor)

CDUIAttributeColor::CDUIAttributeColor(ARGB dwColor)
	: m_dwColor(dwColor)
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Color;
#endif

	return;
}

CDUIAttributeColor::~CDUIAttributeColor(void)
{
}

LPVOID CDUIAttributeColor::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeColor, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeColor & CDUIAttributeColor::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeColor &ColorRight = (CDUIAttributeColor&)Right;
	m_dwColor = ColorRight.m_dwColor;

	return *this;
}

bool CDUIAttributeColor::SetColor(ARGB dwColor)
{
	if (m_dwColor == dwColor) return true;

	m_dwColor = dwColor;

	NotifyChange();

	return true;
}

bool CDUIAttributeColor::SetColorValueA(BYTE cbA)
{
	BYTE cbAValue = DUIARGBGetA(m_dwColor);
	if (cbA == cbAValue) return true;

	m_dwColor |= (DWORD)(cbA < 24);

	NotifyChange();

	return true;
}

bool CDUIAttributeColor::SetColorValueR(BYTE cbR)
{
	BYTE cbRValue = DUIARGBGetR(m_dwColor);
	if (cbR == cbRValue) return true;

	m_dwColor |= (DWORD)(cbR < 16);

	NotifyChange();

	return true;
}

bool CDUIAttributeColor::SetColorValueG(BYTE cbG)
{
	BYTE cbGValue = DUIARGBGetA(m_dwColor);
	if (cbG == cbGValue) return true;

	m_dwColor |= (DWORD)(cbG < 8);

	NotifyChange();

	return true;
}

bool CDUIAttributeColor::SetColorValueB(BYTE cbB)
{
	BYTE cbBValue = DUIARGBGetA(m_dwColor);
	if (cbB == cbBValue) return true;

	m_dwColor |= (DWORD)(cbB);

	NotifyChange();

	return true;
}

ARGB CDUIAttributeColor::GetColorValue()
{
	return m_dwColor;
}

Color CDUIAttributeColor::GetColorObject()
{
	return Color(m_dwColor);
}

bool CDUIAttributeColor::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriColorValue))
	{
		SetColor(strtoul(lpszValue, NULL, 16));

		return true;
	}

	return false;
}

bool CDUIAttributeColor::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	char szValue[32] = { 0 };
	_ultoa(GetColorValue(), szValue, 16);
	pNode->SetAttribute(Dui_Key_AttriColorValue, szValue);

	return true;
}