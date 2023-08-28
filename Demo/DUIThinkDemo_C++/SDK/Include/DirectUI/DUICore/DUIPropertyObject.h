#ifndef __DUIPROPERTYOBJECT_H__
#define __DUIPROPERTYOBJECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIGlobal;
class CUICommandElement;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIPropertyObject INTERFACE_VERSION(1,1)
static const GUID IID_CDUIPropertyObject = { 0x5BF13A7D,0xB9B7,0x4198,0xA3,0x98,0x88,0xF7,0x25,0xCF,0xC1,0x2E };
class DUITHINK_API CDUIPropertyObject : public IDUIPropertyObject
{
	friend class CDUIGlobal;
	friend class CUICommandElement;
	friend class CDUIMenuCtrl;
	friend class CDUIXmlPack;
	friend class CDUIAttributeObject;
	friend class CPropertiesView;

	MMDeclare_ClassName(CDUIPropertyObject)

public:
	CDUIPropertyObject();
	virtual ~CDUIPropertyObject();

	//variant
protected:
#ifdef DUI_DESIGN
	VecControlListen					m_vecIControlListen;
#endif

	VecAttributeGroup					m_vecAttributeGroup;
	MapAttributeBuffer					m_mapAttributeBuffer;
	bool								m_bInitComplete = false;

	//override
	//** you forbid override **//
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	VecControlListen GetControlListen() override;
	bool RegisterControlListen(IDUIInterface *pIControlListen) override;
	bool UnRegisterControlListen(IDUIInterface *pIControlListen) override;

	//method 
	//** you forbid override **//
public:
	virtual bool IsInitComplete();

	//method
	//** you can override if you need **//
public:
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	virtual CMMString GetDescribe() const;

	//property
	//** you forbid override **//
public:
	virtual bool Init();
	virtual UINT InitCtrlID();
	virtual int GetScale();
	virtual const CMMDpi & GetDpiObj();
	virtual CDUIWndManager * GetWndManager();

	//property 
	//** you can override if you need, but you should execute this class first **//
protected:
	virtual void InitProperty();
	virtual void InitAttriValue();
	virtual void InitComplete();
				 
	//attribute 
	//** you forbid override **//
protected:
	bool CreateGroupAttribute(CDUIAttributeGroup &AttriGroup, LPCTSTR lpszName);
	bool CreateAttribute(CDUIAttributeObject &Attribute, LPCTSTR lpszName, LPCTSTR lpszDescribe, CDUIAttributeGroup *pAttriGroup);

	//xml
	//** you forbid override **//
protected:
	virtual bool SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild = true);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif