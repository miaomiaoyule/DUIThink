#include "stdafx.h"
#include "MMDragDrop.h"
#include <strsafe.h>

//////////////////////////////////////////////////////////////////////////
#if (NTDDI_VERSION >= NTDDI_VISTA)
CMMDragDrop::CMMDragDrop()
{
	m_hResOleInit = OleInitialize(NULL);
	CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&m_pIDropTargetHelper));

	return;
}

CMMDragDrop::~CMMDragDrop()
{
	if (SUCCEEDED(m_hResOleInit))
	{
		OleUninitialize();
	}

	return;
}

void CMMDragDrop::Register(HWND hWnd)
{
	if (hWnd == m_hWnd) return;

	if (SUCCEEDED(m_hResOleInit))
	{
		if (SUCCEEDED(RegisterDragDrop(hWnd, this)))
		{
			m_hWnd = hWnd;
		}
	}

	return;
}

void CMMDragDrop::UnRegister()
{
	if (IsWindow(m_hWnd))
	{
		RevokeDragDrop(m_hWnd);
	}

	m_hWnd = NULL;

	return;
}

HRESULT CMMDragDrop::GetDragDropHelper(REFIID riid, void **ppv)
{
	*ppv = NULL;
	return m_pIDropTargetHelper ? m_pIDropTargetHelper->QueryInterface(riid, ppv) : E_NOINTERFACE;
}

HRESULT CMMDragDrop::SetDropTip(IDataObject *pIDataObject, DROPIMAGETYPE type, PCWSTR pszMsg, PCWSTR pszInsert)
{
	if (NULL == pIDataObject) return E_FAIL;

	DROPDESCRIPTION dd = { type };
	StringCchCopyW(dd.szMessage, ARRAYSIZE(dd.szMessage), pszMsg);
	StringCchCopyW(dd.szInsert, ARRAYSIZE(dd.szInsert), pszInsert ? pszInsert : L"");
	static CLIPFORMAT s_cfDropDescription = ::RegisterClipboardFormat(CFSTR_DROPDESCRIPTION);
	return SetBlob(pIDataObject, s_cfDropDescription, &dd, sizeof(DROPDESCRIPTION));
}

IDataObject * CMMDragDrop::GetDataObject()
{
	return m_pIDataObject;
}

IFACEMETHODIMP CMMDragDrop::DragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (m_pIDropTargetHelper)
	{
		m_pIDropTargetHelper->DragEnter(m_hWnd, pIDataObject, (LPPOINT)&pt, *pdwEffect);
	}

	pIDataObject ? pIDataObject->QueryInterface(&m_pIDataObject) : E_NOINTERFACE;

	if (QueryDrop(dwKeyState, pdwEffect))
	{
		OnWinDragEnter(m_pIDataObject, dwKeyState, pt, pdwEffect);
	}

	return S_OK;
}

IFACEMETHODIMP CMMDragDrop::DragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (m_pIDropTargetHelper)
	{
		m_pIDropTargetHelper->DragOver((LPPOINT)&pt, *pdwEffect);
	}
	if (QueryDrop(dwKeyState, pdwEffect))
	{
		OnWinDragOver(dwKeyState, pt, pdwEffect);
	}

	return S_OK;
}

IFACEMETHODIMP CMMDragDrop::DragLeave()
{
	if (m_pIDropTargetHelper)
	{
		m_pIDropTargetHelper->DragLeave();
	}

	OnWinDragLeave();
	
	m_pIDataObject = NULL;

	return S_OK;
}

IFACEMETHODIMP CMMDragDrop::Drop(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (m_pIDropTargetHelper)
	{
		m_pIDropTargetHelper->Drop(pIDataObject, (LPPOINT)&pt, *pdwEffect);
	}
	if (QueryDrop(dwKeyState, pdwEffect))
	{
		OnWinDrop(m_pIDataObject, pt, pdwEffect);
	}

	m_pIDataObject = NULL;

	return S_OK;
}

HRESULT CMMDragDrop::OnWinDropError(IDataObject *pIDataObject)
{
	OutputDebugString(L"OnDropError.\n");
	return S_OK;
}

HRESULT CMMDragDrop::SetBlob(IDataObject *pIDataObject, CLIPFORMAT cf, const void *pvBlob, UINT cbBlob)
{
	void *pv = GlobalAlloc(GPTR, cbBlob);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		CopyMemory(pv, pvBlob, cbBlob);
		FORMATETC fmte = { cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		
		// The STGMEDIUM structure is used to define how to handle a global memory transfer.
		// This structure includes a flag, tymed, which indicates the medium
		// to be used, and a union comprising pointers and a handle for getting whichever
		// medium is specified in tymed.
		STGMEDIUM medium = {};
		medium.tymed = TYMED_HGLOBAL;
		medium.hGlobal = pv;
		hr = pIDataObject->SetData(&fmte, &medium, TRUE);
		if (FAILED(hr))
		{
			GlobalFree(pv);
		}
	}
	else
	{
		OnWinDropError(pIDataObject);
	}

	return hr;
}

bool CMMDragDrop::QueryDrop(DWORD dwKeyState, LPDWORD pdwEffect)
{
	DWORD dwEffect = *pdwEffect;
	//CTRL+SHIFT  -- DROPEFFECT_LINK
	//CTRL        -- DROPEFFECT_COPY
	//SHIFT       -- DROPEFFECT_MOVE
	//no modifier -- DROPEFFECT_MOVE or whatever is allowed by src
	*pdwEffect = (dwKeyState & MK_CONTROL) ?
		((dwKeyState & MK_SHIFT) ? DROPEFFECT_LINK : DROPEFFECT_COPY) :
		((dwKeyState & MK_SHIFT) ? DROPEFFECT_MOVE : 0);
	if (*pdwEffect == 0) 
	{
		// No modifier keys used by user while dragging. 
		if (DROPEFFECT_COPY & dwEffect)
			*pdwEffect = DROPEFFECT_COPY;
		else if (DROPEFFECT_MOVE & dwEffect)
			*pdwEffect = DROPEFFECT_MOVE;
		else if (DROPEFFECT_LINK & dwEffect)
			*pdwEffect = DROPEFFECT_LINK;
		else
			*pdwEffect = DROPEFFECT_NONE;
	}
	else 
	{
		// Check if the drag source application allows the drop effect desired by user.
		// The drag source specifies this in DoDragDrop
		if (0 == (*pdwEffect & dwEffect))
			*pdwEffect = DROPEFFECT_NONE;
	}

	return (DROPEFFECT_NONE == *pdwEffect) ? false : true;
}
#endif
//////////////////////////////////////////////////////////////////////////