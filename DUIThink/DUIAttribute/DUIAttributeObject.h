#ifndef __DUIATTRIBUTEOBJECT_H__
#define __DUIATTRIBUTEOBJECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CUICommandElement;
class CDUIAttributeObject;

typedef std::map<uint32_t, CMMString> MapAttributeName;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeObject INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeObject = { 0x5AF26DBE,0x6DDA,0x4512,0x8F,0x97,0x5E,0x08,0x2A,0xC7,0x72,0xE4 };
class DUITHINK_API CDUIAttributeObject
	: public IDuiInterface
{
	friend class CDUIGlobal;
	friend class CDUIXmlPack;
	friend class CDUIPropertyObject;
	friend class CDUIAttributeGroup;
	friend class CDlgImageSection;
	friend class CDesignerView;
	friend class CPropertiesView;
	friend class CUICommandElement;
	friend class CDUIControlBase;
	friend class CPropertyTextStyle;

	DuiDeclare_CreateAttribute(CDUIAttributeObject)
	MMDeclare_ClassName(CDUIAttributeObject)

public:
	CDUIAttributeObject();
	virtual ~CDUIAttributeObject(void);

protected:
	CDUIPropertyObject *				m_pOwner = NULL;

	uint32_t							m_uNameHash = 0;
	bool								m_bModified = false;

#ifdef DUI_DESIGN
	enDuiAttributeType					m_DuiAttributeType = DuiAttribute_None;
	CDUIAttributeGroup *				m_pGroup = NULL;
	CMMString 							m_strAttriDescr;
	bool								m_bVisible = true;
#endif

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	virtual CDUIAttributeObject & operator = (CDUIAttributeObject &Right);

	//sign
	virtual CMMString FormatInfo();

	//base
	virtual LPCTSTR GetAttributeName();
	virtual enDuiAttributeType GetAttributeType();
	virtual CDUIPropertyObject * GetOwner();
	virtual CDUIWnd * GetOwnerWnd();
	virtual CDUIAttributeGroup * GetGroup();
	virtual LPCTSTR GetGroupName();

	//describe
	virtual LPCTSTR GetAttributeDescr();
	virtual bool SetAttributeDescr(LPCTSTR lpszDescribe);

	//visible
	virtual bool IsVisible();
	virtual void SetVisible(bool bVisible);

	//modified
	virtual bool IsModified();
	virtual bool IsEmpty();

	//dpi
	virtual int GetScale();

	//method
protected:
	//owner
	virtual bool SetOwner(CDUIPropertyObject *pOwner);

	//name
	virtual void SetAttributeName(LPCTSTR lpszName);

	//group
	virtual void SetGroup(CDUIAttributeGroup *pGroup);

	//attribute
protected:
	virtual bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue);
	virtual bool SaveAttribute(tinyxml2::XMLElement *pNode);

	//help
protected:
	virtual uint32_t GetValueID();
	virtual void SetValueID(uint32_t uValueID);
	virtual void NotifyChange();
};

typedef std::vector<CDUIAttributeObject*> VecAttributeObj;
typedef std::unordered_map<CMMString, tinyxml2::XMLElement*> MapAttributeBuffer;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
