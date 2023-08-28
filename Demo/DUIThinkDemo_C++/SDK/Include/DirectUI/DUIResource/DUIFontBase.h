#ifndef __DUIFONTBASE_H__
#define __DUIFONTBASE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIFontBase INTERFACE_VERSION(1,1)
static const GUID IID_CDUIFontBase = { 0x93B50FA6,0x3BAE,0x4138,0x85,0xBB,0x1C,0x1B,0x5F,0xD7,0x5F,0xDD };
class DUITHINK_API CDUIFontBase
	: public CDUIResourceBase
{
	friend class CDUIGlobal;

	DuiDeclare_CreateResource(CDUIFontBase)
	MMDeclare_ClassName(CDUIFontBase)

public:
	CDUIFontBase(void);
	CDUIFontBase(LPCTSTR lpszResName, LPCTSTR lpszFontName, int nSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
	virtual ~CDUIFontBase(void);

	//组件变量
protected:
	HFONT								m_hFont = NULL;
	CMMString							m_strFontName;
	int									m_nSize = 0;
	int									m_nScale = 0;
	LONG								m_lWeight = 0;
	bool								m_bItalic = false;
	bool								m_bUnderline = false;
	bool								m_bStrikeOut = false;
	TEXTMETRIC							m_TextMetric = {};

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	enDuiResType GetResourceType() const override;
	virtual HFONT GetHandle();
	virtual CMMString GetFontName();
	virtual void SetFontName(LPCTSTR lpszName);
	virtual int GetSize();
	virtual void SetSize(int nSize);
	virtual LONG GetWeight();
	virtual void SetWeight(LONG lWeight);
	virtual bool IsBold();
	virtual void SetBold(bool bBold);
	virtual bool IsItalic();
	virtual void SetItalic(bool bItalic);
	virtual bool IsUnderline();
	virtual void SetUnderLine(bool bUnderLine);
	virtual bool IsStrikeOut();
	virtual void SetStrikeOut(bool bStrikeOut);

	//help
protected:
	//construct
	void ConstructResource(int nScale = 100) override;
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
	void OnDpiChanged(int nScale) override;

	//staic
public:
	static CMMString FormatFontDescribe(CMMString strName, int nSize, LONG lWeight, bool bItalic, bool bUnderline, bool bStrikeOut);
};

typedef std::vector<CDUIFontBase*> VecDuiFontBase;
typedef std::unordered_map<CMMString, CDUIFontBase*> MapDuiFontBase;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif