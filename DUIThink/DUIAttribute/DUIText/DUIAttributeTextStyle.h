#ifndef __DUIATTRIBUTETEXTSTYLE_H__
#define __DUIATTRIBUTETEXTSTYLE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, tagDuiTextStyle> UnorderMapDuiAttributeTextStyle;
typedef std::map<uint32_t, tagDuiTextStyle> MapDuiAttributeTextStyle;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeTextStyle INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeTextStyle = { 0x21E859BB,0x67D5,0x4A96,0x96,0x08,0x90,0xFA,0x32,0xA8,0x5E,0xC3 };
class DUITHINK_API CDUIAttributeTextStyle
	: public CDUIAttributeObject
{
	friend class CDUIThinkEditCtrl;
	friend class CPropertyTextStyle;

	DuiDeclare_CreateAttribute(CDUIAttributeTextStyle)
	MMDeclare_ClassName(CDUIAttributeTextStyle)

public:
	CDUIAttributeTextStyle(DWORD dwTextStyle = DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
	virtual ~CDUIAttributeTextStyle(void);

	//variant
protected:
	uint32_t							m_uValueHash = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//½Ó¿Ú
public:
	CDUIAttributeTextStyle & operator = (CDUIAttributeObject &Right) override;

	//render
	virtual void Draw(HDC hDC, CDUIRect &rcPaint, LPCTSTR lpszText, bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, bool bShadow);

	//res
	virtual bool empty();
	virtual void Clear();
	virtual tagDuiTextStyle GetTextStyle();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyle(DWORD dwTextStyle);
	virtual HFONT GetFont();
	virtual int GetFontSize();
	virtual LPCTSTR GetFontResName();
	virtual vector<CMMString> GetFontResSwitch();
	virtual void SetFontResSwitch(const vector<CMMString> &vecResSwitch);
	virtual void SetFontResSwitch(const CMMString &strResSwitch);
	virtual LPCTSTR GetColorResName();
	virtual ARGB GetTextColor();
	virtual vector<CMMString> GetColorResSwitch();
	virtual void SetColorResSwitch(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitch(const CMMString &strResSwitch);

	//align
	virtual bool IsAlignLeft();
	virtual void SetAlignLeft(bool bAlignLeft);
	virtual bool IsAlignCenter();
	virtual void SetAlignCenter(bool bAlignCenter);
	virtual bool IsAlignRight();
	virtual void SetAlignRight(bool bAlignRight);

	//valign
	virtual bool IsVAlignTop();
	virtual void SetVAlignTop(bool bVAlignTop);
	virtual bool IsVAlignCenter();
	virtual void SetVAlignCenter(bool bVAlignCenter);
	virtual bool IsVAlignBottom();
	virtual void SetVAlignBottom(bool bVAlignBottom);

	//multiline
	virtual bool IsWordBreak();
	virtual void SetWordBreak(bool bWordBreak);

	//...
	virtual bool IsEndEllipsis();
	virtual void SetEndEllipsis(bool bEndEllipsis);

	virtual CDUISize MeasureString(LPCTSTR lpszText);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;

	//help
protected:
	uint32_t GetValueID() override;
	void SetValueID(uint32_t uValueID) override;
	void NotifyChange() override;
	CDUIFontBase * GetFontBaseCur();
	CDUIColorBase * GetColorBaseCur();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif