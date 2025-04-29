#include "StdAfx.h"
#include "DUIKLineChartCtrl.h"
#include <atltime.h>

#define Num_KLineChartCount				(80)
#define Dui_Max_KLineChartItemWidth		(5)

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIKLineChartCtrl)
MMImplement_ClassName(CDUIKLineChartCtrl)

CDUIKLineChartCtrl::CDUIKLineChartCtrl()
{
	SYSTEMTIME DateCur;
	GetLocalTime(&DateCur);
	float fStart = 5.0f;
	for (int n = 0; n < 1000; n++)
	{
		CTime tDateCur(DateCur);
		tDateCur -= CTimeSpan(n, 0, 0, 0);

		tagDuiKLineChartItem KLineChartItem = {};
		KLineChartItem.tDate.wYear = tDateCur.GetYear();
		KLineChartItem.tDate.wMonth = tDateCur.GetMonth();
		KLineChartItem.tDate.wDay = tDateCur.GetDay();
		KLineChartItem.fStart = fStart;
		KLineChartItem.fFinish = (0 == rand() % 2) ? fStart + rand() % 3 : fStart - rand() % 3;
		KLineChartItem.fFinish = max(0, KLineChartItem.fFinish);
		KLineChartItem.fFinish == KLineChartItem.fStart ? KLineChartItem.fFinish = KLineChartItem.fStart + 3 : 0;
		float fTop = max(KLineChartItem.fStart, KLineChartItem.fFinish);
		float fBottom = min(KLineChartItem.fStart, KLineChartItem.fFinish);
		KLineChartItem.fLow = fBottom - rand() % 3;
		KLineChartItem.fLow = max(0, KLineChartItem.fLow);
		KLineChartItem.fHigh = fTop + rand() % 3;
		fStart = KLineChartItem.fFinish;

		m_vecKLineChartItem.insert(m_vecKLineChartItem.begin(), KLineChartItem);
	}

	return;
}

CDUIKLineChartCtrl::~CDUIKLineChartCtrl()
{
	return;
}

bool CDUIKLineChartCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj))
	{
		return true;
	}

	return false;
}

LPVOID CDUIKLineChartCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIKLineChartCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIKLineChartCtrl::GetDescribe() const
{
	return Dui_Ctrl_KLineChart;
}

void CDUIKLineChartCtrl::RefreshView()
{
	__super::RefreshView();

	m_szTextPrice = m_AttributeTextStylePrice.MeasureString(_T("00000.000"));
	m_szTextDate = m_AttributeTextStyleDate.MeasureString(_T("2025-12"));
	m_rcKLineChartGridRange = GetAbsoluteRect();
	m_rcKLineChartGridRange.left += m_szTextPrice.cx;
	m_rcKLineChartGridRange.bottom -= m_szTextDate.cy;

	m_szScrollRange.cx = max(0, m_vecKLineChartItem.size() * Dui_Max_KLineChartItemWidth - m_rcKLineChartGridRange.GetWidth());
	-1 == m_szScrollPos.cx ? m_szScrollPos.cx = m_szScrollRange.cx : m_szScrollPos.cx;
	m_szScrollPos.cx = max(0, min(m_szScrollPos.cx, m_szScrollRange.cx));

	return;
}

CDUISize CDUIKLineChartCtrl::GetScrollPos()
{
	return m_szScrollPos;
}

void CDUIKLineChartCtrl::SetScrollPos(CDUISize szPos)
{
	CDUISize szScrollPosOld = GetScrollPos();

	m_szScrollPos = szPos;
	m_szScrollPos.cx = max(m_szScrollPos.cx, 0);
	m_szScrollPos.cx = min(m_szScrollPos.cx, m_szScrollRange.cx);
	m_szScrollPos.cy = max(m_szScrollPos.cy, 0);
	m_szScrollPos.cy = min(m_szScrollPos.cy, m_szScrollRange.cy);
	if (szScrollPosOld == m_szScrollPos) return;

	Invalidate();

	return;
}

CDUISize CDUIKLineChartCtrl::GetScrollRange()
{
	return m_szScrollRange;
}

bool CDUIKLineChartCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (GetScrollRange().cx <= 0) return __super::OnDuiMouseWheel(pt, Msg);

	//direction
	int nWheelDelta = (int)(short)HIWORD(Msg.wParam);
	int nWheelCount = abs(nWheelDelta / WHEEL_DELTA);
	bool bPositive = nWheelDelta > 0;

	if (bPositive)
	{
		CDUISize szScrollPos = GetScrollPos();
		szScrollPos.cx -= Dui_Max_KLineChartItemWidth;
		SetScrollPos(szScrollPos);
	}
	else
	{
		CDUISize szScrollPos = GetScrollPos();
		szScrollPos.cx += Dui_Max_KLineChartItemWidth;
		SetScrollPos(szScrollPos);
	}

	__super::OnDuiMouseWheel(pt, Msg);

	return true;
}

void CDUIKLineChartCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupKLineChart, _T("KLineChart"));
	DuiCreateAttribute(m_AttributeColorGrid, _T("ColorGrid"), _T(""), m_AttributeGroupKLineChart);
	DuiCreateAttribute(m_AttributeTextStylePrice, _T("TextStylePrice"), _T(""), m_AttributeGroupKLineChart);
	DuiCreateAttribute(m_AttributeTextStyleDate, _T("TextStyleDate"), _T(""), m_AttributeGroupKLineChart);

	return;
}

void CDUIKLineChartCtrl::PaintBkColor(HDC hDC)
{
	__super::PaintBkColor(hDC);

	//scene
	float fHigh = 0.0f;
	float fLow = 0x7fffffff;
	int nIndexStart = 0;
	int nIndexFinish = 0;
	int nLeftFrom = 0;
	CalcCurHighLow(fHigh, fLow, nIndexStart, nIndexFinish, nLeftFrom);

	//left line
	CDUIRect rcLine = m_rcKLineChartGridRange;
	rcLine.right = rcLine.left;
	rcLine.top = m_rcKLineChartGridRange.top + m_szTextPrice.cy / 2;
	m_AttributeColorGrid.DrawLine(hDC, rcLine, 1);
	
	//price
	int nHeight = m_rcKLineChartGridRange.GetHeight() - m_szTextPrice.cy / 2;
	int nRowSpace = nHeight / 4;
	float fPriceSpace = (fHigh - fLow) / 4;
	float fPrice = fLow;
	for (int nRowBottom = m_rcKLineChartGridRange.bottom; nRowBottom >= m_rcKLineChartGridRange.top; nRowBottom -= nRowSpace)
	{
		rcLine.right = m_rcKLineChartGridRange.right;
		rcLine.top = rcLine.bottom = nRowBottom;
		m_AttributeColorGrid.DrawLine(hDC, rcLine, 1);

		CDUIRect rcPrice = GetAbsoluteRect();
		rcPrice.top = rcLine.top - m_szTextPrice.cy / 2;
		rcPrice.bottom = rcPrice.top + m_szTextPrice.cy;
		rcPrice.right = m_rcKLineChartGridRange.left;
		fPrice = fLow + (m_rcKLineChartGridRange.bottom - nRowBottom) / nRowSpace * fPriceSpace;
		CMMString strPrice = CMMStrHelp::Format(_T("%0.3f"), fPrice);
		m_AttributeTextStylePrice.Draw(hDC, rcPrice, strPrice, m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), IsShadowText());
	}

	//draw
	for (int n = nIndexStart; n < m_vecKLineChartItem.size() && n <= nIndexFinish; n++)
	{
		tagDuiKLineChartItem Item = m_vecKLineChartItem[n];
		float fTop = max(Item.fStart, Item.fFinish);
		float fBottom = min(Item.fStart, Item.fFinish);
		CDUIRect rcItem = m_rcKLineChartGridRange;

		//kline
		{
			//clip
			CDUIRenderClip Clip;
			Clip.GenerateClip(hDC, m_rcKLineChartGridRange);

			rcItem.top = m_rcKLineChartGridRange.bottom - (fTop - fLow) / (fHigh - fLow) * nHeight;
			rcItem.bottom = m_rcKLineChartGridRange.bottom - (fBottom - fLow) / (fHigh - fLow) * nHeight;
			rcItem.bottom == rcItem.top ? rcItem.bottom = rcItem.top + 1 : rcItem.bottom;
			rcItem.left = nLeftFrom;
			rcItem.right = rcItem.left + Dui_Max_KLineChartItemWidth;
			CMMString strColor = (fTop == Item.fStart ? Name_ColorGreen : Name_ColorRed);
			CDUIRenderEngine::FillRect(hDC, rcItem, _tcstoul(strColor, NULL, 16));

			fTop = Item.fHigh;
			fBottom = Item.fLow;
			rcLine = rcItem;
			rcLine.top = m_rcKLineChartGridRange.bottom - (fTop - fLow) / (fHigh - fLow) * nHeight;
			rcLine.bottom = m_rcKLineChartGridRange.bottom - (fBottom - fLow) / (fHigh - fLow) * nHeight;
			rcLine.bottom == rcLine.top ? rcLine.bottom = rcLine.top + 1 : rcLine.bottom;
			rcLine.left += rcLine.GetWidth() / 2;
			rcLine.right = rcLine.left;
			CDUIRenderEngine::DrawLine(hDC, rcLine, 1, _tcstoul(strColor, NULL, 16));
		}

		//date
		if (1 == Item.tDate.wDay)
		{
			rcLine.top = m_rcKLineChartGridRange.top + m_szTextPrice.cy / 2;
			rcLine.bottom = m_rcKLineChartGridRange.bottom;
			m_AttributeColorGrid.DrawLine(hDC, rcLine, 1);

			rcItem.left -= m_szTextDate.cx / 2;
			rcItem.right = rcItem.left + m_szTextDate.cx;
			rcItem.top = m_rcKLineChartGridRange.bottom;
			rcItem.bottom = GetAbsoluteRect().bottom;
			m_AttributeTextStyleDate.Draw(hDC, rcItem, CMMStrHelp::Format(_T("%d-%d"), Item.tDate.wYear, Item.tDate.wMonth), m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), IsShadowText());
		}

		nLeftFrom += Dui_Max_KLineChartItemWidth;
	}

	return;
}

void CDUIKLineChartCtrl::CalcCurHighLow(OUT float &fHigh, OUT float &fLow, OUT int &nIndexStart, OUT int &nIndexFinish, OUT int &nLeftFrom)
{
	if (m_vecKLineChartItem.empty()) return;

	nIndexStart = (m_szScrollPos.cx + Dui_Max_KLineChartItemWidth - 1) / Dui_Max_KLineChartItemWidth - 1;
	nIndexStart = max(0, nIndexStart);
	nLeftFrom = m_rcKLineChartGridRange.left - (m_szScrollPos.cx - nIndexStart * Dui_Max_KLineChartItemWidth);

	//high low
	int nRightLimit = nLeftFrom;
	for (nIndexFinish = nIndexStart; nIndexFinish < m_vecKLineChartItem.size(); nIndexFinish++)
	{
		nRightLimit += Dui_Max_KLineChartItemWidth;

		fHigh = max(fHigh, m_vecKLineChartItem[nIndexFinish].fHigh);
		fLow = min(fLow, m_vecKLineChartItem[nIndexFinish].fLow);

		if (nRightLimit >= m_rcKLineChartGridRange.right) break;
	}

	return;
}