#include "StdAfx.h"
#include "AnimateView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CAnimateView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CAnimateView::CAnimateView()
{

}

void CAnimateView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////