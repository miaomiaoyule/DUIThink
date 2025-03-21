#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_DRAGDROP_H__
#define __MM_DRAGDROP_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#if (NTDDI_VERSION >= NTDDI_VISTA)
class MMHELPER_API CMMDragDrop : public IDropTarget
{
public:
	CMMDragDrop();
	~CMMDragDrop();

	//variant
private:
	CComPtr<IDropTargetHelper>			m_pIDropTargetHelper = nullptr;
	CComPtr<IDataObject>				m_pIDataObject = nullptr;
	HWND								m_hWnd = nullptr;
	HRESULT								m_hResOleInit = E_FAIL;
	long								m_cRef;

	//method
public:
	void Register(HWND hWnd);
	void UnRegister();

	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
	{
		static const QITAB qit[] =
		{
			QITABENT(CMMDragDrop, IDropTarget),
			{ 0 },
		};

		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&m_cRef);
		if (!cRef)
			delete this;
		return cRef;
	}

	HRESULT GetDragDropHelper(REFIID riid, void **ppv);
	HRESULT SetDropTip(IDataObject *pIDataObject, DROPIMAGETYPE type, PCWSTR pszMsg, PCWSTR pszInsert);
	IDataObject * GetDataObject();
	IFACEMETHODIMP DragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect);
	IFACEMETHODIMP DragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect);
	IFACEMETHODIMP DragLeave();
	IFACEMETHODIMP Drop(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect);

protected:
	virtual void OnWinDragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect) = 0;
	virtual void OnWinDragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect) = 0;
	virtual void OnWinDragLeave() = 0;
	virtual void OnWinDrop(IDataObject *pIDataObject, POINTL pt, DWORD *pdwEffect) = 0;
	virtual HRESULT OnWinDropError(IDataObject *pIDataObject);

	HRESULT SetBlob(IDataObject *pIDataObject, CLIPFORMAT cf, const void *pvBlob, UINT cbBlob);
	bool QueryDrop(DWORD dwKeyState, LPDWORD pdwEffect);

	//static
public:
	static CLIPFORMAT RegisterFormat(LPCTSTR lpszFormat)
	{
		return static_cast<CLIPFORMAT>(::RegisterClipboardFormat(lpszFormat));
	}
};
#endif
//////////////////////////////////////////////////////////////////////////

#endif
#endif

