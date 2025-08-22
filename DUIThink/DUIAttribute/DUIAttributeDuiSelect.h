#ifndef __DUIATTRIMODELSELECT_H__
#define __DUIATTRIMODELSELECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeDuiSelect INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeDuiSelect = { 0x0BF58AFE,0x636F,0x4B3E,0xA7,0x50,0xBC,0xE1,0x7D,0x57,0x76,0xFA };
class DUITHINK_API CDUIAttributeDuiSelect 
	: public CDUIAttributeCombox
{
	DuiDeclare_CreateAttribute(CDUIAttributeDuiSelect)
	MMDeclare_ClassName(CDUIAttributeDuiSelect)

public:
	CDUIAttributeDuiSelect();
	~CDUIAttributeDuiSelect(void);

	//variant
protected:
	enDuiType							m_DuiType = DuiType_Dlg;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//methoed
public:
	void InitSelectItem() override;
	enDuiType GetDuiType() { return m_DuiType; }
	void SetDuiType(enDuiType DuiType) { m_DuiType = DuiType; }

protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif