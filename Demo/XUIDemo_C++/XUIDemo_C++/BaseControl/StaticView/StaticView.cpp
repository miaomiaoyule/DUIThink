#include "StdAfx.h"
#include "StaticView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CStaticView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CStaticView::CStaticView()
{

}

void CStaticView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////