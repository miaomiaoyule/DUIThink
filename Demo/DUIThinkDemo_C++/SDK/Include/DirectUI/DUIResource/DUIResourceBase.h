#ifndef __DUIRESOURCEBASE_H__
#define __DUIRESOURCEBASE_H__

#pragma once
#pragma pack(1)

class CDUIXmlPack;
class CDUIGlobal;
class CDUIResourceBase;
//////////////////////////////////////////////////////////////////////////
#define VER_CDUIResourceBase INTERFACE_VERSION(1,1)
static const GUID IID_CDUIResourceBase = { 0x8610FFDD,0xC1A9,0x49A6,0xBA,0xDA,0xA9,0x77,0x5F,0x87,0x04,0x01 };
class DUITHINK_API CDUIResourceBase 
	: public IDUIInterface
{
	friend class CDUIXmlPack;
	friend class CDUIGlobal;
	friend class CDUIWndImageList;
	friend class CDUIWndFontList;
	friend class CDUIWndColorList;
	friend class CDUIWndResList;
	friend class CProjectView;
	friend class CDUIPreview;

	DuiDeclare_CreateResource(CDUIResourceBase)
	MMDeclare_ClassName(CDUIResourceBase)

public:
	CDUIResourceBase();
	virtual ~CDUIResourceBase(void);

	//variant
protected:
	CMMString							m_strName;
	bool								m_bDesign = false;
#ifdef DUI_DESIGN
	int									m_nIndex = 0;
#endif

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	virtual LPCTSTR GetResourceName() const;
	virtual enDuiResType GetResourceType() const;

	//help
protected:
	bool IsDesign();
	void SetDesign(bool bDesign);
	int GetIndex();
	void SetIndex(int nIndex);

	//help
protected:
	bool SetResourceName(const CMMString &strName);
	virtual void ConstructResource(int nScale = 100);
	virtual bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue);
	virtual bool SaveAttribute(tinyxml2::XMLElement *pNode);
	virtual void OnDpiChanged(int nScale);
};

typedef std::vector<CDUIResourceBase*> VecDuiResourceBase;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif