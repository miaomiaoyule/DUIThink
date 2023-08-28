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

protected:
	std::recursive_mutex				m_DataLock;
	HWND								m_hWnd = NULL;

public:
	//you should call init and uninit on uithread
	virtual bool Init();
	virtual bool UnInit();
	virtual bool PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);

protected:
	static LRESULT CALLBACK OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////

#endif