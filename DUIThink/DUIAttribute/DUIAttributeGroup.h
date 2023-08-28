#ifndef __DUIATTRIBUTEGROUP_H__
#define __DUIATTRIBUTEGROUP_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeGroup INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeGroup = { 0xF1942C49,0xF9EE,0x4C7E,0xA2,0xBD,0xEF,0x2A,0x8D,0xEC,0xDC,0xF0 };
class DUITHINK_API CDUIAttributeGroup 
	: public CDUIAttributeObject
{
	friend class CDUIGlobal;
	friend class CDUIPropertyObject;
	friend class CDUIRotateMenuCtrl;
	friend class CPropertiesView;

	DuiDeclare_CreateAttribute(CDUIAttributeGroup)
	MMDeclare_ClassName(CDUIAttributeGroup)

public:
	CDUIAttributeGroup();
	virtual ~CDUIAttributeGroup(void);

protected:
	VecAttributeObj						m_vecAttribute;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeGroup & operator = (CDUIAttributeGroup &Right);
	
	//child
	virtual VecAttributeObj GetVecAttribute() const;
	virtual bool SwapAttribute(CDUIAttributeObject *pObjectFirst, CDUIAttributeObject *pObjectNext);

	//method
protected:
	bool SetOwner(CDUIPropertyObject *pOwner) override;

	//child
	virtual bool AddAttribute(CDUIAttributeObject *pObject);
	virtual bool RemoveAttribute(CDUIAttributeObject *pObject);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

typedef vector<CDUIAttributeGroup*> VecAttributeGroup;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
