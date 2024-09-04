#ifndef __DUIXMLPACK_H__
#define __DUIXMLPACK_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIXmlPack
{
public:
	static bool SaveProject(LPCTSTR lpszProjPath, LPCTSTR lpszProjName, const MapDuiFontBase &mapResFont, \
		const MapDuiImageBase &mapResImage, const MapDuiColorBase &mapResColor, const VecDuiFile &vecDui, \
		const MapWnd &mapWnd, CMMString strFontResDefault);
	static bool LoadProject(LPCTSTR lpszProject);
	static bool SaveResource(LPCTSTR lpszFile, VecDuiResourceBase &VecRes);
	static bool LoadResource(LPCTSTR lpszFile);
	static bool SaveAttribute(LPCTSTR lpszFile);
	static bool LoadAttribute(LPCTSTR lpszFile);
	static bool SaveCtrlID(const MapDuiControlID &mapCtrlID);
	static bool LoadCtrlID(LPCTSTR lpszFile);
	static bool SaveXmlUI(LPCTSTR lpszFile, CDUIWnd *pWnd);
	static CMMString SaveDui(CDUIPropertyObject *pProp, bool bIncludeChild = true);
	static bool LoadDuiXml(LPCTSTR lpszFile, tinyxml2::XMLDocument &DuiXml);
	static CDUIControlBase * LoadDui(tinyxml2::XMLDocument &DuiXml, CDUIWnd *pWnd);
	static CDUIControlBase * LoadDui(LPCTSTR lpszFile, CDUIWnd *pWnd);
	static CDUIControlBase * ParseDui(tinyxml2::XMLElement *pNodeXml);
	static CDUIControlBase * ParseDui(LPCTSTR lpszXml);
	static bool RefreshAttibute(tinyxml2::XMLElement *pNodeXml, CDUIPropertyObject *pProp);

public:
	static bool LoadResourceFromXML(tinyxml2::XMLElement *pNodeXml, CDUIResourceBase *pDUIResource);
	static bool LoadControlFromXML(tinyxml2::XMLElement *pNodeXml, CDUIControlBase *pControl);
	static bool LoadAtrributeFromXML(tinyxml2::XMLElement *pNodeXml, CDUIAttributeObject *pAttribute);
	static bool LoadAttriValueFromXml(tinyxml2::XMLElement *pNodeXml, std::function<void(tinyxml2::XMLElement*)> pFunc);

	//help
protected:
	static void EncryptXmlFile(CMMString strFile);
};

//////////////////////////////////////////////////////////////////////////

#endif