#include "stdafx.h"
#include "MMDropSource.h"

//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMMDropSource::QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) 
{
	*ppvObject = nullptr;
	if (IID_IUnknown == riid || IID_IDropSource == riid)
		*ppvObject = this;

	if (*ppvObject) 
	{
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CMMDropSource::AddRef(void) 
{
	//ATLTRACE ("CMMDropSource::AddRef\n");
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CMMDropSource::Release(void) 
{
	//ATLTRACE ("CMMDropSource::Release\n");
	long nTemp;
	nTemp = --m_cRefCount;
	ATLASSERT(nTemp >= 0);
	if (nTemp == 0)
		delete this;

	return nTemp;
}

STDMETHODIMP CMMDropSource::QueryContinueDrag(
	/* [in] */ BOOL fEscapePressed,
	/* [in] */ DWORD grfKeyState)
{
	//ATLTRACE("CMMDropSource::QueryContinueDrag %d:%d \n", fEscapePressed, grfKeyState);
	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;
	if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON))) 
	{
		m_bDropped = true;
		return DRAGDROP_S_DROP;
	}

	return S_OK;
}

STDMETHODIMP CMMDropSource::GiveFeedback(
	/* [in] */ DWORD dwEffect) 
{
	//ATLTRACE("CMMDropSource::GiveFeedback\n");
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	return S_OK;
}