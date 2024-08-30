#ifndef __DUIATTRIBUTECTRLID_H__
#define __DUIATTRIBUTECTRLID_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeCtrlID INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeCtrlID = { 0xE7C01C0D,0x36F5,0x41A7,0xB9,0x10,0x94,0xBE,0x4C,0x4A,0x68,0xB0 };
class DUITHINK_API CDUIAttributeCtrlID
	: public CDUIAttributeULong
{
	DuiDeclare_CreateAttribute(CDUIAttributeCtrlID)
	MMDeclare_ClassName(CDUIAttributeCtrlID)

public:
	CDUIAttributeCtrlID();
	virtual ~CDUIAttributeCtrlID();

protected:

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
