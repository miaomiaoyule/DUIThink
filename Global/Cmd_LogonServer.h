#ifndef __MM_CMDLOGON_H__
#define __MM_CMDLOGON_H_

#pragma once

#include "Define.h"

//////////////////////////////////////////////////////////////////////////
#define MDM_LogonServer_Logon			(100)

#define SUB_ToS_Logon					(1)
#define SUB_ToC_LogonResult				(2)
#define SUB_ToC_LogonFinish				(3)
#define SUB_ToC_Offline					(4)
#define SUB_ToC_CtrlInfoStart			(10)
#define SUB_ToC_CtrlInfoResult			(11)
#define SUB_ToC_CtrlInfoFinish			(12)

struct CMD_ToS_LogonWeChat
{
	TCHAR								szAccounts[Len_Account] = {};
	TCHAR								szNickName[Len_NickName] = {};
	TCHAR								szMachineID[Len_Machine_ID] = {};
	DWORD								dwClientAddr = 0;
};
struct CMD_ToC_LogonWeChat
{
	bool								bSuccess = false;
	DWORD								dwUserID = 0;
	SYSTEMTIME							tVipOverTime = {};
	TCHAR								szDescribe[MAX_PATH] = {};
};
struct CMD_ToC_DuiControlInfo
{
	bool								bExtendCtrl = false;
	TCHAR								szCtrlClass[Len_CtrlClass] = {};
	TCHAR								szCtrlIcon[Len_CtrlIcon] = {};
	TCHAR								szCtrlDllNamePublish[Len_CtrlDllName] = {};
	TCHAR								szCtrlAuthorName[Len_CtrlAuthorName] = {};
	TCHAR								szCtrlAuthorQuickMark[Len_CtrlAuthorQuickMark] = {};
};

//////////////////////////////////////////////////////////////////////////

#endif