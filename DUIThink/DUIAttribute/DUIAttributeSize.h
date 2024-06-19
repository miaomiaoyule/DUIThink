#ifndef __DUIATTRIBUTESIZE_H__
#define __DUIATTRIBUTESIZE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeSize INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeSize = { 0x6708342C,0x4E8B,0x40DB,0x96,0x30,0x2E,0x83,0x65,0x1E,0x5A,0xAB };
class DUITHINK_API CDUIAttributeSize
	: public CDUIAttributeObject
	, public SIZE
{
	DuiDeclare_CreateAttribute(CDUIAttributeSize)
	MMDeclare_ClassName(CDUIAttributeSize)

public:
	CDUIAttributeSize(SIZE szValue = {});
	virtual ~CDUIAttributeSize();

	//variant
protected:

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeSize & operator = (CDUIAttributeObject &Right) override;

	virtual CDUISize GetValue();
	virtual void SetValue(long lCx, long lCy);
	virtual void SetValue(const SIZE &szXY);
	virtual void SetCx(long lCx);
	virtual void SetCy(long lCy);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
