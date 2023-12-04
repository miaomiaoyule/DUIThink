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
	m_vecIControlListen.clear();
#endif

	return;
}

bool CDUIPropertyObject::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	auto vecIControlListen = GetControlListen();
	for (auto pInterface : vecIControlListen)
	{
		MMInterfaceHelper(IControlListen, pInterface, pIControlListen);
		if (NULL == pIControlListen) continue;

		pIControlListen->OnAttributeChange(this, pAttributeObj);
	}

	return false;
}

VecControlListen CDUIPropertyObject::GetControlListen()
{
#ifdef DUI_DESIGN
	return m_vecIControlListen;
#else
	return {};
#endif
}

bool CDUIPropertyObject::RegisterControlListen(IDUIInterface *pIControlListen)
{
	if (NULL == pIControlListen) return false;

#ifdef DUI_DESIGN
	if (find(m_vecIControlListen.begin(), m_vecIControlListen.end(), pIControlListen) != m_vecIControlListen.end()) return false;

	m_vecIControlListen.push_back(pIControlListen);
#endif

	return true;
}

bool CDUIPropertyObject::UnRegisterControlListen(IDUIInterface *pIControlListen)
{
	if (NULL == pIControlListen) return false;

#ifdef DUI_DESIGN
	auto FindIt = find(m_vecIControlListen.begin(), m_vecIControlListen.end(), pIControlListen);
	if (FindIt == m_vecIControlListen.end()) return false;

	m_vecIControlListen.erase(FindIt);
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

int CDUIPropertyObject::GetScale()
{
	return 100;
}

const CMMDpi & CDUIPropertyObject::GetDpiObj()
{
	return CDUIGlobal::GetInstance()->GetDpiObj();
}

CDUIWndManager * CDUIPropertyObject::GetWndManager()
{
	return NULL;
}

bool CDUIPropertyObject::Init()
{
	if (IsInitComplete()) return true;

	InitProperty();
	InitAttriValue();
	InitComplete();

	return true;
}

UINT CDUIPropertyObject::InitCtrlID()
{
	return 0;
}

void CDUIPropertyObject::InitProperty()
{

}

void CDUIPropertyObject::InitAttriValue()
{
	return;
}

void CDUIPropertyObject::InitComplete()
{
	m_bInitComplete = true;

	m_mapAttributeBuffer.clear();

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

	//same
#ifdef DUI_DESIGN
	if (CDUIGlobal::GetAttributeObj(this, lpszName))
	{
		MessageBox(NULL, _T("Attribute Name Same"), NULL, NULL);

		return false;
	}
#endif

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

	auto vecIControlListen = pPropertyObj->GetControlListen();
	if (vecIControlListen.empty()) return;

	for (auto pInterface : vecIControlListen)
	{
		MMInterfaceHelper(IControlListen, pInterface, pIControlListen);
		if (NULL == pIControlListen) continue;

		pIControlListen->OnChildAdd(pChild->GetParent(), pChild);

		pChild->RegisterControlListen(pIControlListen);
	}

	return;
}

void CDUIGlobal::PerformNotifyChildRemove(CDUIPropertyObject *pPropertyObj, CDUIControlBase *pChild)
{
	if (NULL == pPropertyObj || NULL == pChild) return;

	auto vecIControlListen = pPropertyObj->GetControlListen();
	if (vecIControlListen.empty()) return;

	for (auto pInterface : vecIControlListen)
	{
		MMInterfaceHelper(IControlListen, pInterface, pIControlListen);
		if (NULL == pIControlListen) continue;

		pIControlListen->OnChildRemove(pChild->GetParent(), pChild);

		pChild->UnRegisterControlListen(pIControlListen);
	}

	return;
}

void CDUIGlobal::PerformNotifyVisibleChange(CDUIPropertyObject *pPropertyObj)
{
	if (NULL == pPropertyObj) return;

	auto vecIControlListen = pPropertyObj->GetControlListen();
	if (vecIControlListen.empty()) return;

	MMInterfaceHelper(CDUIControlBase, pPropertyObj, pControl);
	if (NULL == pControl) return;

	for (auto pInterface : vecIControlListen)
	{
		MMInterfaceHelper(IControlListen, pInterface, pIControlListen);
		if (NULL == pIControlListen) continue;

		pIControlListen->OnVisibleChange(pControl);
	}

	return;
}

void CDUIGlobal::PerformResourceDelete(CDUIControlBase *pControl, CDUIResourceBase *pResourceObj)
{
	if (NULL == pResourceObj) return;

	//self
	for (auto pAttributeGroup : pControl->m_vecAttributeGroup)
	{
		if (NULL == pAttributeGroup) break;

		auto vecAttribute = pAttributeGroup->GetVecAttribute();
		for (auto pAttribute : vecAttribute)
		{
			//pAttribute->OnResourceDelete(pResourceObj);
		}
	}

	return;
}

void CDUIGlobal::PerformResourceSwitch(CDUIControlBase *pControl, int nIndexRes)
{
	if (NULL == pControl) return;

	//self
	for (auto pAttributeGroup : pControl->m_vecAttributeGroup)
	{
		if (NULL == pAttributeGroup) break;

		auto vecAttribute = pAttributeGroup->GetVecAttribute();
		for (auto pAttribute : vecAttribute)
		{
			//pAttribute->OnResourceSwitch(nIndexRes);
		}
	}

	return;
}

bool CDUIGlobal::PerformAddAttributeBuffer(CDUIPropertyObject *pPropertyObj, tinyxml2::XMLElement *pNodeXml)
{
	if (NULL == pPropertyObj || NULL == pNodeXml) return false;

	pPropertyObj->m_mapAttributeBuffer[pNodeXml->Name()] = pNodeXml;

	return true;
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
					switch (CDUIGlobal::GetInstance()->GetResVersion())
					{
						case DuiResVersion_0:
						{
							RichTextItem.vecFontResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), (";"));

							break;
						}
						default:
						{
							RichTextItem.vecFontResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), (";"), CP_UTF8);

							break;
						}
					}
					
					continue;
				}
				if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRichTextItemColorRes))
				{
					RichTextItem.vecColorResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), (";"));
					
					continue;
				}
				if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRichTextItemText))
				{
					switch (CDUIGlobal::GetInstance()->GetResVersion())
					{
						case DuiResVersion_0:
						{
							RichTextItem.strText = pNodeAttribute->Value();

							break;
						}
						default:
						{
							RichTextItem.strText = CA2CT(pNodeAttribute->Value(), CP_UTF8);

							break;
						}
					}

					continue;
				}

			} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

			vecRichTextItem.push_back(RichTextItem);

		} while (pNodeXml = pNodeXml->NextSiblingElement(), pNodeXml);

	} while (false);

	return vecRichTextItem;
}

VecAttributeGroup CDUIGlobal::GetAttributeGroup(CDUIPropertyObject *pPropertyObj)
{
	if (NULL == pPropertyObj) return {};

	return pPropertyObj->m_vecAttributeGroup;
}

int CDUIGlobal::GetAttributeGroupIndex(CDUIAttributeGroup *pAttributeGroup)
{
	if (NULL == pAttributeGroup) return -1;

	CDUIPropertyObject *pPropertyObj = pAttributeGroup->GetOwner();
	if (NULL == pPropertyObj) return -1;

	auto &vecAttributeGroup = pPropertyObj->m_vecAttributeGroup;
	for (int n = 0; n < vecAttributeGroup.size(); n++)
	{
		if (vecAttributeGroup[n] == pAttributeGroup) return n;
	}

	return -1;
}

void CDUIGlobal::SetAttributeGroupIndex(CDUIAttributeGroup *pAttributeGroup, int nIndex)
{
	if (NULL == pAttributeGroup) return;

	CDUIPropertyObject *pPropertyObj = pAttributeGroup->GetOwner();
	if (NULL == pPropertyObj) return;

	int nIndexPre = GetAttributeGroupIndex(pAttributeGroup);
	if (-1 == nIndexPre) return;

	auto &vecAttributeGroup = pPropertyObj->m_vecAttributeGroup;
	nIndex = min(max(0, nIndex), vecAttributeGroup.size() - 1);
	vecAttributeGroup.erase(vecAttributeGroup.begin() + nIndexPre);
	vecAttributeGroup.insert(vecAttributeGroup.begin() + nIndex, pAttributeGroup);

	return;
}

CDUIAttributeObject * CDUIGlobal::GetAttributeObj(CDUIPropertyObject *pPropertyObj, LPCTSTR lpszName)
{
	if (NULL == pPropertyObj) return NULL;

	for (auto &pAttributeGroup : pPropertyObj->m_vecAttributeGroup)
	{
		if (NULL == pAttributeGroup) break;

		auto vecAttribute = pAttributeGroup->GetVecAttribute();
		for (auto pAttribute : vecAttribute)
		{
			if (lstrcmp(pAttribute->GetAttributeName(), lpszName) == 0)
			{
				return pAttribute;
			}
		}
	}

	return NULL;
}

bool CDUIGlobal::CreateAttributeForDesign(CDUIAttributeObject **ppAttribute, enDuiAttributeType attributeType, LPCTSTR lpszName, LPCTSTR lpszDescribe)
{
	if (*ppAttribute) return false;

	switch (attributeType)
	{
		case DuiAttribute_Group:
		{
			*ppAttribute = new CDUIAttributeGroup;
			break;
		}
		case DuiAttribute_Position:
		{
			*ppAttribute = new CDUIAttributePosition;
			break;
		}
		case DuiAttribute_ImageSection:
		{
			*ppAttribute = new CDUIAttriImageSection;
			break;
		}
		case DuiAttribute_Long:
		{
			*ppAttribute = new CDUIAttributeLong;
			break;
		}
		case DuiAttribute_Float:
		{
			*ppAttribute = new CDUIAttributeFloat;
			break;
		}
		case DuiAttribute_Text:
		{
			*ppAttribute = new CDUIAttributeText;
			break;
		}
		case DuiAttribute_TextStyle:
		{
			*ppAttribute = new CDUIAttributeTextStyle;
			break;
		}
		case DuiAttribute_RichText:
		{
			*ppAttribute = new CDUIAttributeRichText;
			break;
		}
		case DuiAttribute_Color:
		{
			*ppAttribute = new CDUIAttributeColor;
			break;
		}
		case DuiAttribute_ColorSwitch:
		{
			*ppAttribute = new CDUIAttributeColorSwitch;
			break;
		}
		case DuiAttribute_Bool:
		{
			*ppAttribute = new CDUIAttributeBool;
			break;
		}
		case DuiAttribute_Combox:
		{
			*ppAttribute = new CDUIAttributeCombox;
			break;
		}
		case DuiAttribute_Cursor:
		{
			*ppAttribute = new CDUIAttributeCursor;
			break;
		}
		case DuiAttribute_ModelSelect:
		{
			*ppAttribute = new CDUIAttriModelSelect;
			break;
		}
		case DuiAttribute_ViewSelect:
		{
			*ppAttribute = new CDUIAttriViewSelect;
			break;
		}
		case DuiAttribute_TabSelect:
		{
			*ppAttribute = new CDUIAttriTabSelect;
			break;
		}
		case DuiAttribute_Size:
		{
			*ppAttribute = new CDUIAttributeSize;
			break;
		}
		case DuiAttribute_Rect:
		{
			*ppAttribute = new CDUIAttributeRect;
			break;
		}
		case DuiAttribute_HotKey:
		{
			*ppAttribute = new CDUIAttributeHotKey;
			break;
		}
		default:
		{
			return false;
			break;
		}
	}

	if (*ppAttribute == NULL) return false;
	(*ppAttribute)->SetAttributeName(lpszName);
	(*ppAttribute)->SetAttributeDescr(lpszDescribe);

	return true;
}

//////////////////////////////////////////////////////////////////////////
#pragma pack()