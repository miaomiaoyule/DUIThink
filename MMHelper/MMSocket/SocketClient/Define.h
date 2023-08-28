#ifndef __MM_DEFINETCPSOCKETCLIENT_H__
#define __MM_DEFINETCPSOCKETCLIENT_H__

//////////////////////////////////////////////////////////////////////////
enum enSocketNotifyType
{
	SocketNotify_Link,
	SocketNotify_Read,
	SocketNotify_Shut,
};

//////////////////////////////////////////////////////////////////////////////////
struct tagSocketNotifyTask
{
	enSocketNotifyType					SocketNotifyType = SocketNotify_Link;
	enSocketConnectCode					SocketConnectCode = SocketConnect_Success;
	enSocketShutReason					SocketShutReason = SocketShut_Normal;
	CMMString							strErrorDescribe;
	WORD								wMainCmd = 0;
	WORD								wSubCmd = 0;
	std::vector<BYTE>					vecData;
};

typedef std::vector<tagSocketNotifyTask> VecSocketNotifyTask;

//////////////////////////////////////////////////////////////////////////////////

#endif