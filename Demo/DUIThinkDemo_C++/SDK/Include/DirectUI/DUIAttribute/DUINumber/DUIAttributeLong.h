#ifndef __DUIATTRIBUTELONG_H__
#define __DUIATTRIBUTELONG_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeLong INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeLong = { 0xA0350CF7,0x287E,0x4976,0xA4,0x38,0x30,0x44,0xAA,0xBD,0x38,0x0A };
class DUITHINK_API CDUIAttributeLong
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeLong)
	MMDeclare_ClassName(CDUIAttributeLong)

public:
	CDUIAttributeLong();
	virtual ~CDUIAttributeLong();

protected:
	long								m_lValue = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeLong & operator = (CDUIAttributeObject &Right) override;

	virtual long GetValue();
	virtual void SetValue(long lValue);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
