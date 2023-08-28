#ifndef __MM_DROPSOURCE_H__
#define __MM_DROPSOURCE_H__

#pragma once

#include "Define.h"

//////////////////////////////////////////////////////////////////////////
class CMMDragDrop;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMDropSource 
	: public IDropSource 
{
	long m_cRefCount;

public:
	bool m_bDropped;

	CMMDropSource() 
		: m_cRefCount(0)
		, m_bDropped(false) 
	{}

	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);
	//IDropSource
	virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(
		/* [in] */ BOOL fEscapePressed,
		/* [in] */ DWORD grfKeyState);

	virtual HRESULT STDMETHODCALLTYPE GiveFeedback(
		/* [in] */ DWORD dwEffect);
private:
	CMMDragDrop *						m_pDragDropHelper = nullptr;
};

//////////////////////////////////////////////////////////////////////////

#endif