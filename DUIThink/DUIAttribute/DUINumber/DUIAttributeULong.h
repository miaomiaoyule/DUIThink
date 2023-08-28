#ifndef __DUIATTRIBUTEULONG_H__
#define __DUIATTRIBUTEULONG_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeULong INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeULong = { 0x8555289D,0x0401,0x45EE,0xB3,0x22,0x1B,0xA7,0xF1,0x25,0x58,0xC4 };
class DUITHINK_API CDUIAttributeULong
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeULong)
	MMDeclare_ClassName(CDUIAttributeULong)

public:
	CDUIAttributeULong();
	virtual ~CDUIAttributeULong();

protected:
	ULONG								m_uValue = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeULong & operator = (CDUIAttributeObject &Right) override;

	virtual ULONG GetValue();
	virtual void SetValue(ULONG uValue);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
