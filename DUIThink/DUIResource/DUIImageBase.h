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

	//image
	Gdiplus::Bitmap *					m_pBitmap = NULL;
	HBITMAP								m_hBitmap = NULL;
	LPBYTE								m_pBits = NULL;
	int									m_nScale = 0;
	int									m_nWidth = 0;
	int									m_nHeight = 0;
	bool								m_bAlpha = false;
	bool								m_bScale = false;

	//animate
	Gdiplus::Bitmap *					m_pImageAnimate = NULL;
	int									m_nFrameCount = 0;
	std::vector<Gdiplus::PropertyItem>	m_vecPropertyItem;							//帧与帧之间间隔时间

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	enDuiResType GetResourceType() const override;
	virtual CMMString GetImageFileFull();
	virtual CMMString GetImageFile();
	virtual CMMString GetImageFileName();
	virtual void SetImageFile(LPCTSTR lpszImageFile);
	virtual HBITMAP GetHandle();
	virtual HBITMAP CloneHandle();
	virtual Gdiplus::Bitmap * GetBitmap();
	virtual Gdiplus::Bitmap * CloneBitmap();
	virtual Gdiplus::Bitmap * GetAnimateImage();
	virtual tagDuiAnimateImageInfo GetAnimateImageInfo();
	virtual int GetWidth();
	virtual int GetHeight();
	virtual bool IsAlpha();
	virtual bool IsScale();

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