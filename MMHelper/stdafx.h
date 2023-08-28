// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <sys/timeb.h>
#include <time.h>

#include "MMHelperHeadInner.h"
#include "MMSocket/SocketClient/Define.h"
#include "MMSocket/SocketClient/MMTCPSocketClient.h"
#include "MMSocket/SocketServer/Define.h"
#include "MMSocket/SocketServer/MMSocketClientItem.h"
#include "MMSocket/SocketServer/MMTCPSocketServer.h"

//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "version.lib")

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