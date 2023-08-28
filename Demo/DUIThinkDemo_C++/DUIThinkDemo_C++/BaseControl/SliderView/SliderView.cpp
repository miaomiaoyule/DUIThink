#include "StdAfx.h"
#include "SliderView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CSliderView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CSliderView::CSliderView()
{

}

void CSliderView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////