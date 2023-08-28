#ifndef __DUIATTRIBUTECOLOR_H__
#define __DUIATTRIBUTECOLOR_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeColor INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeColor = { 0x1FB46B68,0x66AA,0x44B5,0x89,0xBE,0xCB,0x00,0x1C,0xF7,0x61,0xEE };
class DUITHINK_API CDUIAttributeColor 
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeColor)
	MMDeclare_ClassName(CDUIAttributeColor)

public:
	CDUIAttributeColor(void);
	~CDUIAttributeColor(void);

protected:
	ARGB								m_dwColor = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeColor & operator = (CDUIAttributeObject &Right) override;

	virtual bool SetColor(ARGB dwColor);
	virtual bool SetColorValueA(BYTE cbA);
	virtual bool SetColorValueR(BYTE cbR);
	virtual bool SetColorValueG(BYTE cbG);
	virtual bool SetColorValueB(BYTE cbB);

	virtual ARGB GetColorValue();
	virtual Color GetColorObject();

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif