#ifndef __DUIATTRIBUTECURSOR_H__
#define __DUIATTRIBUTECURSOR_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeCursor INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeCursor = { 0x9B55FFA1,0xE9F2,0x4FCA,0xA3,0x52,0x9D,0xBE,0xA1,0xA5,0x74,0x62 };
class DUITHINK_API CDUIAttributeCursor 
	: public CDUIAttributeCombox
{
	DuiDeclare_CreateAttribute(CDUIAttributeCursor)
	MMDeclare_ClassName(CDUIAttributeCursor)

public:
	CDUIAttributeCursor(void);
	~CDUIAttributeCursor(void);

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

protected:

public:

protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif