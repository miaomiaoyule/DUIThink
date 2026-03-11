#ifndef __IDUIEXPORTINTERFACE_H__
#define __IDUIEXPORTINTERFACE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_IDuiInterface INTERFACE_VERSION(1,1)
static const GUID IID_IDuiInterface = { 0x3CA0AA03,0xA27F,0x471A,0xB4,0x51,0xD4,0x77,0x41,0x2B,0x0A,0xFD };
interface DUITHINK_API IDuiInterface : public IMMUnknown
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
};

//////////////////////////////////////////////////////////////////////////
//Notify
interface IDuiNotify
{
	virtual ~IDuiNotify() {}
	virtual void OnNotify(const DuiNotify &Notify) = 0;
};

typedef std::vector<IDuiNotify*> VecIDuiNotify;

//////////////////////////////////////////////////////////////////////////
//PreMessage
interface IDuiPreMessage
{
	virtual ~IDuiPreMessage() {}
	virtual LRESULT OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) = 0;
};

typedef std::vector<IDuiPreMessage*> VecIDuiPreMessage;

//////////////////////////////////////////////////////////////////////////
//control callback
interface IDuiControlCallBack
{
	virtual ~IDuiControlCallBack() {}
	virtual void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) {}
	virtual void OnRelease(CDUIControlBase *pControl) {}
	virtual void OnPaint(CDUIControlBase *pControl, HDC hDC) {}
	virtual void OnSize(CDUIControlBase *pControl) {}
	virtual void OnVisibleChanged(CDUIControlBase *pControl) {}
};

typedef std::vector<IDuiControlCallBack*> VecIDuiControlCallBack;

//////////////////////////////////////////////////////////////////////////
//resource change
#define VER_IDuiResourceCallBack INTERFACE_VERSION(1,1)
static const GUID IID_IDuiResourceCallBack = { 0x8E679207,0x6CDB,0x49E2,0x9C,0x4E,0xC5,0x85,0x38,0xDC,0x3D,0xA6 };
interface DUITHINK_API IDuiResourceCallBack : public IDuiInterface
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	virtual void OnResourceAdd(CDUIResourceBase *pResourceObj) = NULL;
	virtual void OnResourceRemove(CDUIResourceBase *pResourceObj) = NULL;
	virtual void OnResourceRename(CDUIResourceBase *pResourceObj, const CMMString &strNameOld) = NULL;
};

typedef std::vector<IDuiResourceCallBack*> VecIDuiResourceCallBack;

//////////////////////////////////////////////////////////////////////////
//child
#define VER_IDuiControlListen INTERFACE_VERSION(1,1)
static const GUID IID_IDuiControlListen = { 0x816B4030,0xB07E,0x48B3,0x8E,0x6B,0xA2,0x1D,0x3A,0x64,0x38,0xFC };
interface DUITHINK_API IDuiControlListen : public IDuiInterface
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	virtual void OnChildAdd(CDUIContainerCtrl *pParent, CDUIControlBase *pChild) {}
	virtual void OnChildRemove(CDUIContainerCtrl *pParent, CDUIControlBase *pChild) {}
	virtual void OnVisibleChange(CDUIControlBase *pControl) {}
	virtual void OnAttributeChange(CDUIPropertyObject *pPropObj, CDUIAttributeObject *pAttribute) {}
};

typedef std::vector<IDuiControlListen*> VecIDuiControlListen;

//////////////////////////////////////////////////////////////////////////
//wnd
#define VER_IDuiWndNotify INTERFACE_VERSION(1,1)
static const GUID IID_IDuiWndNotify = { 0xC78CF794,0x6ABD,0x4688,0x92,0x1C,0xC9,0xD7,0xEF,0x23,0x6C,0x2D };
interface DUITHINK_API IDuiWndNotify : public IDuiInterface
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	virtual void OnDuiWndCreate(CDUIWnd *pWnd) = NULL;
	virtual void OnDuiWndDestroy(CDUIWnd *pWnd) = NULL;
	virtual void OnDuiWndPaint(CDUIWnd *pWnd) {}
	virtual void OnDuiWndSize(CDUIWnd *pWnd) {}
};

//////////////////////////////////////////////////////////////////////////

#endif
