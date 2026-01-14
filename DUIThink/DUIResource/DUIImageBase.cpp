#include "StdAfx.h"
#include "DUIImageBase.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../DUIUtils/stb_image.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateResource(CDUIImageBase)
MMImplement_ClassName(CDUIImageBase)

CDUIImageBase::CDUIImageBase()
{
	m_strImageFile = _T("");

	return;
}

CDUIImageBase::CDUIImageBase(LPCTSTR lpszResName, LPCTSTR lpszImageFile)
	: m_strImageFile(lpszImageFile)
{
	m_strName = lpszResName;

	return;
}

CDUIImageBase::~CDUIImageBase(void)
{
	ReleaseResource();

	return;
}

LPVOID CDUIImageBase::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIImageBase, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

enDuiResType CDUIImageBase::GetResourceType() const
{
	return DuiResType_Image;
}

CMMString CDUIImageBase::GetImageFileFull()
{
	if (m_strImageFile.length() >= 2 && _T(':') == m_strImageFile[1])
	{
		return m_strImageFile;
	}

	return CDUIGlobal::GetInstance()->GetSkinPath() + m_strImageFile;
}

CMMString CDUIImageBase::GetImageFile()
{
	return m_strImageFile;
}

CMMString CDUIImageBase::GetImageFileName()
{
	CMMString strName;
	CMMFile::ParseFilePathName(m_strImageFile, CMMString(), strName);

	return strName;
}

void CDUIImageBase::SetImageFile(LPCTSTR lpszImageFile)
{
	if (MMInvalidString(lpszImageFile) || m_strImageFile == lpszImageFile) return;

	m_strImageFile = lpszImageFile;

	ReleaseResource();

	return;
}

HBITMAP CDUIImageBase::GetHandle(int nScale)
{
	return GetImageInfo(nScale).hBitmap;
}

HBITMAP CDUIImageBase::CloneHandle(int nScale)
{
	tagDuiImageInfo BmpSrc = GetImageInfo(nScale);
	if (NULL == BmpSrc.hBitmap || NULL == BmpSrc.pBits) return NULL;

	LPBYTE pDest = NULL;
	HBITMAP hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(NULL, BmpSrc.nWidth, BmpSrc.nHeight, &pDest, NULL == BmpSrc.pImageAnimate);
	if (NULL == hBitmap)
	{
		assert(false);
		return NULL;
	}

	for (int i = 0; i < BmpSrc.nHeight; i++)
	{
		CopyMemory(pDest + BmpSrc.nWidth * 4 * i, BmpSrc.pBits + BmpSrc.nWidth * 4 * i, BmpSrc.nWidth * 4);
	}

	return hBitmap;
}

Gdiplus::Bitmap * CDUIImageBase::GetBitmap(int nScale)
{
	GetImageInfo(nScale);

	if (m_mapDpiImageInfo[nScale].hBitmap && NULL == m_mapDpiImageInfo[nScale].pBitmap)
	{
		m_mapDpiImageInfo[nScale].pBitmap = CDUIRenderEngine::GetAlphaBitmap(m_mapDpiImageInfo[nScale].hBitmap);
	}
	else if (NULL == m_mapDpiImageInfo[100].pBitmap)
	{
		m_mapDpiImageInfo[100].pBitmap = CDUIRenderEngine::GetAlphaBitmap(m_mapDpiImageInfo[100].hBitmap);
	}

	return GetImageInfo(nScale).pBitmap;
}

Gdiplus::Bitmap * CDUIImageBase::CloneBitmap(int nScale)
{
	return CDUIRenderEngine::GetAlphaBitmap(GetHandle(nScale));
}

Gdiplus::Bitmap * CDUIImageBase::GetAnimateImage(int nScale)
{
	if (NULL == GetHandle(nScale)) return NULL;

	return GetImageInfo(nScale).pImageAnimate;
}

tagDuiImageInfo CDUIImageBase::GetImageInfo(int nScale)
{
	if (NULL == m_mapDpiImageInfo[nScale].hBitmap)
	{
		ConstructResource(nScale);
	}

	return m_mapDpiImageInfo[nScale].hBitmap ? m_mapDpiImageInfo[nScale] : m_mapDpiImageInfo[100];
}

tagDuiAnimateImageInfo CDUIImageBase::GetAnimateImageInfo(int nScale)
{
	if (NULL == GetHandle(nScale)) return {};

	tagDuiImageInfo ImageInfo = GetImageInfo(nScale);
	if (NULL == ImageInfo.pImageAnimate) return tagDuiAnimateImageInfo();

	tagDuiAnimateImageInfo AnimateImageInfo;
	AnimateImageInfo.AnimateImageType = ImageInfo.nFrameCount > 0 ? AnimateImage_Gif : AnimateImage_None;
	AnimateImageInfo.nFrameCount = ImageInfo.nFrameCount;
	AnimateImageInfo.vecFrameElapse = ImageInfo.vecFrameElapse;

	return AnimateImageInfo;
}

int CDUIImageBase::GetWidth(int nScale)
{
	return GetImageInfo(nScale).nWidth;
}

int CDUIImageBase::GetHeight(int nScale)
{
	return GetImageInfo(nScale).nHeight;
}

bool CDUIImageBase::IsAlpha()
{
	return GetImageInfo(100).bAlpha;
}

bool CDUIImageBase::IsScale(int nScale)
{
	GetImageInfo(nScale);

	return m_mapDpiImageInfo[nScale].hBitmap;
}

void CDUIImageBase::ReleaseResource()
{
	for (auto &ImageInfoItem : m_mapDpiImageInfo)
	{
		auto &ImageInfo = ImageInfoItem.second;

		//image
		MMSafeDelete(ImageInfo.pBitmap);
		MMSafeDeleteObject(ImageInfo.hBitmap);
		ImageInfo.pBits = NULL;

		//animate
		MMSafeDelete(ImageInfo.pImageAnimate);
		ImageInfo.nFrameCount = 0;
	}

	m_mapDpiImageInfo.clear();

	return;
}

bool CDUIImageBase::SetResourceName(const CMMString &strName)
{
	CMMString strOldName = GetResourceName();

	if (false == __super::SetResourceName(strName)) return false;

	//filename
	CMMString strFileFullOld = GetImageFileFull();
	CMMString strPath, strFile;
	CMMFile::ParseFilePathName(strFileFullOld, strPath, strFile);

	strFile.Replace(strOldName, GetResourceName());
	m_strImageFile = strFile;
	CMMString strFileFullNew = strPath + strFile;

	if (0 != ::rename(CT2CA(strFileFullOld), CT2CA(strFileFullNew))) return false;

	return true;
}

void CDUIImageBase::ConstructResource(int nScale)
{
	if (m_mapDpiImageInfo[nScale].hBitmap) return;
	if (m_strImageFile.empty()) return;

	//release image
	MMSafeDelete(m_mapDpiImageInfo[nScale].pBitmap);
	MMSafeDeleteObject(m_mapDpiImageInfo[nScale].hBitmap);
	m_mapDpiImageInfo[nScale].pBits = NULL;
	
	//release animate
	MMSafeDelete(m_mapDpiImageInfo[nScale].pImageAnimate);
	m_mapDpiImageInfo[nScale].nFrameCount = 0;
	m_mapDpiImageInfo.erase(nScale);

	//type
	CMMString strExt;
	CMMFile::ParseFileName(m_strImageFile, CMMString(), strExt);
	bool bSvgFile = 0 == strExt.CompareNoCase(_T("svg"));
	bool bGifFile = 0 == strExt.CompareNoCase(_T("gif"));

	//file
	vector<BYTE> vecData;
	CMMString strFile = GetImageFileFull();

	//scale
	CMMString strScale;
	if (false == bSvgFile && 100 != nScale)
	{
		strScale.Format(_T("@%d"), nScale);

		int nPos = strFile.rfind(_T('.'));
		strFile.insert(nPos, strScale);
	}

	//extract
	do
	{
		if (CDUIGlobal::GetInstance()->ExtractResourceData(vecData, strFile) && false == vecData.empty())
		{
			break;
		}
		if (false == bSvgFile && m_mapDpiImageInfo[100].hBitmap)
		{
			return;
		}
		if (bSvgFile)
		{
			::MessageBox(NULL, _T("Extract Image Fail£¡"), _T("Error"), MB_OK);

			return;
		}

		strFile.Replace(strScale, _T(""));
		if (false == CDUIGlobal::GetInstance()->ExtractResourceData(vecData, strFile) || vecData.empty())
		{
			//::MessageBox(NULL, _T("Extract Image Fail£¡"), _T("Error"), MB_OK);

			return;
		}

		nScale = 100;

	} while (false);

	//parse
	if (bGifFile)
	{
		ConstructAnimate(vecData, nScale);

		return;
	}

	//svg
	if (bSvgFile)
	{
#ifdef MMSvgEnable
		tagDuiImageInfo ImageInfo = {};
		CMMSvg::GetInstance()->ParseImage(vecData, nScale, ImageInfo.hBitmap, ImageInfo.nWidth, ImageInfo.nHeight, &ImageInfo.pBits);

		ImageInfo.bAlpha = true;
		m_mapDpiImageInfo[nScale] = ImageInfo;
#endif

		return;
	}

	//normal
	ConstructNormal(vecData, nScale);

	return;
}

bool CDUIImageBase::ConstructAnimate(std::vector<BYTE> &vecData, int nScale)
{
	if (vecData.size() <= 0) return false;

	//create
	tagDuiImageInfo ImageInfo = {};
	ImageInfo.pImageAnimate = CDUIRenderEngine::GenerateBitmap(vecData);

	//init
	if (NULL == ImageInfo.pImageAnimate) return false;

	UINT nCount = ImageInfo.pImageAnimate->GetFrameDimensionsCount();
	std::vector<GUID> vecDimensionID;
	vecDimensionID.resize(nCount);

	ImageInfo.pImageAnimate->GetFrameDimensionsList(vecDimensionID.data(), nCount);
	ImageInfo.nFrameCount = ImageInfo.pImageAnimate->GetFrameCount(vecDimensionID.data());

	int nSize = ImageInfo.pImageAnimate->GetPropertyItemSize(PropertyTagFrameDelay);
	std::vector<BYTE> vecPropertyItem(nSize);
	Gdiplus::PropertyItem *pPropertyItem = reinterpret_cast<Gdiplus::PropertyItem*>(vecPropertyItem.data());
	if (Gdiplus::Ok == ImageInfo.pImageAnimate->GetPropertyItem(PropertyTagFrameDelay, nSize, pPropertyItem))
	{
		long *pDelays = (long*)pPropertyItem->value;
		ImageInfo.vecFrameElapse.resize(ImageInfo.nFrameCount);
		for (int n = 0; n < ImageInfo.nFrameCount; n++)
		{
			long lDelay = pDelays[n] * 10; 
			ImageInfo.vecFrameElapse[n] = lDelay;
		}
	}

	//info
	BITMAP BmpInfo = {};
	GUID pageGuid = Gdiplus::FrameDimensionTime;
	ImageInfo.pImageAnimate->SelectActiveFrame(&pageGuid, 0);
	ImageInfo.hBitmap = CDUIRenderEngine::GetHBITMAP(ImageInfo.pImageAnimate);
	GetObject(ImageInfo.hBitmap, sizeof(BmpInfo), &BmpInfo);
	ImageInfo.pBits = (LPBYTE)BmpInfo.bmBits;
	ImageInfo.nWidth = ImageInfo.pImageAnimate->GetWidth();
	ImageInfo.nHeight = ImageInfo.pImageAnimate->GetHeight();

	m_mapDpiImageInfo[nScale] = ImageInfo;

	return true;
}

bool CDUIImageBase::ConstructNormal(std::vector<BYTE> &vecData, int nScale)
{
	if (vecData.size() <= 0) return false;

	//normal
	LPBYTE pImage = NULL;
	int x, y, n;
	pImage = stbi_load_from_memory(vecData.data(), vecData.size(), &x, &y, &n, 4);
	if (!pImage)
	{
		assert(false);
		return false;
	}

	bool bRes = ConstructBitmap(pImage, x, y, nScale);

	stbi_image_free(pImage);

	return bRes;
}

bool CDUIImageBase::ConstructBitmap(LPBYTE pPixel, int nWidth, int nHeight, int nScale)
{
	if (NULL == pPixel)
	{
		assert(false);
		return false;
	}

	bool bAlphaChannel = false;
	LPBYTE pDest = NULL;
	HBITMAP hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(NULL, nWidth, nHeight, &pDest);
	if (NULL == hBitmap)
	{
		assert(false);
		return false;
	}

	for (int i = 0; i < nWidth * nHeight; i++)
	{
		pDest[i * 4 + 3] = pPixel[i * 4 + 3];
		if (pDest[i * 4 + 3] < 255)
		{
			pDest[i * 4] = (BYTE)(DWORD(pPixel[i * 4 + 2]) * pPixel[i * 4 + 3] / 255);
			pDest[i * 4 + 1] = (BYTE)(DWORD(pPixel[i * 4 + 1]) * pPixel[i * 4 + 3] / 255);
			pDest[i * 4 + 2] = (BYTE)(DWORD(pPixel[i * 4]) * pPixel[i * 4 + 3] / 255);
			bAlphaChannel = true;
		}
		else
		{
			pDest[i * 4] = pPixel[i * 4 + 2];
			pDest[i * 4 + 1] = pPixel[i * 4 + 1];
			pDest[i * 4 + 2] = pPixel[i * 4];
		}

		if (*(DWORD*)(&pDest[i * 4]) == 0)
		{
			pDest[i * 4] = (BYTE)0;
			pDest[i * 4 + 1] = (BYTE)0;
			pDest[i * 4 + 2] = (BYTE)0;
			pDest[i * 4 + 3] = (BYTE)0;
			bAlphaChannel = true;
		}
	}

	m_mapDpiImageInfo[nScale].hBitmap = hBitmap;
	m_mapDpiImageInfo[nScale].pBits = pDest;
	m_mapDpiImageInfo[nScale].nWidth = nWidth;
	m_mapDpiImageInfo[nScale].nHeight = nHeight;
	m_mapDpiImageInfo[nScale].bAlpha = bAlphaChannel;

	return true;
}

bool CDUIImageBase::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Resource_Key_ImagePath))
	{
		m_strImageFile = CA2CT(lpszValue, CP_UTF8);

		return true;
	}

	return false;
}

bool CDUIImageBase::SaveAttribute(tinyxml2::XMLElement *pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Resource_Key_ImagePath, (LPSTR)CT2CA(m_strImageFile, CP_UTF8));

	return true;
}

void CDUIImageBase::OnDpiChanged(int nScale)
{
	ConstructResource(nScale);

	return __super::OnDpiChanged(nScale);
}