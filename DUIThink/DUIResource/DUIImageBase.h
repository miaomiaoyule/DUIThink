#ifndef __DUIIMAGEBASE_H__
#define __DUIIMAGEBASE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIImageBase INTERFACE_VERSION(1,1)
static const GUID IID_CDUIImageBase = { 0x9EE99C9B,0xD4F7,0x49ED,0x8B,0xB1,0x22,0x3A,0x51,0xA7,0xDE,0xE5 };
class DUITHINK_API CDUIImageBase 
	: public CDUIResourceBase
{
	friend class CDUIRenderEngine;
	friend class CDUIAttriImageSection;
	friend class CImagePreviewWnd;
	friend class CDUIGlobal;

	DuiDeclare_CreateResource(CDUIImageBase)
	MMDeclare_ClassName(CDUIImageBase)

public:
	CDUIImageBase();
	CDUIImageBase(LPCTSTR lpszResName, LPCTSTR lpszImageFile);
	virtual ~CDUIImageBase(void);

	//variant
protected:
	CMMString							m_strImageFile;
	MapDuiDpiImageInfo					m_mapDpiImageInfo;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	enDuiResType GetResourceType() const override;
	void ReleaseResource() override;
	virtual CMMString GetImageFileFull();
	virtual CMMString GetImageFile();
	virtual CMMString GetImageFileName();
	virtual void SetImageFile(LPCTSTR lpszImageFile);
	virtual HBITMAP GetHandle(int nScale = 100);
	virtual HBITMAP CloneHandle(int nScale = 100);
	virtual Gdiplus::Bitmap * GetBitmap(int nScale = 100);
	virtual Gdiplus::Bitmap * CloneBitmap(int nScale = 100);
	virtual Gdiplus::Bitmap * GetAnimateImage(int nScale = 100);
	virtual tagDuiImageInfo GetImageInfo(int nScale = 100);
	virtual tagDuiAnimateImageInfo GetAnimateImageInfo(int nScale = 100);
	virtual int GetWidth(int nScale = 100);
	virtual int GetHeight(int nScale = 100);
	virtual bool IsAlpha();
	virtual bool IsScale(int nScale = 100);

	//help
protected:
	bool SetResourceName(const CMMString &strName);
	void ConstructResource(int nScale = 100) override;
	bool ConstructAnimate(std::vector<BYTE> &vecData, int nScale);
	bool ConstructNormal(std::vector<BYTE> &vecData, int nScale);
	bool ConstructBitmap(LPBYTE pPixel, int nWidth, int nHeight, int nScale);
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
	void OnDpiChanged(int nScale) override;
};

typedef std::vector<CDUIImageBase*> VecDuiImageBase;
typedef std::unordered_map<CMMString, CDUIImageBase*> MapDuiImageBase;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif