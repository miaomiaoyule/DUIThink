#ifndef __DUIWND_H__
#define __DUIWND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWnd INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWnd = { 0xD5D0FF1C,0x106B,0x48C7,0xAD,0xA9,0x8C,0xA3,0x43,0x80,0x1F,0x74 };
class DUITHINK_API CDUIWnd
	: public CDUIPropertyObject
	, public CDUIAnimationWnd
	, public CDUINotifyPump
	, public IDuiPreMessage
#if (NTDDI_VERSION >= NTDDI_VISTA)
	, public CMMDragDrop
#endif
{
	friend class CDUIWnd;
	friend class CDUIGlobal;
	friend class CDUIXmlPack;
	friend class CDesignerView;
	friend class CControlView;
	friend class CDuiLayoutView;
	friend class CDUIContainerCtrl;
	friend class CDlgLogon;

	MMDeclare_ClassName(CDUIWnd)
	DuiDeclare_Message_Map()

public:
	CDUIWnd(LPCTSTR lpszDuiName = NULL, HWND hWndParent = NULL);
	virtual ~CDUIWnd();

	//attribute
private:
	CDUIAttributeGroup					m_AttributeGroupWndInfo;
	CDUIAttributeLong					m_AttributeWndCaptionHeight = 30;
	CDUIAttributeSize					m_AttributeWndInitSize = SIZE{ 500, 500 };
	CDUIAttributeSize					m_AttributeWndMinSize;
	CDUIAttributeSize					m_AttributeWndMaxSize;
	CDUIAttributeRect					m_AttributeWndResizeTrack;

	CDUIAttributeGroup					m_AttributeGroupLayered;
	CDUIAttributeBool					m_AttributeWndLayered;
	CDUIAttributeLong					m_AttributeWndAlpha = 255;

	CDUIAttributeGroup					m_AttributeGroupRenderText;
	CDUIAttributeBool					m_AttributeGdiplusRenderText;
	CDUIAttributeCombox					m_AttributeGdiplusRenderType;

	CDUIAttributeGroup					m_AttributeGroupRenderImage;
	CDUIAttributeBool					m_AttributeGdiplusRenderImage;

	CDUIAttributeGroup					m_AttributeGroupAnimation;
	CDUIAttributeCombox					m_AttributeAnimationType;
	CDUIAttributeLong					m_AttributeAnimationFrame = ANIMATE_FRAME;
	CDUIAttributeLong					m_AttributeAnimationElapse = Dui_TimerAnimate_Elapse;

	CDUIAttributeGroup					m_AttributeGroupCustom;
	CDUIAttributeLong					m_AttributeUserTag;

	//variant
protected:
	//wnd
	CMMString							m_strDuiName;
	HWND								m_hWndParent = NULL;
	HWND								m_hWnd = NULL;
	WNDPROC								m_OldWndProc = NULL;
	bool								m_bSubWindow = false;
	CDUIPoint							m_ptCreate = {};
	UINT								m_uCtrlIDClose = 0;

	//paint
	HDC									m_hDCPaint = NULL;
	HDC									m_hMemDcBackground = NULL;
	HBITMAP								m_hBmpBackgroundOld = NULL;
	HBITMAP								m_hBmpBackground = NULL;
	LPBYTE								m_pBmpBackgroundBits = NULL;

	//tip
	HWND								m_hWndTooltip = NULL;
	TOOLINFO							m_ToolTip = {};
	int									m_nToolTipHoverTime = 400UL;
	CMMString							m_strToolTip;

	//info
	UINT								m_uTimerID = 0x1000;
	POINT								m_ptMousePosLast;
	POINT								m_ptMousePosDown;
	bool								m_bMouseTracking = false;
	bool								m_bRefreshToolTip = false;
	bool								m_bRefreshViewNeeded = false;
	bool								m_bPostedAppMsg = false;
	bool								m_bFirstLayout = true;

	//designer
	bool								m_bDesigned = false;

	//caret
	CDUIRect							m_rcCaret;
	bool								m_bCaretActive = false;
	bool								m_bCaretShowing = false;

	//ctrl
	CDUIContainerCtrl *					m_pRootCtrl = NULL;
	CDUIControlBase *					m_pCaptureCtrl = NULL;
	CDUIControlBase *					m_pFocusCtrl = NULL;
	CDUIControlBase *					m_pHoverCtrl = NULL;
	CDUIControlBase *					m_pEventCtrl = NULL;
	CDUIControlBase *					m_pWinDragCtrl = NULL;
	CDUIControlBase *					m_pWinDragEnterCtrl = NULL;

	//drop
	tagDuiDropData						m_DropData;

	VecIDuiNotify						m_vecINotify;
	VecDuiNotify						m_vecAsynNotify;
	VecDuiTimerInfo						m_vecTimers;
	VecIDuiPreMessage					m_vecPreMessage;
	DequeDuiControlBase					m_queDelayDelete;
	VecDuiControlBase					m_vecFoundControls;
	MapDuiControlBase					m_mapControl;
	MapDuiRadioBoxGroup					m_mapRadioBoxGroup;

	//override
protected:
	bool RegisterControlListen(IDuiInterface *pIControlListen) override;
	bool UnRegisterControlListen(IDuiInterface *pIControlListen) override;
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//override
#if (NTDDI_VERSION >= NTDDI_VISTA)
protected:
	void OnWinDragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect) override;
	void OnWinDragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect) override;
	void OnWinDragLeave() override;
	void OnWinDrop(IDataObject *pIDataObject, POINTL pt, DWORD *pdwEffect) override;
#endif

	//method
public:
	bool IsDesigned();
	void SetDesigned(bool bDesigned);

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	virtual HWND GetWndHandle() const;
	virtual HDC GetWndDC() override;
	virtual operator HWND() const;
	virtual CMMString GetDuiName() const;
	virtual UINT GetClassStyle() const;

	virtual HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
	virtual HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
	virtual HWND SubWindow(HWND hWnd);
	virtual void UnSubWindow();
	virtual void ShowWindow(bool bShow = true, bool bTakeFocus = true);
	virtual UINT DoModal();
	virtual UINT DoBlock();
	virtual void Close(UINT nRet = IDOK);
	virtual void CenterWindow();
	virtual void SetIcon(UINT nRes);

	virtual LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual void ResizeWnd(int cx = -1, int cy = -1);
	virtual void AdjustWndPos();
	virtual LPCTSTR GetSuperClassName() const;

	//capture
	virtual void SetCapture();
	virtual void ReleaseCapture();
	virtual bool IsCaptured();
	virtual CDUIControlBase * GetCaptureControl();

	//focus
	virtual CDUIControlBase * GetFocusControl();
	virtual void SetFocusControl(CDUIControlBase *pFocusCtrl);

	//hover
	virtual CDUIControlBase * GetHoverCtrl();

	//root
	virtual bool AttachRootCtrl(CDUIContainerCtrl *pControl);
	virtual bool ReleaseRootCtrl();
	virtual CDUIContainerCtrl * GetRootCtrl() override;
	virtual CDUIContainerCtrl * DetachRootCtrl();

	//manager ctrl
	virtual bool InitControls(CDUIControlBase *pControl);
	virtual void InitControlIDHash(CDUIControlBase *pControl);
	virtual void UnInitControlIDHash(CDUIControlBase *pControl);
	virtual void ReapControl(CDUIControlBase *pControl);
	virtual bool VerifyControl(CDUIControlBase *pControl);

	//tag
	virtual virtual UINT_PTR GetTag();
	virtual virtual void SetTag(UINT_PTR pTag);

	//timer
	virtual bool SetTimer(CDUIPropertyObject *pPropObj, UINT uTimerID, UINT uElapse);
	virtual bool FindTimer(CDUIPropertyObject *pPropObj, UINT uTimerID);
	virtual bool KillTimer(CDUIPropertyObject *pPropObj, UINT uTimerID);
	virtual bool KillTimer(CDUIPropertyObject *pPropObj);
	virtual bool RemoveAllTimer();

	//radio
	virtual bool AddRadioBoxToGroup(CDUIRadioBoxCtrl *pControl);
	virtual VecDuiRadioBoxCtrl GetRadioBoxGroup(LPCTSTR lpszGroup);
	virtual bool RemoveRadioBoxFromGroup(CDUIRadioBoxCtrl *pControl);
	virtual void RemoveAllRadioBoxGroup();
	virtual CDUIRadioBoxCtrl * GetRadioBoxOfBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl, long lBindIndex);

	//tips
	virtual HWND GetToolTipWindow() const;
	virtual int GetToolTipHoverTime() const;
	virtual void SetToolTipHoverTime(int nTime);

	//refresh
	virtual void Invalidate();
	virtual bool IsRefreshViewNeeded() const;
	virtual void NeedRefreshView();
	virtual POINT GetMousePosLast() const;
	virtual POINT GetMousePosDown() const;

	//window
	virtual SIZE GetClientSize() const;
	virtual CDUIRect GetWindowRect();
	virtual SIZE GetWndInitSize();
	virtual void SetWndInitSize(int cx, int cy);
	virtual SIZE GetWndMinSize();
	virtual void SetWndMinSize(int cx, int cy);
	virtual SIZE GetWndMaxSize();
	virtual void SetWndMaxSize(int cx, int cy);
	virtual RECT GetResizeTrack();
	virtual void SetResizeTrack(RECT &rcResizeTrack);
	virtual int GetCaptionHeight();
	virtual void SetCaptionHeight(int nHeight);
	virtual const CMMDpi & GetDpiObj() override;
	virtual int GetDpi();
	virtual bool SetDpi(int nDpi);
	virtual int GetScale() override;
	virtual bool SetScale(int nScale);

	//caret
	virtual bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
	virtual bool IsShowCaret();
	virtual void ShowCaret(bool bShow);
	virtual void SetCaretPos(CDUIPoint pt);

	//wnd alpha
	virtual BYTE GetWndAlpha() override;
	virtual void SetWndAlpha(BYTE cbWndAlpha);
	virtual bool IsWndLayered() override;
	virtual void SetWndLayered(bool bLayered) override;

	//gdiplus text
	virtual bool IsGdiplusRenderText();
	virtual void SetGdiplusRenderText(bool bGdiplus);
	virtual Gdiplus::TextRenderingHint GetGdiplusRenderTextType();
	virtual void SetGdiplusRenderTextType(Gdiplus::TextRenderingHint RenderType);

	//gdiplus image
	virtual bool IsGdiplusRenderImage();
	virtual void SetGdiplusRenderImage(bool bGdiplus);

	//animation
	virtual enDuiAnimateWndType GetAnimateWndType() override;
	virtual void SetAnimateWndType(enDuiAnimateWndType AnimateWndType);
	virtual int GetAnimateWndFrame() override;
	virtual void SetAnimateWndFrame(int nFrame);
	virtual int GetAnimateWndElapse() override;
	virtual void SetAnimateWndElapse(int nElapse);

	//notify
	virtual bool AddINotify(IDuiNotify *pINotify);
	virtual bool RemoveINotify(IDuiNotify *pINotify);
	virtual void SendNotify(DuiNotify &Notify);
	virtual void SendNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0);
	virtual void PostNotify(DuiNotify &Notify);
	virtual void PostNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0);

	//find ctrl
	virtual CDUIControlBase * FindControl(POINT pt) const;
	virtual CDUIControlBase * FindControl(UINT uCtrlID) const;
	virtual CDUIControlBase * FindControlByShortCut(TCHAR chChar) const;
	virtual CDUIControlBase * FindControlByDrop(POINT pt) const;
	virtual CDUIControlBase * FindSubControlByPoint(CDUIContainerCtrl *pParent, POINT pt);
	virtual CDUIControlBase * FindSubControlByID(CDUIContainerCtrl *pParent, UINT uCtrlID);
	virtual VecDuiControlBase FindSubControlsByClass(CDUIContainerCtrl *pParent, LPCTSTR pstrClass);

	//wnd message
	virtual bool AddPreMessagePtr(IDuiPreMessage *pInterface);
	virtual bool RemovePreMessagePtr(IDuiPreMessage *pInterface);

	//window dragdrop
	virtual bool BeginDragDrop(CDUIControlBase *pControl, WPARAM wParam, LPARAM lParam, int nFlag = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
	virtual void EndDragDrop();
#if (NTDDI_VERSION >= NTDDI_VISTA)
	virtual void SetDropDescription(DROPIMAGETYPE dwDropType, PCWSTR pszMsg, PCWSTR pszInsert);
#endif
	virtual void ClearDescription();
	virtual const tagDuiDropData * GetDropData();
	virtual HBITMAP ProductBitmap(IN std::vector<CDUIControlBase*> vecCtrl, IN CDUIControlBase* pFromCtrl, OUT CDUIRect &rcBitmap, OUT CDUIPoint &ptFromOffset);

	//property	   
protected:
	void InitProperty() override;
	void InitComplete() override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild = true) override;

	//override
protected:
	bool SetTimer(UINT uTimerID, UINT nElapse) override;
	bool KillTimer(UINT uTimerID) override;
	bool KillTimer() override;

	//message
protected:
	LRESULT OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	virtual LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnClose(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcCalcSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDlk(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonDlk(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysChar(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint(CDUIRect rcPaint);
	virtual LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnImeComPosition(WPARAM wParam, LPARAM lParam);

	//notify
protected:
	virtual void OnFindControl() {}
	virtual void OnInitDialog() {}
	virtual void OnFinalMessage();
	virtual void OnDuiTimer(const DuiNotify &Notify);
	virtual void OnDuiClick(const DuiNotify &Notify);
	virtual void OnDuiWndAnimateFinish(const DuiNotify &Notify);

	//help
protected:
	CDUIWnd * GetWndOwner() override;
	void OnDuiDelayDelete();
	void OnDpiChanged(int nScalePre);
	void OnResourceDelete(CDUIResourceBase *pResourceObj);
	void OnResourceSwitch(int nIndexRes);
	void AdjustWndSize();
	void AdjustImagesHSL();
	void RefreshLayout();
	void DelayDelete(CDUIControlBase *pControl);
	void DispatchNotifyAsyn();
	void PostAppMsg();
	void ReleasePaintScene();

private:
	bool RegisterSuperclass();
	bool RegisterWindowClass();

	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	VecDuiControlBase & GetFoundControls();
	static CDUIControlBase * CALLBACK __FindControlFromIDHash(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromCount(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromPoint(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromShortcut(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromDrop(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromID(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlsFromClass(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlsFromUpdate(CDUIControlBase *pThis, LPVOID pData);

	//static
public:
	static void ForegroundWindow(HWND hWnd);
	static UINT MapKeyState();
};

//////////////////////////////////////////////////////////////////////////

#endif