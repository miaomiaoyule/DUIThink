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

#ifdef _DEBUG
	#ifndef MMTRACE
	#define MMTRACE MMTrace
	#endif
#else
	#ifndef MMTRACE
	#define MMTRACE
	#endif
#endif

// P0607R0 Inline Variables For The STL
#if _HAS_CXX17
#define _INLINE_VAR	inline
#else /* _HAS_CXX17 */
#define _INLINE_VAR
#endif /* _HAS_CXX17 */

// [[nodiscard]] attributes on STL functions
#ifndef _HAS_NODISCARD
#define _HAS_NODISCARD	_HAS_CXX17
#endif /* _HAS_NODISCARD */

#if _HAS_NODISCARD
#define _NODISCARD [[nodiscard]]
#define _NODISCARD_PERF
#else /* ^^^ CAN HAZ [[nodiscard]] ^^^ // vvv NO CAN HAZ [[nodiscard]] vvv */
#define _NODISCARD
#define _NODISCARD_PERF
#endif /* _HAS_NODISCARD */

//////////////////////////////////////////////////////////////////////////////////
//包含文件
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <assert.h>
#include <vector>
#include <atomic>
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
#include <winternl.h>
#include <WtsApi32.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "version.lib")

//////////////////////////////////////////////////////////////////////////////////
void MMHELPER_API MMTrace(LPCTSTR pstrFormat, ...);

//////////////////////////////////////////////////////////////////////////
#define MMSvgEnable
#ifdef MMSvgEnable
#include "../ThirdDepend/svg/Include/svg.h"

#ifdef _DEBUG
	#ifdef _DLL
		#ifdef _WIN64
		#pragma comment(lib, "../lib/svg64.lib")
		#else
		#pragma comment(lib, "../lib/svg.lib")
		#endif
	#else
		#ifdef _WIN64
		#pragma comment(lib, "../lib/svg64.lib")
		#else
		#pragma comment(lib, "../lib/svg.lib")
		#endif
	#endif
#else
	#ifdef _DLL
		#ifdef _WIN64
		#pragma comment(lib, "../lib/svg64.lib")
		#else
		#pragma comment(lib, "../lib/svg.lib")
		#endif
	#else
		#ifdef _WIN64
		#pragma comment(lib, "../lib/svg64.lib")
		#else
		#pragma comment(lib, "../lib/svg.lib")
		#endif
	#endif
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////
//导出文件
#include "MMDefine.h"
#include "MMHash.h"
#include "MMString.h"
#include "MMModule.h"
#include "MMFile.h"
#include "MMStrHelp.h"
#include "MMMiniDumper.h"
#include "MMService.h"
#include "MMCommandLine.h"
#include "MMResource.h"
#include "MMMD5CheckSum.h"
#include "MMAsyncObject.h"
#include "MMProcess.h"
#include "MMDpi.h"
#include "MMReDirection.h"
#include "MMVersion.h"
#include "MMSvg.h"
#include "MMDisplayer.h"
#include "MMTrayIcon.h"
#include "MMEncrypt.h"
#include "MMMagnetBox.h"
#include "MMServiceModel/MMServiceMsg.h"
#include "MMServiceModel/MMServiceItem.h"
#include "MMServiceModel/MMTimerPower.h"
#include "MMServiceModel/MMThreadPool.h"
#include "MMShellExecute/MMShellExecute.h"
#include "MMFileMonitor/MMFileMonitor.h"
#include "MMWinTCPing.h"
#include "MMSocket/Define.h"
#include "MMSocket/SocketClient/IMMTCPSocketClient.h"
#include "MMSocket/SocketServer/IMMTCPSocketServer.h"
#include "MMRegMonitor.h"
#include "MMDragDrop/MMDropSource.h"
#include "MMDragDrop/MMDataObject.h"
#include "MMDragDrop/MMDragDrop.h"
#include "MMSocket/SocketClient/Define.h"
#include "MMSocket/SocketClient/MMTCPSocketClient.h"
#include "MMSocket/SocketServer/Define.h"
#include "MMSocket/SocketServer/MMSocketClientItem.h"
#include "MMSocket/SocketServer/MMTCPSocketServer.h"

//////////////////////////////////////////////////////////////////////////////////

#endif