#ifndef __DUIATTRIBUTETABSELECT_H__
#define __DUIATTRIBUTETABSELECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeTabSelect INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeTabSelect = { 0xB2B6A230,0xD8C7,0x4DA2,0x89,0x15,0x16,0x6A,0x85,0xAB,0x97,0x0E };
class DUITHINK_API CDUIAttributeTabSelect 
	: public CDUIAttributeCombox
{
	DuiDeclare_CreateAttribute(CDUIAttributeTabSelect)
	MMDeclare_ClassName(CDUIAttributeTabSelect)

public:
	CDUIAttributeTabSelect(void);
	~CDUIAttributeTabSelect(void);

protected:

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//methoed
public:
	void InitSelectItem() override;

protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif