#ifndef __DUIEXPORTDEFINEANIMATION_H__
#define __DUIEXPORTDEFINEANIMATION_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
enum enDuiAnimateWndType
{
	AnimateWnd_None,
	AnimateWnd_Left,
	AnimateWnd_LeftDiffuse,
	AnimateWnd_Right,
	AnimateWnd_RightDiffuse,
	AnimateWnd_Top,
	AnimateWnd_TopDiffuse,
	AnimateWnd_Bottom,
	AnimateWnd_BottomDiffuse,
	AnimateWnd_Size,
	AnimateWnd_SizeSpring,
	AnimateWnd_SizeDiffuse,
	AnimateWnd_CenterDiffuse,
	AnimateWnd_LeftRightDiffuse,
	AnimateWnd_LeftRightExpand,
	AnimateWnd_LeftRightExpandSpring,
	AnimateWnd_LeftRightCombin,
	AnimateWnd_TopBottomDiffuse,
	AnimateWnd_TopBottomExpand,
	AnimateWnd_TopBottomExpandSpring,
	AnimateWnd_TopBottomCombin,
	AnimateWnd_3DRotateLeftRight,
	AnimateWnd_3DRotateTopBottom,
	AnimateWnd_3DRoundLeft,
	AnimateWnd_3DRoundRight,
	AnimateWnd_3DRoundTop,
	AnimateWnd_3DRoundBottom,
	AnimateWnd_3DRotateReplaseLR,
	AnimateWnd_3DRotateReplaseTB,
	AnimateWnd_3DRoundReplaseLR,
	AnimateWnd_3DRoundReplaseTB,
	AnimateWnd_Count,
};

enum enDuiAnimateImageType
{
	AnimateImage_None,
	AnimateImage_Gif,
	AnimateImage_SequenceFrame,
	AnimateImage_Webp,
};

//////////////////////////////////////////////////////////////////////////

#endif