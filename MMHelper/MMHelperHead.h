#ifndef __MM_HELPER_H__
#define __MM_HELPER_H__

//////////////////////////////////////////////////////////////////////////
//导出定义
#ifndef MMHELPER_API
	#ifdef MMHELPLIB
		#define MMHELPER_API
	#else
		#ifdef MMHELPER_DLL
			#define MMHELPER_API _declspec(dllexport)
		#else
			#define MMHELPER_API _declspec(dllimport)
		#endif
	#endif
#endif

#ifndef MMHELPER_DLL_NAME
#ifdef _DLL
#ifdef _DEBUG
	#define MMHELPER_DLL_NAME			_T("MMHelperD.dll")			
#else		
	#define MMHELPER_DLL_NAME			_T("MMHelper.dll")			
#endif
#else
#ifdef _DEBUG
	#define MMHELPER_DLL_NAME			_T("MMHelper_MT_D.dll")			
#else		
	#define MMHELPER_DLL_NAME			_T("MMHelper_MT.dll")			
#endif
#endif
#endif 

#ifdef _DEBUG
	#ifndef MMTRACE
		#define MMTRACE MMTrace
	#endif
#else
	#ifndef MMTRACE
		#define MMTRACE
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////
//包含文件
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <assert.h>
#include <vector>
#include <memory>
#include <functional>
#include <deque>
#include <mutex>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <thread>
#include <windows.h>
#include <Tlhelp32.h>  
#include <dbghelp.h>
#include <ObjBase.h>
#include <atltypes.h>
#include <atlstr.h>
#include <nb30.h>
#include <CommCtrl.h>
#include <inaddr.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ShTypes.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////
//string
typedef CAtlStringW CMMStringW;

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
typedef CAtlStringA CMMStringA;
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

void MMHELPER_API MMTrace(LPCTSTR pstrFormat, ...);

//////////////////////////////////////////////////////////////////////////
#include "../include/log/BaseLog.h"

//////////////////////////////////////////////////////////////////////////////////
//导出文件
#include "MMDefine.h"
#include "MMString.h"
#include "MMModule.h"
#include "MMFile.h"
#include "MMStrHelp.h"
#include "MMMiniDumper.h"
#include "MMService.h"
#include "MMCommandLine.h"
#include "MMMD5CheckSum.h"
#include "MMAsyncObject.h"
#include "MMProcess.h"
#include "MMDpi.h"
#include "MMHash.h"
#include "MMReDirection.h"
#include "MMVersion.h"
#include "MMSvg.h"
#include "MMDisplayer.h"
#include "MMTrayIcon.h"
#include "MMServiceModel/MMServiceMsg.h"
#include "MMServiceModel/MMServiceItem.h"
#include "MMServiceModel/MMTimerPower.h"
#include "MMServiceModel/MMThreadPool.h"
#include "MMShellExecute/MMShellExecute.h"
#include "MMFileMonitor/MMFileMonitor.h"
#include "MMDownLoad/MMDownLoad.h"
#include "MMSocket/Define.h"
#include "MMSocket/SocketClient/IMMTCPSocketClient.h"
#include "MMSocket/SocketServer/IMMTCPSocketServer.h"
#include "MMRegMonitor.h"
#include "MMDragDrop/MMDropSource.h"
#include "MMDragDrop/MMDataObject.h"
#include "MMDragDrop/MMDragDrop.h"

//////////////////////////////////////////////////////////////////////////////////

#endif