#ifndef __DUIATTRIBUTEBOOL_H__
#define __DUIATTRIBUTEBOOL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeBool INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeBool = { 0xF4C05510,0xB2C1,0x4FE9,0xA8,0x32,0x7B,0xA0,0x52,0x1F,0x7E,0x6E };
class DUITHINK_API CDUIAttributeBool 
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeBool)
	MMDeclare_ClassName(CDUIAttributeBool)

public:
	CDUIAttributeBool(void);
	~CDUIAttributeBool(void);

protected:
	bool								m_bValue = false;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

public:
	CDUIAttributeBool & operator = (CDUIAttributeObject &Right) override;

	virtual bool GetValue();
	virtual void SetValue(bool bValue);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif