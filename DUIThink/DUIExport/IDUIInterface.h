#ifndef __IDUIEXPORTINTERFACE_H__
#define __IDUIEXPORTINTERFACE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_IDUIInterface INTERFACE_VERSION(1,1)
static const GUID IID_IDUIInterface = { 0x3CA0AA03,0xA27F,0x471A,0xB4,0x51,0xD4,0x77,0x41,0x2B,0x0A,0xFD };
interface DUITHINK_API IDUIInterface : public IMMUnknown
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
};

typedef std::vector<IDUIInterface*> VecControlListen;

//////////////////////////////////////////////////////////////////////////
//Notify
interface IDUINotify
{
	virtual ~IDUINotify() {}
	virtual void OnNotify(const DuiNotify &Notify) = 0;
};

typedef std::vector<IDUINotify*> VecIDUINotify;

//////////////////////////////////////////////////////////////////////////
//PreMessage
interface IDUIPreMessage
{
	virtual ~IDUIPreMessage() {}
	virtual LRESULT OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) = 0;
};

typedef std::vector<IDUIPreMessage*> VecIDUIPreMessage;

//////////////////////////////////////////////////////////////////////////
//property obj
#define VER_IDUIPropertyObject INTERFACE_VERSION(1,1)
static const GUID IID_IDUIPropertyObject = { 0x26744DF3,0xFCAE,0x4F87,0x81,0x01,0x27,0xB0,0x70,0xE8,0xF7,0x81 };
interface DUITHINK_API IDUIPropertyObject : public IDUIInterface
{
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	virtual bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) = NULL;
	virtual VecControlListen GetControlListen() = NULL;
	virtual bool RegisterControlListen(IDUIInterface *pIControlListen) = NULL;
	virtual bool UnRegisterControlListen(IDUIInterface *pIControlListen) = NULL;
};

//////////////////////////////////////////////////////////////////////////
//control callback
interface IDUIControlCallBack
{
	virtual ~IDUIControlCallBack() {}
	virtual void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) {}
	virtual void OnRelease(CDUIControlBase *pControl) {}
	virtual void OnPaint(CDUIControlBase *pControl, HDC hDC) {}
	virtual void OnSize(CDUIControlBase *pControl) {}
	virtual void OnVisibleChanged(CDUIControlBase *pControl) {}
};

typedef std::vector<IDUIControlCallBack*> VecIDUIControlCallBack;

//////////////////////////////////////////////////////////////////////////

#endif
