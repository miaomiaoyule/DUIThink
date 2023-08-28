#ifndef __DUIDEFINEWNDMANAGER_H__
#define __DUIDEFINEWNDMANAGER_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define Dui_Dispatch_ModelMouseEvent(pCtrl, Func, pt, Msg, bClick) \
{ \
if (pCtrl \
&& (false == bClick || pCtrl->IsClickTransmit()) \
&& pCtrl->GetOwnerModelCtrl() \
&& pCtrl->GetOwnerModelCtrl()->IsEnabled() \
&& false == pCtrl->GetOwnerModelCtrl()->IsMouseThrough() \
&& (NULL == MMInterfaceHelper(CDUIThinkEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl) \
&& (NULL == MMInterfaceHelper(CDUIEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl) \
&& (NULL == MMInterfaceHelper(CDUIRichEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)) \
	{ \
		pCtrl->GetOwnerModelCtrl()->Func(pt, Msg);\
	} \
}

#define Dui_Dispatch_ModelKeyboardEvent(pCtrl, Func, Msg) \
{ \
if (pCtrl \
&& pCtrl->GetOwnerModelCtrl() \
&& pCtrl->GetOwnerModelCtrl()->IsEnabled() \
&& (NULL == MMInterfaceHelper(CDUIThinkEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl) \
&& (NULL == MMInterfaceHelper(CDUIEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl) \
&& (NULL == MMInterfaceHelper(CDUIRichEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)) \
	{ \
		pCtrl->GetOwnerModelCtrl()->Func(Msg);\
	} \
}

#define Dui_Dispatch_ModelEvent(pCtrl, Func, Msg) \
{ \
if (pCtrl \
&& pCtrl->GetOwnerModelCtrl()) \
	{ \
		pCtrl->GetOwnerModelCtrl()->Func(Msg);\
	} \
}

//////////////////////////////////////////////////////////////////////////

#endif