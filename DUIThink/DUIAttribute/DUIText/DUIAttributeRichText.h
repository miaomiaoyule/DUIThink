#ifndef __DUIATTRIBUTERICHTEXT_H__
#define __DUIATTRIBUTERICHTEXT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, tagDuiRichText> UnorderMapDuiAttributeRichText;
typedef std::map<uint32_t, tagDuiRichText> MapDuiAttributeRichText;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeRichText INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeRichText = { 0x5D1D263F,0x9051,0x4B64,0xAD,0x29,0x80,0xDB,0x7E,0x4B,0x1E,0x17 };
class DUITHINK_API CDUIAttributeRichText
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeRichText)
	MMDeclare_ClassName(CDUIAttributeRichText)

public:
	CDUIAttributeRichText();
	virtual ~CDUIAttributeRichText(void);

	//variant
protected:
	uint32_t							m_uValueHash = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	//render
	virtual void Draw(HDC hDC, CDUIRect &rcPaint, bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, int nLineSpace, bool bShadow);

	//res
	virtual bool IsEmpty();
	virtual void Clear();
	virtual tagDuiRichText GetRichText();
	virtual void SetRichText(const tagDuiRichText &RichText);
	virtual void SetTextStyle(DWORD dwTextStyle);
	virtual VecDuiRichTextItem GetRichTextItem();
	virtual void SetRichTextItem(const VecDuiRichTextItem &vecRichTextItem);
	virtual void AddRichTextItem(const tagDuiRichTextItem &RichTextItem);
	virtual int GetLineLimit();
	virtual void SetLineLimit(int nLineLimit);

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

	virtual CDUISize MeasureString();

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;

	//help
protected:
	uint32_t GetValueID() override;
	void SetValueID(uint32_t uValueID) override;
	void NotifyChange() override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif