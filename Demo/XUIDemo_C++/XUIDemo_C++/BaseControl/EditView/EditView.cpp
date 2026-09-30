#include "StdAfx.h"
#include "EditView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CEditView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CEditView::CEditView()
{

}

void CEditView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////