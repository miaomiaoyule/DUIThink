#include "StdAfx.h"
#include "DUIProgressCtrl.h"

DuiImplement_CreateControl(CDUIProgressCtrl)
MMImplement_ClassName(CDUIProgressCtrl)

CDUIProgressCtrl::CDUIProgressCtrl(void)
{
	{
		static tagDuiCombox AttriCombox;
		if (AttriCombox.vecItem.empty())
		{
			AttriCombox.vecItem.push_back({ ProgressType_Horiz, _T("ProgressHoriz") });
			AttriCombox.vecItem.push_back({ ProgressType_Vert, _T("ProgressVert") });
		}

		m_AttributeProgressType.SetCombox(AttriCombox);
		m_AttributeProgressType.SelectItem(ProgressType_Horiz);
	}
	{
		static tagDuiCombox AttriCombox;
		if (AttriCombox.vecItem.empty())
		{
			AttriCombox.vecItem.push_back({ ProgressMove_Positive, _T("MovePositive") });
			AttriCombox.vecItem.push_back({ ProgressMove_Reverse, _T("MoveReverse") });
		}

		m_AttributeProgressMoveType.SetCombox(AttriCombox);
		m_AttributeProgressMoveType.SelectItem(ProgressMove_Positive);
	}

	return;
}

CDUIProgressCtrl::~CDUIProgressCtrl(void)
{
}

bool CDUIProgressCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeCurValue)
	{
		int nCurValue = GetCurValue();
		nCurValue = max(nCurValue, 0);
		nCurValue = min(nCurValue, GetMaxValue());
		SetCurValue(nCurValue);

		return true;
	}
	if (pAttributeObj == &m_AttributeMaxValue)
	{
		SetMaxValue(min(max(GetMaxValue(), 0), 0x7fffffff));

		if (GetMaxValue() <= GetCurValue())
		{
			SetCurValue(GetMaxValue());
		}

		return true;
	}

	return false;
}

LPVOID CDUIProgressCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIProgressCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIProgressCtrl::GetDescribe() const
{
	return Dui_Ctrl_Progress;
}

void CDUIProgressCtrl::RefreshView()
{
	//滑槽滑条
	CalcSubRect();

	//滑条位置
	CalcSubPos();

	__super::RefreshView();

	return;
}

int CDUIProgressCtrl::GetMaxValue()
{
	return m_AttributeMaxValue.GetValue();
}

void CDUIProgressCtrl::SetMaxValue(int nMaxValue)
{
	if (nMaxValue < 0 || nMaxValue == GetMaxValue()) return;

	m_AttributeMaxValue.SetValue(nMaxValue);

	if (nMaxValue <= GetCurValue())
	{
		SetCurValue(nMaxValue);
	}

	//计算滑条位置
	CalcSubPos();

	Invalidate();

	return;
}

int CDUIProgressCtrl::GetCurValue()
{
	return m_AttributeCurValue.GetValue();
}

void CDUIProgressCtrl::SetCurValue(int nCurValue)
{
	if (nCurValue == GetCurValue()) return;

	nCurValue = max(nCurValue, 0);
	nCurValue = min(nCurValue, GetMaxValue());

	m_AttributeCurValue.SetValue(nCurValue);

	//计算滑条位置
	CalcSubPos();

	//notify
	if (GetWndOwner())
	{
		GetWndOwner()->SendNotify(this, DuiNotify_ValueChanged);
	}

	Invalidate();

	return;
}

double CDUIProgressCtrl::GetValueScale()
{
	int nCurValue = m_AttributeCurValue.GetValue(), nMaxValue = m_AttributeMaxValue.GetValue();
	if (nCurValue <= 0 || nMaxValue <= 0) return 0.0;

	return (double)nCurValue / (double)nMaxValue;
}

vector<CMMString> CDUIProgressCtrl::GetColorResSwitchGrooveNormal()
{
	return m_AttributeColorGrooveNormal.GetColorResSwitch();
}

vector<CMMString> CDUIProgressCtrl::GetColorResSwitchGrooveHot()
{
	return m_AttributeColorGrooveHot.GetColorResSwitch();
}

vector<CMMString> CDUIProgressCtrl::GetColorResSwitchSlipNormal()
{
	return m_AttributeColorSlipNormal.GetColorResSwitch();
}

vector<CMMString> CDUIProgressCtrl::GetColorResSwitchSlipHot()
{
	return m_AttributeColorSlipHot.GetColorResSwitch();
}

void CDUIProgressCtrl::SetColorResSwitchGrooveNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorGrooveNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIProgressCtrl::SetColorResSwitchGrooveHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorGrooveHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIProgressCtrl::SetColorResSwitchSlipNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorSlipNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIProgressCtrl::SetColorResSwitchSlipHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorSlipHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

tagDuiImageSection CDUIProgressCtrl::GetImageSectionGrooveNormal()
{
	return m_AttributeImageGrooveNormal.GetImageSection();
}

tagDuiImageSection CDUIProgressCtrl::GetImageSectionGrooveHot()
{
	return m_AttributeImageGrooveHot.GetImageSection();
}

tagDuiImageSection CDUIProgressCtrl::GetImageSectionSlipNormal()
{
	return m_AttributeImageSlipNormal.GetImageSection();
}

tagDuiImageSection CDUIProgressCtrl::GetImageSectionSlipHot()
{
	return m_AttributeImageSlipHot.GetImageSection();
}

void CDUIProgressCtrl::SetImageSectionGrooveNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageGrooveNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIProgressCtrl::SetImageSectionGrooveHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageGrooveHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIProgressCtrl::SetImageSectionSlipNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSlipNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIProgressCtrl::SetImageSectionSlipHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSlipHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

CDUIRect CDUIProgressCtrl::GetGrooveRect()
{
	return m_rcGroove;
}

CDUIRect CDUIProgressCtrl::GetSlipRect()
{
	return m_rcSlip;
}

enDuiProgressType CDUIProgressCtrl::GetProgressType()
{
	return (enDuiProgressType)m_AttributeProgressType.GetCurSelItem();
}

void CDUIProgressCtrl::SetProgressType(enDuiProgressType ProgressType)
{
	if (ProgressType == GetProgressType()) return;

	switch (ProgressType)
	{
		case ProgressType_Horiz:
		{
			m_AttributeProgressType.SelectItem(ProgressType_Horiz);
			m_AttributeProgressMoveType.SelectItem(ProgressMove_Positive);

			break;
		}
		case ProgressType_Vert:
		{
			m_AttributeProgressType.SelectItem(ProgressType_Vert);
			m_AttributeProgressMoveType.SelectItem(ProgressMove_Reverse);

			break;
		}
		default:
			break;
	}

	NeedRefreshView();

	return;
}

enDuiProgressMoveType CDUIProgressCtrl::GetProgressMoveType()
{
	return (enDuiProgressMoveType)m_AttributeProgressMoveType.GetCurSelItem();
}

void CDUIProgressCtrl::SetProgressMoveType(enDuiProgressMoveType ProgressMoveType)
{
	if (ProgressMoveType == GetProgressMoveType()) return;

	switch (ProgressMoveType)
	{
		case ProgressMove_Positive:
		{
			m_AttributeProgressMoveType.SelectItem(ProgressMove_Positive);

			break;
		}
		case ProgressMove_Reverse:
		{
			m_AttributeProgressMoveType.SelectItem(ProgressMove_Reverse);

			break;
		}
		default:
			break;
	}

	NeedRefreshView();

	return;
}

bool CDUIProgressCtrl::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseEnter(pt, Msg);

	return true;
}

void CDUIProgressCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseLeave(pt, Msg);

	return;
}

void CDUIProgressCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupProgressType, _T("ProgressType"));
	DuiCreateAttribute(m_AttributeProgressType, _T("ProgressType"), _T(""), m_AttributeGroupProgressType);
	DuiCreateAttribute(m_AttributeProgressMoveType, _T("MoveType"), _T(""), m_AttributeGroupProgressType);

	DuiCreateGroupAttribute(m_AttributeGroupValue, _T("ProgressValue"));
	DuiCreateAttribute(m_AttributeMaxValue, _T("MaxValue"), _T(""), m_AttributeGroupValue);
	DuiCreateAttribute(m_AttributeCurValue, _T("CurValue"), _T(""), m_AttributeGroupValue);

	DuiCreateGroupAttribute(m_AttributeGroupGroove, _T("Groove"));
	DuiCreateAttribute(m_AttributeColorGrooveNormal, _T("ColorGrooveNormal"), _T(""), m_AttributeGroupGroove);
	DuiCreateAttribute(m_AttributeColorGrooveHot, _T("ColorGrooveHot"), _T(""), m_AttributeGroupGroove);
	DuiCreateAttribute(m_AttributeImageGrooveNormal, _T("ImageGrooveNormal"), _T(""), m_AttributeGroupGroove);
	DuiCreateAttribute(m_AttributeImageGrooveHot, _T("ImageGrooveHot"), _T(""), m_AttributeGroupGroove);
	DuiCreateAttribute(m_AttributeGrooveInsetToBorder, _T("GrooveInsetToBorder"), _T(""), m_AttributeGroupGroove);

	DuiCreateGroupAttribute(m_AttributeGroupSlip, _T("Slip"));
	DuiCreateAttribute(m_AttributeColorSlipNormal, _T("ColorSlipNormal"), _T(""), m_AttributeGroupSlip);
	DuiCreateAttribute(m_AttributeColorSlipHot, _T("ColorSlipHot"), _T(""), m_AttributeGroupSlip);
	DuiCreateAttribute(m_AttributeImageSlipNormal, _T("ImageSlipNormal"), _T(""), m_AttributeGroupSlip);
	DuiCreateAttribute(m_AttributeImageSlipHot, _T("ImageSlipHot"), _T(""), m_AttributeGroupSlip);
	DuiCreateAttribute(m_AttributeSlipInsetToGroove, _T("SlipInsetToGroove"), _T(""), m_AttributeGroupSlip);

	return;
}

void CDUIProgressCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);

	return;
}

void CDUIProgressCtrl::PaintStatusColor(HDC hDC)
{
	__super::PaintStatusColor(hDC);

	//groove slip
	CDUIAttributeColorSwitch *pAttributeGroove = NULL;
	CDUIAttributeColorSwitch *pAttributeSlip = NULL;
	if (false == IsEnabled())
	{
		pAttributeGroove = &m_AttributeColorGrooveNormal;
		pAttributeSlip = &m_AttributeColorSlipNormal;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttributeGroove = &m_AttributeColorGrooveHot;
		pAttributeSlip = &m_AttributeColorSlipHot;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttributeGroove = &m_AttributeColorGrooveHot;
		pAttributeSlip = &m_AttributeColorSlipHot;
	}
	else if (IsFocused())
	{
		pAttributeGroove = &m_AttributeColorGrooveNormal;
		pAttributeSlip = &m_AttributeColorSlipNormal;
	}
	else
	{
		pAttributeGroove = &m_AttributeColorGrooveNormal;
		pAttributeSlip = &m_AttributeColorSlipNormal;
	}

	//verify
	if (NULL == pAttributeGroove || pAttributeGroove->IsEmpty())
	{
		pAttributeGroove = &m_AttributeColorGrooveNormal;
	}
	if (NULL == pAttributeSlip || pAttributeSlip->IsEmpty())
	{
		pAttributeSlip = &m_AttributeColorSlipNormal;
	}

	//draw
	if (pAttributeGroove)
	{
		pAttributeGroove->FillRect(hDC, m_rcGroove, IsColorHSL());
	}
	if (pAttributeSlip)
	{
		pAttributeSlip->FillRect(hDC, m_rcCurSlip, IsColorHSL());
	}

	return;
}

void CDUIProgressCtrl::PaintStatusImage(HDC hDC)
{
	//groove slip
	CDUIAttriImageSection *pAttributeGroove = NULL;
	CDUIAttriImageSection *pAttributeSlip = NULL;
	if (false == IsEnabled())
	{
		pAttributeGroove = &m_AttributeImageGrooveNormal;
		pAttributeSlip = &m_AttributeImageSlipNormal;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttributeGroove = &m_AttributeImageGrooveHot;
		pAttributeSlip = &m_AttributeImageSlipHot;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttributeGroove = &m_AttributeImageGrooveHot;
		pAttributeSlip = &m_AttributeImageSlipHot;
	}
	else if (IsFocused())
	{
		pAttributeGroove = &m_AttributeImageGrooveNormal;
		pAttributeSlip = &m_AttributeImageSlipNormal;
	}
	else
	{
		pAttributeGroove = &m_AttributeImageGrooveNormal;
		pAttributeSlip = &m_AttributeImageSlipNormal;
	}

	//verify
	if (NULL == pAttributeGroove || pAttributeGroove->IsEmpty())
	{
		pAttributeGroove = &m_AttributeImageGrooveNormal;
	}
	if (NULL == pAttributeSlip || pAttributeSlip->IsEmpty())
	{
		pAttributeSlip = &m_AttributeImageSlipNormal;
	}
	if (pAttributeGroove)
	{
		pAttributeGroove->Draw(hDC, m_rcGroove, m_rcPaint);
	}
	if (pAttributeSlip)
	{
		pAttributeSlip->Draw(hDC, m_rcCurSlip, m_rcPaint);
	}

	return;
}

void CDUIProgressCtrl::CalcSubPos()
{
	CalcSlipPos();

	return;
}

void CDUIProgressCtrl::CalcSubRect()
{
	CalcGrooveAndSlipRect();

	return;
}

void CDUIProgressCtrl::CalcGrooveAndSlipRect()
{
	CDUIRect rcGrooveInset = m_AttributeGrooveInsetToBorder.GetValue();
	CDUIRect rcSlipInset = m_AttributeSlipInsetToGroove.GetValue();

	//滑槽
	m_rcGroove = m_rcAbsolute;
	m_rcGroove.Deflate(rcGrooveInset.left, rcGrooveInset.top, rcGrooveInset.right, rcGrooveInset.bottom);
	m_rcGroove.CheckRect();
	m_rcGroove = m_rcGroove.RectClip(m_rcAbsolute, m_rcGroove);

	//滑条
	m_rcSlip = m_rcGroove;
	m_rcSlip.Deflate(rcSlipInset.left, rcSlipInset.top, rcSlipInset.right, rcSlipInset.bottom);
	m_rcSlip.CheckRect();
	m_rcSlip = m_rcSlip.RectClip(m_rcGroove, m_rcSlip);

	return;
}

void CDUIProgressCtrl::CalcSlipPos()
{
	//区分水平类型还是垂直类型
	if (ProgressType_Horiz == GetProgressType())
		CalcHorizSlipPos();
	else if (ProgressType_Vert == GetProgressType())
		CalcVertSlipPos();

	return;
}

void CDUIProgressCtrl::CalcHorizSlipPos()
{
	//偏移单位初始化
	double lfValue = GetValueScale();
	int nCurPos = m_rcSlip.GetWidth() * lfValue;

	CDUIRect rcCurSlip(m_rcSlip.left, m_rcSlip.top, m_rcSlip.right, m_rcSlip.bottom);

	//正向
	if (ProgressMove_Positive == GetProgressMoveType())
	{
		rcCurSlip.right = rcCurSlip.left + nCurPos;
	}
	//反向
	else if (ProgressMove_Reverse == GetProgressMoveType())
	{
		rcCurSlip.left = rcCurSlip.right - nCurPos;
	}

	//返回
	m_rcCurSlip = rcCurSlip;

	return;
}

void CDUIProgressCtrl::CalcVertSlipPos()
{
	//偏移单位初始化
	double lfValue = GetValueScale();
	int nCurPos = m_rcSlip.GetHeight() * lfValue;

	CDUIRect rcCurSlip(m_rcSlip.left, m_rcSlip.top, m_rcSlip.right, m_rcSlip.bottom);

	//正向
	if (ProgressMove_Positive == GetProgressMoveType())
	{
		rcCurSlip.top = rcCurSlip.bottom - nCurPos;
	}
	//反向
	else if (ProgressMove_Reverse == GetProgressMoveType())
	{
		rcCurSlip.bottom = rcCurSlip.top + nCurPos;
	}

	//返回
	m_rcCurSlip = rcCurSlip;

	return;
}