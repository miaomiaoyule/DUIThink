#include "StdAfx.h"
#include "DUIAttributeHotKey.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeHotKey)
MMImplement_ClassName(CDUIAttributeHotKey)

CDUIAttributeHotKey::CDUIAttributeHotKey()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_HotKey;
#endif

	return;
}

CDUIAttributeHotKey::~CDUIAttributeHotKey(void)
{
}

LPVOID CDUIAttributeHotKey::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeHotKey, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeHotKey & CDUIAttributeHotKey::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeHotKey &HotKeyRight = (CDUIAttributeHotKey&)Right;
	m_cbVKeyCode = HotKeyRight.m_cbVKeyCode;
	m_cbModifierFlag = HotKeyRight.m_cbModifierFlag;

	return *this;
}

WORD CDUIAttributeHotKey::GetValue()
{
	return MAKEWORD(m_cbVKeyCode, m_cbModifierFlag);
}

void CDUIAttributeHotKey::GetValue(BYTE &cbVKeyCode, BYTE &cbModifierFlag)
{
	cbVKeyCode = m_cbVKeyCode;
	cbModifierFlag = m_cbModifierFlag;

	return;
}

void CDUIAttributeHotKey::SetValue(WORD wHotKey)
{
	SetValue(LOBYTE(wHotKey), HIBYTE(wHotKey));

	return;
}

void CDUIAttributeHotKey::SetValue(BYTE cbVKeyCode, BYTE cbModifierFlag)
{
	if (cbVKeyCode == m_cbVKeyCode && cbModifierFlag == m_cbModifierFlag) return;

	m_cbVKeyCode = cbVKeyCode;
	m_cbModifierFlag = cbModifierFlag;

	NotifyChange();

	return;
}

bool CDUIAttributeHotKey::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriHotKeyValue))
	{
		SetValue(strtoul(lpszValue, NULL, 10));

		return true;
	}

	return false;
}

bool CDUIAttributeHotKey::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Key_AttriHotKeyValue, (UINT)GetValue());

	return true;
}