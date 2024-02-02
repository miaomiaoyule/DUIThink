#ifndef __DUIGLOBAL_H__
#define __DUIGLOBAL_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<CMMString, CDUIControlBase*> MapDuiModelStore;
typedef std::map<UINT, CMMString> MapDuiControlID;

//Êä³ö
LPCTSTR DUITHINK_API DUI__TraceMsg(UINT uMsg);

//////////////////////////////////////////////////////////////////////////
class CDUIOperatorHistory;
class CDTDesignerView;
class CProjectView;
class CPropertiesView;
class CDlgDefaultAttribList;
class CDTDesignerDoc;
class CDUIWndImageList;
class CImageResView;
class CFontResView;
class CColorGridWnd;

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIGlobal 
	: public CMMAsyncObject
	, public CMMServiceItem
{
	friend class CDUIControlBase;
	friend class CDUIControlObject;
	friend class CDUIContainerCtrl;
	friend class CDUIMenuItemCtrl;
	friend class CDUIMenuWnd;
	friend class CDUIRotateMenuCtrl;
	friend class CDUICalendarWnd;
	friend class CDUIXmlPack;
	friend class CDuiOperatorHistory;
	friend class CUICommandElement;
	friend class CDesignerView;
	friend class CProjectView;
	friend class CPropertiesView;
	friend class CDlgDefaultAttribList;
	friend class CDesignerTitle;
	friend class CImageResView;
	friend class CFontResView;
	friend class CColorResView;
	friend class CColorGridWnd;
	friend class CDUIWndManager;
	friend class CDUIAttributeObject;
	friend class CDUIAttriImageSection;
	friend class CDUIAttributeTextStyle;
	friend class CDUIAttributeRichText;
	friend class CDUIAttributeText;
	friend class CDUIAttributeColorSwitch;
	friend class CDUIAttributeCombox;
	friend class CDUIAttributePosition;
	friend class CDUIAttributeHotKey;
	friend class CDUIAttributeRect;
	friend class CDUIAttributeSize;
	friend class CDUIAttriTabSelect;
	friend class CDUIPropertyObject;
	friend class CDUIImageBase;
	friend class CDUIPreview;
	friend class CControlView;
	friend class CVSManager;
	friend class CDlgWizardVariant;
	friend class CDlgWizardNotify;
	friend class CDUIPropertyGridProperty;
	friend class CControlNotifyView;
	friend class CDTDesignerApp;

private:
	CDUIGlobal(void);
	~CDUIGlobal(void);

	//variant
private:
	//path file
	CMMString							m_strLastError;
	CMMString							m_strProjectPath;
	CMMString							m_strProjectName;
	CMMString							m_strSkinDir;
	CMMString							m_strDlgDir;
	CMMString							m_strViewDir;
	CMMString							m_strModelListItemDir;
	CMMString							m_strModelTreeNodeDir;
	CMMString							m_strMenuDir;
	CMMString							m_str3DMenuDir;
	CMMString							m_strCalendarDir;
	CMMString							m_strFontResDefault;

	//manager resource	
	MapDuiFontBase						m_mapResourceFont;
	MapDuiColorBase						m_mapResourceColor;
	MapDuiImageBase						m_mapResourceImage;
	VecDuiFile							m_vecDui;
	MapDuiModelStore					m_mapModelStore;
	MapDuiControlID						m_mapControlID;

	//attribute
	bool								m_bAttriWaitSave = false;
	MapAttributeName					m_mapAttriNameValue;
	MapDuiAttributeRichText				m_mapAttriRichTextValue;
	MapDuiAttributeRichText				m_mapAttriRichTextSave;
	MapDuiAttributeText					m_mapAttriTextValue;
	MapDuiAttributeText					m_mapAttriTextSave;
	MapDuiAttributeTextStyle			m_mapAttriTextStyleValue;
	MapDuiAttributeTextStyle			m_mapAttriTextStyleSave;
	MapDuiAttributeColorRes				m_mapAttriColorResValue;
	MapDuiAttributeColorRes				m_mapAttriColorResSave;
	MapDuiAttributeCombox				m_mapAttriComboxValue;
	MapDuiAttributeCombox				m_mapAttriComboxSave;
	MapDuiAttributePosition				m_mapAttriPositionValue;
	MapDuiAttributePosition				m_mapAttriPositionSave;
	MapDuiAttributeRect					m_mapAttriRectValue;
	MapDuiAttributeRect					m_mapAttriRectSave;
	MapDuiAttriImageSection				m_mapAttriImageSectionValue;
	MapDuiAttriImageSection				m_mapAttriImageSectionSave;

	//info
	std::recursive_mutex				m_DataLock;
	bool								m_bProjectExist = false;
	CMMThreadPool						m_ThreadPool;
	CMMDpi								m_DpiInfo;
	std::vector<HMODULE>				m_vecModuleExtendDll;
	MapWndManager						m_mapWndManager;
	HINSTANCE							m_hInstance = NULL;
	HINSTANCE							m_hInstanceResource = NULL;
	enDuiFileResType					m_DuiFileResType = DuiFileResType_File;
	enDuiResVersion						m_DuiResVersion = DuiResVersion_0;
	int									m_nIndexSwitchRes = 0;
	UINT								m_uMaxControlID = Dui_CtrlIDInner_Finish;
	UINT								m_uMaxResourceIndex = 0;

	//zipres
	HZIPDT								m_hResourceZip = NULL;
	std::vector<BYTE>					m_vecZipData;

	//hsl
	bool								m_bUseHSL = false;
	short								m_H = 180;
	short								m_S = 100;
	short								m_L = 100;

	//gdiplus
	ULONG_PTR							m_uToken;
	Gdiplus::GdiplusStartupInput		m_GdiplusInput;

	//override
protected:	
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//override
protected:
	void OnMessage(PtrMMServiceMsg pMessage) override;

	//method
public:
	static CDUIGlobal * GetInstance();
	bool Init(HINSTANCE hInstance);
	bool UnInit() override;
	bool IsLoadProject();
	void InitializeWebkit();
	void UninitializeWebkit();
	bool LoadProjectFromFile(LPCTSTR lpszProjFile);
	bool LoadProjectFromZip(LPCTSTR lpszZipFile, LPCTSTR lpszPassword, LPCTSTR lpszProjName);
	bool LoadProjectFromZip(void *pData, UINT uDataLen, LPCTSTR lpszPassword, LPCTSTR lpszProjName);
	bool LoadProjectFromResZip(HINSTANCE hResModule, LPCTSTR lpszZipName, LPCTSTR lpszPassword, LPCTSTR lpszProjName, LPCTSTR lpszResType = _T("ZIP"));
	CMMString GetDuiLastError();
	void SetDuiLastError(CMMString strError);
	enDuiResVersion GetResVersion() { return m_DuiResVersion; }

	//switch skin
	void PerformSwitchRes(int nIndexRes);
	int GetSwitchResIndex();

	//info
	const CMMDpi & GetDpiObj();
	int GetDpi();
	bool SetDpi(int nDpi);
	int GetScale();
	bool SetScale(int nScale);

	//method resource
public:
	bool AddResource(CDUIResourceBase *pResourceObj);

	int GetFontResourceCount();
	CDUIFontBase * GetFontResource(const CMMString &strName);
	CDUIFontBase * GetFontResource(int nIndex);
	CDUIFontBase * GetFontResDefault();

	int GetColorResourceCount();
	CDUIColorBase * GetColorResource(const CMMString &strName);
	CDUIColorBase * GetColorResource(int nIndex);
	CDUIColorBase * GetColorResource(ARGB dwColor);

	int GetImageResourceCount();
	CDUIImageBase * GetImageResource(const CMMString &strName);
	CDUIImageBase * GetImageResource(int nIndex);
	CDUIImageBase * GetImageResourceByFile(const CMMString &strFileFull);

	int GetDuiCount(enDuiType DuiType);
	CMMString GetDuiName(int nIndex, enDuiType DuiType);
	enDuiType GetDuiType(const CMMString &strName);
	CMMString GetDuiFile(int nIndex, enDuiType DuiType);
	CMMString GetDuiFile(const CMMString &strName);
	CMMString GetDuiFileFull(const CMMString &strName);

	//path
	CMMString GetProjectPath();
	CMMString GetProjectName();
	CMMString GetProjectFileFull();
	CMMString GetSkinPath();
	CMMString GetDuiPath(enDuiType DuiType);

	//resource
	HINSTANCE GetInstanceHandle();
	CMMString GetInstancePath();
	HINSTANCE GetResourceDll();
	HZIPDT GetResourceZipHandle();
	enDuiFileResType GetDuiFileResType();
	
	//hsl
	void GetHSL(short *H, short *S, short *L);
	void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 

	//dui
public:
	CDUIControlBase * LoadDui(const CMMString &strName, CDUIWndManager *pWndManager = NULL);

	//ui manager
protected:
	bool SaveProject();
	bool CloseProject();

	//ctrl id
	void AddCtrlID(UINT uCtrlID, CMMString strCtrlID);
	UINT FindCtrlID(CMMString strCtrlID);
	CMMString FindCtrlID(UINT uCtrlID);
	UINT GenerateCtrlID(CDUIControlBase *pControl);
	bool ModifyCtrlID(UINT uIDOld, UINT uIDNew, CDUIControlBase *pControl);
	bool ModifyCtrlID(CMMString strCtrlIDOld, CMMString strCtrlIDNew, CDUIControlBase *pControl);
	
	//dui
	CMMString CreateDlg();
	CMMString CreateView();
	CMMString CreateModelListItem();
	CMMString CreateModelTreeNode();
	CMMString CreateMenu(bool bSubMenu);
	CMMString Create3DMenu();
	CMMString CreateCalendar();
	CDUIControlBase * ParseDui(tinyxml2::XMLElement *pNodeXml);
	CDUIControlBase * ParseDui(LPCTSTR lpszXml);
	bool SaveDui(LPCTSTR lpszName, CDUIWndManager *pWndManager);
	CMMString SaveDui(CDUIPropertyObject *pProp, bool bIncludeChild = true);
	bool ExtractResourceData(vector<BYTE> &vecData, CMMString strFile);
	
	//refresh
	bool RefreshAttibute(tinyxml2::XMLElement *pNodeXml, CDUIPropertyObject *pProp);

	//module
protected:
	CMMThreadPool * GetThreadPool();

	//help
protected:
	//config
	void LoadPublicResource();
	void LoadConfigCtrl(const CMMString &strConfigFile);
	bool SetProjectPath(LPCTSTR lpszPath);
	bool SetProjectName(LPCTSTR lpszName);
	bool SetResVersion(enDuiResVersion ResVersion);
	
	//resource
	bool AddFontResource(CDUIFontBase *pResourceObj);
	bool AddImageResource(CDUIImageBase *pResourceObj);
	bool AddColorResource(CDUIColorBase *pResourceObj);
	bool AddDui(enDuiType DuiType, const CMMString &strName, const CMMString &strFile);
	bool RenameFontResource(const CMMString &strNameOld, const CMMString &strNameNew);
	bool RenameImageResource(const CMMString &strNameOld, const CMMString &strNameNew);
	bool RenameColorResource(const CMMString &strNameOld, const CMMString &strNameNew);
	bool RenameDui(const CMMString &strNameOld, const CMMString &strNameNew);
	bool RemoveFontResource(const CMMString &strName);
	bool RemoveImageResource(const CMMString &strName);
	bool RemoveColorResource(const CMMString &strName);
	bool RemoveDui(const CMMString &strName);
	void SetFontResDefault(const CMMString &strName);
	void OnDpiChanged(int nScalePre);
	void OnResourceDelete(CDUIResourceBase *pResourceObj);
	void OnResourceSwitch(int nIndexRes);

	//attribute
	LPCTSTR GetAttriName(uint32_t uValueID);
	uint32_t SetAttriName(LPCTSTR lpszName);
	bool SetAttriName(tinyxml2::XMLElement *pNode);
	bool SaveAttriName(tinyxml2::XMLElement *pNode);
	tagDuiRichText GetAttriRichText(uint32_t uValueID);
	uint32_t SetAttriRichText(const tagDuiRichText &RichText);
	bool SetAttriRichText(tinyxml2::XMLElement *pNode);
	bool SaveAttriRichText(tinyxml2::XMLElement *pNode);
	LPCTSTR GetAttriText(uint32_t uValueID);
	uint32_t SetAttriText(LPCTSTR lpszText);
	bool SetAttriText(tinyxml2::XMLElement *pNode);
	bool SaveAttriText(tinyxml2::XMLElement *pNode);
	tagDuiTextStyle GetAttriTextStyle(uint32_t uValueID);
	uint32_t SetAttriTextStyle(const tagDuiTextStyle &TextStyle);
	bool SetAttriTextStyle(tinyxml2::XMLElement *pNode);
	bool SaveAttriTextStyle(tinyxml2::XMLElement *pNode);
	vector<CMMString> GetAttriColorResSwitch(uint32_t uValueID);
	uint32_t SetAttriColorResSwitch(const vector<CMMString> &vecColorRes);
	bool SetAttriColorResSwitch(tinyxml2::XMLElement *pNode);
	bool SaveAttriColorResSwitch(tinyxml2::XMLElement *pNode);
	tagDuiCombox GetAttriCombox(uint32_t uValueID);
	uint32_t SetAttriCombox(const tagDuiCombox &AttriCombox);
	bool SetAttriCombox(tinyxml2::XMLElement *pNode);
	bool SaveAttriCombox(tinyxml2::XMLElement *pNode);
	tagDuiPosition GetAttriPosition(uint32_t uValueID);
	uint32_t SetAttriPosition(const tagDuiPosition &Position);
	bool SetAttriPosition(tinyxml2::XMLElement *pNode);
	bool SaveAttriPosition(tinyxml2::XMLElement *pNode);
	CDUIRect GetAttriRect(uint32_t uValueID);
	uint32_t SetAttriRect(const CDUIRect &rect);
	bool SetAttriRect(tinyxml2::XMLElement *pNode);
	bool SaveAttriRect(tinyxml2::XMLElement *pNode);
	tagDuiImageSection GetAttriImageSection(uint32_t uValueID);
	uint32_t SetAttriImageSection(const tagDuiImageSection &ImageSection);
	bool SetAttriImageSection(tinyxml2::XMLElement *pNode);
	bool SaveAttriImageSection(tinyxml2::XMLElement *pNode);
	void OnAttriValueIDRead(enDuiAttributeType AttriType, uint32_t uID);
	bool SaveAttriValue(tinyxml2::XMLDocument &xmlDoc);

	//WndManager
	void AddWndManager(CDUIWndManager *pWndManager);
	MapWndManager GetWndManagerAll();
	tagDuiFile GetWndManagerInfo(CDUIWndManager *pWndManager);
	void RenameWndManager(const CMMString &strNameOld, const CMMString &strNameNew);
	void RenameWndManager(CDUIWndManager *pWndManager, const CMMString &strNameNew);
	void SetWndManagerDuiType(CDUIWndManager *pWndManager, enDuiType DuiType);
	void RemoveWndManager(CDUIWndManager *pWndManager);

	//release
	void ReleaseResource();
	void ReleaseFontResource();
	void ReleaseImageResource();
	void ReleaseColorResource();
	void ReleaseDui();

	//static
public:
	static void MessageLoop();
	static bool TranslateMessage(const LPMSG pMsg);

	//static help
public:
#ifdef DUITHINKWKE
	static jsValue JS_CALL JsToNative(jsExecState es);
#endif
	static void RegisterWndNotify(IDUIWndNotify *pIDuiWndNotify);
	static void UnRegisterWndNotify(IDUIWndNotify *pIDuiWndNotify);
	static int IsEmoji(TCHAR ch);

	//static help
public:
	static void PerformNotifyChildAdd(CDUIPropertyObject *pPropertyObj, CDUIControlBase *pChild);
	static void PerformNotifyChildRemove(CDUIPropertyObject *pPropertyObj, CDUIControlBase *pChild);
	static void PerformNotifyVisibleChange(CDUIPropertyObject *pPropertyObj);
	static void PerformResourceDelete(CDUIControlBase *pControl, CDUIResourceBase *pResourceObj);
	static void PerformResourceSwitch(CDUIControlBase *pControl, int nIndexRes);
	static bool PerformAddAttributeBuffer(CDUIPropertyObject *pPropertyObj, tinyxml2::XMLElement *pNodeXml);
	static VecDuiRichTextItem ParseVecRichTextItem(LPCSTR lpszValue);
	static VecAttributeGroup GetAttributeGroup(CDUIPropertyObject *pPropertyObj);
	static int GetAttributeGroupIndex(CDUIAttributeGroup *pAttributeGroup);
	static void SetAttributeGroupIndex(CDUIAttributeGroup *pAttributeGroup, int nIndex);
	static CDUIAttributeObject * GetAttributeObj(CDUIPropertyObject *pPropertyObj, LPCTSTR lpszName);
	static bool CreateAttributeForDesign(CDUIAttributeObject **ppAttribute, enDuiAttributeType attributeType, LPCTSTR lpszName, LPCTSTR lpszDescribe);
};

//////////////////////////////////////////////////////////////////////////

#endif