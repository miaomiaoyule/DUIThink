#ifndef __DUICONTROLBASECTRL_H__
#define __DUICONTROLBASECTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIWndManager;
class CDUILayoutView;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIControlBase INTERFACE_VERSION(1,1)
static const GUID IID_CDUIControlBase = { 0x001037EB,0x1B2F,0x4DDD,0x82,0x48,0xB0,0xB4,0xA3,0x95,0xD0,0xD4 };
class DUITHINK_API CDUIControlBase
	: public CDUIPropertyObject
{
	friend class CDUIXmlPack;
	friend class CDUIWndManager;
	friend class CDUIContainerCtrl;
	friend class CDUIVerticalLayoutCtrl;
	friend class CDUIHorizontalLayoutCtrl;
	friend class CDUIListViewCtrl;
	friend class CDUIListItemCtrl;
	friend class CDUITabLayoutCtrl;
	friend class CDUIRichEditCtrl;
	friend class CDUIDragAnimation;
	friend class CDUIGlobal;
	friend class CDUIAttributePosition;
	friend class CDUIAttributeColorSwitch;
	friend class CDUILayoutView;
	friend class CControlNotifyView;

	DuiDeclare_CreateControl(CDUIControlBase)
	MMDeclare_ClassName(CDUIControlBase)

public:
	CDUIControlBase(void);
	virtual ~CDUIControlBase(void);

	//attribute
protected:
	//base
	CDUIAttributeGroup					m_AttributeGroupBase;
	CDUIAttributeULong					m_AttributeObjectID;
	CDUIAttributeBool					m_AttributeEnable;
	CDUIAttributeBool					m_AttributeVisible;
	CDUIAttributeText					m_AttributeUserData;
	CDUIAttributeLong					m_AttributeUserTag;
	CDUIAttributeBool					m_AttributeWinDragEnable;
	CDUIAttributeBool					m_AttributeWinDropEnable;

	//pos
	CDUIAttributeGroup					m_AttributeGroupPosition;
	CDUIAttributePosition				m_AttributePosition;
	CDUIAttributeBool					m_AttributeDpiPadding;
	CDUIAttributeSize					m_AttributeMinSize;
	CDUIAttributeSize					m_AttributeMaxSize;

	//back
	CDUIAttributeGroup					m_AttributeGroupBk;
	CDUIAttributeColorSwitch			m_AttributeColorBk;
	CDUIAttributeBool					m_AttributeIsColorHSL;
	CDUIAttriImageSection				m_AttributeImageBack;
	CDUIAttriImageSection				m_AttributeImageFore;
	CDUIAttributeSize					m_AttributeRoundSize;

	//border
	CDUIAttributeGroup					m_AttributeGroupBorder;
	CDUIAttributeColorSwitch			m_AttributeColorBorder;
	CDUIAttributeColorSwitch			m_AttributeColorBorderFocus;
	CDUIAttributeRect					m_AttributeBorderLine;
	CDUIAttributeCombox					m_AttributeBorderStyle;

	//mouse
	CDUIAttributeGroup					m_AttributeGroupMouse;
	CDUIAttributeBool					m_AttributeMouseThrough;
	CDUIAttributeBool					m_AttributeClickTransmit;
	CDUIAttributeCursor					m_AttributeCursor;
	CDUIAttributeBool					m_AttributeContextMenu;
	CDUIAttributeText					m_AttributeActiveUrl;

	//tip
	CDUIAttributeGroup					m_AttributeGroupToolTip;
	CDUIAttributeText					m_AttributeToolTip;
	CDUIAttributeLong					m_AttributeToolTipWidth;
	CDUIAttributeColor					m_AttributeToolTipBkColor;
	CDUIAttributeColor					m_AttributeToolTipTextColor;

	//variant
protected:
	CDUIContainerCtrl *					m_pParent = NULL;
	CDUIControlBase *					m_pOwnerModelCtrl = NULL;
	CDUIWndManager *					m_pWndManager = NULL;

	//pos
	CDUIRect							m_rcModalParent;
	CDUIRect							m_rcAbsolute;
	CDUIRect							m_rcPaint;

	//refresh
	bool								m_bInternVisible = true;
	bool								m_bRefreshViewNeeded = true;

	//status
	int									m_nControlStatus = ControlStatus_Normal;
	TCHAR								m_chShortcut = 0;

	//custom bk
	Gdiplus::Bitmap *					m_pBmpCustomBack = NULL;
	int									m_nCustomBackAlign = 0;

	//water
	tagDuiRippleBitmap *				m_pRippleBmp = NULL;

	//callback
	VecIDUIControlCallBack				m_vecIControlCallBack;

	//interface
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	virtual void OnResourceDelete(CDUIResourceBase *pResourceObj);
	virtual void OnResourceSwitch(int nIndexRes);

	//method 
	//** you can override if you need **//
public:
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	virtual CMMString GetDescribe() const override;
	virtual UINT GetControlFlags() ;
	virtual CDUIControlBase * Clone(bool bIncludeChild = true, bool bRefreshCtrlID = true);
	virtual int GetScale() override;
	virtual int GetControlCallBackCount();
	virtual IDUIControlCallBack * GetControlCallBack(int nIndex);
	virtual void RegisterControlCallBack(IDUIControlCallBack *pCallBack);
	virtual void UnRegisterControlCallBack(IDUIControlCallBack *pCallBack);

	//basic
	virtual UINT InitCtrlID() override;
	virtual UINT GetCtrlID();
	virtual bool SetCtrlID(UINT uID);
	virtual void RefreshCtrlID();
	virtual HWND GetWndHandle();
	virtual bool SetWndManager(CDUIWndManager *pWndManager);
	virtual CDUIWndManager * GetWndManager() override;
	virtual void SetParent(CDUIContainerCtrl *pParent);
	virtual CDUIContainerCtrl * GetParent() const;
	virtual CDUIControlBase * GetPrevSiblingCtrl();
	virtual CDUIControlBase * GetNextSiblingCtrl();
	virtual void SetVisible(bool bVisible = true);
	virtual bool IsVisible();
	virtual void SetEnabled(bool bEnabled);
	virtual bool IsEnabled();
	virtual bool IsMouseThrough();
	virtual void SetMouseThrough(bool bThrough);
	virtual bool IsClickTransmit();
	virtual void SetClickTransmit(bool bTransmit);
	virtual bool IsCaptured();
	virtual bool IsFocused();
	virtual void SetFocus();
	virtual void KillFocus();
	virtual bool Active();
	virtual enDuiCursor GetCursor();
	virtual void SetCursor(enDuiCursor Cursor);
	virtual bool IsContextMenu();
	virtual void SetContextMenu(bool bContextMenu);
	virtual TCHAR GetShortcut() const;
	virtual void SetShortcut(TCHAR ch);
	virtual CMMString GetActiveUrl();
	virtual void SetActiveUrl(LPCTSTR lpszUrl);

	//Model
	virtual void SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl);
	virtual CDUIControlBase * GetOwnerModelCtrl();

	//Layout
	virtual CDUIRect GetModalParentRect();
	virtual void MovePosition(SIZE szOffset);
	//************************************
	// Description:	if parent is not containerctrl, please don't use this
	// Method:    	SetAbsoluteRect
	// Access:    	public 
	// Returns:   	bool
	// Parameter: 	CDUIRect & rcAbsolute
	//************************************
	virtual bool SetAbsoluteRect(CDUIRect &rcAbsolute);
	virtual CDUIRect GetAbsoluteRect();
	virtual bool IsFloat();
	virtual void SetFloat(bool bFloat);
	virtual int GetWidth();
	virtual int GetFixedWidth();
	virtual void SetFixedWidth(int nWidth);
	virtual int GetHeight();
	virtual int GetFixedHeight();
	virtual void SetFixedHeight(int nHeight);
	virtual enDuiHorizAlignType GetHorizAlignType();
	virtual void SetHorizAlignType(enDuiHorizAlignType AlignType);
	virtual enDuiVertAlignType GetVertAlignType();
	virtual void SetVertAlignType(enDuiVertAlignType AlignType);
	virtual CDUIRect GetPadding();
	virtual void SetPadding(long lLeft, long lTop, long lRight, long lBottom);
	virtual void SetPadding(CDUIRect rcPadding);
	virtual void SetPaddingL(long lLeft);
	virtual void SetPaddingT(long lTop);
	virtual void SetPaddingR(long lRight);
	virtual void SetPaddingB(long lBottom);
	virtual bool IsPaddingHorizCenter();
	virtual void SetPaddingHorizCenter(int nXOffset = 0);
	virtual bool IsPaddingVertCenter();
	virtual void SetPaddingVertCenter(int nYOffset = 0);
	virtual bool IsDpiPadding();
	virtual void SetDpiPadding(bool bDpiPadding);
	virtual long GetMinWidth();
	virtual void SetMinWidth(long lWidth);
	virtual long GetMaxWidth();
	virtual void SetMaxWidth(long lWidth);
	virtual long GetMinHeight();
	virtual void SetMinHeight(long lHeight);
	virtual long GetMaxHeight();
	virtual void SetMaxHeight(long lHeight);

	//timer
	virtual bool SetTimer(UINT uTimerID, UINT nElapse);
	virtual bool KillTimer(UINT uTimerID);
	virtual bool KillTimer();

	//bk
	virtual ARGB GetBkColor();
	virtual void SetBkColor(const vector<CMMString> &vecResSwitch);
	virtual bool IsColorHSL();
	virtual void SetColorHSL(bool bColorHSL);
	virtual DWORD GetAdjustColor(DWORD dwColor);
	virtual tagDuiImageSection GetBkImageSection();
	virtual void SetBkImageSection(const tagDuiImageSection &ImageSection);
	virtual void SetBkImage(CMMString strImageRes);
	virtual void SetBkImage(std::vector<CMMString> vecImageRes);
	virtual tagDuiImageSection GetForeImageSection();
	virtual void SetForeImageSection(const tagDuiImageSection &ImageSection);
	virtual SIZE GetRoundSize();
	virtual void SetRoundSize(SIZE cxyRound);
	virtual Gdiplus::Bitmap * GetCustomBmpBack() const;
	virtual void SetCustomBmpBack(Gdiplus::Bitmap *pBmp, int nAlign = CustomBack_ScaleSuitable | CustomBack_Center | CustomBack_VCenter);
	virtual void SetCustomBmpBack(HBITMAP hBimap, int nAlign = CustomBack_ScaleSuitable | CustomBack_Center | CustomBack_VCenter);

	//border
	virtual enDuiLineStyle GetBorderStyle();
	virtual void SetBorderStyle(enDuiLineStyle LineStyle);
	virtual ARGB GetBorderColor();
	virtual void SetBorderColor(const vector<CMMString> &vecResSwitch);
	virtual ARGB GetFocusBorderColor();
	virtual void SetFocusBorderColor(const vector<CMMString> &vecResSwitch);
	virtual CDUIRect GetBorderLine();
	virtual void SetBorderLine(RECT rcBorder);

	//tip
	virtual CMMString GetToolTip();
	virtual void SetToolTip(LPCTSTR pstrText);
	virtual int GetToolTipWidth(void);
	virtual void SetToolTipWidth(int nWidth);
	virtual ARGB GetToolTipBkColor();
	virtual void SetToolTipBkColor(ARGB dwColor);
	virtual ARGB GetToolTipTextColor();
	virtual void SetToolTipTextColor(ARGB dwColor);

	//userdata
	virtual CMMString GetUserData();
	virtual void SetUserData(LPCTSTR pstrText);
	virtual UINT_PTR GetTag();
	virtual void SetTag(UINT_PTR pTag);

	//refresh
	virtual void Invalidate();
	virtual void NeedRefreshView();
	virtual void NeedParentRefreshView();
	virtual bool IsRefreshViewNeeded() const;
	virtual void RefreshView();
	virtual bool OnDraw(HDC hDC, const RECT &rcPaint, bool bGenerateBmp = false);
	virtual bool DoPaint(HDC hDC, bool bGenerateBmp = false);

	//window dragdrop
	virtual bool IsWinDragEnabled();
	virtual void SetWinDragEnabled(bool bDragEnabled);
	virtual bool IsWinDropEnabled();
	virtual void SetWinDropEnabled(bool bDropEnabled);

	//shadow 
	virtual bool IsShadowText();

	//message 
	//** you can override if you need, but you should execute this class first **//
protected:
	virtual bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg);

	virtual bool OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg);

	virtual bool OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiMouseHover(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg);

	virtual bool OnDuiSetFocus();
	virtual bool OnDuiKillFocus();

	virtual void OnDuiWndManagerAttach();
	virtual void OnDuiWndManagerDetach();
	virtual void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg);
	virtual void OnDuiSize(CDUIRect rcParentAbs);

	virtual LRESULT OnDuiKeyDown(const DuiMessage &Msg);
	virtual LRESULT OnDuiKeyUp(const DuiMessage &Msg);
	virtual LRESULT OnDuiChar(const DuiMessage &Msg);
	virtual LRESULT OnDuiSysKeyDown(const DuiMessage &Msg);
	virtual LRESULT OnDuiSysKeyUp(const DuiMessage &Msg);
	virtual LRESULT OnDuiSysChar(const DuiMessage &Msg);
	virtual LRESULT OnDuiContextMenu(const DuiMessage &Msg);
	virtual LRESULT OnDuiCommand(const DuiMessage &Msg);
	virtual LRESULT OnDuiImeComPosition(const DuiMessage &Msg);

	virtual bool OnWinDragStart(const CDUIPoint &pt, const DuiMessage &Msg);
	virtual bool OnWinDragEnter(const tagDuiDropData *pDropData, DWORD *pdwEffect);
	virtual bool OnWinDragOver(const tagDuiDropData *pDropData, DWORD &dwEffect);
	virtual bool OnWinDragLeave(const tagDuiDropData *pDropData);
	virtual bool OnWinDrop(const tagDuiDropData *pDropData, DWORD *pdwEffect);

	//property 
	//** you can override if you need, but you must execute this class first **//
protected:
	virtual void InitProperty();
	virtual void InitAttriValue();
	virtual void InitComplete();

	//paint
	//** you can override if you need **//
protected:
	virtual void PaintBkColor(HDC hDC);
	virtual void PaintBkImage(HDC hDC);
	virtual void PaintForeImage(HDC hDC);
	virtual void PaintStatusColor(HDC hDC);
	virtual void PaintStatusImage(HDC hDC);
	virtual void PaintText(HDC hDC);
	virtual void PaintBorder(HDC hDC);

	//help
	//** you can override if you need, but you'd better not do that **//
protected:
	virtual void SetInternVisible(bool bVisible = true);
	virtual CDUIControlBase * FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags);
	virtual void ReapControl();
	virtual CDUIAttributeColorSwitch * GetAttributeStatusColor();
	virtual CDUIAttriImageSection * GetAttributeStatusImage();
	virtual CDUIRect GetBorderRect();
	virtual CDUISize GetBorderBreakTop();
	virtual CDUIScrollBarCtrl * GetHorizScrollBar() const { return NULL; }
	virtual CDUIScrollBarCtrl * GetVertScrollBar() const { return NULL; }
};

DUITHINK_API bool operator == (IDUIInterface *pLeft, const CDUIControlBase &pControl);
DUITHINK_API bool operator != (IDUIInterface *pLeft, const CDUIControlBase &pControl);

typedef std::unordered_map<UINT, CDUIControlBase*> MapDuiControlBase;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif