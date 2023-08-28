#ifndef __DUIDEFINEATTRIBUTE_H__
#define __DUIDEFINEATTRIBUTE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
//Position布局定义
#define Dui_Name_HorizAlignLeft						_T("HorizAlignLeft")
#define Dui_Name_HorizAlignRight					_T("HorizAlignRight")
#define Dui_Name_HorizAlignCenter					_T("HorizAlignCenter")
#define Dui_Name_VertAlignTop						_T("VertAlignTop")
#define Dui_Name_VertAlignBottom					_T("VertAlignBottom")
#define Dui_Name_VertAlignMiddle					_T("VertAlignMiddle")

////////////////////////////////////////////////////////////////////////////////////////////////////
//属性XML节点名称
#define Dui_Key_AttriObjName						("Name")
#define Dui_Key_AttriObjNameID						("NameID")
#define Dui_Key_AttriObjDescr						("Descr")
#define Dui_Key_AttriObjVisible						("Visible")
#define Dui_Key_AttriObjModified					("Modified")
#define Dui_Key_AttriObjValueID						("ValueID")

#define Dui_Key_AttriPosition						("Position")
#define Dui_Key_AttriPosFloat						("Float")
#define Dui_Key_AttriPosHorizAlignType				("HAlignType")
#define Dui_Key_AttriPosLeftAlignValue				("HLeftValue")
#define Dui_Key_AttriPosRightAlignValue				("HRightValue")
#define Dui_Key_AttriPosFixedWidth					("HFixedWidth")
#define Dui_Key_AttriPosVertAlignType				("VAlignType")
#define Dui_Key_AttriPosTopAlignValue				("VTopValue")
#define Dui_Key_AttriPosBottomAlignValue			("VBottomValue")
#define Dui_Key_AttriPosFixedHeight					("VFixedHeight")

#define Dui_Key_AttriColorValue						("Color")
#define Dui_Key_AttriColorRes						("ColorRes")

#define Dui_Key_AttriText							("Text")

#define Dui_Key_AttriTextStyleFontRes				("FontRes")
#define Dui_Key_AttriTextStyleColorRes				("ColorRes")
#define Dui_Key_AttriTextStyleStyle					("Style")

#define Dui_Key_AttriVecRichTextItem				("VecRichTextItem")
#define Dui_Key_AttriRichTextItem					("RichTextItem")
#define Dui_Key_AttriRichTextItemFontRes			("FontRes")
#define Dui_Key_AttriRichTextItemColorRes			("ColorRes")
#define Dui_Key_AttriRichTextItemText				("Text")
#define Dui_Key_AttriRichTextStyle					("Style")
#define Dui_Key_AttriRichTextLineLimit				("LineLimit")

#define Dui_Key_AttriImageSection					("ImageSection")
#define Dui_Key_AttriImageSecImageRes				("ImageRes")
#define Dui_Key_AttriImageSecHorizAlign				("HAlign")
#define Dui_Key_AttriImageSecVertAlign				("VAlign")
#define Dui_Key_AttriImageSecHorizOffset			("HOffset")
#define Dui_Key_AttriImageSecVertOffset				("VOffset")
#define Dui_Key_AttriImageSecSourceType				("SourceType")
#define Dui_Key_AttriImageSecSourcePartType			("PartType")
#define Dui_Key_AttriImageSecSourcePartAll			("PartAll")
#define Dui_Key_AttriImageSecSourcePartSel			("PartSel")
#define Dui_Key_AttriImageSecSourceCustom			("Source")
#define Dui_Key_AttriImageSecAlpha					("Alpha")
#define Dui_Key_AttriImageSecMask					("Mask")
#define Dui_Key_AttriImageSecCorner					("Corner")
#define Dui_Key_AttriImageSecHole					("Hole")
#define Dui_Key_AttriImageSecTiledX					("TiledX")
#define Dui_Key_AttriImageSecTiledY					("TiledY")

#define Dui_Key_AttriNumberLong						("Value")
#define Dui_Key_AttriNumberULong					("Value")
#define Dui_Key_AttriNumberFloat					("Value")
#define Dui_Key_AttriBoolValue						("Value")
#define Dui_Key_AttriSizeValue						("Value")
#define Dui_Key_AttriRectValue						("Value")
#define Dui_Key_AttriHotKeyValue					("Value")

#define Dui_Key_AttriComboxItem						("Item")
#define Dui_Key_AttriComboxCur						("Cur")

//////////////////////////////////////////////////////////////////////////
#define StrComboxItem								("[%d]%s")
#define StrPosition									("%d-%d-%d-%d-%d-%d-%d-%d-%d")

//dpi
#define DuiDpiScaleAttri(x)							(m_pOwner ? m_pOwner->GetDpiObj().Scale(x) : x)

#define DuiAttriModifyValue(Value, Store, ID) \
{ \
	do \
	{ \
		Store[ID] = Value; \
\
	} while (false); \
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif