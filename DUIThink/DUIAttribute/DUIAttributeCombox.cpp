#include "StdAfx.h"
#include "DUIAttributeCombox.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeCombox)
MMImplement_ClassName(CDUIAttributeCombox)

CDUIAttributeCombox::CDUIAttributeCombox()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Combox;
#endif

	return;
}

CDUIAttributeCombox::~CDUIAttributeCombox(void)
{
	return;
}

LPVOID CDUIAttributeCombox::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeCombox, Guid, dwQueryVer);
	
	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeCombox & CDUIAttributeCombox::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeCombox &ComboxRight = (CDUIAttributeCombox&)Right;
	m_nCurSel = ComboxRight.m_nCurSel;

	return *this;
}

int CDUIAttributeCombox::GetItemCount()
{
	tagDuiCombox AttriCombox = GetCombox();

	return AttriCombox.vecItem.size();
}

tagDuiCombox CDUIAttributeCombox::GetCombox()
{
	return CDUIGlobal::GetInstance()->GetAttriCombox(m_uValueHash);
}

void CDUIAttributeCombox::SetCombox(const tagDuiCombox &AttriCombox)
{
	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriCombox(AttriCombox);

	return;
}

tagDuiComboxItem CDUIAttributeCombox::GetItemAt(int nIndex)
{
	tagDuiCombox AttriCombox = GetCombox();

	if (nIndex < 0 || nIndex >= AttriCombox.vecItem.size()) return tagDuiComboxItem();

	return AttriCombox.vecItem[nIndex];
}

void CDUIAttributeCombox::RemoveAll()
{
	m_nCurSel = -1;

	m_uValueHash = 0;

	return;
}

void CDUIAttributeCombox::InitSelectItem()
{
	return;
}

int CDUIAttributeCombox::GetCurSelItem()
{
	tagDuiCombox AttriCombox = GetCombox();

	if (m_nCurSel < 0 || m_nCurSel >= AttriCombox.vecItem.size()) return 0;

	return AttriCombox.vecItem[m_nCurSel].nItem;
}

CMMString CDUIAttributeCombox::GetCurSelDescribe()
{
	tagDuiCombox AttriCombox = GetCombox();

	if (m_nCurSel < 0 || m_nCurSel >= AttriCombox.vecItem.size()) return _T("");

	return AttriCombox.vecItem[m_nCurSel].strDescribe;
}

void CDUIAttributeCombox::SetCurSelIndex(int nIndex)
{
	tagDuiCombox AttriCombox = GetCombox();

	if (nIndex < 0 || nIndex >= AttriCombox.vecItem.size() || m_nCurSel == nIndex) return;

	m_nCurSel = nIndex;

	NotifyChange();

	return;
}

void CDUIAttributeCombox::SelectItem(int nItem)
{
	tagDuiCombox AttriCombox = GetCombox();

	for (int n = 0; n < AttriCombox.vecItem.size(); n++)
	{
		if (AttriCombox.vecItem[n].nItem == nItem)
		{
			if (m_nCurSel != n)
			{
				m_nCurSel = n;

				NotifyChange();
			}

			break;
		}
	}

	return;
}

void CDUIAttributeCombox::SelectItem(CMMString strDescribe)
{
	tagDuiCombox AttriCombox = GetCombox();

	for (int n = 0; n < AttriCombox.vecItem.size(); n++)
	{
		if (AttriCombox.vecItem[n].strDescribe == strDescribe)
		{
			if (m_nCurSel != n)
			{
				m_nCurSel = n;

				NotifyChange();
			}

			break;
		}
	}

	return;
}

bool CDUIAttributeCombox::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
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
	if (0 == strcmp(lpszName, Dui_Key_AttriComboxCur))
	{
		m_nCurSel = strtol(lpszValue, NULL, 10);

		return true;
	}

	return false;
}

bool CDUIAttributeCombox::SaveAttribute(tinyxml2::XMLElement *pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Key_AttriComboxCur, m_nCurSel);

	return true;
}

uint32_t CDUIAttributeCombox::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttributeCombox::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}