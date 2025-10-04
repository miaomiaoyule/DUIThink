#ifndef __IDUIWNDMANAGER_H__
#define __IDUIWNDMANAGER_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_IDUIWndManager INTERFACE_VERSION(1,1)
static const GUID IID_IDUIWndManager = { 0x2E296226,0x187E,0x45F3,0x8B,0x3E,0xB3,0xF1,0x6A,0xFC,0x89,0xE0 };
interface DUITHINK_API IDUIWndManager : public IDUIInterface
{
	//method
public:
	virtual CMMString GetDescribe() const = NULL;

	//handle
	virtual HWND GetWndHandle() = NULL;
	virtual HDC GetWndDC() = NULL;

	//capture
	virtual void SetCapture() = NULL;
	virtual void ReleaseCapture() = NULL;
	virtual bool IsCaptured() = NULL;
	virtual CDUIControlBase * GetCaptureControl() = NULL;

	//focus
	virtual CDUIControlBase * GetFocusControl() = NULL;
	virtual void SetFocusControl(CDUIControlBase *pFocusCtrl) = NULL;

	//hover
	virtual CDUIControlBase * GetHoverCtrl() = NULL;

	//root
	virtual bool AttachRootCtrl(CDUIContainerCtrl *pControl) = NULL;
	virtual bool ReleaseRootCtrl() = NULL;
	virtual CDUIContainerCtrl * GetRootCtrl() const = NULL;
	virtual CDUIContainerCtrl * DetachRootCtrl() = NULL;

	//manager ctrl
	virtual bool InitControls(CDUIControlBase *pControl) = NULL;
	virtual void InitControlNameHash(CDUIControlBase *pControl) = NULL;
	virtual void UnInitControlNameHash(CDUIControlBase *pControl) = NULL;
	virtual void ReapControl(CDUIControlBase *pControl) = NULL;
	virtual bool VerifyControl(CDUIControlBase *pControl) = NULL;

	//tag
	virtual UINT_PTR GetTag() = NULL;
	virtual void SetTag(UINT_PTR pTag) = NULL;

	//timer
	virtual bool SetTimer(CDUIPropertyObject *pProp, UINT uTimerID, UINT uElapse) = NULL;
	virtual bool FindTimer(CDUIPropertyObject *pProp, UINT uTimerID) = NULL;
	virtual bool KillTimer(CDUIPropertyObject *pProp, UINT uTimerID) = NULL;
	virtual bool KillTimer(CDUIPropertyObject *pProp) = NULL;
	virtual bool RemoveAllTimer() = NULL;

	//radio
	virtual bool AddRadioBoxToGroup(CDUIRadioBoxCtrl *pControl) = NULL;
	virtual VecRadioBoxCtrl GetRadioBoxGroup(LPCTSTR lpszGroup) = NULL;
	virtual bool RemoveRadioBoxFromGroup(CDUIRadioBoxCtrl *pControl) = NULL;
	virtual void RemoveAllRadioBoxGroup() = NULL;
	virtual CDUIRadioBoxCtrl * GetRadioBoxOfBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl, long lBindIndex) = NULL;

	//tips
	virtual HWND GetToolTipWindow() const = NULL;
	virtual int GetToolTipHoverTime() const = NULL;
	virtual void SetToolTipHoverTime(int nTime) = NULL;

	//refresh
	virtual void Invalidate() = NULL;
	virtual bool IsRefreshViewNeeded() const = NULL;
	virtual void NeedRefreshView() = NULL;
	virtual POINT GetLastMousePos() const = NULL;

	//window
	virtual SIZE GetClientSize() const = NULL;
	virtual CDUIRect GetWindowRect() = NULL;
	virtual SIZE GetWndInitSize() = NULL;
	virtual void SetWndInitSize(int cx, int cy) = NULL;
	virtual SIZE GetWndMinSize() = NULL;
	virtual void SetWndMinSize(int cx, int cy) = NULL;
	virtual SIZE GetWndMaxSize() = NULL;
	virtual void SetWndMaxSize(int cx, int cy) = NULL;
	virtual RECT GetResizeTrack() = NULL;
	virtual void SetResizeTrack(RECT &rcResizeTrack) = NULL;
	virtual int GetCaptionHeight() = NULL;
	virtual void SetCaptionHeight(int nHeight) = NULL;
	virtual SIZE GetRoundCorner() = NULL;
	virtual void SetRoundCorner(int cx, int cy) = NULL;
	virtual const CMMDpi & GetDpiObj() = NULL;
	virtual int GetDpi() = NULL;
	virtual bool SetDpi(int nDpi) = NULL;
	virtual int GetScale() = NULL;
	virtual bool SetScale(int nScale) = NULL;

	//caret
	virtual bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight) = NULL;
	virtual bool IsShowCaret() = NULL;
	virtual void ShowCaret(bool bShow) = NULL;
	virtual void SetCaretPos(CDUIPoint pt) = NULL;

	//wnd alpha
	virtual BYTE GetWndAlpha() = NULL;
	virtual void SetWndAlpha(BYTE cbWndAlpha) = NULL;
	virtual bool IsWndLayered() = NULL;
	virtual void SetWndLayered(bool bLayered) = NULL;

	//Gdiplus text
	virtual bool IsGdiplusRenderText() = NULL;
	virtual void SetGdiplusRenderText(bool bGdiplus) = NULL;
	virtual Gdiplus::TextRenderingHint GetGdiplusRenderTextType() = NULL;
	virtual void SetGdiplusRenderTextType(Gdiplus::TextRenderingHint RenderType) = NULL;

	//animation
	virtual enDuiAnimateWndType GetAnimateWndType() = NULL;
	virtual void SetAnimateWndType(enDuiAnimateWndType AnimateWndType) = NULL;
	virtual int GetAnimateWndFrame() = NULL;
	virtual void SetAnimateWndFrame(int nFrame) = NULL;
	virtual int GetAnimateWndElapse() = NULL;
	virtual void SetAnimateWndElapse(int nElapse) = NULL;
	virtual void StartAnimationWnd(bool bShow = true) = NULL;
	virtual void StopAnimationWnd() = NULL;
	virtual bool IsAnimatingWnd() = NULL;

	//notify
	virtual bool AddINotify(IDUINotify *pINotify) = NULL;
	virtual bool RemoveINotify(IDUINotify *pINotify) = NULL;
	virtual void SendNotify(DUINotify &Notify) = NULL;
	virtual void SendNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0) = NULL;
	virtual void PostNotify(DUINotify &Notify) = NULL;
	virtual void PostNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0) = NULL;

	//find ctrl
	virtual CDUIControlBase * FindControl(POINT pt) const = NULL;
	virtual CDUIControlBase * FindControl(LPCTSTR lpszName) const = NULL;
	virtual CDUIControlBase * FindControlByShortCut(TCHAR chChar) const = NULL;
	virtual CDUIControlBase * FindControlByDrop(POINT pt) const = NULL;
	virtual CDUIControlBase * FindSubControlByName(CDUIContainerCtrl *pParent, LPCTSTR lpszCtrlName) = NULL;
	virtual CDUIControlBase * FindSubControlByPoint(CDUIContainerCtrl *pParent, POINT pt) const = NULL;
	virtual CDUIControlBase * FindSubControlByName(CDUIContainerCtrl *pParent, LPCTSTR lpszName) const = NULL;
	virtual VecDUIControlBase FindSubControlsByClass(CDUIContainerCtrl *pParent, LPCTSTR pstrClass) = NULL;

	//wnd message
	virtual bool AddPreMessagePtr(IDUIPreMessage *pInterface) = NULL;
	virtual bool RemovePreMessagePtr(IDUIPreMessage *pInterface) = NULL;

	//window dragdrop
	virtual bool BeginDragDrop(CDUIControlBase *pControl, WPARAM wParam, LPARAM lParam, int nFlag = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK) = NULL;
	virtual void EndDragDrop() = NULL;
	virtual void SetDropDescription(DROPIMAGETYPE dwDropType, PCWSTR pszMsg, PCWSTR pszInsert) = NULL;
	virtual void ClearDescription() = NULL;
	virtual const tagDuiDropData * GetDropData() = NULL;
	virtual HBITMAP ProductBitmap(IN std::vector<CDUIControlBase*> vecCtrl, OUT CDUIRect &rcBitmap) = NULL;

	//method
public:
	virtual bool OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes) = NULL;

	//static
public:
	//MK_CONTROL、MK_RBUTTON、MK_LBUTTON、MK_SHIFT、MK_ALT;
	static UINT MapKeyState();
};
//////////////////////////////////////////////////////////////////////////

#endif