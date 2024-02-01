#include "StdAfx.h"
#include "DUIAttributeSize.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeSize)
MMImplement_ClassName(CDUIAttributeSize)

CDUIAttributeSize::CDUIAttributeSize()
	: SIZE{}
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Size;
#endif

	return;
}

CDUIAttributeSize::~CDUIAttributeSize(void)
{
}

LPVOID CDUIAttributeSize::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeSize, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeSize & CDUIAttributeSize::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeSize &SizeRight = (CDUIAttributeSize&)Right;
	cx = SizeRight.cx;
	cy = SizeRight.cy;

	return *this;
}

CDUISize CDUIAttributeSize::GetValue()
{
	return *this;
}

void CDUIAttributeSize::SetValue(long lCx, long lCy)
{
	if (cx == lCx && cy == lCy) return;

	SetValue({ lCx, lCy });

	return;
}

void CDUIAttributeSize::SetValue(const SIZE &szXY)
{
	if (szXY == GetValue()) return;

	cx = szXY.cx;
	cy = szXY.cy;

	NotifyChange();

	return;
}

void CDUIAttributeSize::SetCx(long lCx)
{
	return SetValue(lCx, cy);
}

void CDUIAttributeSize::SetCy(long lCy)
{
	return SetValue(cx, lCy);
}

bool CDUIAttributeSize::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriSizeValue))
	{
		LPSTR pStr = NULL;
		CDUISize szTemp;
		szTemp.cx = strtol(lpszValue, &pStr, 10); ASSERT(pStr);
		szTemp.cy = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);

		SetValue(szTemp);

		return true;
	}

	return false;
}

bool CDUIAttributeSize::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	CDUISize sz = GetValue();
	pNode->SetAttribute(Dui_Key_AttriSizeValue, CMMStrHelp::FormatA("%d,%d", sz.cx, sz.cy).c_str());

	return true;
}