#include "StdAfx.h"
#include "DUIAttriImageSection.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttriImageSection)
MMImplement_ClassName(CDUIAttriImageSection)

CDUIAttriImageSection::CDUIAttriImageSection()
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_ImageSection;
#endif

	return;
}

CDUIAttriImageSection::~CDUIAttriImageSection(void)
{
	return;
}

LPVOID CDUIAttriImageSection::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttriImageSection, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

void CDUIAttriImageSection::Draw(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint, bool bDisablePallete, const CDUIRect &rcRound, enDuiRoundType RoundType)
{
	CDUIImageBase *pImageBaseCur = GetCurImageBase();
	if (NULL == pImageBaseCur || NULL == m_pOwner) return;

	//info
	CDUIWnd *pWnd = GetOwnerWnd();
	if (NULL == pWnd) return;

	tagDuiImageSection ImageSection = GetImageSection();

	Draw(pImageBaseCur->GetHandle(GetScale()), pWnd->IsGdiplusRenderImage() ? pImageBaseCur->GetBitmap(GetScale()) : NULL, ImageSection, hDC, rcItem, rcPaint, bDisablePallete, pImageBaseCur->IsScale(GetScale()), pImageBaseCur->IsAlpha(), rcRound, RoundType);

	return;
}

void CDUIAttriImageSection::Draw(HBITMAP hBitmap, Gdiplus::Bitmap *pBitmap, const tagDuiImageSection &ImageSection, HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint, bool bDisablePallete, bool bScale, bool bAlpha, const CDUIRect &rcRound, enDuiRoundType RoundType)
{
	if (NULL == hBitmap && NULL == pBitmap) return;

	CDUIWnd *pWnd = GetOwnerWnd();
	if (NULL == pWnd) return;

	HBITMAP hBmpPaint = hBitmap;
	Gdiplus::Bitmap *pBmpPaint = pBitmap;

	//source
	CDUIRect rcSource = GetSource(ImageSection);

	//dest
	CDUIRect rcDest = GetDest(rcSource, rcItem);
	rcDest.Offset(rcItem.left, rcItem.top);
	if (false == IntersectRect(&CDUIRect(), &rcDest, &rcItem)) return;

	//corner
	CDUIRect rcCorner = bScale ? DuiDpiScaleAttri(ImageSection.rcCorner) : ImageSection.rcCorner;

	//mask
	if (ImageSection.dwMask > 0x00ffffff || bDisablePallete || 255 != ImageSection.cbAlpha)
	{
		hBmpPaint = CDUIRenderEngine::CopyBitmap(hBitmap, ImageSection.dwMask, bDisablePallete, ImageSection.cbAlpha * 1.0 / 255);
	}
	if ((pWnd->IsGdiplusRenderImage() && NULL == pBmpPaint)
		|| (hBmpPaint != hBitmap))
	{
		pBmpPaint = CDUIRenderEngine::GetAlphaBitmap(hBmpPaint);
	}
	do
	{
		//verify
		if (rcDest.Empty()) return;

		//draw
		if (pBmpPaint)
		{
			CDUIRenderEngine::DrawImage(hDC, pBmpPaint, rcDest, rcPaint, rcSource, 
				rcCorner, ImageSection.bCornerHole,
				HorizImageAlign_Tile == ImageSection.HorizImageAlign, VertImageAlign_Tile == ImageSection.VertImageAlign, rcRound, RoundType);
		}
		else
		{
			CDUIRenderEngine::DrawImage(hDC, hBmpPaint, rcDest, rcPaint, rcSource,
				rcCorner, ImageSection.cbAlpha, bAlpha, ImageSection.bCornerHole,
				HorizImageAlign_Tile == ImageSection.HorizImageAlign, VertImageAlign_Tile == ImageSection.VertImageAlign, rcRound, RoundType);
		}

	} while (false);

	if (hBmpPaint != hBitmap)
	{
		MMSafeDeleteObject(hBmpPaint);
	}
	if (pBmpPaint != pBitmap)
	{
		MMSafeDelete(pBmpPaint);
	}

	return;
}

void CDUIAttriImageSection::DrawParallelogram(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint, bool bDisablePallete)
{
	return Draw(hDC, rcItem, rcPaint, bDisablePallete, {}, Round_Parallelogram);
}

void CDUIAttriImageSection::DrawRhomb(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint, bool bDisablePallete)
{
	return Draw(hDC, rcItem, rcPaint, bDisablePallete, {}, Round_Rhomb);
}

void CDUIAttriImageSection::DrawEllipse(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint, bool bDisablePallete)
{
	return Draw(hDC, rcItem, rcPaint, bDisablePallete, {}, Round_Ellipse);
}

void CDUIAttriImageSection::DrawAnimate(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint, const tagDuiAnimateImageInfo &AnimateImageInfo, int nFrameCur, const CDUIRect &rcRound)
{
	CDUIImageBase *pImageBaseCur = GetCurImageBase();
	if (NULL == pImageBaseCur || NULL == pImageBaseCur->GetHandle(GetScale())) return;

	CDUIWnd *pWnd = GetOwnerWnd();
	if (NULL == pWnd) return;

	//dest
	CDUIRect rcSource = { 0, 0, pImageBaseCur->GetWidth(GetScale()), pImageBaseCur->GetHeight(GetScale()) };
	CDUIRect rcDest = GetDest(rcSource, rcItem);
	rcDest.Offset(rcItem.left, rcItem.top);
	IntersectRect(&rcDest, &rcDest, &rcItem);

	//gif
	if (AnimateImage_Gif == AnimateImageInfo.AnimateImageType)
	{
		Gdiplus::Bitmap *pBmpAnimate = pImageBaseCur->GetAnimateImage(GetScale());
		if (NULL == pBmpAnimate) return;

		GUID pageGuid = Gdiplus::FrameDimensionTime;
		pBmpAnimate->SelectActiveFrame(&pageGuid, nFrameCur);
		CDUIRenderEngine::DrawImage(hDC, pBmpAnimate, rcDest, rcRound);

		return;
	}

	//webp
	if (AnimateImage_Webp == AnimateImageInfo.AnimateImageType)
	{
		tagDuiImageSection ImageSection = GetImageSection();
		tagDuiImageInfo ImageInfo = pImageBaseCur->GetImageInfo(GetScale());
		if (nFrameCur >= ImageInfo.vecImageWebp.size()) return;

		Draw(NULL, ImageInfo.vecImageWebp[nFrameCur], ImageSection, hDC, rcItem, rcPaint, false, pImageBaseCur->IsScale(GetScale()), pImageBaseCur->IsAlpha(), rcRound);

		return;
	}

	//sequence frame
	CDUISize szSequenceFrameSize = AnimateImageInfo.szSequenceFrameSize;
	if (AnimateImage_SequenceFrame == AnimateImageInfo.AnimateImageType
		&& szSequenceFrameSize.cx > 0
		&& szSequenceFrameSize.cy > 0)
	{
		tagDuiImageSection ImageSection = GetImageSection();

		int nRow = nFrameCur / (pImageBaseCur->GetWidth(GetScale()) / szSequenceFrameSize.cx);
		int nColumn = nFrameCur % (pImageBaseCur->GetWidth(GetScale()) / szSequenceFrameSize.cx);
		CDUIRect rcSource;
		rcSource.left = szSequenceFrameSize.cx * nColumn;
		rcSource.top = szSequenceFrameSize.cy * nRow;
		rcSource.right = rcSource.left + szSequenceFrameSize.cx;
		rcSource.bottom = rcSource.top + szSequenceFrameSize.cy;
		ImageSection.ImageSourceType = ImageSource_Custom;
		ImageSection.mapSourceCustomScale[100] = rcSource;

		Draw(pImageBaseCur->GetHandle(GetScale()), pWnd->IsGdiplusRenderImage() ? pImageBaseCur->GetBitmap(GetScale()) : NULL, ImageSection, hDC, rcItem, rcPaint, false, pImageBaseCur->IsScale(GetScale()), pImageBaseCur->IsAlpha(), rcRound);
	}

	return;
}

bool CDUIAttriImageSection::IsEmpty()
{
	return GetImageSection().vecImageResSwitch.empty();
}

tagDuiImageSection CDUIAttriImageSection::GetImageSection()
{
	return CDUIGlobal::GetInstance()->GetAttriImageSection(m_uValueHash);
}

void CDUIAttriImageSection::SetImageSection(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSection()) return;

	m_uValueHash = CDUIGlobal::GetInstance()->SetAttriImageSection(ImageSection);

	NotifyChange();

	return;
}

CDUIImageBase * CDUIAttriImageSection::GetCurImageBase()
{
	tagDuiImageSection ImageSection = GetImageSection();
	int nIndexRes = CDUIGlobal::GetInstance()->GetSwitchResIndex();
	nIndexRes = min(nIndexRes, (int)ImageSection.vecImageResSwitch.size() - 1);
	if (nIndexRes < ImageSection.vecImageResSwitch.size())
	{
		return CDUIGlobal::GetInstance()->GetImageResource(ImageSection.vecImageResSwitch[nIndexRes]);
	}

	return NULL;
}

int CDUIAttriImageSection::GetCurImageWidth()
{
	CDUIImageBase *pImageBase = GetCurImageBase();

	return pImageBase ? pImageBase->GetWidth(GetScale()) : 0;
}

int CDUIAttriImageSection::GetCurImageHeight()
{
	CDUIImageBase *pImageBase = GetCurImageBase();

	return pImageBase ? pImageBase->GetHeight(GetScale()) : 0;
}

void CDUIAttriImageSection::SetImageResSwitch(const vector<CMMString> &vecImageResSwitch)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.vecImageResSwitch = vecImageResSwitch;
	SetImageSection(ImageSection);

	NotifyChange();

	return;
}

void CDUIAttriImageSection::SetImageResSwitch(const CMMString &strImageResSwitch)
{
	return SetImageResSwitch(CMMStrHelp::ParseStrFromString(strImageResSwitch, _T(";")));
}

vector<CMMString> CDUIAttriImageSection::GetImageResSwitch()
{
	tagDuiImageSection ImageSection = GetImageSection();

	return ImageSection.vecImageResSwitch;
}

LPCTSTR CDUIAttriImageSection::GetImageResName()
{
	CDUIImageBase *pImageBaseCur = GetCurImageBase();
	return pImageBaseCur ? pImageBaseCur->GetResourceName() : _T("");
}

enDuiHorizImageAlignType CDUIAttriImageSection::GetHorizImageAlignType()
{
	tagDuiImageSection ImageSection = GetImageSection();

	return ImageSection.HorizImageAlign;
}

void CDUIAttriImageSection::SetHorizImageAlignType(enDuiHorizImageAlignType ImageAlignType)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.HorizImageAlign = ImageAlignType;
	SetImageSection(ImageSection);

	NotifyChange();

	return;
}

enDuiVertImageAlignType CDUIAttriImageSection::GetVertImageAlignType()
{
	tagDuiImageSection ImageSection = GetImageSection();

	return ImageSection.VertImageAlign;
}

void CDUIAttriImageSection::SetVertImageAlignType(enDuiVertImageAlignType ImageAlignType)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.VertImageAlign = ImageAlignType;
	SetImageSection(ImageSection);

	NotifyChange();

	return;
}

CDUIRect CDUIAttriImageSection::GetSource()
{
	return GetSource(GetImageSection());
}

CDUIRect CDUIAttriImageSection::GetSource(tagDuiImageSection ImageSection)
{
	CDUIImageBase *pImageBase = NULL;
	int nIndexRes = CDUIGlobal::GetInstance()->GetSwitchResIndex();
	nIndexRes = min(nIndexRes, (int)ImageSection.vecImageResSwitch.size() - 1);
	if (nIndexRes < ImageSection.vecImageResSwitch.size())
	{
		pImageBase = CDUIGlobal::GetInstance()->GetImageResource(ImageSection.vecImageResSwitch[nIndexRes]);
	}
	if (NULL == pImageBase)
	{
		return {};
	}

	CDUIRect rcSource;
	switch (ImageSection.ImageSourceType)
	{
		case ImageSource_Normal:
		{
			rcSource.right = pImageBase->GetWidth(GetScale());
			rcSource.bottom = pImageBase->GetHeight(GetScale());

			break;
		}
		case ImageSource_Part:
		{
			if (ImageSection.cbPartAll <= 0)
			{
				assert(false);
				break;
			}

			int nWidth = 0, nHeight = 0;
			if (false == ImageSection.bPartVert)
			{
				nWidth = pImageBase->GetWidth(GetScale()) / ImageSection.cbPartAll;
				nHeight = pImageBase->GetHeight(GetScale());
				rcSource.right = nWidth * ImageSection.cbPartSel;
				rcSource.bottom = nHeight;
			}
			else
			{
				nHeight = pImageBase->GetHeight(GetScale()) / ImageSection.cbPartAll;
				nWidth = pImageBase->GetWidth(GetScale());
				rcSource.bottom = nHeight * ImageSection.cbPartSel;
				rcSource.right = nWidth;
			}

			rcSource.left = rcSource.right - nWidth;
			rcSource.top = rcSource.bottom - nHeight;

			break;
		}
		case ImageSource_Custom:
		{
			if (ImageSection.mapSourceCustomScale.find(GetScale()) != ImageSection.mapSourceCustomScale.end())
			{
				rcSource = ImageSection.mapSourceCustomScale[GetScale()];
			}
			else if (ImageSection.mapSourceCustomScale.find(100) != ImageSection.mapSourceCustomScale.end())
			{
				rcSource = ImageSection.mapSourceCustomScale[100];
			}
			else
			{
				rcSource = CDUIRect(0, 0, pImageBase->GetWidth(GetScale()), pImageBase->GetHeight(GetScale()));
			}

			break;
		}
	}

	return rcSource;
}

void CDUIAttriImageSection::SetSourceNormal()
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.ImageSourceType = ImageSource_Normal;

	SetImageSection(ImageSection);

	return;
}

void CDUIAttriImageSection::SetSourcePart(BYTE cbPartAll, BYTE cbPartSel)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.ImageSourceType = ImageSource_Part;
	ImageSection.cbPartAll = cbPartAll;
	ImageSection.cbPartSel = cbPartSel;

	SetImageSection(ImageSection);

	return;
}

void CDUIAttriImageSection::SetSourceCustom(const CDUIRect &rcSource)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.ImageSourceType = ImageSource_Custom;
	ImageSection.mapSourceCustomScale[100] = rcSource;

	SetImageSection(ImageSection);

	return;
}

BYTE CDUIAttriImageSection::GetAlpha()
{
	tagDuiImageSection ImageSection = GetImageSection();

	return ImageSection.cbAlpha;
}

void CDUIAttriImageSection::SetAlpha(BYTE cbAlpha)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.cbAlpha = cbAlpha;
	SetImageSection(ImageSection);

	NotifyChange();

	return;
}

ARGB CDUIAttriImageSection::GetMask()
{
	tagDuiImageSection ImageSection = GetImageSection();

	return ImageSection.dwMask;
}

void CDUIAttriImageSection::SetMask(ARGB dwMask)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.dwMask = dwMask;
	SetImageSection(ImageSection);

	NotifyChange();

	return;
}

CDUIRect CDUIAttriImageSection::GetCorner()
{
	tagDuiImageSection ImageSection = GetImageSection();

	return ImageSection.rcCorner;
}

void CDUIAttriImageSection::SetCorner(const CDUIRect &rcCorner)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.rcCorner = rcCorner;
	SetImageSection(ImageSection);

	NotifyChange();

	return;
}

bool CDUIAttriImageSection::IsCornerHole()
{
	tagDuiImageSection ImageSection = GetImageSection();

	return ImageSection.bCornerHole;
}

void CDUIAttriImageSection::SetCornerHole(bool bHole)
{
	tagDuiImageSection ImageSection = GetImageSection();
	ImageSection.bCornerHole = bHole;
	SetImageSection(ImageSection);

	NotifyChange();

	return;
}

bool CDUIAttriImageSection::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriObjValueID))
	{
		m_uValueHash = strtoul(lpszValue, NULL, 10);

#ifdef DUI_DESIGN
		CDUIGlobal::GetInstance()->OnAttriValueIDRead(GetAttributeType(), m_uValueHash);
#endif

		return true;
	}

	return false;
}

uint32_t CDUIAttriImageSection::GetValueID()
{
	return m_uValueHash;
}

void CDUIAttriImageSection::SetValueID(uint32_t uValueID)
{
	m_uValueHash = uValueID;

	return;
}

CDUIRect CDUIAttriImageSection::GetDest(CDUIRect rcSource, CDUIRect rcItem)
{
	tagDuiImageSection ImageSection = GetImageSection();
	CDUIRect rcDest(0, 0, rcItem.GetWidth(), rcItem.GetHeight());

	do
	{
		if (HorizImageAlign_None == ImageSection.HorizImageAlign && VertImageAlign_None == ImageSection.VertImageAlign)
		{
			break;
		}
		switch (ImageSection.HorizImageAlign)
		{
			case HorizImageAlign_Right:
			{
				rcDest.left = rcDest.right - rcSource.GetWidth();
				rcDest.Offset(-ImageSection.nHorizOffset, 0);

				break;
			}
			case HorizImageAlign_Center:
			{
				rcDest.left += (rcDest.GetWidth() - rcSource.GetWidth()) / 2;
				rcDest.left += ImageSection.nHorizOffset;
				rcDest.right = rcDest.left + rcSource.GetWidth();

				break;
			}
			case HorizImageAlign_Left:
			default:
			{
				rcDest.left += ImageSection.nHorizOffset;
				rcDest.right = rcDest.left + rcSource.GetWidth();

				break;
			}
		}
		switch (ImageSection.VertImageAlign)
		{
			case VertImageAlign_Bottom:
			{
				rcDest.top = rcDest.bottom - rcSource.GetHeight();
				rcDest.Offset(0, -ImageSection.nVertOffset);

				break;
			}
			case VertImageAlign_Center:
			{
				rcDest.top += (rcDest.GetHeight() - rcSource.GetHeight()) / 2;
				rcDest.top += ImageSection.nVertOffset;
				rcDest.bottom = rcDest.top + rcSource.GetHeight();

				break;
			}
			case VertImageAlign_Top:
			default:
			{
				rcDest.top += ImageSection.nVertOffset;
				rcDest.bottom = rcDest.top + rcSource.GetHeight();

				break;
			}
		}

	} while (false);

	return rcDest;
}