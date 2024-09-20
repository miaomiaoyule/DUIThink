#include "StdAfx.h"
#include "DUISliderCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUISliderCtrl)
MMImplement_ClassName(CDUISliderCtrl)

CDUISliderCtrl::CDUISliderCtrl(void)
{
}

CDUISliderCtrl::~CDUISliderCtrl(void)
{
}

LPVOID CDUISliderCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUISliderCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUISliderCtrl::GetDescribe() const
{
	return Dui_Ctrl_Slider;
}

void CDUISliderCtrl::SetThumbSize(CDUISize szThumb)
{
	//区分水平类型还是垂直类型
	if (ProgressType_Horiz == GetProgressType())
	{
		if (Dui_Min_ThumbSize <= szThumb.cx && szThumb.cx <= m_rcSlip.GetWidth())
		{
			m_AttributeThumbHorizFixedWidth.SetValue(szThumb.cx);
		}
	}
	else if (ProgressType_Vert == GetProgressType())
	{
		if (Dui_Min_ThumbSize <= szThumb.cy && szThumb.cy <= m_rcSlip.GetHeight())
		{
			m_AttributeThumbVertFixedHeight.SetValue(szThumb.cy);
		}
	}

	//计算滑条位置
	CalcSubPos();

	Invalidate();

	return;
}

int CDUISliderCtrl::GetThumbVertInsetLeft()
{
	return m_AttributeThumbVertInsetLeft.GetValue();
}

int CDUISliderCtrl::GetThumbVertInsetRight()
{
	return m_AttributeThumbVertInsetRight.GetValue();
}

int CDUISliderCtrl::GetThumbVertFixedHeight()
{
	return m_AttributeThumbVertFixedHeight.GetValue();
}

int CDUISliderCtrl::GetThumbHorizInsetTop()
{
	return m_AttributeThumbHorizInsetTop.GetValue();
}

int CDUISliderCtrl::GetThumbHorizInsetBottom()
{
	return m_AttributeThumbHorizInsetBottom.GetValue();
}

int CDUISliderCtrl::GetThumbHorizFixedWidth()
{
	return m_AttributeThumbHorizFixedWidth.GetValue();
}

void CDUISliderCtrl::SetThumbVertInsetLeft(int nLeft)
{
	if (nLeft == GetThumbVertInsetLeft()) return;

	m_AttributeThumbVertInsetLeft.SetValue(nLeft);

	NeedRefreshView();

	return;
}

void CDUISliderCtrl::SetThumbVertInsetRight(int nRight)
{
	if (nRight == GetThumbVertInsetRight()) return;

	m_AttributeThumbVertInsetRight.SetValue(nRight);

	NeedRefreshView();

	return;
}

void CDUISliderCtrl::SetThumbVertFixedHeight(int nHeight)
{
	if (nHeight == GetThumbVertFixedHeight()) return;

	m_AttributeThumbVertFixedHeight.SetValue(nHeight);

	NeedRefreshView();

	return;
}

void CDUISliderCtrl::SetThumbHorizInsetTop(int nTop)
{
	if (nTop == GetThumbHorizInsetTop()) return;

	m_AttributeThumbHorizInsetTop.SetValue(nTop);

	NeedRefreshView();

	return;
}

void CDUISliderCtrl::SetThumbHorizInsetBottom(int nBottom)
{
	if (nBottom == GetThumbHorizInsetBottom()) return;

	m_AttributeThumbHorizInsetBottom.SetValue(nBottom);

	NeedRefreshView();

	return;
}

void CDUISliderCtrl::SetThumbHorizFixedWidth(int nWidth)
{
	if (nWidth == GetThumbHorizFixedWidth()) return;

	m_AttributeThumbHorizFixedWidth.SetValue(nWidth);

	NeedRefreshView();

	return;
}

vector<CMMString> CDUISliderCtrl::GetColorResSwitchThumbNormal()
{
	return m_AttributeColorThumbNormal.GetColorResSwitch();
}

vector<CMMString> CDUISliderCtrl::GetColorResSwitchThumbHot()
{
	return m_AttributeColorThumbHot.GetColorResSwitch();
}

vector<CMMString> CDUISliderCtrl::GetColorResSwitchThumbPushed()
{
	return m_AttributeColorThumbPushed.GetColorResSwitch();
}

void CDUISliderCtrl::SetColorResSwitchThumbNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorThumbNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUISliderCtrl::SetColorResSwitchThumbHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorThumbHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUISliderCtrl::SetColorResSwitchThumbPushed(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorThumbPushed.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

tagDuiImageSection CDUISliderCtrl::GetImageSectionThumbNormal()
{
	return m_AttributeImageThumbNormal.GetImageSection();
}

tagDuiImageSection CDUISliderCtrl::GetImageSectionThumbHot()
{
	return m_AttributeImageThumbHot.GetImageSection();
}

tagDuiImageSection CDUISliderCtrl::GetImageSectionThumbPushed()
{
	return m_AttributeImageThumbPushed.GetImageSection();
}

void CDUISliderCtrl::SetImageSectionThumbNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageThumbNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUISliderCtrl::SetImageSectionThumbHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageThumbHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUISliderCtrl::SetImageSectionThumbPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageThumbPushed.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUISliderCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupThumbVert, _T("ThumbVert"));
	DuiCreateAttribute(m_AttributeThumbVertInsetLeft, _T("ThumbVertInsetLeft"), _T(""), m_AttributeGroupThumbVert);
	DuiCreateAttribute(m_AttributeThumbVertInsetRight, _T("ThumbVertInsetRight"), _T(""), m_AttributeGroupThumbVert);
	DuiCreateAttribute(m_AttributeThumbVertFixedHeight, _T("ThumbVertFixedHeight"), _T(""), m_AttributeGroupThumbVert);

	DuiCreateGroupAttribute(m_AttributeGroupThumbHoriz, _T("ThumbHoriz"));
	DuiCreateAttribute(m_AttributeThumbHorizInsetTop, _T("ThumbHorizInsetTop"), _T(""), m_AttributeGroupThumbHoriz);
	DuiCreateAttribute(m_AttributeThumbHorizInsetBottom, _T("ThumbHorizInsetBottom"), _T(""), m_AttributeGroupThumbHoriz);
	DuiCreateAttribute(m_AttributeThumbHorizFixedWidth, _T("ThumbHorizFixedWidth"), _T(""), m_AttributeGroupThumbHoriz);

	DuiCreateGroupAttribute(m_AttributeGroupThumbStatus, _T("ThumbStatus"));
	DuiCreateAttribute(m_AttributeColorThumbNormal, _T("ColorThumbNormal"), _T(""), m_AttributeGroupThumbStatus);
	DuiCreateAttribute(m_AttributeColorThumbHot, _T("ColorThumbHot"), _T(""), m_AttributeGroupThumbStatus);
	DuiCreateAttribute(m_AttributeColorThumbPushed, _T("ColorThumbPushed"), _T(""), m_AttributeGroupThumbStatus);
	DuiCreateAttribute(m_AttributeImageThumbNormal, _T("ImageThumbNormal"), _T(""), m_AttributeGroupThumbStatus);
	DuiCreateAttribute(m_AttributeImageThumbHot, _T("ImageThumbHot"), _T(""), m_AttributeGroupThumbStatus);
	DuiCreateAttribute(m_AttributeImageThumbPushed, _T("ImageThumbPushed"), _T(""), m_AttributeGroupThumbStatus);

	return;
}

void CDUISliderCtrl::PaintStatusColor(HDC hDC)
{
	__super::PaintStatusColor(hDC);

	//滑块
	CDUIAttributeColorSwitch *pAttribute = NULL;
	if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeColorThumbPushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeColorThumbHot;
	}
	else
	{
		pAttribute = &m_AttributeColorThumbNormal;
	}

	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& false == m_AttributeColorThumbNormal.IsEmpty())
	{
		pAttribute = &m_AttributeColorThumbNormal;
	}
	if (pAttribute)
	{
		pAttribute->FillRect(hDC, m_rcCurThumb, IsColorHSL());
	}

	return;
}

void CDUISliderCtrl::PaintStatusImage(HDC hDC)
{
	__super::PaintStatusImage(hDC);

	CDUIAttriImageSection* pAttribute = NULL;
	if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeImageThumbPushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeImageThumbHot;
	}
	else
	{
		pAttribute = &m_AttributeImageThumbNormal;
	}

	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& false == m_AttributeImageThumbNormal.IsEmpty())
	{
		pAttribute = &m_AttributeImageThumbNormal;
	}
	if (pAttribute)
	{
		pAttribute->Draw(hDC, m_rcCurThumb, m_rcPaint);
	}

	return;
}

bool CDUISliderCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiLButtonDown(pt, Msg);

	m_ptLButtonDown = pt;

	CalcThumbPosOnMouseDown(pt);

	Invalidate();

	return true;
}

bool CDUISliderCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	return __super::OnDuiLButtonUp(pt, Msg);
}

bool CDUISliderCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (m_pWndOwner)
	{
		m_pWndOwner->SendNotify(this, DuiNotify_MouseMove, Msg.wParam, Msg.lParam);
	}
	if (0 == (m_nControlStatus & ControlStatus_Pushed) && m_rcCurThumb.PtInRect(pt))
	{
		m_nControlStatus |= ControlStatus_Hot;
	}

	//滑动滑块
	if ((m_nControlStatus & ControlStatus_Pushed))
	{
		CDUIPoint ptScroll = m_ptThumbLButtonDown;

		ptScroll.x += (pt.x - m_ptLButtonDown.x);
		ptScroll.y += (pt.y - m_ptLButtonDown.y);

		CalcCurValueFromPt(ptScroll);
	}

	Invalidate();

	return true;
}

void CDUISliderCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	Invalidate();

	return __super::OnDuiMouseLeave(pt, Msg);
}

LRESULT CDUISliderCtrl::OnDuiKeyDown(const DuiMessage &Msg)
{
	LRESULT lRes = __super::OnDuiKeyDown(Msg);

	switch (Msg.chKey)
	{
		case VK_LEFT:
		{
			if (ProgressType_Horiz == GetProgressType())
			{
				SetCurValue(max(GetCurValue() - Dui_ScrollSpeed_Normal, 0));
			}

			break;
		}
		case VK_RIGHT:
		{
			if (ProgressType_Horiz == GetProgressType())
			{
				SetCurValue(max(GetCurValue() + Dui_ScrollSpeed_Normal, 0));
			}

			break;
		}
		case VK_UP:
		{
			if (ProgressType_Vert == GetProgressType())
			{
				SetCurValue(max(GetCurValue() - Dui_ScrollSpeed_Normal, 0));
			}

			break;
		}
		case VK_DOWN:
		{
			if (ProgressType_Vert == GetProgressType())
			{
				SetCurValue(max(GetCurValue() + Dui_ScrollSpeed_Normal, 0));
			}

			break;
		}
	}

	return lRes;
}

void CDUISliderCtrl::CalcSubPos()
{
	__super::CalcSubPos();

	CalcThumbPos();

	return;
}

void CDUISliderCtrl::CalcThumbPos()
{
	if (ProgressType_Horiz == GetProgressType())
	{
		CalcThumbPosHoriz();
	}
	else if (ProgressType_Vert == GetProgressType())
	{
		CalcThumbPosVert();
	}

	return;
}

void CDUISliderCtrl::CalcThumbPosHoriz()
{
	//偏移单位初始化
	int nThumbInsetTop = m_AttributeThumbHorizInsetTop.GetValue(), nThumbInsetBottom = m_AttributeThumbHorizInsetBottom.GetValue();
	int nThumbFixedWidth = m_AttributeThumbHorizFixedWidth.GetValue();
	nThumbFixedWidth = max(nThumbFixedWidth, Dui_Min_ThumbSize);

	//偏移单位计算
	int nLenth = m_rcSlip.GetWidth() - nThumbFixedWidth;
	nLenth = max(nLenth, 0);
	double lfValue = GetValueScale();

	//滑块位置
	CDUIRect rcCurThumb(m_rcSlip.left, m_rcAbsolute.top + nThumbInsetTop, m_rcSlip.right, m_rcAbsolute.bottom - nThumbInsetBottom);

	//正向
	if (ProgressMove_Positive == GetProgressMoveType())
	{
		rcCurThumb.left = rcCurThumb.left + nLenth * lfValue;
		rcCurThumb.right = rcCurThumb.left + nThumbFixedWidth;
	}
	//反向
	else if (ProgressMove_Reverse == GetProgressMoveType())
	{
		rcCurThumb.right = rcCurThumb.right - nLenth * lfValue;
		rcCurThumb.left = rcCurThumb.right - nThumbFixedWidth;
	}

	//返回
	m_rcCurThumb = rcCurThumb.RectClip(m_rcAbsolute, rcCurThumb);

	return;
}

void CDUISliderCtrl::CalcThumbPosVert()
{
	//偏移单位初始化
	int nThumbInsetLeft = m_AttributeThumbVertInsetLeft.GetValue(), nThumbInsetRight = m_AttributeThumbVertInsetRight.GetValue();
	int nThumbFixedHeight = m_AttributeThumbVertFixedHeight.GetValue();
	nThumbFixedHeight = max(nThumbFixedHeight, Dui_Min_ThumbSize);

	//偏移单位计算
	int nLenth = m_rcSlip.GetHeight() - nThumbFixedHeight;
	nLenth = max(nLenth, 0);
	double lfValue = GetValueScale();

	//滑块位置
	CDUIRect rcCurThumb(m_rcAbsolute.left + nThumbInsetLeft, m_rcSlip.top, m_rcAbsolute.right - nThumbInsetRight, m_rcSlip.bottom);

	//正向
	if (ProgressMove_Positive == GetProgressMoveType())
	{
		rcCurThumb.bottom = rcCurThumb.bottom - nLenth * lfValue;
		rcCurThumb.top = rcCurThumb.bottom - nThumbFixedHeight;
	}
	//反向
	else if (ProgressMove_Reverse == GetProgressMoveType())
	{
		rcCurThumb.top = rcCurThumb.top + nLenth * lfValue;
		rcCurThumb.bottom = rcCurThumb.top + nThumbFixedHeight;
	}

	//返回
	m_rcCurThumb = rcCurThumb.RectClip(m_rcAbsolute, rcCurThumb);

	return;
}

void CDUISliderCtrl::CalcCurValueFromPt(const CDUIPoint &pt)
{
	if (ProgressType_Horiz == GetProgressType())
	{
		CalcCurValueFromPtHoriz(pt);
	}
	else if (ProgressType_Vert == GetProgressType())
	{
		CalcCurValueFromPtVert(pt);
	}

	return;
}

void CDUISliderCtrl::CalcCurValueFromPtHoriz(const CDUIPoint &pt)
{
	CDUIPoint ptMouse = pt;
	ptMouse.x = (ptMouse.x < m_rcSlip.left) ? m_rcSlip.left : ptMouse.x;
	ptMouse.x = (ptMouse.x > m_rcSlip.right) ? m_rcSlip.right : ptMouse.x;

	//偏移单位初始化
	int nMaxValue = GetMaxValue();
	int nThumbFixedWidth = m_AttributeThumbHorizFixedWidth.GetValue();

	//偏移单位计算
	int nLenth = m_rcSlip.GetWidth() - nThumbFixedWidth;
	nLenth = max(nLenth, 1);

	//正向
	if (ProgressMove_Positive == GetProgressMoveType())
	{
		double lfValue = ((double)(ptMouse.x) - (double)(m_rcSlip.left)) / (double)nLenth;
		SetCurValue(nMaxValue * lfValue);
	}
	//反向
	else if (ProgressMove_Reverse == GetProgressMoveType())
	{
		double lfValue = ((double)(m_rcSlip.right) - (double)(ptMouse.x)) / (double)nLenth;
		SetCurValue(nMaxValue * lfValue);
	}

	return;
}

void CDUISliderCtrl::CalcCurValueFromPtVert(const CDUIPoint &pt)
{
	CDUIPoint ptMouse = pt;
	ptMouse.y = (ptMouse.y < m_rcSlip.top) ? m_rcSlip.top : ptMouse.y;
	ptMouse.y = (ptMouse.y > m_rcSlip.bottom) ? m_rcSlip.bottom : ptMouse.y;

	//偏移单位初始化
	int nMaxValue = GetMaxValue();
	int nThumbFixedHeight = m_AttributeThumbVertFixedHeight.GetValue();

	//偏移单位计算
	int nLenth = m_rcSlip.GetHeight() - nThumbFixedHeight;
	nLenth = max(nLenth, 1);

	//正向
	if (ProgressMove_Positive == GetProgressMoveType())
	{
		double lfValue = ((double)(m_rcSlip.bottom) - (double)(ptMouse.y)) / (double)nLenth;
		SetCurValue(nMaxValue * lfValue);
	}
	//反向
	else if (ProgressMove_Reverse == GetProgressMoveType())
	{
		double lfValue = ((double)(ptMouse.y) - (double)(m_rcSlip.top)) / (double)nLenth;
		SetCurValue(nMaxValue * lfValue);
	}

	return;
}

void CDUISliderCtrl::CalcThumbPosOnMouseDown(const CDUIPoint &pt)
{
	if (false == m_rcCurThumb.PtInRect(pt))
	{
		int nFixedWidth = 0, nFixedHeight = 0;
		nFixedWidth = m_AttributeThumbHorizFixedWidth.GetValue();
		nFixedHeight = m_AttributeThumbVertFixedHeight.GetValue();

		CDUIPoint ptTemp;

		//区分正向类型还是反向类型
		if (ProgressMove_Positive == GetProgressMoveType())
		{
			ptTemp = CDUIPoint(m_rcCurThumb.left, m_rcCurThumb.bottom);
			ptTemp.x += (pt.x - ptTemp.x) - nFixedWidth / 2;
			ptTemp.y += (pt.y - ptTemp.y) + nFixedHeight / 2;
		}
		else if (ProgressMove_Reverse == GetProgressMoveType())
		{
			ptTemp = CDUIPoint(m_rcCurThumb.right, m_rcCurThumb.top);
			ptTemp.x += (pt.x - ptTemp.x) + nFixedWidth / 2;
			ptTemp.y += (pt.y - ptTemp.y) - nFixedHeight / 2;
		}

		//从新计算CurValue
		CalcCurValueFromPt(ptTemp);
	}

	//记录初始滑块坐标
	if (ProgressMove_Positive == GetProgressMoveType())
	{
		m_ptThumbLButtonDown.x = m_rcCurThumb.left;
		m_ptThumbLButtonDown.y = m_rcCurThumb.bottom;
	}
	else if (ProgressMove_Reverse == GetProgressMoveType())
	{
		m_ptThumbLButtonDown.x = m_rcCurThumb.right;
		m_ptThumbLButtonDown.y = m_rcCurThumb.top;
	}

	return;
}