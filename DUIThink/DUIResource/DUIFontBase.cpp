#include "StdAfx.h"
#include "DUIFontBase.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateResource(CDUIFontBase)
MMImplement_ClassName(CDUIFontBase)

CDUIFontBase::CDUIFontBase(void)
{
	return;
}

CDUIFontBase::CDUIFontBase(LPCTSTR lpszResName, LPCTSTR lpszFontName, int nSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut)
	: m_strFontName(lpszFontName)
	, m_nSize(nSize)
	, m_lWeight(lWeight)
	, m_bItalic(bItalic)
	, m_bUnderline(bUnderline)
	, m_bStrikeOut(bStrikeOut)
{
	m_strName = lpszResName;

	return;
}

CDUIFontBase::~CDUIFontBase(void)
{
	ReleaseResource();

	return;
}

LPVOID CDUIFontBase::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIFontBase, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

enDuiResType CDUIFontBase::GetResourceType() const
{
	return DuiResType_Font;
}

HFONT CDUIFontBase::GetHandle(int nScale)
{
	if (NULL == m_mapDpiFont[nScale])
	{
		ConstructResource(nScale);
	}

	return m_mapDpiFont[nScale];
}

CMMString CDUIFontBase::GetFontName()
{
	return m_strFontName;
}

void CDUIFontBase::SetFontName(LPCTSTR lpszName)
{
	if (lpszName == GetFontName()) return;

	m_strFontName = lpszName;

	ConstructResource();

	return;
}

int CDUIFontBase::GetSize(int nScale)
{
	//pre gethandle m_nScale is 0
	GetHandle(nScale);

	return MulDiv(m_nSize, nScale, 100);
}

void CDUIFontBase::SetSize(int nSize)
{
	if (nSize == GetSize(100)) return;

	m_nSize = nSize;

	ReleaseResource();
	ConstructResource();

	return;
}

LONG CDUIFontBase::GetWeight()
{
	return m_lWeight;
}

void CDUIFontBase::SetWeight(LONG lWeight)
{
	if (lWeight == GetWeight()) return;

	m_lWeight = lWeight;

	ConstructResource();

	return;
}

bool CDUIFontBase::IsBold()
{
	return GetWeight() > 400;
}

void CDUIFontBase::SetBold(bool bBold)
{
	return SetWeight(600);
}

bool CDUIFontBase::IsItalic()
{
	return m_bItalic;
}

void CDUIFontBase::SetItalic(bool bItalic)
{
	if (bItalic == IsItalic()) return;

	m_bItalic = bItalic;

	ConstructResource();

	return;
}

bool CDUIFontBase::IsUnderline()
{
	return m_bUnderline;
}

void CDUIFontBase::SetUnderLine(bool bUnderLine)
{
	if (bUnderLine == IsUnderline()) return;

	m_bUnderline = bUnderLine;

	ConstructResource();

	return;
}

bool CDUIFontBase::IsStrikeOut()
{
	return m_bStrikeOut;
}

void CDUIFontBase::SetStrikeOut(bool bStrikeOut)
{
	if (bStrikeOut == IsStrikeOut()) return;

	m_bStrikeOut = bStrikeOut;

	ConstructResource();

	return;
}

void CDUIFontBase::ConstructResource(int nScale)
{
	if (m_mapDpiFont[nScale]) return;

	//release
	MMSafeDeleteObject(m_mapDpiFont[nScale]);

	//create
	LOGFONT lf = {};
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcsncpy(lf.lfFaceName, m_strFontName, LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfQuality = CLEARTYPE_QUALITY;
	lf.lfHeight = -MulDiv(m_nSize, nScale, 100);
	lf.lfWeight = m_lWeight;
	lf.lfItalic = m_bItalic;
	lf.lfUnderline = m_bUnderline;
	lf.lfStrikeOut = m_bStrikeOut;
	m_mapDpiFont[nScale] = ::CreateFontIndirect(&lf);

	/*if (m_DefaultFontInfo.tm.tmHeight == 0)
	{
	HFONT hOldFont = (HFONT) ::SelectObject(m_hDCPaint, m_DefaultFontInfo.hFont);
	::GetTextMetrics(m_hDCPaint, &m_DefaultFontInfo.tm);
	::SelectObject(m_hDCPaint, hOldFont);
	}*/

	return;
}

void CDUIFontBase::ReleaseResource()
{
	for (auto &FontItem : m_mapDpiFont)
	{
		MMSafeDeleteObject(FontItem.second);
	}

	m_mapDpiFont.clear();

	return;
}

bool CDUIFontBase::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Resource_Key_FontName))
	{
		m_strFontName = CA2CT(lpszValue, CP_UTF8);

		return true;
	}
	if (0 == strcmp(lpszName, Dui_Resource_Key_FontSize))
	{
		m_nSize = atol(lpszValue);

		return true;
	}
	if (0 == strcmp(lpszName, Dui_Resource_Key_FontBold))
	{
		m_lWeight = atol(lpszValue);

		return true;
	}
	if (0 == strcmp(lpszName, Dui_Resource_Key_FontItalic))
	{
		m_bItalic = atol(lpszValue);

		return true;
	}
	if (0 == strcmp(lpszName, Dui_Resource_Key_FontUnderLine))
	{
		m_bUnderline = atol(lpszValue);

		return true;
	}
	if (0 == strcmp(lpszName, Dui_Resource_Key_FontStrikeOut))
	{
		m_bStrikeOut = atol(lpszValue);

		return true;
	}

	return false;
}

bool CDUIFontBase::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Resource_Key_FontName, (LPSTR)CT2CA(m_strFontName, CP_UTF8));
	pNode->SetAttribute(Dui_Resource_Key_FontSize, m_nSize);
	pNode->SetAttribute(Dui_Resource_Key_FontBold, m_lWeight);
	pNode->SetAttribute(Dui_Resource_Key_FontItalic, (int)m_bItalic);
	pNode->SetAttribute(Dui_Resource_Key_FontUnderLine, (int)m_bUnderline);
	pNode->SetAttribute(Dui_Resource_Key_FontStrikeOut, (int)m_bStrikeOut);

	return true;
}

void CDUIFontBase::OnDpiChanged(int nScale)
{
	ConstructResource(nScale);

	return __super::OnDpiChanged(nScale);
}

CMMString CDUIFontBase::FormatFontDescribe(CMMString strName, int nSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut)
{
	CMMString strResName;
	strResName.Format(_T("%s-字号:%d-粗细:%d-%s-%s-%s"), strName.c_str(), nSize, lWeight, bItalic ? _T("斜体") : _T("非斜体"),\
		bUnderline ? _T("下划线") : _T("无下划线"), bStrikeOut ? _T("删除线") : _T("无删除线"));

	return strResName;
}