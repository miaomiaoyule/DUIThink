#ifndef __DUIATTRIBUTETEXT_H__
#define __DUIATTRIBUTETEXT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, CMMString> MapDuiAttributeText;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeText INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeText = { 0xA51B0F6E,0xD653,0x4263,0x8E,0xF1,0x5D,0x0A,0x67,0x3C,0x43,0x71 };
class DUITHINK_API CDUIAttributeText
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeText)
	MMDeclare_ClassName(CDUIAttributeText)

public:
	CDUIAttributeText();
	virtual ~CDUIAttributeText(void);

	//variant
protected:
	uint32_t							m_uValueHash = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	virtual LPCTSTR GetValue();
	virtual bool SetValue(LPCTSTR lpszText);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;

	//help
protected:
	uint32_t GetValueID() override;
	void SetValueID(uint32_t uValueID) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif