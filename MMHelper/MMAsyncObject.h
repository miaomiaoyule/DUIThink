#ifndef __MM_ASYNCOBJECT_H__
#define __MM_ASYNCOBJECT_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMAsyncObject
{
	MMDeclare_ClassName(CMMAsyncObject)

public:
	CMMAsyncObject();
	virtual ~CMMAsyncObject();

	// Internal timer storage structure
	struct TimerInfo
	{
		std::function<void()>	func;
		bool					repeat;
	};

protected:
	std::recursive_mutex				m_DataLock;
	HWND								m_hWnd = NULL;
	const UINT							m_uMsgAsyncTask = WM_APP + 1;
	
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

public:
	// Post an async task (returns true if posted). Task executes as bound void().
	template <typename F, typename... Args>
	bool AsyncExecute(F&& f, Args&&... args)
	{
		auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		if (!m_hWnd) return false;
		auto pTask = new std::function<void()>(std::move(bound));

		BOOL ok = ::PostMessage(m_hWnd, m_uMsgAsyncTask, reinterpret_cast<WPARAM>(pTask), 0);
		if (!ok)
		{
			delete pTask;
			return false;
		}

		return true;
	}

	// Post a one-shot delayed task (ms delay). Returns timer id (0 on failure).
	template <typename F, typename... Args>
	UINT_PTR TimerExecute(unsigned int ms, bool bRepeat, F&& f, Args&&... args)
	{
		auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		return StartTimerInternal(ms, std::move(bound), bRepeat);
	}

	// Stop timer by id
	bool StopTimer(UINT_PTR timerId);

	//help
protected:
	// Helper used by template wrappers (defined in cpp)
	UINT_PTR StartTimerInternal(unsigned int ms, std::function<void()>&& fn, bool repeat);

protected:
	static LRESULT CALLBACK OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////

#endif