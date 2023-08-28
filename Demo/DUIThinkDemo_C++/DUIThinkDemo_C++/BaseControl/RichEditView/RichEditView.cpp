#include "StdAfx.h"
#include "RichEditView.h"

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CRichEditView, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WndInited, OnDuiWndInited)
DuiEnd_Message_Map()

CRichEditView::CRichEditView()
{

}

void CRichEditView::OnDuiWndInited(const DuiNotify &Notify)
{
	return;
}

//////////////////////////////////////////////////////////////////////////