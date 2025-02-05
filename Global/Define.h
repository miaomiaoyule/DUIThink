#ifndef __MM_DEFINEGLOBAL_H__
#define __MM_DEFINEGLOBAL_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	#define Name_AppPreviewExe			_T("DUIPreviewD.exe")
	#define Name_AppUpdateExe			_T("MMUpdate.exe")
#else
	#define Name_AppPreviewExe			_T("DUIPreview.exe")
	#define Name_AppUpdateExe			_T("MMUpdate.exe")
#endif

#define Name_AppDUIThink				_T("DUIThink")

//////////////////////////////////////////////////////////////////////////
#define Len_Account						(32)
#define Len_NickName					(32)
#define Len_WeChatCode					(MAX_PATH)
#define Len_CtrlClass					(50)
#define Len_CtrlIcon					(50)
#define Len_CtrlDllName					(50)
#define Len_CtrlAuthorName				(50)
#define Len_CtrlAuthorQuickMark			(50)

//////////////////////////////////////////////////////////////////////////
#define Key_Dui_PreviewProject			_T("PreviewProject")
#define Key_Dui_PreviewDuiName			_T("PreviewDuiName")
#define Key_Dui_PreviewParentWnd		_T("PreviewParentWnd")
#define Key_Dui_PreviewDpiScale			_T("PreviewDpiScale")
#define Key_Dui_PreviewFrameWnd			_T("PreviewFrameWnd")
#define Key_Dui_LaunchFromVS			_T("LaunchFromVS")

//////////////////////////////////////////////////////////////////////////

#endif