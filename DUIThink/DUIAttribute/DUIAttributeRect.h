#ifndef __DUIATTRIBUTERECT_H__
#define __DUIATTRIBUTERECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, CDUIRect> MapDuiAttributeRect;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeRect INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeRect = { 0x8615897E,0x18FA,0x46AE,0xAD,0x57,0x40,0x75,0xBF,0xC1,0x1C,0x9E };
class DUITHINK_API CDUIAttributeRect
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeRect)
	MMDeclare_ClassName(CDUIAttributeRect)

public:
	CDUIAttributeRect();
	virtual ~CDUIAttributeRect();

	//variant
protected:
	uint32_t							m_uValueHash = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	virtual CDUIRect GetValue();
	virtual void SetValue(long lLeft, long lTop, long lRight, long lBottom);
	virtual void SetValue(const RECT &rect);
	virtual void SetLeft(long lLeft);
	virtual void SetTop(long lTop);
	virtual void SetRight(long lRight);
	virtual void SetBottom(long lBottom);

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
