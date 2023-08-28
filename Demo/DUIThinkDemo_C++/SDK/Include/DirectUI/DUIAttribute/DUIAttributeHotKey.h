#ifndef __DUIATTRIBUTEHOTKEY_H__
#define __DUIATTRIBUTEHOTKEY_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeHotKey INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeHotKey = { 0x10F8061D,0x177A,0x49E4,0xB2,0xA0,0x82,0x26,0xD1,0xA8,0x2C,0xD1 }; 
class DUITHINK_API CDUIAttributeHotKey
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeHotKey)
	MMDeclare_ClassName(CDUIAttributeHotKey)

public:
	CDUIAttributeHotKey();
	virtual ~CDUIAttributeHotKey();

protected:
	BYTE								m_cbVKeyCode = 0;
	BYTE								m_cbModifierFlag = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeHotKey & operator = (CDUIAttributeObject &Right) override;
	
	WORD GetValue();
	void GetValue(BYTE &cbVKeyCode, BYTE &cbModifierFlag);
	void SetValue(WORD wHotKey);
	void SetValue(BYTE cbVKeyCode, BYTE cbModifierFlag);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
