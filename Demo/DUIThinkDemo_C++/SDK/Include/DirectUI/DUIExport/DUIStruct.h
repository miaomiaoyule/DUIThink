#ifndef __DUISTRUCT_H__
#define __DUISTRUCT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
struct tagDuiFile
{
	enDuiType							DuiType;
	CMMString							strName;
	CMMString							strFile;
};

typedef std::vector<tagDuiFile> VecDuiFile;

//////////////////////////////////////////////////////////////////////////
struct tagDuiTextStyle
{
	std::vector<CMMString>				vecFontResSwitch;
	std::vector<CMMString>				vecColorResSwitch;
	DWORD								dwTextStyle = DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS;

	uint32_t GetID() const;
	bool IsWordBreak()
	{
		return 0 != (dwTextStyle & DT_WORDBREAK);
	}
	bool IsEndEllipsis()
	{
		return 0 != (dwTextStyle & DT_END_ELLIPSIS);
	}
	bool operator == (const tagDuiTextStyle &Right) const
	{
		return dwTextStyle == Right.dwTextStyle
			&& vecFontResSwitch == Right.vecFontResSwitch
			&& vecColorResSwitch == Right.vecColorResSwitch;
	}
};

template<> struct hash<tagDuiTextStyle>
{
	inline uint32_t operator()(const tagDuiTextStyle &TextStyle) const noexcept
	{
		CMMString strInfo;
		for (auto &strFontRes : TextStyle.vecFontResSwitch)
		{
			strInfo += strFontRes;
		}
		for (auto &strColorRes : TextStyle.vecColorResSwitch)
		{
			strInfo += strColorRes;
		}

		strInfo += CMMStrHelp::Format(_T("-%d"), (int)TextStyle.dwTextStyle);

		return CMMHash::GetHash(strInfo);
	}
};

inline uint32_t tagDuiTextStyle::GetID() const
{
	return hash<tagDuiTextStyle>()(*this);
}

typedef std::vector<tagDuiTextStyle> VecDuiTextStyle;

//////////////////////////////////////////////////////////////////////////
struct tagDuiRichTextBase
{
	//type
	enDuiRichTextItemType				ItemType = RichTextItem_Text;

	//text
	CMMString							strText;

	//image
	CMMString							strImageResName;
};

struct tagDuiRichTextItem : public tagDuiRichTextBase
{
	//text
	std::vector<CMMString>				vecFontResSwitch;
	std::vector<CMMString>				vecColorResSwitch;

	//image
	int									nAnimateFrameCur = 0;
	int									nAnimateElapseCur = 0;

	tagDuiRichTextItem()
	{
	}
	tagDuiRichTextItem(const tagDuiRichTextBase &Copy)
	{
		operator = (Copy);

		return;
	}
	bool operator == (const tagDuiRichTextItem &Right) const
	{
		return ItemType == Right.ItemType
			&& strText == Right.strText
			&& strImageResName == Right.strImageResName
			&& vecFontResSwitch == Right.vecFontResSwitch
			&& vecColorResSwitch == Right.vecColorResSwitch
			&& nAnimateFrameCur == Right.nAnimateFrameCur
			&& nAnimateElapseCur == Right.nAnimateElapseCur;
	}
	tagDuiRichTextItem & operator = (const tagDuiRichTextBase &Right)
	{
		ItemType = Right.ItemType;
		strText = Right.strText;
		strImageResName = Right.strImageResName;

		return *this;
	}
	bool IsEmpty()
	{
		return (RichTextItem_Text == ItemType && strText.IsEmpty())
			|| (RichTextItem_Image == ItemType && strImageResName.IsEmpty());
	}
};

typedef std::vector<tagDuiRichTextItem> VecDuiRichTextItem;

struct tagDuiRichText
{
	VecDuiRichTextItem					vecRichTextItem;
	DWORD								dwTextStyle = DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS;
	int									nLineLimit = -1;

	uint32_t GetID() const;
	bool IsWordBreak()
	{
		return 0 != (dwTextStyle & DT_WORDBREAK);
	}
	bool IsEndEllipsis()
	{
		return 0 != (dwTextStyle & DT_END_ELLIPSIS);
	}
	operator DWORD() const
	{
		return dwTextStyle;
	}
	bool operator == (const tagDuiRichText &Right) const
	{
		return (DWORD)(*this) == Right.operator DWORD()
			&& nLineLimit == Right.nLineLimit
			&& vecRichTextItem == Right.vecRichTextItem;
	}
};

template<> struct hash<tagDuiRichText>
{
	inline uint32_t operator()(const tagDuiRichText &RichText) const noexcept
	{
		CMMString strInfo;
		for (auto &RichTextItem : RichText.vecRichTextItem)
		{
			for (auto &strFontRes : RichTextItem.vecFontResSwitch)
			{
				strInfo += strFontRes;
			}
			for (auto &strColorRes : RichTextItem.vecColorResSwitch)
			{
				strInfo += strColorRes;
			}

			strInfo += CMMStrHelp::Format(_T("-%d-%s-%s"), (int)RichTextItem.ItemType, (LPCTSTR)RichTextItem.strText, (LPCTSTR)RichTextItem.strImageResName);
		}

		strInfo += CMMStrHelp::Format(_T("-%d-%d"), (int)RichText.dwTextStyle, RichText.nLineLimit);

		return CMMHash::GetHash(strInfo);
	}
};

inline uint32_t tagDuiRichText::GetID() const
{
	return hash<tagDuiRichText>()(*this);
}

struct tagDuiRichTextDraw : public tagDuiRichTextItem
{
	//target
	CDUIRect							rcDraw;
	int									nLine = 0;

	//text
	HFONT								hFont = NULL;
	ARGB								dwColor = 0;
};

typedef std::vector<tagDuiRichTextBase> VecDuiRichTextBase;
typedef std::vector<tagDuiRichText> VecDuiRichText;
typedef std::vector<tagDuiRichTextDraw> VecDuiRichTextDraw;
typedef std::map<int, VecDuiRichTextDraw> MapLineVecDuiRichTextDraw;

//////////////////////////////////////////////////////////////////////////
struct tagDuiAnimateImageInfo
{
	enDuiAnimateImageType				AnimateImageType = AnimateImage_None;
	int									nFrameCount = 0;
	int									nSequenceFrameSpeed = 0;
	CDUISize							szSequenceFrameSize;
	std::vector<Gdiplus::PropertyItem>	vecGifPropertyItem;
};

typedef std::vector<tagDuiAnimateImageInfo> VecDuiAnimateImageInfo;

//////////////////////////////////////////////////////////////////////////
struct tagDuiImageSection
{
	//image
	std::vector<CMMString>				vecImageResSwitch;

	//dest
	enDuiHorizImageAlignType			HorizImageAlign = HorizImageAlign_None;
	enDuiVertImageAlignType				VertImageAlign = VertImageAlign_None;
	short								nHorizOffset = 0;
	short								nVertOffset = 0;

	//source
	enDuiImageSourceType				ImageSourceType = ImageSource_Normal;
	bool								bPartVert = false;
	BYTE								cbPartAll = 1;
	BYTE								cbPartSel = 1;
	std::unordered_map<WORD, CDUIRect>	mapSourceCustomScale;

	BYTE								cbAlpha = 255;
	ARGB								dwMask = 0;

	CDUIRect							rcCorner;
	bool								bHole = false;
	bool								bTiledX = false;
	bool								bTiledY = false;

	uint32_t GetID() const;
	bool operator == (const tagDuiImageSection &Right) const
	{
		return vecImageResSwitch == Right.vecImageResSwitch
			&& HorizImageAlign == Right.HorizImageAlign
			&& VertImageAlign == Right.VertImageAlign
			&& nHorizOffset == Right.nHorizOffset
			&& nVertOffset == Right.nVertOffset
			&& ImageSourceType == Right.ImageSourceType
			&& bPartVert == Right.bPartVert
			&& cbPartAll == Right.cbPartAll
			&& cbPartSel == Right.cbPartSel
			&& mapSourceCustomScale == Right.mapSourceCustomScale
			&& cbAlpha == Right.cbAlpha
			&& dwMask == Right.dwMask
			&& rcCorner == Right.rcCorner
			&& bHole == Right.bHole
			&& bTiledX == Right.bTiledX
			&& bTiledY == Right.bTiledY;
	}
};

template<> struct hash<tagDuiImageSection>
{
	inline uint32_t operator()(const tagDuiImageSection &ImageSection) const noexcept
	{
		CMMString strInfo;
		for (auto &strImageRes : ImageSection.vecImageResSwitch)
		{
			strInfo += strImageRes;
		}

		strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d-%d-%d-%d-%d"), 
			ImageSection.HorizImageAlign, 
			ImageSection.VertImageAlign,
			(int)ImageSection.nHorizOffset, 
			(int)ImageSection.nVertOffset, 
			ImageSection.ImageSourceType, 
			ImageSection.bPartVert, 
			(int)ImageSection.cbPartAll,
			(int)ImageSection.cbPartSel);

		for (auto &SourceCustomScale : ImageSection.mapSourceCustomScale)
		{
			strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d"), SourceCustomScale.first, SourceCustomScale.second.left,
				SourceCustomScale.second.top, SourceCustomScale.second.right, SourceCustomScale.second.bottom);
		}

		strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d-%d-%d-%d-%d-%d"), (int)ImageSection.cbAlpha, (int)ImageSection.dwMask,
			(int)ImageSection.rcCorner.left, (int)ImageSection.rcCorner.top, (int)ImageSection.rcCorner.right, (int)ImageSection.rcCorner.bottom,
			(int)ImageSection.bHole, (int)ImageSection.bTiledX, (int)ImageSection.bTiledY);

		return CMMHash::GetHash(strInfo);
	}
};

inline uint32_t tagDuiImageSection::GetID() const
{
	return hash<tagDuiImageSection>()(*this);
}

//////////////////////////////////////////////////////////////////////////
struct tagDuiComboxItem
{
	int									nItem = 0;
	CMMString							strDescribe;
};

typedef vector<tagDuiComboxItem> VecDuiComboxItem;

struct tagDuiCombox
{
	VecDuiComboxItem					vecItem;

	uint32_t GetID() const;
};

template<> struct hash<tagDuiCombox>
{
	inline uint32_t operator()(const tagDuiCombox &AttriCombox) const noexcept
	{
		CMMString strInfo;
		for (auto &Item : AttriCombox.vecItem)
		{
			strInfo += CMMStrHelp::Format(_T("%d"), Item.nItem);
			strInfo += Item.strDescribe;
		}

		return CMMHash::GetHash(strInfo);
	}
};

inline uint32_t tagDuiCombox::GetID() const
{
	return hash<tagDuiCombox>()(*this);
}

//////////////////////////////////////////////////////////////////////////
//horizstruct
struct tagHorizPosition
{
	enDuiHorizAlignType					HorizAlignType = HorizAlign_Left;
	int									nLeftAlignValue = 0;
	int									nRightAlignValue = 0;
	int									nFixedWidth = 0;
};

//vertstruct
struct tagVertPosition
{
	enDuiVertAlignType					VertAlignType = VertAlign_Top;
	int									nTopAlignValue = 0;
	int									nBottomAlignValue = 0;
	int									nFixedHeight = 0;
};

struct tagDuiPosition
{
	bool								bFloat = false;
	tagHorizPosition					HorizPosition = {};
	tagVertPosition						VertPosition = {};

	uint32_t GetID() const;
};

template<> struct hash<tagDuiPosition>
{
	inline uint32_t operator()(const tagDuiPosition &Position) const noexcept
	{
		CMMString strInfo;
		strInfo += CMMStrHelp::Format(_T("%d"), Position.bFloat);
		strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d"), Position.HorizPosition.HorizAlignType, Position.HorizPosition.nLeftAlignValue,
			Position.HorizPosition.nRightAlignValue, Position.HorizPosition.nFixedWidth);
		strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d"), Position.VertPosition.VertAlignType, Position.VertPosition.nTopAlignValue,
			Position.VertPosition.nBottomAlignValue, Position.VertPosition.nFixedHeight);

		return CMMHash::GetHash(strInfo);
	}
};

inline uint32_t tagDuiPosition::GetID() const
{
	return hash<tagDuiPosition>()(*this);
}

//////////////////////////////////////////////////////////////////////////
struct tagDuiListInfo
{
	CDUIAttributeTextStyle *			pAttributeItemTextStyleNormal = NULL;
	CDUIAttributeTextStyle *			pAttributeItemTextStyleHot = NULL;
	CDUIAttributeTextStyle *			pAttributeItemTextStyleSelNormal = NULL;
	CDUIAttributeTextStyle *			pAttributeItemTextStyleSelHot = NULL;
	CDUIAttributeTextStyle *			pAttributeItemTextStyleDisabled = NULL;

	//item status
	CDUIAttributeColorSwitch *			pAttributeItemStatusColorNormal = NULL;
	CDUIAttributeColorSwitch *			pAttributeItemStatusColorHot = NULL;
	CDUIAttributeColorSwitch *			pAttributeItemStatusColorSelNormal = NULL;
	CDUIAttributeColorSwitch *			pAttributeItemStatusColorSelHot = NULL;
	CDUIAttributeColorSwitch *			pAttributeItemStatusColorDisabled = NULL;

	CDUIAttriImageSection *				pAttributeItemStatusImageNormal = NULL;
	CDUIAttriImageSection *				pAttributeItemStatusImageHot = NULL;
	CDUIAttriImageSection *				pAttributeItemStatusImageSelNormal = NULL;
	CDUIAttriImageSection *				pAttributeItemStatusImageSelHot = NULL;
	CDUIAttriImageSection *				pAttributeItemStatusImageDisabled = NULL;

	CDUIAttributeColorSwitch *			pAttributeColorLine = NULL;
};

//////////////////////////////////////////////////////////////////////////
struct tagDuiRipple
{
	CDUISize							szBmpBuffer;
	std::vector<int8_t>					vecBufferRipple1;
	std::vector<int8_t>					vecBufferRipple2;
	bool								bHaveRipple = false;

	bool operator == (const tagDuiRipple &Right)
	{
		return Right.szBmpBuffer == szBmpBuffer
			&& Right.vecBufferRipple1 == vecBufferRipple1
			&& Right.vecBufferRipple2 == vecBufferRipple2;
	}
};

struct tagDuiRippleBitmap
{
	tagDuiRipple						Ripple;
	std::vector<ARGB>					vecBmpSwap;
	std::vector<ARGB>					vecBmpDest;
	HBITMAP								hBitmapSwap = NULL;
};

//////////////////////////////////////////////////////////////////////////
struct tagDuiThinkEditHistory
{
	bool								bAdd = false;
	enDuiRichTextItemType				ItemType = RichTextItem_Text;
	int									nCaretRowPre = -1;
	int									nCaretColumnPre = -1;
	int									nCaretRowNext = -1;
	int									nCaretColumnNext = -1;
	CMMString							strText;
	CMMString							strImageResName;
};

typedef std::vector<tagDuiThinkEditHistory> VecDuiThinkEditHistory;
typedef std::deque<tagDuiThinkEditHistory> QueDuiThinkEditHistory;

//////////////////////////////////////////////////////////////////////////
struct tagDuiDropData
{
	DWORD								dwKeyState = 0;
	DWORD								dwEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
	CMMDragDrop *						pDragDropFrom = NULL;
	IDataObject *						pIDataObject = nullptr;
	CDUIPoint							ptMouse;
	bool								bHasGUID = false;
	std::vector<CMMString>				vecDropFiles;
	std::vector<PCIDLIST_ABSOLUTE>		vecPIDL;

	//CFSTR_FILENAMEMAP: List of file names when renaming with HDROP
	//解析FileNameMapW结构，兼容从回收站拖出，该结构存回收站中文件对应正确文件名
	std::vector<CMMString>				vecFileNameMap;

	//IDataObject对象是否支持异步操作,这里默认设为true
	//大部分shell数据操作会在线程中异步执行
	bool								bAsyncMode = true;
	WPARAM								wParam = 0;
	LPARAM								lParam = 0;
};

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<CDUIWndManager*, tagDuiFile> MapWndManager;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
