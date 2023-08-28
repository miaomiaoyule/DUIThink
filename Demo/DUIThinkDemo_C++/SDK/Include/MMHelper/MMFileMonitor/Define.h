#ifndef __MM_DEFINEFILEMONITOR_H__
#define __MM_DEFINEFILEMONITOR_H__

#pragma once

interface IMMFileMonitorAttemperSink;

//////////////////////////////////////////////////////////////////////////
//inner
enum enMonitorTask
{
	MonitorTask_Register,
	MonitorTask_UnRegister,
};

struct tagMonitorTask 
{
	enMonitorTask						MonitorTask;
	bool								bRecursive = false;
	PCIDLIST_ABSOLUTE					pPCIDL = NULL;
	IMMFileMonitorAttemperSink *		pIAttemperSink = NULL;
};

struct tagMonitorFile
{
	PCIDLIST_ABSOLUTE					pPCIDL = NULL;
	UINT								uHanlde = NULL;
	UINT								uMsg = 0;
	bool								bRecursive = false;
	vector<IMMFileMonitorAttemperSink*> vecAttemperSink;
};

typedef std::vector<std::shared_ptr<tagMonitorTask>> VecMonitorTask;
typedef std::vector<tagMonitorFile> VecMonitorFile;

//////////////////////////////////////////////////////////////////////////
//outer
interface IMMFileMonitorAttemperSink
{
	virtual void OnNotify(LONG lEvent, PCIDLIST_ABSOLUTE pPCIDLFirst, PCIDLIST_ABSOLUTE pPCIDLNext) = NULL;
};

typedef std::vector<IMMFileMonitorAttemperSink*> VecMMFileMonitorAttemperSink;

//////////////////////////////////////////////////////////////////////////

#endif