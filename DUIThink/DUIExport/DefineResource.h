#ifndef __DUIEXPORTDEFINERESOURCE_H__
#define __DUIEXPORTDEFINERESOURCE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define Name_ColorNone					_T("0x00000000")
#define Name_OneAlpha					_T("0x10000000")
#define Name_ColorBlack					_T("0xff000000")
#define Name_ColorWhite					_T("0xffffffff")
#define Name_ColorRed					_T("0xffff0000")
#define Name_ColorGreen					_T("0xff00ff00")
#define Name_ColorBlue					_T("0xff0000ff")
#define Name_ColorGray					_T("0xffcccccc")
#define Name_ColorDefault				_T("0xfff0f0f0")
#define Name_ColorSelBk					_T("0xff0078d7")
#define Name_Colorff0066cc				_T("0xff0066cc")
#define Name_Colorffe5f3ff				_T("0xffe5f3ff")
#define Name_Colorffcde8ff				_T("0xffcde8ff")
#define Name_Colorff252429				_T("0xff252429")
#define Name_Colorff727176				_T("0xff727176")
#define Name_Colorffbababf				_T("0xffbababf")

static TCHAR g_szPublicColor[][MAX_PATH] = 
{
	Name_OneAlpha,
	Name_ColorBlack,
	Name_ColorWhite,
	Name_ColorRed,
	Name_ColorGreen,
	Name_ColorBlue,
	Name_ColorGray,
	Name_ColorDefault,
	Name_ColorSelBk,
	Name_Colorff0066cc,
	Name_Colorffe5f3ff,
	Name_Colorffcde8ff,
	Name_Colorff252429,
	Name_Colorff727176,
	Name_Colorffbababf,
};

//////////////////////////////////////////////////////////////////////////
enum enDuiResVersion
{
	DuiResVersion_0 = 0,
	DuiResVersion_1,
	DuiResVersion_2,
	DuiResVersion_Max = DuiResVersion_2,
};

enum enDuiResType
{
	DuiResType_None = -1,
	DuiResType_Font,
	DuiResType_Image,
	DuiResType_Color,
};

//////////////////////////////////////////////////////////////////////////

#endif