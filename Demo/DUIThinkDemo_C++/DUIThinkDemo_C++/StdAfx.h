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
#include "../../../DUIThink/DUIThinkHead.h"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../../../lib/MMHelper.201764D.lib")
#pragma comment(lib, "../../../lib/DUIThink.201764D.lib")
#else
#pragma comment(lib, "../../../lib/MMHelper.201764.lib")
#pragma comment(lib, "../../../lib/DUIThink.201764.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../../../lib/MMHelper.2017D.lib")
#pragma comment(lib, "../../../lib/DUIThink.2017D.lib")
#else
#pragma comment(lib, "../../../lib/MMHelper.2017.lib")
#pragma comment(lib, "../../../lib/DUIThink.2017.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////
#include "../DUIThinkDemo.DuiProj/ResourceCtrlID.h"
#include "Resource.h"
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
#include "DlgChatTip.h"
#include "DlgDemo.h"

//////////////////////////////////////////////////////////////////////////