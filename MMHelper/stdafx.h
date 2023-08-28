// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

// TODO:  在此处引用程序需要的其他头文件
#include <sys/timeb.h>
#include <time.h>

#include "MMHelperHead.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	#ifdef _DLL
	#ifdef _WIN64
		#pragma comment(lib, "../lib/jsoncpp64D.lib")
		#pragma comment(lib, "../lib/libcurl64D.lib")
		#pragma comment(lib, "../lib/svg64.lib")
	#else
		#pragma comment(lib, "../lib/jsoncppD.lib")
		#pragma comment(lib, "../lib/libcurlD.lib")
		#pragma comment(lib, "../lib/svg.lib")
	#endif
	#else
	#ifdef _WIN64
		#pragma comment(lib, "../lib/jsoncpp64_MT_D.lib")
		#pragma comment(lib, "../lib/libcurl64_MT_D.lib")
		#pragma comment(lib, "../lib/svg64.lib")
	#else
		#pragma comment(lib, "../lib/jsoncpp_MT_D.lib")
		#pragma comment(lib, "../lib/libcurl_MT_D.lib")
		#pragma comment(lib, "../lib/svg.lib")
	#endif
	#endif
#else
	#ifdef _DLL
	#ifdef _WIN64
		#pragma comment(lib, "../lib/jsoncpp64.lib")
		#pragma comment(lib, "../lib/libcurl64.lib")
		#pragma comment(lib, "../lib/svg64.lib")
	#else
		#pragma comment(lib, "../lib/jsoncpp.lib")
		#pragma comment(lib, "../lib/libcurl.lib")
		#pragma comment(lib, "../lib/svg.lib")
	#endif
	#else
	#ifdef _WIN64
		#pragma comment(lib, "../lib/jsoncpp64_MT.lib")
		#pragma comment(lib, "../lib/libcurl64_MT.lib")
		#pragma comment(lib, "../lib/svg64.lib")
	#else
		#pragma comment(lib, "../lib/jsoncpp_MT.lib")
		#pragma comment(lib, "../lib/libcurl_MT.lib")
		#pragma comment(lib, "../lib/svg.lib")
	#endif
	#endif
#endif
