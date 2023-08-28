#include "StdAfx.h"
#include "DUIScrollBarCtrl.h"

DuiImplement_CreateControl(CDUIScrollBarCtrl)
MMImplement_ClassName(CDUIScrollBarCtrl)

CDUIScrollBarCtrl::CDUIScrollBarCtrl(void)
{
	m_nUpBtnStatus = ControlStatus_Normal;
	m_nDownBtnStatus = ControlStatus_Normal;

	delay_deltaY_ = 0;
	delay_number_ = 0;
	delay_left_ = 0;

	m_pOwnerCtrl = NULL;
}

CDUIScrollBarCtrl::~CDUIScrollBarCtrl(void)
{
}

LPVOID CDUIScrollBarCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIScrollBarCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIScrollBarCtrl::GetDescribe() const
{
	return Dui_Ctrl_ScrollBar;
}

void CDUIScrollBarCtrl::SetProgressType(enDuiProgressType ProgressType)
{
	__super::SetProgressType(ProgressType);

	//hide
	DuiInitAttriVisible(m_AttributeGroupThumbHoriz, ProgressType_Horiz == GetProgressType());
	DuiInitAttriVisible(m_AttributeGroupUpDownBtnHoriz, ProgressType_Horiz == GetProgressType());
	DuiInitAttriVisible(m_AttributeGroupThumbVert, ProgressType_Vert == GetProgressType());
	DuiInitAttriVisible(m_AttributeGroupUpDownBtnVert, ProgressType_Vert == GetProgressType());

	//move type
	__super::SetProgressMoveType(ProgressType_Horiz == GetProgressType() ? ProgressMove_Positive : ProgressMove_Reverse);

	return;
}

void CDUIScrollBarCtrl::SetProgressMoveType(enDuiProgressMoveType ProgressMoveType)
{
}

void CDUIScrollBarCtrl::SetOwnerCtrl(CDUIContainerCtrl *pOwnerCtrl)
{
	m_pOwnerCtrl = pOwnerCtrl;

	return;
}

void CDUIScrollBarCtrl::SetMaxValue(int nMaxValue)
{
	__super::SetMaxValue(nMaxValue);

	CalcScrollBarThumbSize();

	return;
}

void CDUIScrollBarCtrl::SetCurValue(int nCurValue)
{
	int nCurValueOld = GetCurValue();

	__super::SetCurValue(nCurValue);

	if (m_pOwnerCtrl)
	{
		enDuiProgressType ProgressType = GetProgressType();
		if ((ProgressType_Horiz == ProgressType) && (GetCurValue() != nCurValueOld))
			m_pOwnerCtrl->ScrollChilds(CDUISize(GetCurValue() - nCurValueOld, 0));
		else if ((ProgressType_Vert == ProgressType) && ((GetCurValue() != nCurValueOld)))
			m_pOwnerCtrl->ScrollChilds(CDUISize(0, GetCurValue() - nCurValueOld));
	}

	return;
}

int CDUIScrollBarCtrl::GetScrollMinSpeed()
{
	return m_AttributeMinScrollSpeed.GetValue();
}

int CDUIScrollBarCtrl::GetUpDownBtnVertInsetLeft()
{
	return m_AttributeUpDownBtnVertInsetLeft.GetValue();
}

int CDUIScrollBarCtrl::GetUpDownBtnVertInsetRight()
{
	return m_AttributeUpDownBtnVertInsetRight.GetValue();
}

int CDUIScrollBarCtrl::GetUpDownBtnVertFixedHeight()
{
	return m_AttributeUpDownBtnVertFixedHeight.GetValue();
}

int CDUIScrollBarCtrl::GetUpDownBtnHorizInsetTop()
{
	return m_AttributeUpDownBtnHorizInsetTop.GetValue();
}

int CDUIScrollBarCtrl::GetUpDownBtnHorizInsetBottom()
{
	return m_AttributeUpDownBtnHorizInsetBottom.GetValue();
}

int CDUIScrollBarCtrl::GetUpDownBtnHorizFixedWidth()
{
	return m_AttributeUpDownBtnHorizFixedWidth.GetValue();
}

void CDUIScrollBarCtrl::SetUpDownBtnVertInsetLeft(int nLeft)
{
	if (nLeft == GetUpDownBtnVertInsetLeft()) return;

	m_AttributeUpDownBtnVertInsetLeft.SetValue(nLeft);

	NeedRefreshView();

	return;
}

void CDUIScrollBarCtrl::SetUpDownBtnVertInsetRight(int nRight)
{
	if (nRight == GetUpDownBtnVertInsetRight()) return;

	m_AttributeUpDownBtnVertInsetRight.SetValue(nRight);

	NeedRefreshView();

	return;
}

void CDUIScrollBarCtrl::SetUpDownBtnVertFixedHeight(int nHeight)
{
	if (nHeight == GetUpDownBtnVertFixedHeight()) return;

	m_AttributeUpDownBtnVertFixedHeight.SetValue(nHeight);

	NeedRefreshView();

	return;
}

void CDUIScrollBarCtrl::SetUpDownBtnHorizInsetTop(int nTop)
{
	if (nTop == GetUpDownBtnHorizInsetTop()) return;

	m_AttributeUpDownBtnHorizInsetTop.SetValue(nTop);

	NeedRefreshView();

	return;
}

void CDUIScrollBarCtrl::SetUpDownBtnHorizInsetBottom(int nBottom)
{
	if (nBottom == GetUpDownBtnHorizInsetBottom()) return;

	m_AttributeUpDownBtnHorizInsetBottom.SetValue(nBottom);

	NeedRefreshView();

	return;
}

void CDUIScrollBarCtrl::SetUpDownBtnHorizFixedWidth(int nWidth)
{
	if (nWidth == GetUpDownBtnHorizFixedWidth()) return;

	m_AttributeUpDownBtnHorizFixedWidth.SetValue(nWidth);

	NeedRefreshView();

	return;
}

//upbtn status
vector<CMMString> CDUIScrollBarCtrl::GetColorResSwitchUpBtnNormal()
{
	return m_AttributeColorUpBtnNormal.GetColorResSwitch();
}

vector<CMMString> CDUIScrollBarCtrl::GetColorResSwitchUpBtnHot()
{
	return m_AttributeColorUpBtnHot.GetColorResSwitch();
}

vector<CMMString> CDUIScrollBarCtrl::GetColorResSwitchUpBtnPushed()
{
	return m_AttributeColorUpBtnPushed.GetColorResSwitch();
}

void CDUIScrollBarCtrl::SetColorResSwitchUpBtnNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorUpBtnNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetColorResSwitchUpBtnHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorUpBtnHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetColorResSwitchUpBtnPushed(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorUpBtnPushed.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

tagDuiImageSection CDUIScrollBarCtrl::GetImageSectionUpBtnNormal()
{
	return m_AttributeImageUpBtnNormal.GetImageSection();
}

tagDuiImageSection CDUIScrollBarCtrl::GetImageSectionUpBtnHot()
{
	return m_AttributeImageUpBtnHot.GetImageSection();
}

tagDuiImageSection CDUIScrollBarCtrl::GetImageSectionUpBtnPushed()
{
	return m_AttributeImageUpBtnPushed.GetImageSection();
}

void CDUIScrollBarCtrl::SetImageSectionUpBtnNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageUpBtnNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetImageSectionUpBtnHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageUpBtnHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetImageSectionUpBtnPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageUpBtnPushed.SetImageSection(ImageSection);

	Invalidate();

	return;
}

//downbtn status
vector<CMMString> CDUIScrollBarCtrl::GetColorResSwitchDownBtnNormal()
{
	return m_AttributeColorDownBtnNormal.GetColorResSwitch();
}

vector<CMMString> CDUIScrollBarCtrl::GetColorResSwitchDownBtnHot()
{
	return m_AttributeColorDownBtnHot.GetColorResSwitch();
}

vector<CMMString> CDUIScrollBarCtrl::GetColorResSwitchDownBtnPushed()
{
	return m_AttributeColorDownBtnPushed.GetColorResSwitch();
}

void CDUIScrollBarCtrl::SetColorResSwitchDownBtnNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorDownBtnNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetColorResSwitchDownBtnHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorDownBtnHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetColorResSwitchDownBtnPushed(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorDownBtnPushed.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

tagDuiImageSection CDUIScrollBarCtrl::GetImageSectionDownBtnNormal()
{
	return m_AttributeImageDownBtnNormal.GetImageSection();
}

tagDuiImageSection CDUIScrollBarCtrl::GetImageSectionDownBtnHot()
{
	return m_AttributeImageDownBtnHot.GetImageSection();
}

tagDuiImageSection CDUIScrollBarCtrl::GetImageSectionDownBtnPushed()
{
	return m_AttributeImageDownBtnPushed.GetImageSection();
}

void CDUIScrollBarCtrl::SetImageSectionDownBtnNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDownBtnNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetImageSectionDownBtnHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDownBtnHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::SetImageSectionDownBtnPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDownBtnPushed.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIScrollBarCtrl::InitProperty()
{
	__super::InitProperty();

	//create
	DuiCreateAttribute(m_AttributeMinScrollSpeed, _T("ScrollMinSpeed"), _T(""), m_AttributeGroupMouse);

	DuiCreateGroupAttribute(m_AttributeGroupUpDownBtnVert, _T("UpDownBtnVert"));
	DuiCreateAttribute(m_AttributeUpDownBtnVertInsetLeft, _T("UpDownBtnVertInsetLeft"), _T(""), m_AttributeGroupUpDownBtnVert);
	DuiCreateAttribute(m_AttributeUpDownBtnVertInsetRight, _T("UpDownBtnVertInsetRight"), _T(""), m_AttributeGroupUpDownBtnVert);
	DuiCreateAttribute(m_AttributeUpDownBtnVertFixedHeight, _T("UpDownBtnVertFixedHeight"), _T(""), m_AttributeGroupUpDownBtnVert);

	DuiCreateGroupAttribute(m_AttributeGroupUpDownBtnHoriz, _T("UpDownBtnHoriz"));
	DuiCreateAttribute(m_AttributeUpDownBtnHorizInsetTop, _T("UpDownBtnHorizInsetTop"), _T(""), m_AttributeGroupUpDownBtnHoriz);
	DuiCreateAttribute(m_AttributeUpDownBtnHorizInsetBottom, _T("UpDownBtnHorizInsetBottom"), _T(""), m_AttributeGroupUpDownBtnHoriz);
	DuiCreateAttribute(m_AttributeUpDownBtnHorizFixedWidth, _T("UpDownBtnHorizFixedWidth"), _T(""), m_AttributeGroupUpDownBtnHoriz);

	DuiCreateGroupAttribute(m_AttributeGroupUpBtnStatus, _T("UpBtnStatus"));
	DuiCreateAttribute(m_AttributeColorUpBtnNormal, _T("ColorUpBtnNormal"), _T(""), m_AttributeGroupUpBtnStatus);
	DuiCreateAttribute(m_AttributeColorUpBtnHot, _T("ColorUpBtnHot"), _T(""), m_AttributeGroupUpBtnStatus);
	DuiCreateAttribute(m_AttributeColorUpBtnPushed, _T("ColorUpBtnPushed"), _T(""), m_AttributeGroupUpBtnStatus);
	DuiCreateAttribute(m_AttributeImageUpBtnNormal, _T("ImageUpBtnNormal"), _T(""), m_AttributeGroupUpBtnStatus);
	DuiCreateAttribute(m_AttributeImageUpBtnHot, _T("ImageUpBtnHot"), _T(""), m_AttributeGroupUpBtnStatus);
	DuiCreateAttribute(m_AttributeImageUpBtnPushed, _T("ImageUpBtnPushed"), _T(""), m_AttributeGroupUpBtnStatus);

	DuiCreateGroupAttribute(m_AttributeGroupDownBtnStatus, _T("DownBtnStatus"));
	DuiCreateAttribute(m_AttributeColorDownBtnNormal, _T("ColorDownBtnNormal"), _T(""), m_AttributeGroupDownBtnStatus);
	DuiCreateAttribute(m_AttributeColorDownBtnHot, _T("ColorDownBtnHot"), _T(""), m_AttributeGroupDownBtnStatus);
	DuiCreateAttribute(m_AttributeColorDownBtnPushed, _T("ColorDownBtnPushed"), _T(""), m_AttributeGroupDownBtnStatus);
	DuiCreateAttribute(m_AttributeImageDownBtnNormal, _T("ImageDownBtnNormal"), _T(""), m_AttributeGroupDownBtnStatus);
	DuiCreateAttribute(m_AttributeImageDownBtnHot, _T("ImageDownBtnHot"), _T(""), m_AttributeGroupDownBtnStatus);
	DuiCreateAttribute(m_AttributeImageDownBtnPushed, _T("ImageDownBtnPushed"), _T(""), m_AttributeGroupDownBtnStatus);

	return;
}

void CDUIScrollBarCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	DuiInitAttriValue(m_AttributeUpDownBtnVertFixedHeight, 30);
	DuiInitAttriValue(m_AttributeUpDownBtnHorizFixedWidth, 30);
	DuiInitAttriValue(m_AttributeMinScrollSpeed, Dui_ScrollSpeed_Normal);

	return;
}

void CDUIScrollBarCtrl::InitComplete()
{
	__super::InitComplete();

	//指定水平为正向，垂直为负向
	if (ProgressType_Horiz == GetProgressType())
	{
		__super::SetProgressMoveType(ProgressMove_Positive);
	}
	if (ProgressType_Vert == GetProgressType())
	{
		__super::SetProgressMoveType(ProgressMove_Reverse);
	}

	//hide
	DuiInitAttriVisible(m_AttributeGroupSlip, false);
	DuiInitAttriVisible(m_AttributeGroupProgressType, false);
	DuiInitAttriVisible(m_AttributeGroupValue, false);
	DuiInitAttriVisible(m_AttributeGroupThumbHoriz, ProgressType_Horiz == GetProgressType());
	DuiInitAttriVisible(m_AttributeGroupUpDownBtnHoriz, ProgressType_Horiz == GetProgressType());
	DuiInitAttriVisible(m_AttributeGroupThumbVert, ProgressType_Vert == GetProgressType());
	DuiInitAttriVisible(m_AttributeGroupUpDownBtnVert, ProgressType_Vert == GetProgressType());
	DuiInitAttriVisible(m_AttributeThumbVertFixedHeight, false);
	DuiInitAttriVisible(m_AttributeThumbHorizFixedWidth, false);

	return;
}

void CDUIScrollBarCtrl::PaintStatusColor(HDC hDC)
{
	__super::PaintStatusColor(hDC);

	//up btn
	CDUIAttributeColorSwitch *pAttribute = NULL;
	if (m_nUpBtnStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeColorUpBtnPushed;
	}
	else if (m_nUpBtnStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeColorUpBtnHot;
	}
	else
	{
		pAttribute = &m_AttributeColorUpBtnNormal;
	}

	if (pAttribute)
	{
		pAttribute->FillRect(hDC, m_rcUpBtn, IsColorHSL());
	}

	//down btn
	pAttribute = NULL;
	if (m_nDownBtnStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeColorDownBtnPushed;
	}
	else if (m_nDownBtnStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeColorDownBtnHot;
	}
	else
	{
		pAttribute = &m_AttributeColorDownBtnNormal;
	}

	if (pAttribute)
	{
		pAttribute->FillRect(hDC, m_rcDownBtn, IsColorHSL());
	}

	return;
}

void CDUIScrollBarCtrl::PaintStatusImage(HDC hDC)
{
	__super::PaintStatusImage(hDC);

	//up btn
	CDUIAttriImageSection *pAttribute = NULL;
	if (m_nUpBtnStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeImageUpBtnPushed;
	}
	else if (m_nUpBtnStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeImageUpBtnHot;
	}
	else
	{
		pAttribute = &m_AttributeImageUpBtnNormal;
	}

	if (pAttribute)
	{
		pAttribute->Draw(hDC, m_rcUpBtn, m_rcPaint);
	}

	//down btn
	pAttribute = NULL;
	if (m_nDownBtnStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeImageDownBtnPushed;
	}
	else if (m_nDownBtnStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeImageDownBtnHot;
	}
	else
	{
		pAttribute = &m_AttributeImageDownBtnNormal;
	}

	if (pAttribute)
	{
		pAttribute->Draw(hDC, m_rcDownBtn, m_rcPaint);
	}

	return;
}

bool CDUIScrollBarCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	//up btn
	if (m_rcUpBtn.PtInRect(pt))
	{
		m_nUpBtnStatus |= ControlStatus_Pushed;

		SetCurValue(max(GetCurValue() - GetScrollMinSpeed(), 0));

		//定时滑动
		m_ptOnUpDownBtn = pt;
		if (m_pWndManager)
		{
			m_pWndManager->SetTimer(this, Dui_TimerScrollAuto_ID, Dui_TimerScrollAuto_Elapse);
		}

		Invalidate();

		return true;
	}

	//down btn
	if (m_rcDownBtn.PtInRect(pt))
	{
		m_nDownBtnStatus |= ControlStatus_Pushed;

		SetCurValue(min(GetCurValue() + GetScrollMinSpeed(), GetMaxValue()));

		//定时滑动
		m_ptOnUpDownBtn = pt;
		if (m_pWndManager)
		{
			m_pWndManager->SetTimer(this, Dui_TimerScrollAuto_ID, Dui_TimerScrollAuto_Elapse);
		}

		Invalidate();

		return true;
	}

	return __super::OnDuiLButtonDown(pt, Msg);
}

bool CDUIScrollBarCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	//滚动定时
	if (m_pWndManager)
	{
		m_pWndManager->KillTimer(this, Dui_TimerScrollAuto_ID);
	}

	m_nUpBtnStatus &= ~ControlStatus_Pushed;
	m_nDownBtnStatus &= ~ControlStatus_Pushed;

	return __super::OnDuiLButtonUp(pt, Msg);
}

bool CDUIScrollBarCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	//按down btn
	if (m_nUpBtnStatus & ControlStatus_Pushed)
	{
		return true;
	}
	if (m_nDownBtnStatus & ControlStatus_Pushed)
	{
		return true;
	}
	if (m_nControlStatus & ControlStatus_Pushed)
	{
		return __super::OnDuiMouseMove(pt, Msg);
	}

	//按钮、滑块
	m_nUpBtnStatus &= ~ControlStatus_Hot;
	m_nDownBtnStatus &= ~ControlStatus_Hot;
	m_nControlStatus &= ~ControlStatus_Hot;
	if (m_rcUpBtn.PtInRect(pt) && 0 == (m_nUpBtnStatus & ControlStatus_Hot))
	{
		m_nUpBtnStatus |= ControlStatus_Hot;

		Invalidate();
	}
	else if (m_rcDownBtn.PtInRect(pt) && 0 == (m_nDownBtnStatus & ControlStatus_Hot))
	{
		m_nDownBtnStatus |= ControlStatus_Hot;

		Invalidate();
	}
	else if (0 == (m_nControlStatus & ControlStatus_Hot))
	{
		m_nControlStatus |= ControlStatus_Hot;

		Invalidate();
	}

	return true;
}

void CDUIScrollBarCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseLeave(pt, Msg);

	m_nUpBtnStatus &= ~ControlStatus_Hot;
	m_nDownBtnStatus &= ~ControlStatus_Hot;

	Invalidate();

	return;
}

bool CDUIScrollBarCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (NULL == m_pOwnerCtrl) return true;

	//no speed
	if (false == m_pOwnerCtrl->IsScrollBarSpeedModel())
	{
		int nCurValue = GetCurValue();
		nCurValue += WHEEL_DELTA == (int)(short)HIWORD(Msg.wParam) ? -GetScrollMinSpeed() : GetScrollMinSpeed();
		SetCurValue(nCurValue);

		return true;
	}

	//speed
	LONG lDeltaY = 0;
	if (delay_number_ > 0) lDeltaY = (LONG)(pow((double)delay_left_ / delay_number_, 2) * delay_deltaY_);

	switch ((int)(short)HIWORD(Msg.wParam))
	{
		case WHEEL_DELTA:
		{
			delay_deltaY_ = (delay_deltaY_ >= 0) ? lDeltaY + GetScrollMinSpeed() : lDeltaY + 12;
			break;
		}
		case -WHEEL_DELTA:
		{
			delay_deltaY_ = (delay_deltaY_ <= 0) ? lDeltaY - GetScrollMinSpeed() : lDeltaY - 12;
			break;
		}
	}

	delay_deltaY_ = (delay_deltaY_ > 100) ? 100 : delay_deltaY_;
	delay_deltaY_ = (delay_deltaY_ < -100) ? -100 : delay_deltaY_;

	delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
	delay_left_ = delay_number_;
	if (m_pWndManager)
	{
		m_pWndManager->SetTimer(this, Dui_TimerScrollSlowdown_ID, Dui_TimerScrollSlowdown_Elapse);
	}

	return true;
}

void CDUIScrollBarCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	//按钮自滚动
	if (uTimerID == Dui_TimerScrollAuto_ID)
	{
		POINT ptMouse = {};
		::GetCursorPos(&ptMouse);
		if (m_pWndManager) ::ScreenToClient(m_pWndManager->GetWndHandle(), &ptMouse);

		OnTimerScrollUpDownBtn(CDUIPoint(ptMouse.x, ptMouse.y));

		return;
	}

	//滚轮滚动
	if (uTimerID == Dui_TimerScrollSlowdown_ID)
	{
		OnTimerScrollMouseWheel();
	}

	return;
}

void CDUIScrollBarCtrl::CalcSubRect()
{
	CalcUpDownBtnRect();

	return __super::CalcSubRect();
}

void CDUIScrollBarCtrl::CalcUpDownBtnRect()
{
	//区分水平类型还是垂直类型
	if (ProgressType_Horiz == GetProgressType())
		CalcUpDownBtnRectHoriz();
	else if (ProgressType_Vert == GetProgressType())
		CalcUpDownBtnRectVert();

	return;
}

void CDUIScrollBarCtrl::CalcUpDownBtnRectHoriz()
{
	//偏移单位初始化
	int nUpDownBtnInsetTop = m_AttributeUpDownBtnHorizInsetTop.GetValue();
	int nUpDownBtnInsetBottom = m_AttributeUpDownBtnHorizInsetBottom.GetValue();
	int nUpDownBtnFixedWidth = m_AttributeUpDownBtnHorizFixedWidth.GetValue();

	//上down btn位置
	m_rcUpBtn.left = m_rcAbsolute.left;
	m_rcUpBtn.top = m_rcAbsolute.top + nUpDownBtnInsetTop;
	m_rcUpBtn.right = m_rcUpBtn.left + nUpDownBtnFixedWidth;
	m_rcUpBtn.bottom = m_rcAbsolute.bottom - nUpDownBtnInsetBottom;

	m_rcDownBtn.top = m_rcUpBtn.top;
	m_rcDownBtn.bottom = m_rcUpBtn.bottom;
	m_rcDownBtn.right = m_rcAbsolute.right;
	m_rcDownBtn.left = m_rcDownBtn.right - nUpDownBtnFixedWidth;

	//检测
	m_rcUpBtn.CheckRect();
	m_rcDownBtn.CheckRect();

	m_rcUpBtn = m_rcUpBtn.RectClip(m_rcAbsolute, m_rcUpBtn);
	m_rcDownBtn = m_rcDownBtn.RectClip(m_rcAbsolute, m_rcDownBtn);

	return;
}

void CDUIScrollBarCtrl::CalcUpDownBtnRectVert()
{
	//偏移单位初始化
	int nUpDownBtnInsetLeft = m_AttributeUpDownBtnVertInsetLeft.GetValue();
	int nUpDownBtnInsetRight = m_AttributeUpDownBtnVertInsetRight.GetValue();
	int nUpDownBtnFixedHeight = m_AttributeUpDownBtnVertFixedHeight.GetValue();

	//上down btn位置
	m_rcUpBtn.left = m_rcAbsolute.left + nUpDownBtnInsetLeft;
	m_rcUpBtn.top = m_rcAbsolute.top;
	m_rcUpBtn.right = m_rcAbsolute.right - nUpDownBtnInsetRight;
	m_rcUpBtn.bottom = m_rcUpBtn.top + nUpDownBtnFixedHeight;

	m_rcDownBtn.left = m_rcUpBtn.left;
	m_rcDownBtn.right = m_rcUpBtn.right;
	m_rcDownBtn.bottom = m_rcAbsolute.bottom;
	m_rcDownBtn.top = m_rcDownBtn.bottom - nUpDownBtnFixedHeight;

	//检测
	m_rcUpBtn.CheckRect();
	m_rcDownBtn.CheckRect();

	m_rcUpBtn = m_rcUpBtn.RectClip(m_rcAbsolute, m_rcUpBtn);
	m_rcDownBtn = m_rcDownBtn.RectClip(m_rcAbsolute, m_rcDownBtn);

	return;
}

void CDUIScrollBarCtrl::CalcSlipPos()
{
	return;
}

void CDUIScrollBarCtrl::CalcGrooveAndSlipRect()
{
	CDUIRect rcGrooveInset = m_AttributeGrooveInsetToBorder.GetValue();
	CDUIRect rcSlipInset = m_AttributeSlipInsetToGroove.GetValue();

	//去掉上down btn的区域
	//区分水平类型还是垂直类型
	CDUIRect rcTemp;
	if (ProgressType_Horiz == GetProgressType())
		rcTemp = CDUIRect(m_rcAbsolute.left + m_rcUpBtn.GetWidth(), m_rcAbsolute.top, m_rcAbsolute.right - m_rcDownBtn.GetWidth(), m_rcAbsolute.bottom);
	else if (ProgressType_Vert == GetProgressType())
		rcTemp = CDUIRect(m_rcAbsolute.left, m_rcAbsolute.top + m_rcUpBtn.GetHeight(), m_rcAbsolute.right, m_rcAbsolute.bottom - m_rcDownBtn.GetHeight());

	//滑槽
	m_rcGroove = rcTemp;
	m_rcGroove.Deflate(rcGrooveInset.left, rcGrooveInset.top, rcGrooveInset.right, rcGrooveInset.bottom);
	m_rcGroove.CheckRect();
	m_rcGroove = m_rcGroove.RectClip(rcTemp, m_rcGroove);

	//滑条
	m_rcSlip = m_rcGroove;
	m_rcSlip.Deflate(rcSlipInset.left, rcSlipInset.top, rcSlipInset.right, rcSlipInset.bottom);
	m_rcSlip.CheckRect();
	m_rcSlip = m_rcSlip.RectClip(m_rcGroove, m_rcSlip);

	return;
}

void CDUIScrollBarCtrl::CalcScrollBarThumbSize()
{
	if (NULL == m_pOwnerCtrl) return;

	//水平类型
	if (ProgressType_Horiz == GetProgressType())
	{
		//消失
		if (GetMaxValue() <= 0)
		{
			SetThumbSize(CDUISize(GetSlipRect().GetWidth(), 0));

			SetVisible(false);

			return;
		}

		//出现
		if (m_pOwnerCtrl->IsUseHorizScrollBar())
		{
			SetVisible(true);

			int nScrollSlipWidth = GetSlipRect().GetWidth();

			//小于滑条一半
			if (GetMaxValue() <= nScrollSlipWidth / 2)
			{
				SetThumbSize(CDUISize(nScrollSlipWidth - GetMaxValue(), 0));
			}
			//大于滑条一半，根据趋势缩小滑块大小
			else
			{
				int nValue = nScrollSlipWidth / 2, nThumbWidth = 0;
				nValue += (GetMaxValue() - nScrollSlipWidth / 2) / Dui_Min_ThumbInterval + 1;
				nThumbWidth = nScrollSlipWidth - nValue;
				nThumbWidth = max(nThumbWidth, Dui_Min_ThumbSize);

				SetThumbSize(CDUISize(nThumbWidth, 0));
			}
		}

		return;
	}

	//垂直类型
	if (ProgressType_Vert == GetProgressType())
	{
		//消失
		if (GetMaxValue() <= 0)
		{
			SetThumbSize(CDUISize(0, GetSlipRect().GetHeight()));

			SetVisible(false);

			return;
		}

		//出现
		if (m_pOwnerCtrl->IsUseVertScrollBar())
		{
			SetVisible(true);

			int nScrollSlipHeight = GetSlipRect().GetHeight();

			//小于滑条一半
			if (GetMaxValue() <= nScrollSlipHeight / 2)
			{
				SetThumbSize(CDUISize(0, nScrollSlipHeight - GetMaxValue()));
			}
			//大于滑条一半，根据趋势缩小滑块大小
			else
			{
				int nValue = nScrollSlipHeight / 2, nThumbHeight = 0;
				nValue += (GetMaxValue() - nScrollSlipHeight / 2) / Dui_Min_ThumbInterval + 1;
				nThumbHeight = nScrollSlipHeight - nValue;
				nThumbHeight = max(nThumbHeight, Dui_Min_ThumbSize);

				SetThumbSize(CDUISize(0, nThumbHeight));
			}
		}
	}

	return;
}

void CDUIScrollBarCtrl::OnTimerScrollUpDownBtn(CDUIPoint ptCurMouse)
{
	int nCurValue = GetCurValue();

	//水平类型
	if (ProgressType_Horiz == GetProgressType())
	{
		//up btn
		if (m_rcUpBtn.PtInRect(m_ptOnUpDownBtn))
		{
			int nSpeedValue = m_ptOnUpDownBtn.x - ptCurMouse.x + GetScrollMinSpeed();
			nSpeedValue = (nSpeedValue >= 0) ? nSpeedValue : 0;

			SetCurValue((nCurValue >= nSpeedValue) ? (nCurValue - nSpeedValue) : 0);

			return;
		}

		//down btn
		if (m_rcDownBtn.PtInRect(m_ptOnUpDownBtn))
		{
			int nSpeedValue = ptCurMouse.x - m_ptOnUpDownBtn.x + GetScrollMinSpeed();
			nSpeedValue = (nSpeedValue >= 0) ? nSpeedValue : 0;

			SetCurValue(((nCurValue + nSpeedValue) <= GetMaxValue()) ? (nCurValue + nSpeedValue) : GetMaxValue());

			return;
		}

		return;
	}

	//垂直类型
	if (ProgressType_Vert == GetProgressType())
	{
		//up btn
		if (m_rcUpBtn.PtInRect(m_ptOnUpDownBtn))
		{
			int nSpeedValue = m_ptOnUpDownBtn.y - ptCurMouse.y + GetScrollMinSpeed();
			nSpeedValue = (nSpeedValue >= 0) ? nSpeedValue : 0;

			SetCurValue((nCurValue >= nSpeedValue) ? (nCurValue - nSpeedValue) : 0);

			return;
		}

		//down btn
		if (m_rcDownBtn.PtInRect(m_ptOnUpDownBtn))
		{
			int nSpeedValue = ptCurMouse.y - m_ptOnUpDownBtn.y + GetScrollMinSpeed();
			nSpeedValue = (nSpeedValue >= 0) ? nSpeedValue : 0;

			SetCurValue(((nCurValue + nSpeedValue) <= GetMaxValue()) ? (nCurValue + nSpeedValue) : GetMaxValue());

			return;
		}

		return;
	}

	return;
}

void CDUIScrollBarCtrl::OnTimerScrollMouseWheel()
{
	if (NULL == m_pOwnerCtrl) return;

	if (delay_left_ > 0 && m_pOwnerCtrl->IsScrollBarSpeedModel())
	{
		--delay_left_;
		if (delay_number_ == 0) return;

		int nCurValue = GetCurValue();
		LONG lDeltaY = (LONG)(pow((double)delay_left_ / delay_number_, 2) * delay_deltaY_);
		if ((lDeltaY > 0 && nCurValue >= 0) || (lDeltaY < 0 && nCurValue <= GetMaxValue()))
		{
			nCurValue -= lDeltaY;
			SetCurValue(nCurValue);
			return;
		}
	}

	delay_deltaY_ = 0;
	delay_number_ = 0;
	delay_left_ = 0;

	if (m_pWndManager)
	{
		m_pWndManager->KillTimer(this, Dui_TimerScrollSlowdown_ID);
	}

	Invalidate();

	return;
}