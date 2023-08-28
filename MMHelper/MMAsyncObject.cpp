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
		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
		PostMessage(WM_CLOSE, NULL, NULL);

		m_hWnd = NULL;
	}

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
	return 0;
}