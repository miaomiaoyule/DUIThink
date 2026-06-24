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
	MMSafeDelete(m_pBmpClockCenterDot);
	MMSafeDelete(m_pBmpClockHourPointer);
	MMSafeDelete(m_pBmpClockMinutePointer);
	MMSafeDelete(m_pBmpClockSecondPointer);

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
	MMSafeDelete(m_pBmpClockHourPointer);

	Invalidate();

	return;
}

void CDUIClockCtrl::SetImageSectionClockMinute(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockMinute.SetImageSection(ImageSection);
	MMSafeDelete(m_pBmpClockMinutePointer);

	Invalidate();

	return;
}

void CDUIClockCtrl::SetImageSectionClockSecond(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockSecond.SetImageSection(ImageSection);
	MMSafeDelete(m_pBmpClockSecondPointer);

	Invalidate();

	return;
}

void CDUIClockCtrl::SetImageSectionClockCenterDot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageClockCenterDot.SetImageSection(ImageSection);
	MMSafeDelete(m_pBmpClockCenterDot);

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
	if (NULL == m_pBmpClockHourPointer)
	{
		tagDuiImageSection ImageSection = m_AttributeImageClockHour.GetImageSection();
		CDUIImageBase *pImageBase = m_AttributeImageClockHour.GetCurImageBase();
		if (pImageBase)
		{
			Gdiplus::Bitmap *pBitmap = pImageBase->GetBitmap(GetScale());
			if (pBitmap)
			{
				CDUIRect rcSource = ImageSection.mapSourceCustomScale[100];
				if (ImageSection.mapSourceCustomScale.find(GetScale()) != ImageSection.mapSourceCustomScale.end())
				{
					rcSource = ImageSection.mapSourceCustomScale[GetScale()];
				}

				m_pBmpClockHourPointer = pBitmap->Clone(rcSource.left, rcSource.top, rcSource.GetWidth(), rcSource.GetHeight(), PixelFormat32bppARGB);
			}
		}
	}
	if (m_pBmpClockHourPointer)
	{
		Gdiplus::Matrix matrixH(1, 0, 0, 1, ptCenter.x, ptCenter.y);
		Gdiplus::PointF pointsH[] =
		{
			PointF(0, 0),
			PointF(m_pBmpClockHourPointer->GetWidth(), 0),
			PointF(0, m_pBmpClockHourPointer->GetHeight())
		};
		matrixH.Rotate(SystemTime.wHour * 30 + SystemTime.wMinute * 1.0 / 2.0);
		matrixH.Translate(m_pBmpClockHourPointer->GetWidth() / -2.0f, m_pBmpClockHourPointer->GetHeight() / -2.0f);
		matrixH.TransformPoints(pointsH, 3);
		Gp.DrawImage(m_pBmpClockHourPointer, pointsH, 3);
	}

	//minute
	if (NULL == m_pBmpClockMinutePointer)
	{
		tagDuiImageSection ImageSection = m_AttributeImageClockMinute.GetImageSection();
		CDUIImageBase *pImageBase = m_AttributeImageClockMinute.GetCurImageBase();
		if (pImageBase)
		{
			Gdiplus::Bitmap *pBitmap = pImageBase->GetBitmap(GetScale());
			if (pBitmap)
			{
				CDUIRect rcSource = ImageSection.mapSourceCustomScale[100];
				if (ImageSection.mapSourceCustomScale.find(GetScale()) != ImageSection.mapSourceCustomScale.end())
				{
					rcSource = ImageSection.mapSourceCustomScale[GetScale()];
				}

				m_pBmpClockMinutePointer = pBitmap->Clone(rcSource.left, rcSource.top, rcSource.GetWidth(), rcSource.GetHeight(), PixelFormat32bppARGB);
			}
		}
	}
	if (m_pBmpClockMinutePointer)
	{
		Gdiplus::Matrix matrixM(1, 0, 0, 1, ptCenter.x, ptCenter.y);
		Gdiplus::PointF pointsM[] =
		{
			PointF(0, 0),
			PointF(m_pBmpClockMinutePointer->GetWidth(),0),
			PointF(0, m_pBmpClockMinutePointer->GetHeight())
		};
		matrixM.Rotate(SystemTime.wMinute * 6);
		matrixM.Translate(m_pBmpClockMinutePointer->GetWidth() / -2.0f, m_pBmpClockMinutePointer->GetHeight() / -2.0f);
		matrixM.TransformPoints(pointsM, 3);
		Gp.DrawImage(m_pBmpClockMinutePointer, pointsM, 3);
	}

	//second
	if (NULL == m_pBmpClockSecondPointer)
	{
		tagDuiImageSection ImageSection = m_AttributeImageClockSecond.GetImageSection();
		CDUIImageBase *pImageBase = m_AttributeImageClockSecond.GetCurImageBase();
		if (pImageBase)
		{
			Gdiplus::Bitmap *pBitmap = pImageBase->GetBitmap(GetScale());
			if (pBitmap)
			{
				CDUIRect rcSource = ImageSection.mapSourceCustomScale[100];
				if (ImageSection.mapSourceCustomScale.find(GetScale()) != ImageSection.mapSourceCustomScale.end())
				{
					rcSource = ImageSection.mapSourceCustomScale[GetScale()];
				}

				m_pBmpClockSecondPointer = pBitmap->Clone(rcSource.left, rcSource.top, rcSource.GetWidth(), rcSource.GetHeight(), PixelFormat32bppARGB);
			}
		}
	}
	if (m_pBmpClockSecondPointer)
	{
		Gdiplus::Matrix matrixS(1, 0, 0, 1, ptCenter.x, ptCenter.y);
		Gdiplus::PointF pointsS[] =
		{
			PointF(0, 0),
			PointF(m_pBmpClockSecondPointer->GetWidth(), 0),
			PointF(0, m_pBmpClockSecondPointer->GetHeight())
		};
		matrixS.Rotate(SystemTime.wSecond * 6);
		matrixS.Translate(m_pBmpClockSecondPointer->GetWidth() / -2.0f, m_pBmpClockSecondPointer->GetHeight() / -2.0f);
		matrixS.TransformPoints(pointsS, 3);
		Gp.DrawImage(m_pBmpClockSecondPointer, pointsS, 3);
	}

	//center dot
	if (NULL == m_pBmpClockCenterDot)
	{
		tagDuiImageSection ImageSection = m_AttributeImageClockCenterDot.GetImageSection();
		CDUIImageBase *pImageBase = m_AttributeImageClockCenterDot.GetCurImageBase();
		if (pImageBase)
		{
			Gdiplus::Bitmap *pBitmap = pImageBase->GetBitmap(GetScale());
			if (pBitmap)
			{
				CDUIRect rcSource = ImageSection.mapSourceCustomScale[100];
				if (ImageSection.mapSourceCustomScale.find(GetScale()) != ImageSection.mapSourceCustomScale.end())
				{
					rcSource = ImageSection.mapSourceCustomScale[GetScale()];
				}

				m_pBmpClockCenterDot = pBitmap->Clone(rcSource.left, rcSource.top, rcSource.GetWidth(), rcSource.GetHeight(), PixelFormat32bppARGB);
			}
		}
	}
	if (m_pBmpClockCenterDot)
	{
		CDUIRect rcCenterDot;
		rcCenterDot.left = ptCenter.x - m_pBmpClockCenterDot->GetWidth() / 2;
		rcCenterDot.top = ptCenter.y - m_pBmpClockCenterDot->GetHeight() / 2;
		rcCenterDot.right = rcCenterDot.left + m_pBmpClockCenterDot->GetWidth();
		rcCenterDot.bottom = rcCenterDot.top + m_pBmpClockCenterDot->GetHeight();
		Gp.DrawImage(m_pBmpClockCenterDot, rcCenterDot.left, rcCenterDot.top, m_pBmpClockCenterDot->GetWidth(), m_pBmpClockCenterDot->GetHeight());
	}

	return;
}