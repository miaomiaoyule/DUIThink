#ifndef __DUIWND_H__
#define __DUIWND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWnd INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWnd = { 0xD5D0FF1C,0x106B,0x48C7,0xAD,0xA9,0x8C,0xA3,0x43,0x80,0x1F,0x74 };
class DUITHINK_API CDUIWnd
#if defined DuiPlatform_SDL
	: public CDUIWndSDL
#else
	: public CDUIWndWin32
#endif
{
	friend class CDUIGlobal;
	friend class CDUIXmlPack;
	friend class CDesignerView;
	friend class CControlView;
	friend class CLayoutView;
	friend class CDUIContainerCtrl;
	friend class CDlgLogon;

	MMDeclare_ClassName(CDUIWnd)
	DuiDeclare_Message_Map()

public:
	CDUIWnd(LPCTSTR lpszDuiName = NULL, HWND hWndParent = NULL);
	virtual ~CDUIWnd();

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//timer
	bool SetTimer(CDUIPropertyObject *pPropObj, UINT uTimerID, UINT uElapse) override;
	bool KillTimer(CDUIPropertyObject *pPropObj, UINT uTimerID) override;
	bool KillTimer(CDUIPropertyObject *pPropObj) override;
	bool RemoveAllTimer() override;

	//property
protected:
	void InitProperty() override;

	//message
protected:
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam) override;

	//help
protected:
	CDUIWnd * GetWndOwner() override;
	void DispatchModelMouseEvent(CDUIControlBase *pCtrl, MouseEventPtr pFunc, const CDUIPoint &pt, const DuiMessage &Msg, bool bClick) override;
	void DispatchModelKeyboardEvent(CDUIControlBase *pCtrl, KeyboardEventPtr pFunc, const DuiMessage &Msg) override;

	//help
public:
	static UINT MapKeyState();
};

//////////////////////////////////////////////////////////////////////////

#endif