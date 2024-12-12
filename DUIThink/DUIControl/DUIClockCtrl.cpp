#include "StdAfx.h"
#include "DUIClockCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIClockCtrl)
MMImplement_ClassName(CDUIClockCtrl)

CDUIClockCtrl::CDUIClockCtrl(void)
{
	return;
}

CDUIClockCtrl::~CDUIClockCtrl(void)
{
	return;
}

bool CDUIClockCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	return false;
}

LPVOID CDUIClockCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIClockCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIClockCtrl::GetDescribe() const
{
	return Dui_Ctrl_Clock;
}

tagDuiImageSection CDUIClockCtrl::GetImageSectionClockDial()
{
	return m_AttributeImageClockDial.GetImageSection();
}

tagDuiImageSection CDUIClockCtrl::GetImageSectionClockHour()
{
	return m_AttributeImageClockHour.GetImageSection();
}

tagDuiImageSection CDUIClockCtrl::GetImageSectionClockMinute()
{
	return m_AttributeImageClockMinute.GetImageSection();
}

tagDuiImageSection CDUIClockCtrl::GetImageSectionClockSecond()
{
	return m_AttributeImageClockSecond.GetImageSection();
}

tagDuiImageSection CDUIClockCtrl::GetImageSectionClockCenterDot()
{
	return m_AttributeImageClockCenterDot.GetImageSection();
}

void CDUIClockCtrl::SetImageSectionClockDial(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockDial.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIClockCtrl::SetImageSectionClockHour(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockHour.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIClockCtrl::SetImageSectionClockMinute(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockMinute.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIClockCtrl::SetImageSectionClockSecond(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockSecond.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIClockCtrl::SetImageSectionClockCenterDot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockCenterDot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIClockCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	SetTimer(Dui_TimerClock_ID, Dui_TimerClock_Elapse);

	return;
}

void CDUIClockCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	if (Dui_TimerClock_ID == uTimerID)
	{
		Invalidate();

		return;
	}

	return __super::OnDuiTimer(uTimerID, Msg);
}

void CDUIClockCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupClock, _T("Clock"));
	DuiCreateAttribute(m_AttributeImageClockDial, _T("ClockDial"), _T(""), m_AttributeGroupClock);
	DuiCreateAttribute(m_AttributeImageClockHour, _T("ClockHour"), _T(""), m_AttributeGroupClock);
	DuiCreateAttribute(m_AttributeImageClockMinute, _T("ClockMinute"), _T(""), m_AttributeGroupClock);
	DuiCreateAttribute(m_AttributeImageClockSecond, _T("ClockSecond"), _T(""), m_AttributeGroupClock);
	DuiCreateAttribute(m_AttributeImageClockCenterDot, _T("ClockCenterDot"), _T(""), m_AttributeGroupClock);

	return;
}

void CDUIClockCtrl::InitComplete()
{
	__super::InitComplete();

	EnableScrollBar(false, false);

	return;
}

void CDUIClockCtrl::PaintBkImage(HDC hDC)
{
	__super::PaintBkImage(hDC);

	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	Gdiplus::Graphics Gp(hDC);

	//dial
	m_AttributeImageClockDial.Draw(hDC, m_rcAbsolute, m_rcPaint);

	//center
	CDUIPoint ptCenter;
	ptCenter.x = m_rcAbsolute.left + GetWidth() / 2;
	ptCenter.y = m_rcAbsolute.top + GetHeight() / 2;

	//hour
	CDUIImageBase *pImageBase = m_AttributeImageClockHour.GetCurImageBase();
	if (pImageBase)
	{
		Gdiplus::Bitmap *pBitmap = pImageBase->GetBitmap(GetScale());
		if (pBitmap)
		{
			Gdiplus::Matrix matrixH(1, 0, 0, 1, ptCenter.x, ptCenter.y);
			Gdiplus::Point pointsH[] =
			{
				Point(0, 0),
				Point(pImageBase->GetWidth(GetScale()), 0),
				Point(0, pImageBase->GetHeight(GetScale()))
			};
			matrixH.Rotate(SystemTime.wHour * 30 + SystemTime.wMinute * 1.0 / 2.0);
			matrixH.Translate(-pImageBase->GetWidth(GetScale()) / 2, -pImageBase->GetHeight(GetScale()) / 2);
			matrixH.TransformPoints(pointsH, 3);
			Gp.DrawImage(pBitmap, pointsH, 3);
		}
	}

	//minute
	pImageBase = m_AttributeImageClockMinute.GetCurImageBase();
	if (pImageBase)
	{
		Gdiplus::Bitmap *pBitmap = pImageBase->GetBitmap(GetScale());
		if (pBitmap)
		{
			Gdiplus::Matrix matrixM(1, 0, 0, 1, ptCenter.x, ptCenter.y);	
			Gdiplus::Point pointsM[] = 
			{ 
				Point(0, 0), 
				Point(pImageBase->GetWidth(GetScale()),0),
				Point(0, pImageBase->GetHeight(GetScale()))
			};
			matrixM.Rotate(SystemTime.wMinute * 6);
			matrixM.Translate(-pImageBase->GetWidth(GetScale()) / 2, -pImageBase->GetHeight(GetScale()) / 2);
			matrixM.TransformPoints(pointsM, 3);
			Gp.DrawImage(pBitmap, pointsM, 3);
		}
	}

	//second
	pImageBase = m_AttributeImageClockSecond.GetCurImageBase();
	if (pImageBase)
	{
		Gdiplus::Bitmap *pBitmap = pImageBase->GetBitmap(GetScale());
		if (pBitmap)
		{
			Gdiplus::Matrix matrixS(1, 0, 0, 1, ptCenter.x, ptCenter.y);
			Gdiplus::Point pointsS[] = 
			{ 
				Point(0, 0),
				Point(pImageBase->GetWidth(GetScale()), 0),
				Point(0, pImageBase->GetHeight(GetScale()))
			};
			matrixS.Rotate(SystemTime.wSecond * 6);
			matrixS.Translate(-pImageBase->GetWidth(GetScale()) / 2, -pImageBase->GetHeight(GetScale()) / 2);
			matrixS.TransformPoints(pointsS, 3);
			Gp.DrawImage(pBitmap, pointsS, 3);
		}
	}

	//center dot
	CDUIRect rcCenterDot;
	rcCenterDot.left = ptCenter.x - m_AttributeImageClockCenterDot.GetCurImageWidth() / 2;
	rcCenterDot.top = ptCenter.y - m_AttributeImageClockCenterDot.GetCurImageHeight() / 2;
	rcCenterDot.right = rcCenterDot.left + m_AttributeImageClockCenterDot.GetCurImageWidth();
	rcCenterDot.bottom = rcCenterDot.top + m_AttributeImageClockCenterDot.GetCurImageHeight();
	m_AttributeImageClockCenterDot.Draw(hDC, rcCenterDot, m_rcPaint);

	return;
}