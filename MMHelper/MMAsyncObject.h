#ifndef __MM_ASYNCOBJECT_H__
#define __MM_ASYNCOBJECT_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CMMAsyncObject;
struct tagMMSdlAsyncMsg
{
	CMMAsyncObject *					pWnd = NULL;
	UINT								uMsg = 0;
	WPARAM								wParam = 0;
	LPARAM								lParam = 0;
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMAsyncObject
{
	MMDeclare_ClassName(CMMAsyncObject)

public:
	CMMAsyncObject();
	virtual ~CMMAsyncObject();

	struct TaskBase
	{
		std::function<void()>			func;
	};
	struct TimerInfo : public TaskBase
	{
		bool							bRepeat = false;
#if defined(DuiPlatform_SDL)
		Uint32							uTimerIDSdl = 0;
		void *							sdlTimerParam = nullptr;
#endif
	};

protected:
	std::recursive_mutex				m_AsyncDataLock;
	HWND								m_hWndAsync = NULL;
	Uint32								m_uWndID = 0;
#if defined DuiPlatform_SDL
	Uint32								m_uMsgAsyncTask = 0;
#else
	const UINT							m_uMsgAsyncTask = WM_APP + 1;
#endif

	// Map timer id -> TimerInfo
	std::map<UINT_PTR, TimerInfo>		m_TimerTasks;
	std::atomic<UINT_PTR>				m_NextTimerId{ 1 };

public:
	//you should call init and uninit on uithread
	virtual bool Init();
	virtual bool UnInit();
	virtual bool PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
	bool AsyncTask(std::function<void()> pFunc);
	UINT_PTR TimerTask(unsigned int ms, bool bRepeat, std::function<void()> pFunc);
	bool StopTimer(UINT_PTR timerId);

	//help
protected:
	// Helper used by template wrappers (defined in cpp)
	UINT_PTR StartTimerInternal(unsigned int ms, std::function<void()>&& fn, bool repeat);

protected:
#if defined(DuiPlatform_SDL)
	static bool SDLCALL SDLEventWatch(void *userdata, SDL_Event *e);
	static Uint32 SDLCALL SDLTimerCallback(void *userdata, SDL_TimerID timerID, Uint32 interval);
#else
	static LRESULT CALLBACK OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

//////////////////////////////////////////////////////////////////////////

#endif