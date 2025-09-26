#ifndef __DUIKLINECHARTCTRL_H__
#define __DUIKLINECHARTCTRL_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIKLineChartCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIKLineChartCtrl = { 0xC88570C2,0x1D5F,0x461C,0x96,0xDE,0x73,0x38,0xAF,0xFD,0x2B,0xFC }; 
class DUITHINK_API CDUIKLineChartCtrl
	: public CDUIControlBase
{
	DuiDeclare_CreateControl(CDUIKLineChartCtrl)
	MMDeclare_ClassName(CDUIKLineChartCtrl)

public:
	CDUIKLineChartCtrl();
	~CDUIKLineChartCtrl();

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupKLineChart;
	CDUIAttributeColorSwitch			m_AttributeColorGrid;
	CDUIAttributeTextStyle				m_AttributeTextStylePrice;
	CDUIAttributeTextStyle				m_AttributeTextStyleDate;

	//variant
protected:
	VecDuiKLineChartItem				m_vecKLineChartItem;
	enDuiKLineChartType					m_KLineChartType = KLineChart_Day;
	CDUISize							m_szScrollPos = { -1, -1 };
	CDUISize							m_szScrollRange = { -1, -1 };
	CDUISize							m_szTextPrice;
	CDUISize							m_szTextDate;
	CDUIRect							m_rcKLineChartGridRange;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method 
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void RefreshView() override;

	//kline chart
	virtual CDUISize GetScrollPos();
	virtual void SetScrollPos(CDUISize szPos);
	virtual CDUISize GetScrollRange();

	//message
protected:
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	//property
protected:
	void InitProperty() override;

	//paint
protected:
	void PaintBkColor(HDC hDC) override;

	//help
protected:
	void CalcCurHighLow(OUT float &fHigh, OUT float &fLow, OUT int &nIndexStart, OUT int &nIndexFinish, OUT int &nLeftFrom);
};

//////////////////////////////////////////////////////////////////////////
#endif