#ifndef __DUIATTRIMODELSELECT_H__
#define __DUIATTRIMODELSELECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttriModelSelect INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttriModelSelect = { 0x0BF58AFE,0x636F,0x4B3E,0xA7,0x50,0xBC,0xE1,0x7D,0x57,0x76,0xFA };
class DUITHINK_API CDUIAttriModelSelect 
	: public CDUIAttributeCombox
{
	DuiDeclare_CreateAttribute(CDUIAttriModelSelect)
	MMDeclare_ClassName(CDUIAttriModelSelect)

public:
	CDUIAttriModelSelect(void);
	~CDUIAttriModelSelect(void);

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