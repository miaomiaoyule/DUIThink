// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//////////////////////////////////////////////////////////////////////////
// TODO:  在此处引用程序需要的其他头文件
#if defined(__ANDROID__)
#include <time.h>
struct timeb
{
	time_t time;
	unsigned short millitm;
	short timezone;
	short dstflag;
};
inline void ftime(struct timeb *tb)
{
	if (!tb) return;
	struct timespec ts = {};
	clock_gettime(CLOCK_REALTIME, &ts);
	tb->time = ts.tv_sec;
	tb->millitm = (unsigned short)(ts.tv_nsec / 1000000);
	tb->timezone = 0;
	tb->dstflag = 0;
}
#else
#include <sys/timeb.h>
#include <time.h>
#endif

#include "MMHelperHead.h"

//////////////////////////////////////////////////////////////////////////
