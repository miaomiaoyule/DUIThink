#ifndef __DUIINTERFACE_H__
#define __DUIINTERFACE_H__

#pragma once

class CDUIControlBase;
class CDUIContainerCtrl;

//////////////////////////////////////////////////////////////////////////
//child
#define VER_IControlListen INTERFACE_VERSION(1,1)
static const GUID IID_IControlListen = { 0x816B4030,0xB07E,0x48B3,0x8E,0x6B,0xA2,0x1D,0x3A,0x64,0x38,0xFC };
interface DUITHINK_API IControlListen : public IDUIInterface
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	virtual void OnChildAdd(CDUIContainerCtrl *pParent, CDUIControlBase *pChild) {}
	virtual void OnChildRemove(CDUIContainerCtrl *pParent, CDUIControlBase *pChild) {}
	virtual void OnVisibleChange(CDUIControlBase *pControl) {}
	virtual void OnAttributeChange(CDUIPropertyObject *pProp, CDUIAttributeObject *pAttribute) {}
	virtual void OnRefreshView(CDUIPropertyObject *pProp) {}
};

//////////////////////////////////////////////////////////////////////////
//wnd
#define VER_IDUIWndNotify INTERFACE_VERSION(1,1)
static const GUID IID_IDUIWndNotify = { 0xC78CF794,0x6ABD,0x4688,0x92,0x1C,0xC9,0xD7,0xEF,0x23,0x6C,0x2D };
interface DUITHINK_API IDUIWndNotify : public IDUIInterface
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	virtual void OnDuiWndCreate(CDUIWndManager *pWndManager) = NULL;
	virtual void OnDuiWndClose(CDUIWndManager *pWndManager) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#endif