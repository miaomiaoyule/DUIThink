#include "StdAfx.h"
#include "Utils.h"

/////////////////////////////////////////////////////////////////////////////
static UINT HashKey(LPCTSTR Key)
{
	UINT i = 0;
	SIZE_T len = _tcslen(Key);
	while (len-- > 0) i = (i << 5) + i + Key[len];
	return i;
}

static UINT HashKey(const CMMString& Key)
{
	return HashKey((LPCTSTR)Key);
};

CStdStringPtrMap::CStdStringPtrMap(int nSize) : m_nCount(0)
{
	if (nSize < 16) nSize = 16;
	m_nBuckets = nSize;
	m_aT = new TITEM*[nSize];
	memset(m_aT, 0, nSize * sizeof(TITEM*));
}

CStdStringPtrMap::~CStdStringPtrMap()
{
	if (m_aT) {
		int len = m_nBuckets;
		while (len--) {
			TITEM* pItem = m_aT[len];
			while (pItem) {
				TITEM* pKill = pItem;
				pItem = pItem->pNext;
				delete pKill;
			}
		}
		delete[] m_aT;
		m_aT = NULL;
	}
}

void CStdStringPtrMap::RemoveAll()
{
	this->Resize(m_nBuckets);
}

void CStdStringPtrMap::Resize(int nSize)
{
	if (m_aT) {
		int len = m_nBuckets;
		while (len--) {
			TITEM* pItem = m_aT[len];
			while (pItem) {
				TITEM* pKill = pItem;
				pItem = pItem->pNext;
				delete pKill;
			}
		}
		delete[] m_aT;
		m_aT = NULL;
	}

	if (nSize < 0) nSize = 0;
	if (nSize > 0) {
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}
	m_nBuckets = nSize;
	m_nCount = 0;
}

LPVOID CStdStringPtrMap::Find(LPCTSTR key, bool optimize) const
{
	if (m_nBuckets == 0 || GetSize() == 0) return NULL;

	UINT slot = HashKey(key) % m_nBuckets;
	for (TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext)
	{
		if (pItem->Key == key)
		{
			if (optimize && pItem != m_aT[slot])
			{
				if (pItem->pNext)
				{
					pItem->pNext->pPrev = pItem->pPrev;
				}
				pItem->pPrev->pNext = pItem->pNext;
				pItem->pPrev = NULL;
				pItem->pNext = m_aT[slot];
				pItem->pNext->pPrev = pItem;
				
				//将item移动至链条头部
				m_aT[slot] = pItem;
			}

			return pItem->Data;
		}
	}

	return NULL;
}

bool CStdStringPtrMap::Insert(LPCTSTR key, void* pData)
{
	if (m_nBuckets == 0) return false;
	if (Find(key)) return false;

	// Add first in bucket
	UINT slot = HashKey(key) % m_nBuckets;
	TITEM* pItem = new TITEM;
	pItem->Key = key;
	pItem->Data = pData;
	pItem->pPrev = NULL;
	pItem->pNext = m_aT[slot];
	if (pItem->pNext)
		pItem->pNext->pPrev = pItem;
	m_aT[slot] = pItem;
	m_nCount++;

	return true;
}

LPVOID CStdStringPtrMap::Set(LPCTSTR key, void* pData)
{
	if (m_nBuckets == 0) return pData;

	if (GetSize() > 0)
	{
		UINT slot = HashKey(key) % m_nBuckets;
		// Modify existing item
		for (TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext)
		{
			if (pItem->Key == key)
			{
				LPVOID pOldData = pItem->Data;
				pItem->Data = pData;
				return pOldData;
			}
		}
	}

	Insert(key, pData);
	return NULL;
}

bool CStdStringPtrMap::Remove(LPCTSTR key)
{
	if (m_nBuckets == 0 || GetSize() == 0 || MMInvalidString(key)) return false;

	UINT slot = HashKey(key) % m_nBuckets;
	TITEM** ppItem = &m_aT[slot];
	while (*ppItem)
	{
		if ((*ppItem)->Key == key)
		{
			TITEM* pKill = *ppItem;
			*ppItem = (*ppItem)->pNext;
			if (*ppItem)
			{
				(*ppItem)->pPrev = pKill->pPrev;
			}

			delete pKill;
			m_nCount--;

			return true;
		}

		ppItem = &((*ppItem)->pNext);
	}

	return false;
}

int CStdStringPtrMap::GetSize() const
{
#if 0//def _DEBUG
	int nCount = 0;
	int len = m_nBuckets;
	while (len--) {
		for (const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext) nCount++;
	}
	ASSERT(m_nCount == nCount);
#endif
	return m_nCount;
}

std::vector<CMMString> CStdStringPtrMap::ExtractKey()
{
	std::vector<CMMString> vecKey;
	for (int n = 0; n < GetSize(); n++)
	{
		vecKey.push_back(GetAt(n));
	}

	return vecKey;
}

LPCTSTR CStdStringPtrMap::GetAt(int nIndex) const
{
	if (m_nBuckets == 0 || GetSize() == 0) return NULL;

	int pos = 0;
	int len = m_nBuckets;
	while (len--)
	{
		for (TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext)
		{
			if (pos++ == nIndex)
			{
				return pItem->Key.GetBuffer(0);
			}
		}
	}

	return NULL;
}

LPCTSTR CStdStringPtrMap::operator[] (int nIndex) const
{
	return GetAt(nIndex);
}

/////////////////////////////////////////////////////////////////////////////////////
void CDUIRenderHelp::MeasureRichText(IN HDC hDC, IN const tagDuiRichText &RichText, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, 
	OUT CDUIRect &rcMeasure, IN CDUIRect rcItem, IN bool bGdiplusRender, IN Gdiplus::TextRenderingHint RenderType, IN int nLineSpace)
{
	//style
	DWORD dwTextStyle = RichText;
	bool bSingleLine = (dwTextStyle & DT_SINGLELINE) || 0 == (dwTextStyle & DT_WORDBREAK);
	bool bAlignRight = (dwTextStyle & DT_RIGHT);
	bool bAlignBottom = (dwTextStyle & DT_BOTTOM);
	DWORD dwTextStyleCalc = DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT;

	//measure
	CDUIRect rcMeasureLine = rcMeasure;
	int nLine = 0, nIndex = 0;
	bool bMeasureFinish = false;

	mapLineVecRichTextDraw.clear();
	tagDuiRichTextDraw RichTextDraw;
	do
	{
		if (false == bMeasureFinish && RichTextDraw.IsEmpty())
		{
			if (nIndex >= RichText.vecRichTextItem.size())
			{
				bMeasureFinish = true;

				continue;
			}

			tagDuiRichTextItem RichTextItem = RichText.vecRichTextItem[nIndex++];
			RichTextDraw.ItemType = RichTextItem.ItemType;
			RichTextDraw.strText = RichTextItem.strText;
			RichTextDraw.strImageResName = RichTextItem.strImageResName;
			RichTextDraw.vecFontResSwitch = RichTextItem.vecFontResSwitch;
			RichTextDraw.vecColorResSwitch = RichTextItem.vecColorResSwitch;
			RichTextDraw.nAnimateElapseCur = RichTextItem.nAnimateElapseCur;
			RichTextDraw.nAnimateFrameCur = RichTextItem.nAnimateFrameCur;

			continue;
		}
		if (bMeasureFinish && RichTextDraw.IsEmpty())
		{
			break;
		}

		CDUIRect rcRichItem;

		//text
		if (RichTextItem_Text == RichTextDraw.ItemType)
		{
			//font
			CDUIFontBase *pFontBase = NULL;
			int nResIndex = CDUIGlobal::GetInstance()->GetSwitchResIndex();
			nResIndex = max(0, min(nResIndex, (int)RichTextDraw.vecFontResSwitch.size() - 1));
			if (nResIndex >= RichTextDraw.vecFontResSwitch.size())
			{
				pFontBase = CDUIGlobal::GetInstance()->GetFontResDefault();
			}
			else
			{
				pFontBase = CDUIGlobal::GetInstance()->GetFontResource(RichTextDraw.vecFontResSwitch[nResIndex]);
			}
			if (NULL == pFontBase || NULL == pFontBase->GetHandle())
			{
				RichTextDraw.strText.Empty();

				continue;
			}

			RichTextDraw.pFontBase = pFontBase;
			HFONT hFont = pFontBase->GetHandle();

			//color
			CDUIColorBase *pColorBase = NULL;
			nResIndex = CDUIGlobal::GetInstance()->GetSwitchResIndex();
			nResIndex = max(0, min(nResIndex, (int)RichTextDraw.vecColorResSwitch.size() - 1));
			if (nResIndex < RichTextDraw.vecColorResSwitch.size())
			{
				pColorBase = CDUIGlobal::GetInstance()->GetColorResource(RichTextDraw.vecColorResSwitch[nResIndex]);
			}

			NULL == pColorBase ? RichTextDraw.dwColor = 0xff000000 : RichTextDraw.dwColor = pColorBase->GetColor();

			//measure
			rcRichItem = { rcMeasureLine.right, rcMeasureLine.top, rcMeasureLine.right + 9999, rcMeasureLine.top + 9999 };
			CDUIRenderEngine::DrawText(hDC, hFont, rcRichItem, RichTextDraw.strText, 0, dwTextStyleCalc, bGdiplusRender, RenderType, false);

			//italic add 2 pixel
			if (pFontBase->IsItalic())
			{
				rcRichItem.right += 1;

				//single word plus
				if (1 == RichTextDraw.strText.GetLength())
				{
					rcRichItem.right += 1;
				}
			}
		}
		else if (RichTextItem_Image == RichTextDraw.ItemType)
		{
			rcRichItem = rcMeasureLine;

			//image
			CDUIImageBase *pImageBase = CDUIGlobal::GetInstance()->GetImageResource(RichTextDraw.strImageResName);
			if (NULL == pImageBase)
			{
				RichTextDraw.strImageResName.Empty();

				continue;
			}

			rcRichItem.right = rcRichItem.left + min(rcItem.GetWidth(), pImageBase->GetWidth());
			rcRichItem.bottom = rcRichItem.top + min(rcItem.GetHeight(), pImageBase->GetHeight());
		}

		//overright
		CMMString strSurplus;
		bool bOverRight = rcMeasureLine.right + rcRichItem.GetWidth() > rcItem.right;
		if (bOverRight)
		{
			rcRichItem = { rcMeasureLine.right, rcMeasureLine.top, rcMeasureLine.right, rcMeasureLine.top };

			//text
			if (RichTextItem_Text == RichTextDraw.ItemType)
			{
				for (int nChar = 0; nChar < RichTextDraw.strText.GetLength(); nChar++)
				{
					int nEmoji = CDUIGlobal::IsEmoji(RichTextDraw.strText[nChar]);
					CMMString strDraw(RichTextDraw.strText.GetBuffer() + nChar, max(1, nEmoji));
					nEmoji > 0 ? nChar += (nEmoji - 1) : 0;

					CDUIRect rcChar = { rcMeasureLine.right, rcMeasureLine.top, rcMeasureLine.right + 9999, rcMeasureLine.top + 9999 };
					HFONT hFont = RichTextDraw.pFontBase ? RichTextDraw.pFontBase->GetHandle() : NULL;
					CDUIRenderEngine::DrawText(hDC, hFont, rcChar, strDraw, 0, dwTextStyleCalc, bGdiplusRender, RenderType, false);

					//italic add 2 pixel
					if (RichTextDraw.pFontBase && RichTextDraw.pFontBase->IsItalic())
					{
						rcChar.right += 1;

						//single word plus
						if (1 == strDraw.GetLength())
						{
							rcChar.right += 1;
						}
					}
					if (rcRichItem.right + rcChar.GetWidth() <= rcItem.right)
					{
						bOverRight = false;
						rcRichItem.right += rcChar.GetWidth();
						rcRichItem.bottom = max(rcRichItem.bottom, rcChar.bottom);

						continue;
					}

					strSurplus = RichTextDraw.strText.GetBuffer() + nChar;
					RichTextDraw.strText.SetAt(nChar, _T('\0'));

					break;
				}
			}
			//image
			else if (RichTextItem_Image == RichTextDraw.ItemType)
			{
				strSurplus = RichTextDraw.strImageResName;
				RichTextDraw.strImageResName.Empty();
			}
		}
		
		//move
		rcRichItem.Offset(rcMeasureLine.right - rcRichItem.left, rcMeasureLine.top - rcRichItem.top);
		rcMeasureLine.bottom = max(rcMeasureLine.bottom, rcRichItem.bottom);
		rcMeasureLine.right += rcRichItem.GetWidth();

		//push item
		if ((RichTextItem_Text == RichTextDraw.ItemType && false == RichTextDraw.strText.IsEmpty())
			|| (RichTextItem_Image == RichTextDraw.ItemType && false == RichTextDraw.strImageResName.IsEmpty()))
		{
			RichTextDraw.rcDraw = rcRichItem;
			RichTextDraw.nLine = nLine;
			mapLineVecRichTextDraw[nLine].push_back(RichTextDraw);
		}

		//line finish
		bool bWordWrap = nIndex < RichText.vecRichTextItem.size()
			&& (_T("\n") == RichText.vecRichTextItem[nIndex].strText.Left(1) || _T("\r") == RichText.vecRichTextItem[nIndex].strText.Left(1));
		bool bLineFinish = (bOverRight || rcMeasureLine.right >= rcItem.right || bWordWrap);
		if (bLineFinish)
		{
			rcMeasure.right = max(rcMeasure.right, rcMeasureLine.right);
			rcMeasure.bottom = rcMeasureLine.bottom;

			if (false == bSingleLine)
			{
				nLine++;

				rcMeasureLine.right = rcMeasureLine.left;
				rcMeasureLine.top = rcMeasureLine.bottom;
				rcMeasureLine.bottom = rcMeasureLine.top;
				rcMeasureLine.Offset(0, nLineSpace);
			}
		}

		//measure finish
		false == bMeasureFinish ? bMeasureFinish = (-1 != RichText.nLineLimit && nLine >= RichText.nLineLimit) : bMeasureFinish;
		false == bMeasureFinish ? bMeasureFinish = (strSurplus.IsEmpty() && nIndex >= RichText.vecRichTextItem.size()) : bMeasureFinish;
		false == bMeasureFinish ? bMeasureFinish = (-1 == RichText.nLineLimit && rcMeasureLine.bottom > rcItem.bottom && false == bSingleLine && false == bAlignBottom) : bMeasureFinish;
		false == bMeasureFinish ? bMeasureFinish = (-1 == RichText.nLineLimit && bLineFinish && rcMeasureLine.bottom == rcItem.bottom && false == bSingleLine && false == bAlignBottom) : bMeasureFinish;
		false == bMeasureFinish ? bMeasureFinish = (bLineFinish && bSingleLine) : bMeasureFinish;
		false == bMeasureFinish ? bMeasureFinish = rcItem.IsEmpty() : bMeasureFinish;
		if (bMeasureFinish)
		{
			do 
			{
				if (mapLineVecRichTextDraw.empty()) break;

				//surplus
				auto &vecRichTextDraw = mapLineVecRichTextDraw[mapLineVecRichTextDraw.size() - 1];

				//right plus 
				tagDuiRichTextDraw &RichTextDraw = *vecRichTextDraw.rbegin();
				RichTextDraw.rcDraw.right += 1;
				RichTextDraw.rcDraw.right = min(RichTextDraw.rcDraw.right, rcItem.right);
				rcMeasure.right = max(rcMeasure.right, rcMeasureLine.right);
				rcMeasure.right = max(rcMeasure.right, RichTextDraw.rcDraw.right);
				rcMeasure.bottom = rcMeasureLine.bottom;

				bool bEndMore = false == strSurplus.IsEmpty();
				RichTextDraw.strText += strSurplus;
				for (int m = nIndex; m < RichText.vecRichTextItem.size(); m++)
				{
					false == bEndMore ? bEndMore = false == RichText.vecRichTextItem[m].strText.IsEmpty() : bEndMore;
					RichTextDraw.strText += RichText.vecRichTextItem[m].strText;
				}
				if (bEndMore && mapLineVecRichTextDraw.size() > 1)
				{
					for (int n = 0; n < vecRichTextDraw.size(); n++)
					{
						tagDuiRichTextDraw &RichTextDraw = vecRichTextDraw[n];
						RichTextDraw.rcDraw.Offset(1, 0);
					}
				}

				//adjust...
				if (bOverRight)
				{
					while (vecRichTextDraw.size() > 1)
					{
						tagDuiRichTextDraw &RichTextDrawLast = *vecRichTextDraw.rbegin();

						CDUIRect rcExtendCharEllipsis = { rcItem.left, rcItem.top, rcItem.left + 9999, rcItem.top + 9999 };
						CDUIRect rcCharEllipsis = { rcItem.left, rcItem.top, rcItem.left + 9999, rcItem.top + 9999 };
						CDUIRect rcEllipsis = { rcItem.left, rcItem.top, rcItem.left + 9999, rcItem.top + 9999 };

						//adjust ...
						HFONT hFont = RichTextDrawLast.pFontBase ? RichTextDrawLast.pFontBase->GetHandle() : NULL;
						CDUIRenderEngine::DrawText(hDC, hFont, rcEllipsis, _T("..."), 0, dwTextStyleCalc, bGdiplusRender, RenderType, false);
						if (RichTextDrawLast.rcDraw.GetWidth() <= rcEllipsis.GetWidth())
						{
							tagDuiRichTextDraw &RichTextDrawPre = *(++vecRichTextDraw.rbegin());
							RichTextDrawPre.strText += RichTextDrawLast.strText;
							RichTextDrawPre.rcDraw.right = RichTextDrawLast.rcDraw.right;
							vecRichTextDraw.erase(--vecRichTextDraw.rbegin().base());

							continue;
						}

						CMMString strCharEllipsis = _T("...");
						int nLen = 0;
						while (_T('\0') != RichTextDrawLast.strText[nLen])
						{
							strCharEllipsis = RichTextDrawLast.strText[nLen] + strCharEllipsis;

							if (_T(' ') != RichTextDrawLast.strText[nLen++]) break;
						}

						CDUIRenderEngine::DrawText(hDC, hFont, rcCharEllipsis, strCharEllipsis, 0, dwTextStyleCalc, bGdiplusRender, RenderType, false);
						if (RichTextDrawLast.rcDraw.GetWidth() >= rcEllipsis.GetWidth() && RichTextDrawLast.rcDraw.GetWidth() <= rcCharEllipsis.GetWidth())
						{
							RichTextDrawLast.strText = _T("...");

							break;
						}

						CMMString strExtendCharEllipsis = _T("T") + strCharEllipsis;
						CDUIRenderEngine::DrawText(hDC, hFont, rcExtendCharEllipsis, strExtendCharEllipsis, 0, dwTextStyleCalc, bGdiplusRender, RenderType, false);
						if (RichTextDrawLast.rcDraw.GetWidth() >= rcCharEllipsis.GetWidth() && RichTextDrawLast.rcDraw.GetWidth() <= rcExtendCharEllipsis.GetWidth())
						{
							RichTextDrawLast.strText = strCharEllipsis;

							break;
						}

						break;
					}
				}

			} while (false);

			break;
		}

		if (false == strSurplus.IsEmpty())
		{
			RichTextItem_Text == RichTextDraw.ItemType ? RichTextDraw.strText = strSurplus : RichTextDraw.strImageResName = strSurplus;

			continue;
		}

		RichTextDraw.strText.Empty();
		RichTextDraw.strImageResName.Empty();

	} while (true);

	return;
}

void CDUIRenderHelp::AdjustRichText(IN const tagDuiRichText &RichText, IN CDUIRect rcItem, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, 
	IN CDUIRect rcMeasure)
{
	DWORD dwTextStyle = RichText;

	//horiz adjust
	for (auto &LineVecRichTextDraw : mapLineVecRichTextDraw)
	{
		//same bottom
		{
			int nLineBottom = 0;
			for (int n = 0; n < LineVecRichTextDraw.second.size(); n++)
			{
				tagDuiRichTextDraw &RichTextDraw = LineVecRichTextDraw.second[n];
				0 == nLineBottom ? nLineBottom = RichTextDraw.rcDraw.bottom : nLineBottom;
				nLineBottom = max(nLineBottom, RichTextDraw.rcDraw.bottom);
			}
			for (int n = 0; n < LineVecRichTextDraw.second.size(); n++)
			{
				tagDuiRichTextDraw &RichTextDraw = LineVecRichTextDraw.second[n];
				RichTextDraw.rcDraw.Offset(0, nLineBottom - RichTextDraw.rcDraw.bottom);
			}
		}

		if (dwTextStyle & DT_CENTER)
		{
			int nWidthTotal = 0;
			for (int n = 0; n < LineVecRichTextDraw.second.size(); n++)
			{
				tagDuiRichTextDraw &RichTextDraw = LineVecRichTextDraw.second[n];
				nWidthTotal += RichTextDraw.rcDraw.GetWidth();
			}

			int nOffset = (rcItem.GetWidth() - nWidthTotal) / 2;
			for (int n = 0; n < LineVecRichTextDraw.second.size(); n++)
			{
				tagDuiRichTextDraw &RichTextDraw = LineVecRichTextDraw.second[n];
				RichTextDraw.rcDraw.Offset(nOffset, 0);
			}
		}
		if (dwTextStyle & DT_RIGHT)
		{
			int nRight = rcItem.right;
			for (int n = LineVecRichTextDraw.second.size() - 1; n >= 0; n--)
			{
				tagDuiRichTextDraw &RichTextDraw = LineVecRichTextDraw.second[n];
				RichTextDraw.rcDraw.Offset(nRight - RichTextDraw.rcDraw.right, 0);

				nRight -= RichTextDraw.rcDraw.GetWidth();
			}
		}
	}

	//vert adjust
	if (dwTextStyle & DT_VCENTER)
	{
		int nOffset = (rcItem.GetHeight() - rcMeasure.GetHeight()) / 2;
		int nTop = rcItem.top + nOffset;

		for (auto &LineVecRichTextDraw : mapLineVecRichTextDraw)
		{
			int nHeightMax = 0;
			for (int n = 0; n < LineVecRichTextDraw.second.size(); n++)
			{
				tagDuiRichTextDraw &RichTextDraw = LineVecRichTextDraw.second[n];
				nHeightMax = max(nHeightMax, RichTextDraw.rcDraw.GetHeight());
			}
			for (int n = 0; n < LineVecRichTextDraw.second.size(); n++)
			{
				tagDuiRichTextDraw &RichTextDraw = LineVecRichTextDraw.second[n];
				RichTextDraw.rcDraw.Offset(0, nTop + nHeightMax - RichTextDraw.rcDraw.bottom);
			}

			nTop += nHeightMax;
		}
	}
	if (dwTextStyle & DT_BOTTOM)
	{
		int nBottom = rcItem.bottom;

		for (auto LineIt = --mapLineVecRichTextDraw.end();; --LineIt)
		{
			int nHeightMax = 0;
			for (int n = LineIt->second.size() - 1; n >= 0; n--)
			{
				tagDuiRichTextDraw &RichTextDraw = LineIt->second[n];
				nHeightMax = max(nHeightMax, RichTextDraw.rcDraw.GetHeight());
				RichTextDraw.rcDraw.Offset(0, nBottom - RichTextDraw.rcDraw.bottom);
			}

			nBottom -= nHeightMax;

			if (LineIt == mapLineVecRichTextDraw.begin()) break;
		}
	}

	return;
}

CDUIRect CDUIRenderHelp::GetLineRange(MapLineVecDuiRichTextDraw mapLineVecRichTextDraw, int nLine)
{
	if (nLine < 0 || nLine >= mapLineVecRichTextDraw.size()) return {};

	CDUIRect rcRange;
	auto LineVecRichTextDraw = mapLineVecRichTextDraw[nLine];
	for (auto RichTextDrawItem : LineVecRichTextDraw)
	{
		if (rcRange.IsEmpty())
		{
			rcRange = RichTextDrawItem.rcDraw;
		}

		rcRange.top = min(rcRange.top, RichTextDrawItem.rcDraw.top);
		rcRange.bottom = max(rcRange.bottom, RichTextDrawItem.rcDraw.bottom);
		rcRange.right = RichTextDrawItem.rcDraw.right;
	}

	return rcRange;
}

//////////////////////////////////////////////////////////////////////////
