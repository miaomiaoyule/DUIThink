#ifndef __DUICOLORBASE_H__
#define __DUICOLORBASE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIColorBase INTERFACE_VERSION(1,1)
static const GUID IID_CDUIColorBase = { 0x371BBE94,0xAB17,0x4F2E,0x91,0x94,0x64,0x6B,0xF0,0xE2,0x5A,0xF5 };
class DUITHINK_API CDUIColorBase 
	: public CDUIResourceBase
{
	DuiDeclare_CreateResource(CDUIColorBase)
	MMDeclare_ClassName(CDUIColorBase)

public:
	CDUIColorBase(void);
	CDUIColorBase(LPCTSTR lpszResName, DWORD dwColor);
	virtual ~CDUIColorBase(void);

	//组件变量
protected:
	DWORD								m_dwColor = 0x00000000;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	enDuiResType GetResourceType() const override;
	virtual DWORD GetColor();
	virtual void SetColor(DWORD dwColor);

	//property
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

typedef std::vector<CDUIColorBase*> VecDuiColorBase;
typedef std::unordered_map<CMMString, CDUIColorBase*> MapDuiColorBase;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif