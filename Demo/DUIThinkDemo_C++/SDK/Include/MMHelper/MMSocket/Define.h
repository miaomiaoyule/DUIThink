#ifndef __MM_DEFINESOCKET_H__
#define __MM_DEFINESOCKET_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define SOCKET_TCP_BUFFER				(16384)								//网络缓冲

#define MaskClient_PC					(0x01)								//PC端
#define MaskClient_Mobile				(0x02)								//手机端
#define MaskClient_All					(0xff)								//所有端

#define MDM_CmdInner_Start				(1)
#define MDM_CmdInner_Finish				(10)

//////////////////////////////////////////////////////////////////////////////////
//shut reason
enum enSocketShutReason
{
	SocketShut_Inside,					//内部原因
	SocketShut_Normal,					//正常关闭
	SocketShut_Remote,					//远程关闭
	SocketShut_TimeOut,					//网络超时
	SocketShut_Exception,				//异常关闭
};

//socket status
enum enSocketStatus
{
	SocketStatus_Idle,					 //空闲状态
	SocketStatus_Wait,					 //等待状态
	SocketStatus_Connect				 //连接状态
};

//connect code
enum enSocketConnectCode
{
	SocketConnect_Success,				//连接成功
	SocketConnect_Fail,					//连接失败
	SocketConnect_Exception,			//参数异常
	SocketConnect_ProxyFail,			//连接失败
	SocketConnect_ProxyUserInvalid,		//用户无效									
};

//////////////////////////////////////////////////////////////////////////
//pack head
struct tagTCPHead
{
	WORD								wPacketSize = 0;
	WORD								wMainCmd = 0;
	WORD								wSubCmd = 0;
	DWORD								dwReserve = 0;
};

//////////////////////////////////////////////////////////////////////////

#endif