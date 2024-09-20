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

typedef std::vector<IDuiInterface*> VecControlListen;

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
//property obj
#define VER_IDuiPropertyObject INTERFACE_VERSION(1,1)
static const GUID IID_IDuiPropertyObject = { 0x26744DF3,0xFCAE,0x4F87,0x81,0x01,0x27,0xB0,0x70,0xE8,0xF7,0x81 };
interface DUITHINK_API IDuiPropertyObject : public IDuiInterface
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	virtual bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) = NULL;
	virtual VecControlListen GetControlListen() = NULL;
	virtual bool RegisterControlListen(IDuiInterface *pIControlListen) = NULL;
	virtual bool UnRegisterControlListen(IDuiInterface *pIControlListen) = NULL;
};

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

#endif
