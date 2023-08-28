// header.h: ��׼ϵͳ�����ļ��İ����ļ���
// ���ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ�����
// Windows ͷ�ļ�
#include <windows.h>
// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <CommDlg.h>

//////////////////////////////////////////////////////////////////////////
#include "../../../DirectUI/DirectUIInner.h"

#ifdef _DEBUG
#pragma comment(lib, "../../../lib/MMHelperD.lib")
#pragma comment(lib, "../../../lib/DirectUID.lib")
#else
#pragma comment(lib, "../../../lib/MMHelper.lib")
#pragma comment(lib, "../../../lib/DirectUI.lib")
#endif

//#include "../SDK/Include/DirectUI/DirectUI.h"
//
//#ifdef _DEBUG
//	#pragma comment(lib, "../SDK/lib/Debug/MMHelperD.lib")
//	#pragma comment(lib, "../SDK/lib/Debug/DirectUID.lib")
//#else
//	#pragma comment(lib, "../SDK/lib/Release/MMHelper.lib")
//	#pragma comment(lib, "../SDK/lib/Release/DirectUI.lib")
//#endif

//////////////////////////////////////////////////////////////////////////
#include "../DUIThinkDemo.DuiProj/ResourceCtrlID.h"
#include "Define.h"
#include "BaseControl/StaticView/StaticView.h"
#include "BaseControl/CheckBoxView/CheckBoxView.h"
#include "BaseControl/RadioBoxView/RadioBoxView.h"
#include "BaseControl/ProgressView/ProgressView.h"
#include "BaseControl/ComboxView/ComboxView.h"
#include "Usage/DpiDlg/DpiDlgView.h"
#include "Usage/SvgImage/SvgImageView.h"
#include "Usage/SwitchSkin/SwitchSkinView.h"
#include "Usage/AnimateDlg/AnimateDlgView.h"
#include "QQ/QQView.h"
#include "QQ/DlgQQFace.h"
#include "QQ/DlgQQChat.h"
#include "DlgDpi.h"
#include "DlgDemo.h"

//////////////////////////////////////////////////////////////////////////
extern CDUIGlobalHelper g_DuiGlobal;