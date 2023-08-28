#ifndef __DUTILS_H__
#define __DUTILS_H__

#pragma once
#pragma pack(1)

/////////////////////////////////////////////////////////////////////////////////////
struct TITEM
{
	CMMString							Key;
	void *								Data;
	struct TITEM *						pPrev;
	struct TITEM *						pNext;
};

class DUITHINK_API CStdStringPtrMap
{
public:
	CStdStringPtrMap(int nSize = 83);
	~CStdStringPtrMap();

	void Resize(int nSize = 83);
	void * Find(LPCTSTR key, bool optimize = true) const;
	bool Insert(LPCTSTR key, void* pData);
	void * Set(LPCTSTR key, void* pData);
	bool Remove(LPCTSTR key);
	void RemoveAll();
	int GetSize() const;
	std::vector<CMMString> ExtractKey();
	LPCTSTR GetAt(int nIndex) const;
	LPCTSTR operator[] (int nIndex) const;

protected:
	TITEM **							m_aT;
	int									m_nBuckets;
	int									m_nCount;
};

/////////////////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIRenderHelp
{
public:
	static void MeasureRichText(IN HDC hDC, IN const tagDuiRichText &RichText, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, 
		OUT CDUIRect &rcMeasure, IN CDUIRect rcItem, IN bool bGdiplusRender, IN Gdiplus::TextRenderingHint RenderType, IN int nLineSpace);
	static void AdjustRichText(IN const tagDuiRichText &RichText, IN CDUIRect rcItem, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw,
		IN CDUIRect rcMeasure);
	static CDUIRect GetLineRange(MapLineVecDuiRichTextDraw mapLineVecRichTextDraw, int nLine);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif