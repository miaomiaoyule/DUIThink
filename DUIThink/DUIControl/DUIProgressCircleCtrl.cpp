#include "StdAfx.h"
#include "DUIProgressCircleCtrl.h"

DuiImplement_CreateControl(CDUIProgressCircleCtrl)
MMImplement_ClassName(CDUIProgressCircleCtrl)

CDUIProgressCircleCtrl::CDUIProgressCircleCtrl(void)
{
	return;
}

CDUIProgressCircleCtrl::~CDUIProgressCircleCtrl(void)
{
}

bool CDUIProgressCircleCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeSlipWide)
	{
		Invalidate();

		return true;
	}

	return false;
}

LPVOID CDUIProgressCircleCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIProgressCircleCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIProgressCircleCtrl::GetDescribe() const
{
	return Dui_Ctrl_ProgressCircle;
}

int CDUIProgressCircleCtrl::GetSlipWide()
{
	return m_AttributeSlipWide.GetValue();
}

void CDUIProgressCircleCtrl::SetSlipWide(int nWide)
{
	if (nWide < 0 || nWide == GetSlipWide()) return;

	m_AttributeSlipWide.SetValue(nWide);

	Invalidate();

	return;
}

void CDUIProgressCircleCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateAttribute(m_AttributeSlipWide, _T("SlipWide"), _T(""), m_AttributeGroupSlip);

	return;
}

void CDUIProgressCircleCtrl::PaintStatusColor(HDC hDC)
{
	//groove slip
	CDUIAttributeColorSwitch *pAttributeGroove = GetAttributeStatusColorGroove();
	CDUIAttributeColorSwitch *pAttributeSlip = GetAttributeStatusColorSlip();

	//draw
	if (pAttributeGroove)
	{
		pAttributeGroove->FillEllipse(hDC, m_rcGroove, IsColorHSL());
	}
	if (pAttributeSlip)
	{
		pAttributeSlip->DrawArc(hDC, m_rcSlip, GetSlipWide(), 270, GetCurValue() * 1.0 / GetMaxValue() * 360);
	}

	return;
}

void CDUIProgressCircleCtrl::PaintStatusImage(HDC hDC)
{
	//groove slip
	CDUIAttriImageSection *pAttributeGroove = GetAttributeStatusImageGroove();
	CDUIAttriImageSection *pAttributeSlip = GetAttributeStatusImageSlip();

	//draw
	if (pAttributeGroove)
	{
		pAttributeGroove->DrawEllipse(hDC, GetBorderRect(), m_rcGroove);
	}
	if (pAttributeSlip)
	{
		//pAttributeSlip->DrawArc(hDC, m_rcSlip, GetSlipWide(), 90, GetCurValue() * 1.0 / GetMaxValue() * 360);
	}

	return;
}