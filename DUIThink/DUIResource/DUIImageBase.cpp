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
	//image
	MMSafeDelete(m_pBitmap);
	MMSafeDeleteObject(m_hBitmap);
	m_pBits = NULL;

	//animate
	MMSafeDelete(m_pImageAnimate);
	m_vecPropertyItem.clear();
	m_nFrameCount = 0;

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

	MMSafeDelete(m_pBitmap);
	MMSafeDeleteObject(m_hBitmap);
	m_pBits = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bAlpha = false;

	ConstructResource(m_nScale);

	return;
}

HBITMAP CDUIImageBase::GetHandle()
{
	if (NULL == m_hBitmap)
	{
		ConstructResource(CDUIGlobal::GetInstance()->GetScale());
	}

	return m_hBitmap;
}

HBITMAP CDUIImageBase::CloneHandle()
{
	HBITMAP hBmpSrc = GetHandle();
	if (NULL == hBmpSrc || NULL == m_pBits) return NULL;

	LPBYTE pDest = NULL;
	HBITMAP hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(NULL, m_nWidth, m_nHeight, &pDest, NULL == m_pImageAnimate);
	if (NULL == hBitmap)
	{
		assert(false);
		return NULL;
	}

	for (int i = 0; i < m_nHeight; i++)
	{
		CopyMemory(pDest + m_nWidth * 4 * i, m_pBits + m_nWidth * 4 * i, m_nWidth * 4);
	}

	return hBitmap;
}

Gdiplus::Bitmap * CDUIImageBase::GetBitmap()
{
	if (NULL == m_pBitmap)
	{
		m_pBitmap = CDUIRenderEngine::GetAlphaBitmap(GetHandle());
	}

	return m_pBitmap;
}

Gdiplus::Bitmap * CDUIImageBase::CloneBitmap()
{
	return CDUIRenderEngine::GetAlphaBitmap(GetHandle());
}

Gdiplus::Bitmap * CDUIImageBase::GetAnimateImage()
{
	if (NULL == GetHandle()) return NULL;

	return m_pImageAnimate;
}

tagDuiAnimateImageInfo CDUIImageBase::GetAnimateImageInfo()
{
	if (NULL == GetHandle()) return {};
	if (NULL == m_pImageAnimate) return tagDuiAnimateImageInfo();

	tagDuiAnimateImageInfo AnimateImageInfo;
	AnimateImageInfo.AnimateImageType = m_nFrameCount > 0 ? AnimateImage_Gif : AnimateImage_None;
	AnimateImageInfo.nFrameCount = m_nFrameCount;
	AnimateImageInfo.vecGifPropertyItem = m_vecPropertyItem;

	return AnimateImageInfo;
}

int CDUIImageBase::GetWidth()
{
	GetHandle();

	return m_nWidth;
}

int CDUIImageBase::GetHeight()
{
	GetHandle();

	return m_nHeight;
}

bool CDUIImageBase::IsAlpha()
{
	GetHandle();

	return m_bAlpha;
}

bool CDUIImageBase::IsScale()
{
	return m_bScale;
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
	if (m_hBitmap && m_nScale == nScale) return;
	if (m_strImageFile.empty()) return;

	//release image
	MMSafeDelete(m_pBitmap);
	MMSafeDeleteObject(m_hBitmap);
	m_pBits = NULL;
	m_nScale = nScale;
	m_bScale = false;
	
	//release animate
	MMSafeDelete(m_pImageAnimate);
	m_vecPropertyItem.clear();
	m_nFrameCount = 0;

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
			m_bScale = 100 != nScale;

			break;
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
		CMMSvg::GetInstance()->ParseImage(vecData, nScale, m_hBitmap, m_nWidth, m_nHeight, &m_pBits);

		m_bAlpha = true;

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
	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, vecData.size());
	BYTE *pMem = (BYTE*)::GlobalLock(hMem);

	CopyMemory(pMem, vecData.data(), vecData.size());

	IStream *pIStream = NULL;
	::CreateStreamOnHGlobal(hMem, TRUE, &pIStream);
	if (NULL == pIStream)
	{
		::GlobalUnlock(hMem);

		return false;
	}

	m_pImageAnimate = Gdiplus::Bitmap::FromStream(pIStream);
	pIStream->Release();
	::GlobalUnlock(hMem);

	//init
	if (NULL == m_pImageAnimate) return false;

	UINT nCount = m_pImageAnimate->GetFrameDimensionsCount();
	std::vector<GUID> vecDimensionID;
	vecDimensionID.resize(nCount);

	m_pImageAnimate->GetFrameDimensionsList(vecDimensionID.data(), nCount);
	m_nFrameCount = m_pImageAnimate->GetFrameCount(vecDimensionID.data());

	int nSize = m_pImageAnimate->GetPropertyItemSize(PropertyTagFrameDelay);
	m_vecPropertyItem.resize(nSize);
	m_pImageAnimate->GetPropertyItem(PropertyTagFrameDelay, nSize, m_vecPropertyItem.data());

	//info
	BITMAP BmpInfo = {};
	GUID pageGuid = Gdiplus::FrameDimensionTime;
	m_pImageAnimate->SelectActiveFrame(&pageGuid, 0);
	m_pImageAnimate->GetHBITMAP(Color(0, 0, 0, 0), &m_hBitmap);
	GetObject(m_hBitmap, sizeof(BmpInfo), &BmpInfo);
	m_pBits = (LPBYTE)BmpInfo.bmBits;
	m_nWidth = m_pImageAnimate->GetWidth();
	m_nHeight = m_pImageAnimate->GetHeight();

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

	m_hBitmap = hBitmap;
	m_pBits = pDest;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_bAlpha = bAlphaChannel;

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