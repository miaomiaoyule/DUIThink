#include "stdafx.h"
#include "MMAsyncObject.h"

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CMMAsyncObject::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CMMAsyncObject *pThis = (CMMAsyncObject*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	LRESULT lRes = 0;
	
	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CMMAsyncObject*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
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
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (IsWindow(m_hWnd))
	{
		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

		return true;
	}

	//“Ï≤Ω“¿¿µ
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

	m_hWnd = ::CreateWindowEx(0, strClassName, GetClass(), 0, 0, 0, 1, 1, HWND_MESSAGE, 0, 0, this);
	if (NULL == m_hWnd)
	{
		assert(false);
		return false;
	}

	return true;
}

bool CMMAsyncObject::UnInit()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	if (IsWindow(m_hWnd))
	{
		// kill any timers
		for (auto& kv : m_TimerTasks)
		{
			::KillTimer(m_hWnd, kv.first);
		}

		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
		PostMessage(WM_CLOSE, NULL, NULL);

		m_hWnd = NULL;
	}

	m_TimerTasks.clear();

	return true;
}

bool CMMAsyncObject::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

bool CMMAsyncObject::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CMMAsyncObject::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (uMsg == m_uMsgAsyncTask)
	{
		auto pTask = reinterpret_cast<std::function<void()>*>(wParam);
		if (pTask)
		{
			try
			{
				(*pTask)();
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
		bool repeat = false;

		{
			std::lock_guard<std::recursive_mutex> lock(m_DataLock);
			auto it = m_TimerTasks.find(id);
			if (it != m_TimerTasks.end())
			{
				fn = it->second.func;
				repeat = it->second.repeat;
				if (!repeat)
				{
					::KillTimer(m_hWnd, id);
					m_TimerTasks.erase(it);
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

// Internal timer start helper
UINT_PTR CMMAsyncObject::StartTimerInternal(unsigned int ms, std::function<void()>&& fn, bool repeat)
{
	if (!m_hWnd) return 0;
	if (!fn) return 0;

	UINT_PTR id = m_NextTimerId.fetch_add(1);
	{
		std::lock_guard<std::recursive_mutex> lock(m_DataLock);
		m_TimerTasks[id] = TimerInfo{ std::move(fn), repeat };
	}

	if (!::SetTimer(m_hWnd, id, ms, NULL))
	{
		// failed -> cleanup
		std::lock_guard<std::recursive_mutex> lock(m_DataLock);
		m_TimerTasks.erase(id);
		return 0;
	}

	return id;
}

bool CMMAsyncObject::StopTimer(UINT_PTR timerId)
{
	if (!m_hWnd) return false;

	{
		std::lock_guard<std::recursive_mutex> lock(m_DataLock);
		auto it = m_TimerTasks.find(timerId);
		if (it == m_TimerTasks.end()) return false;
		::KillTimer(m_hWnd, timerId);
		m_TimerTasks.erase(it);
	}

	return true;
}