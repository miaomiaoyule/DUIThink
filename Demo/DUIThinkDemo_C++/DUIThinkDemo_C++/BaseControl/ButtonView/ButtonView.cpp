#include "StdAfx.h"
#include "ButtonView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CButtonView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CButtonView::CButtonView()
{

}

void CButtonView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////