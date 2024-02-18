#ifndef __DUIWNDSUBCLASS_H__
#define __DUIWNDSUBCLASS_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDTDesignerDoc;

typedef struct tagDuiTimerInfo
{
	CDUIPropertyObject *				pProp = NULL;
	CDUIControlBase *					pControl = NULL;
	UINT								nLocalID = 0;
	HWND								hWnd = NULL;
	UINT								uWinTimer = 0;
	bool								bKilled = false;
} DUITimerInfo;

typedef struct tagDuiFindShortCut
{
	TCHAR								chChar = 0;
} DUIFindShortCut;

typedef std::vector<DUITimerInfo> VecDuiTimerInfo;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWndManager INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWndManager = { 0xD5D0FF1C,0x106B,0x48C7,0xAD,0xA9,0x8C,0xA3,0x43,0x80,0x1F,0x74 };
class DUITHINK_API CDUIWndManager
	: public CDUIPropertyObject
	, public CDUIAnimationWnd
#if (NTDDI_VERSION >= NTDDI_VISTA)
	, public CMMDragDrop
#endif
{
	friend class CDUIWnd;
	friend class CDUIGlobal;
	friend class CDUIXmlPack;
	friend class CDTDesignerDoc;
	friend class CDesignerView;
	friend class CControlView;
	friend class CDuiLayoutView;
	friend class CDUIContainerCtrl;
	friend class CDlgLogon;

	DuiDeclare_CreateWndManager(CDUIWndManager)
	MMDeclare_ClassName(CDUIWndManager)

public:
	CDUIWndManager();
	virtual ~CDUIWndManager(void);

	//attribute
private:
	CDUIAttributeGroup					m_AttributeGroupWndInfo;
	CDUIAttributeLong					m_AttributeWndCaptionHeight;
	CDUIAttributeSize					m_AttributeWndInitSize;
	CDUIAttributeSize					m_AttributeWndMinSize;
	CDUIAttributeSize					m_AttributeWndMaxSize;
	CDUIAttributeRect					m_AttributeWndResizeTrack;

	CDUIAttributeGroup					m_AttributeGroupLayered;
	CDUIAttributeBool					m_AttributeWndLayered;
	CDUIAttributeLong					m_AttributeWndAlpha;

	CDUIAttributeGroup					m_AttributeGroupRenderText;
	CDUIAttributeBool					m_AttributeGdiplusRenderText;
	CDUIAttributeCombox					m_AttributeGdiplusRenderType;

	CDUIAttributeGroup					m_AttributeGroupRenderImage;
	CDUIAttributeBool					m_AttributeGdiplusRenderImage;

	CDUIAttributeGroup					m_AttributeGroupAnimation;
	CDUIAttributeCombox					m_AttributeAnimationType;
	CDUIAttributeLong					m_AttributeAnimationFrame;
	CDUIAttributeLong					m_AttributeAnimationElapse;

	CDUIAttributeGroup					m_AttributeGroupCustom;
	CDUIAttributeLong					m_AttributeUserTag;

	//variant
private:
	//handle
	HWND								m_hWndAttach = NULL;
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

	//duithink
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

	VecIDUINotify						m_vecINotify;
	VecDuiNotify						m_vecAsynNotify;
	VecDuiTimerInfo						m_vecTimers;
	VecIDUIPreMessage					m_vecPreMessage;
	DequeDuiControlBase					m_queDelayDelete;
	VecDuiControlBase					m_vecFoundControls;
	MapDuiControlBase					m_mapControl;
	MapDuiRadioBoxGroup					m_mapRadioBoxGroup;

	//override
protected:
	bool RegisterControlListen(IDUIInterface *pIControlListen) override;
	bool UnRegisterControlListen(IDUIInterface *pIControlListen) override;
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

#if (NTDDI_VERSION >= NTDDI_VISTA)
	//override
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

	//handle
	HWND GetWndHandle() override;
	HDC GetWndDC() override;

	//capture
	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured();
	CDUIControlBase * GetCaptureControl();

	//focus
	CDUIControlBase * GetFocusControl();
	void SetFocusControl(CDUIControlBase *pFocusCtrl);

	//hover
	CDUIControlBase * GetHoverCtrl();

	//root
	void AttachDialog(HWND hWnd);
	bool AttachRootCtrl(CDUIContainerCtrl *pControl);
	bool ReleaseRootCtrl();
	CDUIContainerCtrl * GetRootCtrl() override;
	CDUIContainerCtrl * DetachRootCtrl();

	//manager ctrl
	bool InitControls(CDUIControlBase *pControl);
	void InitControlIDHash(CDUIControlBase *pControl);
	void UnInitControlIDHash(CDUIControlBase *pControl);
	void ReapControl(CDUIControlBase *pControl);
	bool VerifyControl(CDUIControlBase *pControl);

	//tag
	virtual UINT_PTR GetTag();
	virtual void SetTag(UINT_PTR pTag);

	//timer
	bool SetTimer(CDUIPropertyObject *pProp, UINT uTimerID, UINT uElapse);
	bool FindTimer(CDUIPropertyObject *pProp, UINT uTimerID);
	bool KillTimer(CDUIPropertyObject *pProp, UINT uTimerID);
	bool KillTimer(CDUIPropertyObject *pProp);
	bool RemoveAllTimer();

	//radio
	bool AddRadioBoxToGroup(CDUIRadioBoxCtrl *pControl);
	VecDuiRadioBoxCtrl GetRadioBoxGroup(LPCTSTR lpszGroup);
	bool RemoveRadioBoxFromGroup(CDUIRadioBoxCtrl *pControl);
	void RemoveAllRadioBoxGroup();
	CDUIRadioBoxCtrl * GetRadioBoxOfBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl, long lBindIndex);

	//tips
	HWND GetToolTipWindow() const;
	int GetToolTipHoverTime() const;
	void SetToolTipHoverTime(int nTime);

	//refresh
	void Invalidate();
	bool IsRefreshViewNeeded() const;
	void NeedRefreshView();
	POINT GetMousePosLast() const;
	POINT GetMousePosDown() const;

	//window
	SIZE GetClientSize() const;
	CDUIRect GetWindowRect();
	SIZE GetWndInitSize();
	void SetWndInitSize(int cx, int cy);
	SIZE GetWndMinSize();
	void SetWndMinSize(int cx, int cy);
	SIZE GetWndMaxSize();
	void SetWndMaxSize(int cx, int cy);
	RECT GetResizeTrack();
	void SetResizeTrack(RECT &rcResizeTrack);
	int GetCaptionHeight();
	void SetCaptionHeight(int nHeight);
	const CMMDpi & GetDpiObj() override;
	int GetDpi();
	bool SetDpi(int nDpi);
	int GetScale() override;
	bool SetScale(int nScale);
	CDUIWndManager * GetWndManager() override;

	//caret
	bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
	bool IsShowCaret();
	void ShowCaret(bool bShow);
	void SetCaretPos(CDUIPoint pt);

	//wnd alpha
	BYTE GetWndAlpha() override;
	void SetWndAlpha(BYTE cbWndAlpha);
	bool IsWndLayered() override;
	void SetWndLayered(bool bLayered) override;

	//ddiplus text
	bool IsGdiplusRenderText();
	void SetGdiplusRenderText(bool bGdiplus);
	Gdiplus::TextRenderingHint GetGdiplusRenderTextType();
	void SetGdiplusRenderTextType(Gdiplus::TextRenderingHint RenderType);

	//gdiplus image
	bool IsGdiplusRenderImage();
	void SetGdiplusRenderImage(bool bGdiplus);

	//animation
	enDuiAnimateWndType GetAnimateWndType() override;
	void SetAnimateWndType(enDuiAnimateWndType AnimateWndType);
	int GetAnimateWndFrame() override;
	void SetAnimateWndFrame(int nFrame);
	int GetAnimateWndElapse() override;
	void SetAnimateWndElapse(int nElapse);

	//notify
	bool AddINotify(IDUINotify *pINotify);
	bool RemoveINotify(IDUINotify *pINotify);
	void SendNotify(DuiNotify &Notify);
	void SendNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0);
	void PostNotify(DuiNotify &Notify);
	void PostNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0);

	//find ctrl
	CDUIControlBase * FindControl(POINT pt) const;
	CDUIControlBase * FindControl(UINT uCtrlID) const;
	CDUIControlBase * FindControlByShortCut(TCHAR chChar) const;
	CDUIControlBase * FindControlByDrop(POINT pt) const;
	CDUIControlBase * FindControlByWheel(POINT pt, int nWheelDelta = WHEEL_DELTA) const;
	CDUIControlBase * FindSubControlByPoint(CDUIContainerCtrl *pParent, POINT pt);
	CDUIControlBase * FindSubControlByID(CDUIContainerCtrl *pParent, UINT uCtrlID);
	VecDuiControlBase FindSubControlsByClass(CDUIContainerCtrl *pParent, LPCTSTR pstrClass);

	//wnd message
	bool AddPreMessagePtr(IDUIPreMessage *pInterface);
	bool RemovePreMessagePtr(IDUIPreMessage *pInterface);

	//window dragdrop
	bool BeginDragDrop(CDUIControlBase *pControl, WPARAM wParam, LPARAM lParam, int nFlag = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
	void EndDragDrop();
#if (NTDDI_VERSION >= NTDDI_VISTA)
	void SetDropDescription(DROPIMAGETYPE dwDropType, PCWSTR pszMsg, PCWSTR pszInsert);
#endif
	void ClearDescription();
	const tagDuiDropData * GetDropData();
	HBITMAP ProductBitmap(IN std::vector<CDUIControlBase*> vecCtrl, IN CDUIControlBase* pFromCtrl, OUT CDUIRect &rcBitmap, OUT CDUIPoint &ptFromOffset);

	//message
public:
	bool OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
	bool OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);

	//property	   
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode);

	//override
protected:
	bool SetTimer(UINT uTimerID, UINT nElapse);
	bool KillTimer(UINT uTimerID);
	bool KillTimer();

	//message
private:
	LRESULT OnDraw(CDUIRect rcPaint);

	LRESULT OnDuiDelayDelete();
	LRESULT OnDuiLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiLButtonDlk(WPARAM wParam, LPARAM lParam);

	LRESULT OnDuiRButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiRButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiRButtonDlk(WPARAM wParam, LPARAM lParam);

	LRESULT OnDuiMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMouseHover(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMouseLeave(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMouseWheel(WPARAM wParam, LPARAM lParam);

	LRESULT OnDuiTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiSize();

	LRESULT OnDuiKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiChar(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiSysKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiSysKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiSysChar(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiContextMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiKillFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiSetCursor(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiDpiChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiImeComPosition(WPARAM wParam, LPARAM lParam);

	void OnDpiChanged(int nScalePre);
	void OnResourceDelete(CDUIResourceBase *pResourceObj);
	void OnResourceSwitch(int nIndexRes);

	//help
protected:
	void AdjustWndSize();
	void AdjustImagesHSL();
	void RefreshLayout();
	void DelayDelete(CDUIControlBase *pControl);
	void DispatchNotify();
	void PostAppMsg();
	void ReleasePaintScene();

private:
	VecDuiControlBase & GetFoundControls();
	static CDUIControlBase * CALLBACK __FindControlFromIDHash(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromCount(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromPoint(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromShortcut(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromDrop(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromID(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromWheel(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlsFromClass(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlsFromUpdate(CDUIControlBase *pThis, LPVOID pData);

	//static
public:
	static UINT MapKeyState();
};

//////////////////////////////////////////////////////////////////////////

#endif