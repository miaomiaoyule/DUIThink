#ifndef __DUICOLORPALETTECTRL_H__
#define __DUICOLORPALETTECTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIColorPaletteCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIColorPaletteCtrl = { 0xED79C96E,0xC23F,0x43D4,0x9B,0xD5,0x6B,0x79,0x2E,0xF0,0xF2,0x42 };
class XUI_API CDUIColorPaletteCtrl
	: public CDUIContainerCtrl
	, public IDuiControlCallBack
{
	MMDeclare_Super(CDUIContainerCtrl)
	DuiDeclare_CreateControl(CDUIColorPaletteCtrl)
	MMDeclare_ClassName(CDUIColorPaletteCtrl)

public:
	CDUIColorPaletteCtrl(void);
	~CDUIColorPaletteCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupPalette;
	CDUIAttributeColor					m_AttributeColor = DUIARGB(255, 170, 85, 85);
	CDUIAttributeTextStyle				m_AttributeTextStyle;
	CDUIAttributeLong					m_AttributeEditHeight = 28;

	//variant
protected:
	CDUIXEditCtrl *						m_pEditColorCtrl = NULL;
	Gdiplus::Bitmap *					m_pBmpWheel = NULL;
	Gdiplus::Bitmap *					m_pBmpValue = NULL;
	Gdiplus::Bitmap *					m_pBmpMark = NULL;
	CDUIRect							m_rcPalette;
	CDUIRect							m_rcValue;
	Gdiplus::ARGB						m_dwValueTop = 0;
	float								m_fCx = 0.0f;
	float								m_fCy = 0.0f;
	float								m_fOuter = 0.0f;
	float								m_fHue = 0.0f;
	float								m_fSat = 0.5f;
	float								m_fVal = 1.0f;
	float								m_fMarkR = 0.0f;
	BYTE								m_cbAlpha = 255;
	int									m_nWheelLeft = 0;
	int									m_nWheelTop = 0;
	int									m_nWheelSize = 0;
	int									m_nMarkSize = 0;
	int									m_nDragPart = 0;
	bool								m_bSyncEdit = false;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnRelease(CDUIControlBase *pControl) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void RefreshView() override;

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIXEditCtrl * GetChildAt(int nIndex) const override;

	//text
	virtual HFONT GetFont();
	virtual Gdiplus::ARGB GetTextColor();
	virtual tagDuiTextStyle GetTextStyle();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);

	//color
	virtual Gdiplus::ARGB GetPaletteColor();
	virtual void SetPaletteColor(Gdiplus::ARGB dwColor);
	virtual CDUIXEditCtrl * GetEditColorCtrl();
	virtual int GetEditHeight();
	virtual void SetEditHeight(int nHeight);

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiWndManagerAttach() override;

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:
	void PaintBkImage(HDC hDC) override;

	//help
protected:
	void ApplyColor(Gdiplus::ARGB dwColor);
	void SyncEditText();
	void SyncPaletteRect();
	void LayoutWheel();
	void UpdateColorFromPoint(const CDUIPoint &pt);
	void EnsureWheelBitmap();
	void EnsureValueBitmap();
	void EnsureMarker();
	int HitPart(const CDUIPoint &pt) const;
	Gdiplus::ARGB MakeColor() const;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
