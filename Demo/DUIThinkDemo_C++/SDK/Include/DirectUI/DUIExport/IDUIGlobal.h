#ifndef __IDUIGLOBAL_H__
#define __IDUIGLOBAL_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_IDUIGlobal INTERFACE_VERSION(1,1)
static const GUID IID_IDUIGlobal = { 0xB5C891DF,0x0FB4,0x4277,0x8B,0xA6,0x9A,0x80,0xDE,0x98,0xA8,0x0A };
interface DUITHINK_API IDUIGlobal : public IDUIInterface
{
	//method
public:
	virtual bool Init(HINSTANCE hInstance) = NULL;
	virtual bool UnInit() = NULL;
	virtual bool IsLoadProject() = NULL;
	virtual void InitializeWebkit() = NULL;
	virtual void UninitializeWebkit() = NULL;
	virtual bool LoadProjectFromFile(LPCTSTR lpszProjFile) = NULL;
	virtual bool LoadProjectFromZip(LPCTSTR lpszZipFile, LPCTSTR lpszPassword, LPCTSTR lpszProjName) = NULL;
	virtual bool LoadProjectFromZip(void *pData, UINT uDataLen, LPCTSTR lpszPassword, LPCTSTR lpszProjName) = NULL;
	virtual bool LoadProjectFromResZip(HINSTANCE hResModule, LPCTSTR lpszZipName, LPCTSTR lpszPassword, LPCTSTR lpszProjName, LPCTSTR lpszResType = _T("ZIP")) = NULL;

	//switch skin
	virtual void PerformSwitchRes(int nIndexRes) = NULL;
	virtual int GetSwitchResIndex() = NULL;

	virtual const CMMDpi & GetDpiObj() = NULL;
	virtual int GetDpi() = NULL;
	virtual bool SetDpi(int nDpi) = NULL;
	virtual int GetScale() = NULL;
	virtual bool SetScale(int nScale) = NULL;

	//resource
public:
	virtual bool AddResource(CDUIResourceBase *pResourceObj) = NULL;

	virtual int GetFontResourceCount() = NULL;
	virtual CDUIFontBase * GetFontResource(const CMMString &strName) = NULL;
	virtual CDUIFontBase * GetFontResource(int nIndex) = NULL;
	virtual CDUIFontBase * GetFontResDefault() = NULL;

	virtual int GetColorResourceCount() = NULL;
	virtual CDUIColorBase * GetColorResource(const CMMString &strName) = NULL;
	virtual CDUIColorBase * GetColorResource(int nIndex) = NULL;
	virtual CDUIColorBase * GetColorResource(ARGB dwColor) = NULL;

	virtual int GetImageResourceCount() = NULL;
	virtual CDUIImageBase * GetImageResource(const CMMString &strName) = NULL;
	virtual CDUIImageBase * GetImageResource(int nIndex) = NULL;
	virtual CDUIImageBase * GetImageResourceByFile(const CMMString &strFileFull) = NULL;

	virtual int GetDuiCount(enDuiType DuiType) = NULL;
	virtual CMMString GetDuiName(int nIndex, enDuiType DuiType) = NULL;
	virtual enDuiType GetDuiType(const CMMString &strName) = NULL;
	virtual CMMString GetDuiFile(int nIndex, enDuiType DuiType) = NULL;
	virtual CMMString GetDuiFile(const CMMString &strName) = NULL;
	virtual CMMString GetDuiFileFull(const CMMString &strName) = NULL;

	//path
	virtual CMMString GetProjectPath() = NULL;
	virtual CMMString GetProjectName() = NULL;
	virtual CMMString GetProjectFileFull() = NULL;
	virtual CMMString GetSkinPath() = NULL;
	virtual CMMString GetDuiPath(enDuiType DuiType) = NULL;

	virtual HINSTANCE GetInstanceHandle() = NULL;
	virtual CMMString GetInstancePath() = NULL;
	virtual HINSTANCE GetResourceDll() = NULL;
	virtual HZIP GetResourceZipHandle() = NULL;
	virtual enDuiFileResType GetDuiFileResType() = NULL;

	virtual void GetHSL(short* H, short* S, short* L) = NULL;
	virtual void SetHSL(bool bUseHSL, short H, short S, short L) = NULL; // H:0~360, S:0~200, L:0~200 

	//UI
public:
	//************************************
	// Description:	if pWndManager is not null£¬method will fresh it's attribute from res
	//************************************
	virtual CDUIControlBase * LoadDui(const CMMString &strName, CDUIWndManager *pWndManager = NULL) = NULL;

	//static
public:
	static void MessageLoop();
};
//////////////////////////////////////////////////////////////////////////

#endif