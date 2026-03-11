#include "StdAfx.h"
#include "DUIAttributeColorSwitch.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeColorSwitch)
MMImplement_ClassName(CDUIAttributeColorSwitch)

CDUIAttributeColorSwitch::CDUIAttributeColorSwitch(LPCTSTR lpszColorSwitch)
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_ColorSwitch;
#endif

	if (lpszColorSwitch)
	{
		SetColorResSwitch({ lpszColorSwitch });
	}

	return;
}

CDUIAttributeColorSwitch::~CDUIAttributeColorSwitch(void)
{
	return;
}

LPVOID CDUIAttributeColorSwitch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeColorSwitch, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeColorSwitch & CDUIAttributeColorSwitch::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeColorSwitch &ColorSwitchRight = (CDUIAttributeColorSwitch&)Right;

	return *this;
}

CMMString CDUIAttributeColorSwitch::FormatInfo()
{
	std::vector<CMMString> vecColorRes = GetColorResSwitch();
	CMMString strFormat;

	//color
	for (auto &strResName : vecColorRes)
	{
		strFormat += strResName;
		strFormat += _T(';');
	}

	return strFormat;
}

bool CDUIAttributeColorSwitch::DrawRect(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, CDUISize szBreakTop)
{
	if (nBorderSize <= 0) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	CDUIRenderEngine::DrawRect(hDC, rcPaint, nBorderSize, dwColor, szBreakTop);

	return true;
}

bool CDUIAttributeColorSwitch::DrawPath(HDC hDC, const std::vector<CDUIPoint> &vecPtList, int nBorderSize)
{
	if (nBorderSize <= 0) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	CDUIRenderEngine::DrawPath(hDC, vecPtList, nBorderSize, dwColor);

	return true;
}

bool CDUIAttributeColorSwitch::DrawRound(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, const CDUIRect &rcRound, bool bHSLAdjust)
{
	if (nBorderSize <= 0
		|| false == (rcRound.left > 0 || rcRound.top > 0 || rcRound.right > 0 || rcRound.bottom > 0)) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);
	}

	CDUIRenderEngine::DrawRound(hDC, rcPaint, rcRound, nBorderSize, dwColor);

	return true;
}

bool CDUIAttributeColorSwitch::DrawRoundRect(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, const CDUIRect &rcRound, bool bHSLAdjust, CDUISize szBreakTop)
{
	if (nBorderSize <= 0
		|| false == (rcRound.left > 0 || rcRound.top > 0 || rcRound.right > 0 || rcRound.bottom > 0)) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);
	}

	CDUIRenderEngine::DrawRoundRect(hDC, rcPaint, rcRound, nBorderSize, dwColor, szBreakTop);

	return true;
}

bool CDUIAttributeColorSwitch::DrawLine(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, enDuiLineStyle LineStyle, bool bHSLAdjust)
{
	if (nBorderSize <= 0) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);
	}

	CDUIRenderEngine::DrawLine(hDC, rcPaint, nBorderSize, dwColor, LineStyle);

	return true;
}

bool CDUIAttributeColorSwitch::DrawParallelogram(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, enDuiLineStyle LineStyle)
{
	if (nBorderSize <= 0) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	CDUIRenderEngine::DrawParallelogram(hDC, rcPaint, nBorderSize, dwColor, LineStyle);

	return true;
}

bool CDUIAttributeColorSwitch::DrawRhomb(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, enDuiLineStyle LineStyle)
{
	if (nBorderSize <= 0) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	CDUIRenderEngine::DrawRhomb(hDC, rcPaint, nBorderSize, dwColor, LineStyle);

	return true;
}

bool CDUIAttributeColorSwitch::DrawEllipse(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, enDuiLineStyle LineStyle)
{
	if (nBorderSize <= 0) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	CDUIRenderEngine::DrawEllipse(hDC, rcPaint, nBorderSize, dwColor, LineStyle);

	return true;
}

bool CDUIAttributeColorSwitch::DrawArc(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, float fStartAngle, float fSweepAngle, enDuiLineStyle LineStyle)
{
	if (nBorderSize <= 0) return false;

	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	CDUIRenderEngine::DrawArc(hDC, rcPaint, nBorderSize, dwColor, fStartAngle, fSweepAngle, LineStyle);

	return true;
}

bool CDUIAttributeColorSwitch::FillRect(HDC hDC, const CDUIRect &rcPaint, bool bHSLAdjust, ARGB dwColorGradient)
{
	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);

		if (0 != dwColorGradient)
		{
			dwColorGradient = CDUIRenderEngine::AdjustColor(dwColorGradient, H, S, L);
		}
	}

	CDUIRenderEngine::FillRect(hDC, rcPaint, dwColor, dwColorGradient);

	return true;
}

bool CDUIAttributeColorSwitch::FillRoundRect(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, const CDUIRect &rcRound, bool bHSLAdjust, ARGB dwColorGradient)
{
	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);

		if (0 != dwColorGradient)
		{
			dwColorGradient = CDUIRenderEngine::AdjustColor(dwColorGradient, H, S, L);
		}
	}

	CDUIRenderEngine::FillRoundRect(hDC, rcPaint, rcRound, nBorderSize, dwColor, dwColorGradient);

	return true;
}

bool CDUIAttributeColorSwitch::FillParallelogram(HDC hDC, const CDUIRect &rcPaint, bool bHSLAdjust, ARGB dwColorGradient)
{
	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);

		if (0 != dwColorGradient)
		{
			dwColorGradient = CDUIRenderEngine::AdjustColor(dwColorGradient, H, S, L);
		}
	}

	CDUIRenderEngine::FillParallelogram(hDC, rcPaint, dwColor, dwColorGradient);

	return true;
}

bool CDUIAttributeColorSwitch::FillRhomb(HDC hDC, const CDUIRect &rcPaint, bool bHSLAdjust, ARGB dwColorGradient)
{
	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);

		if (0 != dwColorGradient)
		{
			dwColorGradient = CDUIRenderEngine::AdjustColor(dwColorGradient, H, S, L);
		}
	}

	CDUIRenderEngine::FillRhomb(hDC, rcPaint, dwColor, dwColorGradient);

	return true;
}

bool CDUIAttributeColorSwitch::FillEllipse(HDC hDC, const CDUIRect &rcPaint, bool bHSLAdjust, ARGB dwColorGradient)
{
	DWORD dwColor = GetColorValue();
	if (0 == dwColor) return false;

	if (bHSLAdjust)
	{
		short H, S, L;
		CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
		dwColor = CDUIRenderEngine::AdjustColor(dwColor, H, S, L);

		if (0 != dwColorGradient)
		{
			dwColorGradient = CDUIRenderEngine::AdjustColor(dwColorGradient, H, S, L);
		}
	}

	CDUIRenderEngine::FillEllipse(hDC, rcPaint, dwColor, dwColorGradient);

	return true;
}

bool CDUIAttributeColorSwitch::IsEmpty()
{
	return GetColorResSwitch().empty();
}

ARGB CDUIAttributeColorSwitch::GetColorValue()
{
	CDUIColorBase *pColorBaseCur = GetColorBaseCur();

	return pColorBaseCur ? pColorBaseCur->GetColor() : 0;
}

Color CDUIAttributeColorSwitch::GetColorObject()
{
	CDUIColorBase *pColorBaseCur = GetColorBaseCur();

	return pColorBaseCur ? pColorBaseCur->GetColor() : 0;
}

vector<CMMString> CDUIAttributeColorSwitch::GetColorResSwitch()
{
	return CDUIGlobal::GetInstance()->GetAttriColorResSwitch(m_uValueHash);
}

bool CDUIAttributeColorSwitch::SetColorResSwitch(const vector<CMMString> &vecColorRes)
{
	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriColorResSwitch(vecColorRes);

	NotifyChange();

	return true;
}

bool CDUIAttributeColorSwitch::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
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

	return true;
}

bool CDUIAttributeColorSwitch::SaveAttribute(tinyxml2::XMLElement *pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	return true;
}

uint32_t CDUIAttributeColorSwitch::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttributeColorSwitch::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}

CDUIColorBase * CDUIAttributeColorSwitch::GetColorBaseCur()
{
	std::vector<CMMString> vecColorRes = GetColorResSwitch();
	int nIndexRes = CDUIGlobal::GetInstance()->GetSwitchResIndex();
	nIndexRes = min(nIndexRes, (int)vecColorRes.size() - 1);
	if (nIndexRes < vecColorRes.size())
	{
		return CDUIGlobal::GetInstance()->GetColorResource(vecColorRes[nIndexRes]);
	}

	return NULL;
}