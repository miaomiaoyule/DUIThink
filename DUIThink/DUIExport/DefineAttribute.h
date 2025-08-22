#ifndef __DUIEXPORTDEFINEATTRIBUTE_H__
#define __DUIEXPORTDEFINEATTRIBUTE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
//attribute type
enum enDuiAttributeType
{
	DuiAttribute_None = 0,
	DuiAttribute_Group,
	DuiAttribute_Position,				
	DuiAttribute_Long,
	DuiAttribute_ULong,
	DuiAttribute_Float,
	DuiAttribute_Color,
	DuiAttribute_ColorSwitch,
	DuiAttribute_ImageSection,
	DuiAttribute_Text,
	DuiAttribute_RichText,
	DuiAttribute_TextStyle,
	DuiAttribute_Bool,
	DuiAttribute_Combox,
	DuiAttribute_Cursor,
	DuiAttribute_DuiSelect,
	DuiAttribute_TabSelect,
	DuiAttribute_Size,
	DuiAttribute_Rect,
	DuiAttribute_HotKey,
	DuiAttribute_CtrlID,
	DuiAttribute_Count
};

//horiz
enum enDuiHorizAlignType
{
	HorizAlign_Left,
	HorizAlign_Right,
	HorizAlign_Center,
};

//vert
enum enDuiVertAlignType
{
	VertAlign_Top = 10,
	VertAlign_Bottom,
	VertAlign_Middle,
};

//image section
enum enDuiHorizImageAlignType
{
	HorizImageAlign_None,
	HorizImageAlign_Left,
	HorizImageAlign_Right,
	HorizImageAlign_Center,
	HorizImageAlign_Tile,
};

//image section
enum enDuiVertImageAlignType
{
	VertImageAlign_None,
	VertImageAlign_Top,
	VertImageAlign_Bottom,
	VertImageAlign_Center,
	VertImageAlign_Tile,
};

//image section
enum enDuiImageSourceType
{
	ImageSource_Normal,
	ImageSource_Part,
	ImageSource_Custom,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif