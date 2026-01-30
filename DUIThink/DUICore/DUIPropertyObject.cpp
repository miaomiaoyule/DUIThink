#include "stdafx.h"
#include "DUIPropertyObject.h"

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIPropertyObject)

CDUIPropertyObject::CDUIPropertyObject()
{
	return;
}

CDUIPropertyObject::~CDUIPropertyObject()
{
	m_vecAttributeGroup.clear();

#ifdef DUI_DESIGN
	m_vecIDuiControlListen.clear();
#endif

	return;
}

bool CDUIPropertyObject::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	auto vecIDuiControlListen = GetControlListen();
	for (auto pIDuiControlListen : vecIDuiControlListen)
	{
		if (NULL == pIDuiControlListen) continue;

		pIDuiControlListen->OnAttributeChange(this, pAttributeObj);
	}

	return false;
}

VecIDuiControlListen CDUIPropertyObject::GetControlListen()
{
#ifdef DUI_DESIGN
	return m_vecIDuiControlListen;
#else
	return {};
#endif
}

bool CDUIPropertyObject::RegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (NULL == pIDuiControlListen) return false;

#ifdef DUI_DESIGN
	if (find(m_vecIDuiControlListen.begin(), m_vecIDuiControlListen.end(), pIDuiControlListen) != m_vecIDuiControlListen.end()) return false;

	m_vecIDuiControlListen.push_back(pIDuiControlListen);
#endif

	return true;
}

bool CDUIPropertyObject::UnRegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (NULL == pIDuiControlListen) return false;

#ifdef DUI_DESIGN
	auto FindIt = find(m_vecIDuiControlListen.begin(), m_vecIDuiControlListen.end(), pIDuiControlListen);
	if (FindIt == m_vecIDuiControlListen.end()) return false;

	m_vecIDuiControlListen.erase(FindIt);
#endif

	return true;
}

bool CDUIPropertyObject::IsInitComplete()
{
	return m_bInitComplete;
}

LPVOID CDUIPropertyObject::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIPropertyObject, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIPropertyObject::GetDescribe() const
{
	return Dui_Prop_Object;
}

CDUIAttributeObject * CDUIPropertyObject::GetAttributeObj(LPCTSTR lpszName)
{
	for (auto &pAttributeGroup : m_vecAttributeGroup)
	{
		if (NULL == pAttributeGroup) break;

		auto vecAttribute = pAttributeGroup->GetVecAttribute();
		for (auto pAttribute : vecAttribute)
		{
			if (0 == lstrcmp(pAttribute->GetAttributeName(), lpszName))
			{
				return pAttribute;
			}
		}
	}

	return NULL;
}

int CDUIPropertyObject::GetAttributeGroupIndex(CDUIAttributeGroup *pAttributeGroup)
{
	for (int n = 0; n < m_vecAttributeGroup.size(); n++)
	{
		if (m_vecAttributeGroup[n] == pAttributeGroup) return n;
	}

	return -1;
}

void CDUIPropertyObject::SetAttributeGroupIndex(CDUIAttributeGroup *pAttributeGroup, int nIndex)
{
	if (NULL == pAttributeGroup) return;

	int nIndexPre = GetAttributeGroupIndex(pAttributeGroup);
	if (-1 == nIndexPre) return;

	nIndex = min(max(0, nIndex), m_vecAttributeGroup.size() - 1);
	nIndex = nIndex < nIndexPre ? nIndex : nIndex - 1;
	m_vecAttributeGroup.erase(m_vecAttributeGroup.begin() + nIndexPre);
	m_vecAttributeGroup.insert(m_vecAttributeGroup.begin() + nIndex, pAttributeGroup);

	return;
}

VecAttributeGroup CDUIPropertyObject::GetAttributeGroup()
{
	return m_vecAttributeGroup;
}

int CDUIPropertyObject::GetScale()
{
	return 100;
}

const CMMDpi & CDUIPropertyObject::GetDpiObj()
{
	return CDUIGlobal::GetInstance()->GetDpiObj();
}

CDUIWnd * CDUIPropertyObject::GetWndOwner()
{
	return NULL;
}

bool CDUIPropertyObject::Init()
{
	if (IsInitComplete()) return true;

	bool bFromXml = false == m_mapAttributeBuffer.empty();
	
	InitProperty();
	InitComplete();

	if (false == bFromXml)
	{
		InitNormalSkin();
	}

	return true;
}

UINT CDUIPropertyObject::InitCtrlID()
{
	return 0;
}

void CDUIPropertyObject::InitProperty()
{

}

void CDUIPropertyObject::InitComplete()
{
	m_bInitComplete = true;

	m_mapAttributeBuffer.clear();

	return;
}

void CDUIPropertyObject::InitNormalSkin()
{
	return;
}

bool CDUIPropertyObject::CreateGroupAttribute(CDUIAttributeGroup &AttriGroup, LPCTSTR lpszName)
{
	AttriGroup.SetAttributeName(lpszName);
	AttriGroup.SetOwner(this);

	m_vecAttributeGroup.push_back(&AttriGroup);

	return true;
}

bool CDUIPropertyObject::CreateAttribute(CDUIAttributeObject &Attribute, LPCTSTR lpszName, LPCTSTR lpszDescribe, CDUIAttributeGroup *pAttriGroup)
{	
	if (NULL == pAttriGroup) return false;

	//create
	auto FindIt = false == m_mapAttributeBuffer.empty() ? m_mapAttributeBuffer.find(lpszName) : m_mapAttributeBuffer.end();
	if (FindIt != m_mapAttributeBuffer.end())
	{
		CDUIXmlPack::LoadAtrributeFromXML(FindIt->second, &Attribute);

		m_mapAttributeBuffer.erase(FindIt);
	}
	else
	{
		Attribute.SetAttributeName(lpszName);
		Attribute.SetAttributeDescr(lpszDescribe);
	}

	//group
	pAttriGroup->AddAttribute(&Attribute);

	return true;
}

bool CDUIPropertyObject::AddAttributeBuffer(tinyxml2::XMLElement *pNodeXml)
{
	if (NULL == pNodeXml) return false;

	m_mapAttributeBuffer[pNodeXml->Name()] = pNodeXml;

	return true;
}

bool CDUIPropertyObject::SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild)
{
	if (NULL == pNode) return false;

	//propertyobj
	for (int nIndex = 0; nIndex < m_vecAttributeGroup.size(); nIndex++)
	{
		CDUIAttributeGroup *pGroup = m_vecAttributeGroup[nIndex];
		if (NULL == pGroup) continue;

		pGroup->SaveAttribute(pNode);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void CDUIGlobal::PerformNotifyChildAdd(CDUIPropertyObject *pPropertyObj, CDUIControlBase *pChild)
{
	if (NULL == pPropertyObj || NULL == pChild) return;

	auto vecIDuiControlListen = pPropertyObj->GetControlListen();
	if (vecIDuiControlListen.empty()) return;

	for (auto pIDuiControlListen : vecIDuiControlListen)
	{
		if (NULL == pIDuiControlListen) continue;

		pIDuiControlListen->OnChildAdd(pChild->GetParent(), pChild);

		pChild->RegisterControlListen(pIDuiControlListen);
	}

	return;
}

void CDUIGlobal::PerformNotifyChildRemove(CDUIPropertyObject *pPropertyObj, CDUIControlBase *pChild)
{
	if (NULL == pPropertyObj || NULL == pChild) return;

	auto vecIDuiControlListen = pPropertyObj->GetControlListen();
	if (vecIDuiControlListen.empty()) return;

	for (auto pIDuiControlListen : vecIDuiControlListen)
	{
		if (NULL == pIDuiControlListen) continue;

		pIDuiControlListen->OnChildRemove(pChild->GetParent(), pChild);

		pChild->UnRegisterControlListen(pIDuiControlListen);
	}

	return;
}

void CDUIGlobal::PerformNotifyVisibleChange(CDUIPropertyObject *pPropertyObj)
{
	if (NULL == pPropertyObj) return;

	auto vecIDuiControlListen = pPropertyObj->GetControlListen();
	if (vecIDuiControlListen.empty()) return;

	MMInterfaceHelper(CDUIControlBase, pPropertyObj, pControl);
	if (NULL == pControl) return;

	for (auto pIDuiControlListen : vecIDuiControlListen)
	{
		if (NULL == pIDuiControlListen) continue;

		pIDuiControlListen->OnVisibleChange(pControl);
	}

	return;
}

VecDuiRichTextItem CDUIGlobal::ParseVecRichTextItem(LPCSTR lpszValue)
{
	VecDuiRichTextItem vecRichTextItem;
	if (NULL == lpszValue) return vecRichTextItem;

	do
	{
		tinyxml2::XMLDocument xmlDoc;
		if (XML_SUCCESS != xmlDoc.Parse(lpszValue, strlen(lpszValue))) break;

		tinyxml2::XMLElement *pNodeXml = xmlDoc.RootElement();
		if (NULL == pNodeXml) break;

		pNodeXml = pNodeXml->FirstChildElement();
		if (NULL == pNodeXml) break;

		do
		{
			const tinyxml2::XMLAttribute *pNodeAttribute = pNodeXml->FirstAttribute();
			if (NULL == pNodeAttribute) break;

			//参数节点
			tagDuiRichTextItem RichTextItem;
			do
			{
				if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRichTextItemFontRes))
				{
					RichTextItem.vecFontResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), (";"), CP_UTF8);
					
					continue;
				}
				if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRichTextItemColorRes))
				{
					RichTextItem.vecColorResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), (";"));
					
					continue;
				}
				if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRichTextItemText))
				{
					RichTextItem.strText = CA2CT(pNodeAttribute->Value(), CP_UTF8);

					continue;
				}

			} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

			vecRichTextItem.push_back(RichTextItem);

		} while (pNodeXml = pNodeXml->NextSiblingElement(), pNodeXml);

	} while (false);

	return vecRichTextItem;
}

//////////////////////////////////////////////////////////////////////////
#pragma pack()