#ifndef __MM_HELPERINNER_H__
#define __MM_HELPERINNER_H__

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
#include "../include/svg/svg.h"

//////////////////////////////////////////////////////////////////////////
#include "MMHelperHead.h"
#include "MMEncrypt.h"
#include "MMSocket/SocketClient/MMTCPSocketClient.h"
#include "MMSocket/SocketServer/Define.h"
#include "MMSocket/SocketServer/MMSocketClientItem.h"
#include "MMSocket/SocketServer/MMTCPSocketServer.h"

//////////////////////////////////////////////////////////////////////////////////

#endif