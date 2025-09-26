#include "StdAfx.h"
#include "DUIXMLPack.h"

//////////////////////////////////////////////////////////////////////////
bool CDUIXmlPack::SaveProject(LPCTSTR lpszProjPath, LPCTSTR lpszProjName, const MapDuiFontBase &mapResFont, \
	const MapDuiImageBase &mapResImage, const MapDuiColorBase &mapResColor, const VecDuiFile &vecDui, \
	const MapWnd &mapWnd, CMMString strFontResDefault)
{
	if (MMInvalidString(lpszProjPath) || MMInvalidString(lpszProjName)) return false;

	//xml
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLDeclaration *pDeclaration = xmlDoc.NewDeclaration();
	xmlDoc.LinkEndChild(pDeclaration);

	//version node
	tinyxml2::XMLElement* pXmlVersionRes = xmlDoc.NewElement(Dui_Resource_Version);
	pXmlVersionRes->SetAttribute(Dui_Resource_Key_Version, DuiResVersion_Max);
	xmlDoc.LinkEndChild(pXmlVersionRes);

	//image res file
	CStringA strImageRes;
	strImageRes = strImageRes + Dui_Resource_ImageRes + (".xml");
	tinyxml2::XMLElement *pXmlImageRes = xmlDoc.NewElement(Dui_Resource_ImageRes);
	pXmlImageRes->SetAttribute(Dui_Resource_Key_ImageResFile, strImageRes);
	xmlDoc.LinkEndChild(pXmlImageRes);

	//font res file
	CStringA strFontRes;
	strFontRes = strFontRes + Dui_Resource_FontRes + (".xml");
	tinyxml2::XMLElement *pXmlFontRes = xmlDoc.NewElement(Dui_Resource_FontRes);
	pXmlFontRes->SetAttribute(Dui_Resource_Key_FontResFile, strFontRes);
	pXmlFontRes->SetAttribute(Dui_Resource_Key_FontResDefault, (LPCSTR)CT2CA(strFontResDefault));
	xmlDoc.LinkEndChild(pXmlFontRes);

	//color res file
	CStringA strColorRes;
	strColorRes = strColorRes + Dui_Resource_ColorRes + (".xml");
	tinyxml2::XMLElement *pXmlColorRes = xmlDoc.NewElement(Dui_Resource_ColorRes);
	pXmlColorRes->SetAttribute(Dui_Resource_Key_ColorResFile, strColorRes);
	xmlDoc.LinkEndChild(pXmlColorRes);

	//attribute file
	CStringA strAttribute;
	strAttribute = strAttribute + Dui_Resource_Attribute + (".xml");
	tinyxml2::XMLElement *pXmlAttribute = xmlDoc.NewElement(Dui_Resource_Attribute);
	pXmlAttribute->SetAttribute(Dui_Resource_Key_AttributeFile, strAttribute);
	xmlDoc.LinkEndChild(pXmlAttribute);

	//ctrl id file
	CStringA strCtrlID;
	strCtrlID = strCtrlID + Dui_Resource_CtrlID + (".h");
	tinyxml2::XMLElement *pXmlCtrlID = xmlDoc.NewElement(Dui_Resource_CtrlID);
	pXmlCtrlID->SetAttribute(Dui_Resource_Key_CtrlIDFile, strCtrlID);
	xmlDoc.LinkEndChild(pXmlCtrlID);

	//dui node
	for (auto It = vecDui.begin(); It != vecDui.end(); ++It)
	{
		tinyxml2::XMLElement *pXmlDui = xmlDoc.NewElement(Dui_Resource_DirectUI);
		pXmlDui->SetAttribute(Dui_Resource_Key_DuiName, (LPSTR)CT2CA(It->strName));
		pXmlDui->SetAttribute(Dui_Resource_Key_DuiType, It->DuiType);
		pXmlDui->SetAttribute(Dui_Resource_Key_DuiFile, (LPSTR)CT2CA(It->strFile));
		xmlDoc.LinkEndChild(pXmlDui);
	}

	//project file
	TCHAR szPath[MAX_PATH] = {};
	lstrcpyn(szPath, lpszProjPath, MMCountString(szPath));
	::PathAddBackslash(szPath);
	CMMString strProj = szPath;
	strProj += lpszProjName;
	strProj += Dui_Name_ProjectExt;
	xmlDoc.SaveFile((LPSTR)CT2CA(strProj));

	//encrypt
	EncryptXmlFile(strProj);

	//dui xml
	for (auto &Item : mapWnd)
	{
		if (Item.second.strName.empty() || Item.first->IsDesigned()) continue;

		SaveXmlUI(CDUIGlobal::GetInstance()->GetDuiFileFull(Item.second.strName), Item.first);
	}

	//image res
	VecDuiResourceBase vecResource;
	vecResource.clear();
	for (auto &ResImageItem : mapResImage) vecResource.push_back(ResImageItem.second);
	CMMString strImageResFile = szPath + strImageRes;
	SaveResource(strImageResFile.c_str(), vecResource);

	//font res
	vecResource.clear();
	for (auto &ResFontItem : mapResFont) vecResource.push_back(ResFontItem.second);
	CMMString strFontResFile = szPath + strFontRes;
	SaveResource(strFontResFile.c_str(), vecResource);

	//color res
	vecResource.clear();
	for (auto &ResColorItem : mapResColor) vecResource.push_back(ResColorItem.second);
	CMMString strColorResFile = szPath + strColorRes;
	SaveResource(strColorResFile.c_str(), vecResource);

	//attribute
	CMMString strAttributeFile = szPath + strAttribute;
	SaveAttribute(strAttributeFile.c_str());

	return true;
}

bool CDUIXmlPack::LoadProject(LPCTSTR lpszProject)
{
	CMMString strProjPath, strProjName, strFile;
	strFile = lpszProject;

	//parse path name
	CMMFile::ParseFilePathName(strFile, strProjPath, CMMString());
	CMMFile::ParseFileName(strFile, strProjName, CMMString());

	vector<BYTE> vecData;
	tinyxml2::XMLDocument xmlDoc;
	if (false == CDUIGlobal::GetInstance()->ExtractResourceData(vecData, strFile)
		|| (/*vecData = CMMEncrypt::MapDecrypt(vecData, Key_ResourceEncrypt), */vecData.empty())
		|| XML_SUCCESS != xmlDoc.Parse((const char*)vecData.data(), vecData.size()))
	{
		CMMString strError = xmlDoc.ErrorStr();

		CMMString strWarning;
		strWarning.Format(_T("加载Dui工程(%s)XML文件失败, cause[%s]"), strFile.c_str(), strError.c_str());
		::MessageBox(NULL, strWarning, NULL, MB_OK);
		return false;
	}

	//保存路径
	CDUIGlobal::GetInstance()->SetProjectPath(strProjPath);
	CDUIGlobal::GetInstance()->SetProjectName(strProjName);

	//开始导入
	tinyxml2::XMLElement *pXmlElement = xmlDoc.RootElement();
	if (NULL == pXmlElement) return false;

	//varaint
	do
	{
		//version node
		if (0 == strcmp(pXmlElement->Name(), Dui_Resource_Version))
		{
			int nResVersion = strtol(pXmlElement->Attribute(Dui_Resource_Key_Version), NULL, 10);
	
			CDUIGlobal::GetInstance()->SetResVersion((enDuiResVersion)nResVersion);

			continue;
		}

		//ImageResNode
		if (0 == strcmp(pXmlElement->Name(), Dui_Resource_ImageRes))
		{
			strFile = pXmlElement->Attribute(Dui_Resource_Key_ImageResFile);
			strFile = strProjPath + strFile;
			LoadResource(strFile);

			continue;
		}

		//FontRes
		if (0 == strcmp(pXmlElement->Name(), Dui_Resource_FontRes))
		{
			//default
			CDUIGlobal::GetInstance()->SetFontResDefault(pXmlElement->Attribute(Dui_Resource_Key_FontResDefault));

			//res
			strFile = pXmlElement->Attribute(Dui_Resource_Key_FontResFile);
			strFile = strProjPath + strFile;
			LoadResource(strFile);

			continue;
		}

		//ColorRes
		if (0 == strcmp(pXmlElement->Name(), Dui_Resource_ColorRes))
		{
			strFile = pXmlElement->Attribute(Dui_Resource_Key_ColorResFile);
			strFile = strProjPath + strFile;
			LoadResource(strFile);

			continue;
		}

		//AttributeNode
		if (0 == strcmp(pXmlElement->Name(), Dui_Resource_Attribute))
		{
			strFile = pXmlElement->Attribute(Dui_Resource_Key_AttributeFile);
			strFile = strProjPath + strFile;
			LoadAttribute(strFile);

			continue;
		}

		//CtrlID
		if (0 == strcmp(pXmlElement->Name(), Dui_Resource_CtrlID))
		{
			strFile = pXmlElement->Attribute(Dui_Resource_Key_CtrlIDFile);
			strFile = strProjPath + strFile;
			LoadCtrlID(strFile);

			continue;
		}

		//DirectUI
		if (0 == strcmp(pXmlElement->Name(), Dui_Resource_DirectUI))
		{
			enDuiType DuiType = DuiType_Dlg;

			//参数节点
			DuiType = (enDuiType)strtol(pXmlElement->Attribute(Dui_Resource_Key_DuiType), NULL, 10);
			strFile = pXmlElement->Attribute(Dui_Resource_Key_DuiFile);

			if (false == CDUIGlobal::GetInstance()->AddDui(DuiType, pXmlElement->Attribute(Dui_Resource_Key_DuiName), strFile))
			{
			}

			continue;
		}

	} while (pXmlElement = pXmlElement->NextSiblingElement(), pXmlElement);

	//none fontres
	if (CDUIGlobal::GetInstance()->GetFontResourceCount() <= 0)
	{
		CMMString strResName = CDUIFontBase::FormatFontDescribe(_T("微软雅黑"), 12, 400, false, false, false);
		CDUIFontBase *pFontBase = new CDUIFontBase(strResName, _T("微软雅黑"), 12, 400, false, false, false);

		if (false == CDUIGlobal::GetInstance()->AddResource(pFontBase))
		{
			MMSafeDelete(pFontBase);
		}
	}

	return true;
}

bool CDUIXmlPack::SaveResource(LPCTSTR lpszFile, VecDuiResourceBase &vecRes)
{
	if (MMInvalidString(lpszFile)) return false;

	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLDeclaration *pDeclaration = xmlDoc.NewDeclaration();
	xmlDoc.LinkEndChild(pDeclaration);

	//sort
	sort(vecRes.begin(), vecRes.end(), [&](CDUIResourceBase *pFirst, CDUIResourceBase *pNext)
	{
		return pFirst->GetIndex() < pNext->GetIndex();
	});
	for (auto pResourceObj : vecRes)
	{
		if (NULL == pResourceObj || pResourceObj->IsDesign()) continue;

		tinyxml2::XMLElement *pNode = xmlDoc.NewElement((LPCSTR)CT2CA(pResourceObj->GetClass()));
		pResourceObj->SaveAttribute(pNode);
		xmlDoc.LinkEndChild(pNode);
	}

	XMLError nRes = xmlDoc.SaveFile((LPSTR)CT2CA(lpszFile));
	assert(XML_SUCCESS == nRes);
	if (XML_SUCCESS != nRes)
	{
		CMMString strError = xmlDoc.ErrorStr();

		CMMString strWarning;
		strWarning.Format(_T("保存资源(%s)XML文件失败, cause[%s]"), lpszFile, strError.c_str());
		::MessageBox(NULL, strWarning, NULL, MB_OK);
		return false;
	}

	//encrypt
	EncryptXmlFile(lpszFile);

	return true;
}

bool CDUIXmlPack::LoadResource(LPCTSTR lpszFile)
{
	vector<BYTE> vecData;
	tinyxml2::XMLDocument xmlDoc;
	if (false == CDUIGlobal::GetInstance()->ExtractResourceData(vecData, lpszFile)
		|| (/*vecData = CMMEncrypt::MapDecrypt(vecData, Key_ResourceEncrypt), */vecData.empty())
		|| XML_SUCCESS != xmlDoc.Parse((const char*)vecData.data(), vecData.size()))
	{
		CMMString strError = xmlDoc.ErrorStr();

		CMMString strWarning;
		strWarning.Format(_T("加载资源(%s)XML文件失败, cause[%s]"), lpszFile, strError.c_str());
		::MessageBox(NULL, strWarning, NULL, MB_OK);
		return false;
	}

	//开始导入
	tinyxml2::XMLElement *pNodeXml = xmlDoc.RootElement();
	if (NULL == pNodeXml) return false;

	do
	{
		CDUIResourceBase *pDUIResource = NULL;
		pDUIResource = CDUIFactory::GetInstance()->CreateResourceObj(pNodeXml->Name());
		if (pDUIResource)
		{
			LoadResourceFromXML(pNodeXml, pDUIResource);

			if (false == CDUIGlobal::GetInstance()->AddResource(pDUIResource))
			{
				MMSafeDelete(pDUIResource);
			}

			continue;
		}

	} while (pNodeXml = pNodeXml->NextSiblingElement(), pNodeXml);

	return true;
}

bool CDUIXmlPack::SaveAttribute(LPCTSTR lpszFile)
{
	if (MMInvalidString(lpszFile)) return false;

	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLDeclaration *pDeclaration = xmlDoc.NewDeclaration();
	xmlDoc.LinkEndChild(pDeclaration);

	CDUIGlobal::GetInstance()->SaveAttriValue(xmlDoc);

	XMLError nRes = xmlDoc.SaveFile((LPSTR)CT2CA(lpszFile));
	assert(XML_SUCCESS == nRes);
	if (XML_SUCCESS != nRes)
	{
		CMMString strError = xmlDoc.ErrorStr();

		CMMString strWarning;
		strWarning.Format(_T("保存资源(%s)XML文件失败, cause[%s]"), lpszFile, strError.c_str());
		::MessageBox(NULL, strWarning, NULL, MB_OK);
		return false;
	}

	//encrypt
	EncryptXmlFile(lpszFile);

	return true;
}

bool CDUIXmlPack::LoadAttribute(LPCTSTR lpszFile)
{
	vector<BYTE> vecData;
	tinyxml2::XMLDocument xmlDoc;
	if (false == CDUIGlobal::GetInstance()->ExtractResourceData(vecData, lpszFile)
		|| (/*vecData = CMMEncrypt::MapDecrypt(vecData, Key_ResourceEncrypt), */vecData.empty())
		|| XML_SUCCESS != xmlDoc.Parse((const char*)vecData.data(), vecData.size()))
	{
		CMMString strError = xmlDoc.ErrorStr();

		CMMString strWarning;
		strWarning.Format(_T("加载资源(%s)XML文件失败, cause[%s]"), lpszFile, strError.c_str());
		::MessageBox(NULL, strWarning, NULL, MB_OK);
		return false;
	}

	//开始导入
	tinyxml2::XMLElement *pNodeXml = xmlDoc.RootElement();
	if (NULL == pNodeXml) return false;

	do
	{
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributeObject"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriName(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributeRichText"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriRichText(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributeText"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriText(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributeTextStyle"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriTextStyle(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributeColorSwitch"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriColorResSwitch(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributeCombox"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriCombox(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributePosition"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriPosition(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttributeRect"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriRect(pNode); });

			continue;
		}
		if (0 == strcmp(pNodeXml->Name(), "CDUIAttriImageSection"))
		{
			LoadAttriValueFromXml(pNodeXml->FirstChildElement(), [&](tinyxml2::XMLElement *pNode) { CDUIGlobal::GetInstance()->SetAttriImageSection(pNode); });

			continue;
		}

	} while (pNodeXml = pNodeXml->NextSiblingElement(), pNodeXml);

	return true;
}

bool CDUIXmlPack::SaveCtrlID(const MapDuiControlID &mapCtrlID)
{
	CMMString strProjectPath = CDUIGlobal::GetInstance()->GetProjectPath();
	TCHAR szPath[MAX_PATH] = {};
	lstrcpyn(szPath, strProjectPath, MMCountString(szPath));
	::PathAddBackslash(szPath);

	CStringA strCtrlIDFile;
	strCtrlIDFile = CStringA(szPath) + Dui_Resource_CtrlID + (".h");

	FILE *pFile = fopen(strCtrlIDFile, "w+");
	if (NULL == pFile)
	{
		CDUIGlobal::GetInstance()->SetDuiLastError(_T("Define Control ID File Lost"));
		return false;
	}

	//write
	CStringA strTip = ("//you should not modify this\n");
	fwrite(strTip.GetBuffer(), strTip.GetLength(), 1, pFile);

	CStringA strInfo;
	for (auto &Item : mapCtrlID)
	{
		if (Item.first < Dui_CtrlIDInner_Finish) continue;

		strInfo.AppendFormat("#define %s \t\t\t\t(%u)\n", (LPCSTR)CT2CA(Item.second), Item.first);
	}
	fwrite(strInfo.GetBuffer(), strInfo.GetLength(), 1, pFile);

	//close
	fclose(pFile);
	pFile = NULL;

	return true;
}

bool CDUIXmlPack::LoadCtrlID(LPCTSTR lpszFile)
{
	if (false == PathFileExists(lpszFile))
	{
		CDUIGlobal::GetInstance()->SetDuiLastError(_T("Lost CtrlID File"));
		return false;
	}

	CMMString strData;
	CMMFile::GetFileData(lpszFile, strData);

	//ctrl id
	int nPosFrom = strData.find(_T("\n#define"));
	do
	{
		if (-1 == nPosFrom) break;

		TCHAR szCtrlID[MAX_PATH] = {};
		UINT uCtrlID = 0;
		int nScan = swscanf_s(strData.c_str() + nPosFrom, _T("\n#define %[^\(](%u)"), szCtrlID, MAX_PATH, &uCtrlID);
		if (nScan <= 0) break;

		CMMString strCtrlID = szCtrlID;
		strCtrlID.Trim();
		if (false == strCtrlID.empty() && uCtrlID > 0)
		{
			CDUIGlobal::GetInstance()->AddCtrlID(uCtrlID, strCtrlID);
		}

		nPosFrom = strData.find(_T("\n#define"), nPosFrom + 1);

	} while (true);

	return true;
}

bool CDUIXmlPack::SaveXmlUI(LPCTSTR lpszFile, CDUIWnd *pWnd)
{
	if (true == MMInvalidString(lpszFile)
		|| NULL == pWnd
		|| NULL == pWnd->GetRootCtrl()
		|| pWnd->IsDesigned())
	{
		return false;
	}

	//路径
	std::string strPathA = (LPSTR)CT2CA(lpszFile);

	//节点
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLDeclaration *pDecl = xmlDoc.NewDeclaration();
	xmlDoc.LinkEndChild(pDecl);

	tinyxml2::XMLElement *pNode = xmlDoc.NewElement((LPCSTR)CT2CA(pWnd->GetClass()));
	pWnd->SaveAttribute(pNode);
	xmlDoc.LinkEndChild(pNode);

	xmlDoc.SaveFile(strPathA.c_str());

	//encrypt
	EncryptXmlFile(lpszFile);

	return true;
}

CMMString CDUIXmlPack::SaveDui(CDUIPropertyObject *pPropObj, bool bIncludeChild)
{
	if (NULL == pPropObj) return _T("");

	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLElement *pNode = xmlDoc.NewElement((LPCSTR)CT2CA(pPropObj->GetClass()));
	if (NULL == pNode) return _T("");

	pPropObj->SaveAttribute(pNode, bIncludeChild);

	tinyxml2::XMLPrinter xmlPrinter;
	pNode->Accept(&xmlPrinter);

	return xmlPrinter.CStr();;
}

bool CDUIXmlPack::LoadDuiXml(LPCTSTR lpszFile, tinyxml2::XMLDocument &DuiXml)
{
	if (MMInvalidString(lpszFile)) return false;

	vector<BYTE> vecData;
	if (false == CDUIGlobal::GetInstance()->ExtractResourceData(vecData, lpszFile)
		|| (/*vecData = CMMEncrypt::MapDecrypt(vecData, Key_ResourceEncrypt), */vecData.empty())
		|| XML_SUCCESS != DuiXml.Parse((const char*)vecData.data(), vecData.size()))
	{
		CMMString strError = DuiXml.ErrorStr();

		CMMString strWarning;
		strWarning.Format(_T("解析xml[%s]失败, cause[%s]"), lpszFile, strError.c_str());
		::MessageBox(NULL, strWarning, NULL, MB_OK);

		return false;
	}

	return true;
}

CDUIControlBase * CDUIXmlPack::LoadDui(tinyxml2::XMLDocument &DuiXml, CDUIWnd *pWnd)
{
	DWORD dwTickCount = GetTickCount();

	//开始导入
	tinyxml2::XMLElement *pXMLRoot = DuiXml.RootElement();
	if (NULL == pXMLRoot)
	{
		CDUIGlobal::GetInstance()->SetDuiLastError(CMMStrHelp::Format(_T("解析xml为空")));

		return NULL;
	}

	//创建属性
	CDUIContainerCtrl *pRootCtrl = NULL;
	tinyxml2::XMLElement *pNodeXml = pXMLRoot->FirstChildElement();
	if (NULL == pNodeXml)
	{
		CDUIGlobal::GetInstance()->SetDuiLastError(CMMStrHelp::Format(_T("解析xml节点错误")));

		return NULL;
	}
	do
	{
		//control
		if (NULL == pRootCtrl)
		{
			CDUIControlBase *pControl = CDUIFactory::GetInstance()->CreateControlObj(pNodeXml->Name());
			if (pControl)
			{
				pRootCtrl = MMInterfaceHelper(CDUIContainerCtrl, pControl);
				if (NULL == pRootCtrl)
				{
					MMSafeDelete(pControl);

					continue;
				}

				LoadControlFromXML(pNodeXml, pRootCtrl);

				break;
			}
		}

		//wndmanager
		if (pWnd)
		{
			//refresh attribute
			if (pWnd->IsInitComplete())
			{
				CDUIAttributeObject Attribute;
				LoadAtrributeFromXML(pNodeXml, &Attribute);

				CMMString strName = Attribute.GetAttributeName();
				CDUIAttributeObject *pAttributeRefresh = pWnd->GetAttributeObj(strName);
				if (pAttributeRefresh)
				{
					LoadAtrributeFromXML(pNodeXml, pAttributeRefresh);
				}

				continue;
			}

			//new attribute
			if (false == pWnd->AddAttributeBuffer(pNodeXml))
			{
				assert(false);
			}

			continue;
		}

	} while (pNodeXml = pNodeXml->NextSiblingElement(), pNodeXml);

	if (pWnd)
	{
		pWnd->Init();
	}
	if (pRootCtrl)
	{
		pRootCtrl->RefreshChildInternVisible(true);
	}

	MMTRACE(_T("LoadDui:%s-Time:%u"), _T(""), GetTickCount() - dwTickCount);

	return pRootCtrl;
}

CDUIControlBase * CDUIXmlPack::LoadDui(LPCTSTR lpszFile, CDUIWnd *pWnd)
{
	if (MMInvalidString(lpszFile))
	{
		CDUIGlobal::GetInstance()->SetDuiLastError(CMMStrHelp::Format(_T("duifile:[%s]不存在"), lpszFile));

		return NULL;
	}

	DWORD dwTickCount = GetTickCount();

	tinyxml2::XMLDocument xmlDoc;
	if (false == LoadDuiXml(lpszFile, xmlDoc))
	{
		CDUIGlobal::GetInstance()->SetDuiLastError(CMMStrHelp::Format(_T("duifile:[%s]提取xml解析失败"), lpszFile));

		return NULL;
	}

	MMTRACE(_T("LoadDuiFile:%s-Time:%u"), lpszFile, GetTickCount() - dwTickCount);

	return LoadDui(xmlDoc, pWnd);
}

CDUIControlBase * CDUIXmlPack::ParseDui(tinyxml2::XMLElement *pNodeXml)
{
	if (NULL == pNodeXml) return NULL;

	CDUIControlBase *pCtrl = static_cast<CDUIControlBase*>(CDUIFactory::GetInstance()->CreateControlObj(pNodeXml->Name()));
	if (pCtrl)
	{
		LoadControlFromXML(pNodeXml, pCtrl);
	}

	return pCtrl;
}

CDUIControlBase * CDUIXmlPack::ParseDui(LPCTSTR lpszXml)
{
	if (NULL == lpszXml) return NULL;

	tinyxml2::XMLDocument xmlDoc;
	CStringA strXml = lpszXml;
	xmlDoc.Parse(strXml, strXml.GetLength());

	return ParseDui(xmlDoc.RootElement());
}

bool CDUIXmlPack::RefreshAttibute(tinyxml2::XMLElement *pNodeXml, CDUIPropertyObject *pPropObj)
{
	if (NULL == pNodeXml || NULL == pPropObj) return false;

	//参数节点
	tinyxml2::XMLElement *pNodeSub = pNodeXml->FirstChildElement();
	if (NULL == pNodeSub) return false;

	do
	{
		CDUIAttributeObject *pAttribute = pPropObj->GetAttributeObj((LPCTSTR)CA2CT(pNodeSub->Name()));
		if (pAttribute)
		{
			LoadAtrributeFromXML(pNodeSub, pAttribute);

			pAttribute->NotifyChange();

			continue;
		}

	} while (pNodeSub = pNodeSub->NextSiblingElement(), NULL != pNodeSub);

	return true;
}

bool CDUIXmlPack::LoadResourceFromXML(tinyxml2::XMLElement *pNodeXml, CDUIResourceBase *pDUIResource)
{
	if (NULL == pNodeXml || NULL == pDUIResource) return false;

	CMMString strName, strValue;

	//参数节点
	const tinyxml2::XMLAttribute* pNodeAttribute = pNodeXml->FirstAttribute();
	if (NULL == pNodeAttribute) return false;

	do
	{
		pDUIResource->SetAttribute(pNodeAttribute->Name(), pNodeAttribute->Value());

	} while (pNodeAttribute = pNodeAttribute->Next(), NULL != pNodeAttribute);

	return true;
}

bool CDUIXmlPack::LoadControlFromXML(tinyxml2::XMLElement *pNodeXml, CDUIControlBase *pControl)
{
	if (NULL == pNodeXml || NULL == pControl) return false;

	//child node
	pNodeXml = pNodeXml->FirstChildElement();
	if (NULL == pNodeXml)
	{
		pControl->Init();

		return false;
	}

	bool bPropertyInited = false;
	do
	{
		//control
		CDUIControlBase *pChildCtrl = CDUIFactory::GetInstance()->CreateControlObj(pNodeXml->Name());
		if (pChildCtrl)
		{
			if (false == bPropertyInited)
			{
				bPropertyInited = true;

				pControl->InitProperty();
			}

			LoadControlFromXML(pNodeXml, pChildCtrl);

			MMInterfaceHelper(CDUIContainerCtrl, pControl, pContainer);
			if (NULL == pContainer)
			{
				MMSafeDelete(pChildCtrl);
				continue;
			}
			if (false == pContainer->InsertChild(pChildCtrl))
			{
				MMSafeDelete(pChildCtrl);
				continue;
			}

			continue;
		}

		//attribute
		if (false == pControl->AddAttributeBuffer(pNodeXml))
		{
			assert(false);
		}

		continue;

	} while (pNodeXml = pNodeXml->NextSiblingElement(), NULL != pNodeXml);

	if (bPropertyInited)
	{
		pControl->InitComplete();
	}
	else
	{
		pControl->Init();
	}

	return true;
}

bool CDUIXmlPack::LoadAtrributeFromXML(tinyxml2::XMLElement *pNodeXml, CDUIAttributeObject *pAttribute)
{
	if (NULL == pNodeXml || NULL == pAttribute) return false;

	CMMString strName, strValue;

	//参数节点
	const tinyxml2::XMLAttribute* pNodeAttribute = pNodeXml->FirstAttribute();
	if (NULL == pNodeAttribute) return false;

	do
	{
		pAttribute->SetAttribute(pNodeAttribute->Name(), pNodeAttribute->Value());

	} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	return true;
}

bool CDUIXmlPack::LoadAttriValueFromXml(tinyxml2::XMLElement *pNodeXml, std::function<void(tinyxml2::XMLElement*)> pFunc)
{
	if (NULL == pNodeXml || NULL == pFunc) return false;

	do
	{
		pFunc(pNodeXml);

	} while (pNodeXml = pNodeXml->NextSiblingElement(), pNodeXml);

	return true;
}

void CDUIXmlPack::EncryptXmlFile(CMMString strFile)
{
	return;

	//copy
#ifdef DUI_DESIGN 
#ifdef _DEBUG
	/*CMMString strPath, strFileName;
	CMMFile::ParseFilePathName(strFile, strPath, strFileName);

	CMMString strName, strExt;
	CMMFile::ParseFileName(strFileName, strName, strExt);

	CMMString strFileCopy = CMMService::GetUserDataPath(Name_AppDUIThink) + _T("Copy\\");
	SHCreateDirectory(NULL, strFileCopy);
	strFileCopy += strName + _T(".") + strExt;
	CopyFile(strFile, strFileCopy, false);*/
#endif
#endif

	//encrypt
	std::vector<BYTE> vecData;
	CMMFile::GetFileData(strFile, vecData);
	vecData = CMMEncrypt::MapEncrypt(vecData, Key_ResourceEncrypt);
	FILE *pFile = fopen((LPCSTR)CT2CA(strFile), "wb");
	if (pFile)
	{
		vecData.size() > 0 ? fwrite(vecData.data(), vecData.size(), 1, pFile) : NULL;

		fclose(pFile);
	}

	return;
}