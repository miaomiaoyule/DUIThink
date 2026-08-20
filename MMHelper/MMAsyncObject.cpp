#include "stdafx.h"
#include "MMAsyncObject.h"

//////////////////////////////////////////////////////////////////////////
namespace {
	// Param passed to SDL timer callback
	struct SDLTmrCallbackParam
	{
		CMMAsyncObject * self;
		UINT_PTR id;
		Uint32 interval;
		bool repeat;
	};
}

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CMMAsyncObject)

CMMAsyncObject::CMMAsyncObject()
{

}

CMMAsyncObject::~CMMAsyncObject()
{

}

bool CMMAsyncObject::Init()
{
	std::lock_guard<std::recursive_mutex> Lock(m_AsyncDataLock);

	if (IsWindow(m_hWndAsync))
	{
#ifndef DuiPlatform_SDL
		::SetWindowLongPtr(m_hWndAsync, GWLP_USERDATA, (LONG_PTR)this);
#endif

		return true;
	}

	//window
#if defined(DuiPlatform_SDL)
	m_hWndAsync = SDL_CreateWindow("", 1, 1, SDL_WINDOW_HIDDEN);
	if (m_hWndAsync == nullptr)
	{
		assert(false);
		return false;
	}
	if (m_uMsgAsyncTask == 0)
	{
		m_uMsgAsyncTask = SDL_RegisterEvents(1);
	}

	m_uWndID = SDL_GetWindowID(m_hWndAsync);
	SDL_AddEventWatch(&CMMAsyncObject::SDLEventWatch, this);
#else
	CMMString strClassName = GetClass() + CMMService::ProductGUID();

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = OnWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = strClassName;

	::RegisterClass(&wc);

	m_hWndAsync = ::CreateWindowEx(0, strClassName, GetClass(), 0, 0, 0, 1, 1, HWND_MESSAGE, 0, 0, this);
	if (NULL == m_hWndAsync)
	{
		assert(false);
		return false;
	}

	m_uWndID = (UINT)m_hWndAsync;
#endif

	return true;
}

bool CMMAsyncObject::UnInit()
{
	std::lock_guard<std::recursive_mutex> Lock(m_AsyncDataLock);

	if (IsWindow(m_hWndAsync))
	{
#if defined(DuiPlatform_SDL)
		for (auto &kv : m_TimerTasks)
		{
			if (kv.second.uTimerIDSdl)
			{
				SDL_RemoveTimer((SDL_TimerID)kv.second.uTimerIDSdl);
				kv.second.uTimerIDSdl = 0;
			}
			if (kv.second.sdlTimerParam)
			{
				delete static_cast<SDLTmrCallbackParam*>(kv.second.sdlTimerParam);
				kv.second.sdlTimerParam = nullptr;
			}
		}

		SDL_RemoveEventWatch(&CMMAsyncObject::SDLEventWatch, this);
		SDL_DestroyWindow(m_hWndAsync);

		m_hWndAsync = NULL;
		m_uMsgAsyncTask = 0;
#else
		for (auto& kv : m_TimerTasks)
		{
			::KillTimer(m_hWndAsync, kv.first);
		}

		::SetWindowLongPtr(m_hWndAsync, GWLP_USERDATA, NULL);
		PostMessage(WM_CLOSE, NULL, NULL);

		m_hWndAsync = NULL;
#endif	
	}

	m_TimerTasks.clear();

	return true;
}

bool CMMAsyncObject::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#if defined(DuiPlatform_SDL)
	if (m_hWndAsync == nullptr || m_uMsgAsyncTask == 0) return false;

	tagMMSdlAsyncMsg *pAsyncMsg = new (std::nothrow) tagMMSdlAsyncMsg();
	if (pAsyncMsg == nullptr) return false;

	pAsyncMsg->pWnd = reinterpret_cast<CMMAsyncObject*>(this);
	pAsyncMsg->uMsg = uMsg;
	pAsyncMsg->wParam = wParam;
	pAsyncMsg->lParam = lParam;

	SDL_Event e = {};
	e.type = m_uMsgAsyncTask;
	e.user.timestamp = SDL_GetTicksNS();
	e.user.windowID = SDL_GetWindowID(m_hWndAsync);
	e.user.code = 0;
	e.user.data1 = pAsyncMsg;
	e.user.data2 = nullptr;

	if (SDL_PushEvent(&e) == 0)
	{
		delete pAsyncMsg;
		return false;
	}

	return true;
#else
	return ::PostMessage(m_hWndAsync, uMsg, wParam, lParam);
#endif
}

bool CMMAsyncObject::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#if defined(DuiPlatform_SDL)
	bool bHandled = false;
	HandleMessage(uMsg, wParam, lParam, bHandled);

	return true;
#else
	return ::SendMessage(m_hWndAsync, uMsg, wParam, lParam);
#endif
}

LRESULT CMMAsyncObject::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (uMsg == m_uMsgAsyncTask)
	{
		auto pTask = reinterpret_cast<TaskBase*>(wParam);
		if (pTask)
		{
			try
			{
				pTask->func();
			}
			catch (...)
			{
			}

			delete pTask;
		}

		bHandled = true;

		return 0;
	}
	else if (uMsg == WM_TIMER)
	{
		UINT_PTR id = (UINT_PTR)wParam;
		std::function<void()> fn;
		bool bRepeat = false;

		{
			std::lock_guard<std::recursive_mutex> lock(m_AsyncDataLock);
			auto it = m_TimerTasks.find(id);
			if (it != m_TimerTasks.end())
			{
				fn = it->second.func;
				bRepeat = it->second.bRepeat;
				if (false == bRepeat)
				{
					StopTimer(it->first);
				}
			}
		}

		if (fn)
		{
			try
			{
				fn();
			}
			catch (...)
			{
			}
		}

		bHandled = true;
		return 0;
	}

	return 0;
}

bool CMMAsyncObject::AsyncTask(std::function<void()> pFunc)
{
	if (!m_hWndAsync) return false;
	auto pTask = new TaskBase{ std::move(pFunc) };

	BOOL ok = PostMessage(m_uMsgAsyncTask, reinterpret_cast<WPARAM>(pTask), 0);
	if (!ok)
	{
		delete pTask;
		return false;
	}

	return true;
}

UINT_PTR CMMAsyncObject::TimerTask(unsigned int ms, bool bRepeat, std::function<void()> pFunc)
{
	return StartTimerInternal(ms, std::move(pFunc), bRepeat);
}

// Stop timer by id
bool CMMAsyncObject::StopTimer(UINT_PTR timerId)
{
	if (!m_hWndAsync) return false;

	{
		std::lock_guard<std::recursive_mutex> lock(m_AsyncDataLock);
		auto it = m_TimerTasks.find(timerId);
		if (it == m_TimerTasks.end()) return false;

#if defined(DuiPlatform_SDL)
		// Remove SDL timer and free param
		if (it->second.uTimerIDSdl)
		{
			SDL_RemoveTimer((SDL_TimerID)it->second.uTimerIDSdl);
			it->second.uTimerIDSdl = 0;
		}
		if (it->second.sdlTimerParam)
		{
			delete static_cast<SDLTmrCallbackParam*>(it->second.sdlTimerParam);
			it->second.sdlTimerParam = nullptr;
		}
#else
		::KillTimer(m_hWndAsync, timerId);
#endif

		m_TimerTasks.erase(it);
	}

	return true;
}

UINT_PTR CMMAsyncObject::StartTimerInternal(unsigned int ms, std::function<void()> &&fn, bool repeat)
{
	if (!m_hWndAsync) return 0;
	if (!fn) return 0;

	UINT_PTR id = m_NextTimerId.fetch_add(1);

#if defined(DuiPlatform_SDL)
	// Setup SDL timer: allocate param and call SDL_AddTimer
	SDLTmrCallbackParam *pParam = new (std::nothrow) SDLTmrCallbackParam();
	if (pParam == nullptr)
	{
		return 0;
	}

	pParam->self = this;
	pParam->id = id;
	pParam->interval = ms;
	pParam->repeat = repeat;

	SDL_TimerID sdlTimerId = SDL_AddTimer(ms, SDLTimerCallback, pParam);
	if (sdlTimerId == 0)
	{
		// failed -> cleanup
		delete pParam;
		return 0;
	}

	{
		std::lock_guard<std::recursive_mutex> lock(m_AsyncDataLock);
		m_TimerTasks[id].func = std::move(fn);
		m_TimerTasks[id].bRepeat = repeat;
		m_TimerTasks[id].uTimerIDSdl = (Uint32)sdlTimerId;
		m_TimerTasks[id].sdlTimerParam = pParam;
	}

	return id;
#else
	if (!::SetTimer(m_hWndAsync, id, ms, NULL))
	{
		return 0;
	}

	{
		std::lock_guard<std::recursive_mutex> lock(m_AsyncDataLock);
		m_TimerTasks[id].func = std::move(fn);
		m_TimerTasks[id].bRepeat = repeat;
	}

	return id;
#endif
}

//////////////////////////////////////////////////////////////////////////
#if defined(DuiPlatform_SDL)
bool SDLCALL CMMAsyncObject::SDLEventWatch(void *userdata, SDL_Event *e)
{
	CMMAsyncObject *self = static_cast<CMMAsyncObject *>(userdata);
	if (NULL == self || e->window.windowID != self->m_uWndID) return false;

	//user event
	if (e->type == self->m_uMsgAsyncTask)
	{
		tagMMSdlAsyncMsg *pAsyncMsg = static_cast<tagMMSdlAsyncMsg *>(e->user.data1);
		if (pAsyncMsg && pAsyncMsg->pWnd == self)
		{
			bool bHandled = false;
			self->HandleMessage(pAsyncMsg->uMsg, pAsyncMsg->wParam, pAsyncMsg->lParam, bHandled);

			MMSafeDelete(pAsyncMsg);
			e->user.data1 = NULL;
		}

		return false;
	}

	//event
	bool bHandled = false;
	//self->HandleMessage(e->type, pAsyncMsg->wParam, pAsyncMsg->lParam, bHandled);

	return true;
}

Uint32 SDLCALL CMMAsyncObject::SDLTimerCallback(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
	SDLTmrCallbackParam *p = static_cast<SDLTmrCallbackParam *>(userdata);
	if (p == nullptr || p->self == nullptr) return 0;

	tagMMSdlAsyncMsg *pAsyncMsg = new (std::nothrow) tagMMSdlAsyncMsg();
	if (pAsyncMsg != nullptr)
	{
		pAsyncMsg->pWnd = reinterpret_cast<CMMAsyncObject *>(p->self);
		pAsyncMsg->uMsg = WM_TIMER;
		pAsyncMsg->wParam = static_cast<WPARAM>(p->id);
		pAsyncMsg->lParam = 0;

		SDL_Event e = {};
		e.type = p->self->m_uMsgAsyncTask;
		e.user.timestamp = SDL_GetTicksNS();
		e.user.windowID = p->self->m_uWndID;
		e.user.code = 0;
		e.user.data1 = pAsyncMsg;
		e.user.data2 = nullptr;

		if (SDL_PushEvent(&e) == 0)
		{
			delete pAsyncMsg;
		}
	}

	return p->repeat ? interval : 0;
}
#else
LRESULT CALLBACK CMMAsyncObject::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CMMAsyncObject *pThis = (CMMAsyncObject*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	LRESULT lRes = 0;

	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CMMAsyncObject*>(lpcs->lpCreateParams);
		pThis->m_hWndAsync = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
	}
	if (pThis)
	{
		bool bHandled = false;
		lRes = pThis->HandleMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled)
		{
			return lRes;
		}
	}

	lRes = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	return lRes;
}
#endif