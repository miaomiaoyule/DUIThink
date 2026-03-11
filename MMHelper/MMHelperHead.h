#ifndef __MM_HELPER_H__
#define __MM_HELPER_H__

// #define __LINUX__
#if defined(__linux__) || defined(__linux)
	#ifndef __LINUX__
		#define __LINUX__
	#endif
#endif
//#define __APPLE__

//////////////////////////////////////////////////////////////////////////
//导出定义
#ifndef MMHELPER_API
	#ifdef _WIN32
		#ifdef MMHELPLIB
			#define MMHELPER_API
		#else
			#ifdef MMHELPER_DLL
				#define MMHELPER_API __declspec(dllexport)
			#else
				#define MMHELPER_API __declspec(dllimport)
			#endif
		#endif
	#else
		#define MMHELPER_API __attribute__((visibility("default")))
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
//#include <tchar.h>
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

#ifdef _WIN32
#include <tchar.h>
#include <windows.h>
#include <Tlhelp32.h>  
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
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#endif

using namespace std;

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "Wtsapi32.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////
void MMHELPER_API MMTrace(LPCTSTR pstrFormat, ...);

//////////////////////////////////////////////////////////////////////////