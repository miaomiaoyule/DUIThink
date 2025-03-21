#include "stdafx.h"
#include "MMDataObject.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
//////////////////////////////////////////////////////////////////////////
IEnumFormatEtc::IEnumFormatEtc(const FormatEtcArray& ArrFE) 
	: m_cRefCount(0)
	, m_iCur(0) 
{
	ATLTRACE("IEnumFormatEtc::IEnumFormatEtc()\n");
	for (size_t i = 0; i < ArrFE.size(); ++i)
		m_pFmtEtc.push_back(ArrFE[i]);
}

IEnumFormatEtc::IEnumFormatEtc(const PFormatEtcArray& ArrFE) 
	: m_cRefCount(0)
	, m_iCur(0)
{
	for (size_t i = 0; i < ArrFE.size(); ++i)
		m_pFmtEtc.push_back(*ArrFE[i]);
}

STDMETHODIMP  IEnumFormatEtc::QueryInterface(REFIID refiid, void FAR* FAR* ppv) 
{
	//ATLTRACE ("IEnumFormatEtc::QueryInterface()\n");
	*ppv = nullptr;
	if (IID_IUnknown == refiid || IID_IEnumFORMATETC == refiid)
		*ppv = this;

	if (*ppv)
	{
		((LPUNKNOWN)*ppv)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IEnumFormatEtc::AddRef(void) 
{
	//ATLTRACE ("IEnumFormatEtc::AddRef()\n");
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) IEnumFormatEtc::Release(void) 
{
	//ATLTRACE ("IEnumFormatEtc::Release()\n");
	long nTemp = --m_cRefCount;
	ATLASSERT(nTemp >= 0);
	if (nTemp == 0)
		delete this;

	return nTemp;
}

STDMETHODIMP IEnumFormatEtc::Next(ULONG celt, LPFORMATETC lpFormatEtc, ULONG FAR *pceltFetched)
{
	//ATLTRACE ("IEnumFormatEtc::Next()\n");
	if (pceltFetched)
		*pceltFetched = 0;

	ULONG cReturn = celt;

	if (celt <= 0 || !lpFormatEtc || (size_t)m_iCur >= m_pFmtEtc.size())
		return S_FALSE;

	if (!pceltFetched && celt != 1) // pceltFetched can be nullptr only for 1 item request
		return S_FALSE;

	while (m_iCur < (int)m_pFmtEtc.size() && cReturn > 0)
	{
		*lpFormatEtc++ = m_pFmtEtc[m_iCur++];
		--cReturn;
	}
	if (pceltFetched)
		*pceltFetched = celt - cReturn;

	return (cReturn == 0) ? S_OK : S_FALSE;
}

STDMETHODIMP IEnumFormatEtc::Skip(ULONG celt) 
{
	//ATLTRACE ("IEnumFormatEtc::Skip()\n");
	if ((size_t)(m_iCur + int(celt)) >= m_pFmtEtc.size())
		return S_FALSE;
	m_iCur += celt;

	return S_OK;
}

STDMETHODIMP IEnumFormatEtc::Reset(void) 
{
	//ATLTRACE ("IEnumFormatEtc::Reset()\n");
	m_iCur = 0;
	return S_OK;
}

STDMETHODIMP IEnumFormatEtc::Clone(IEnumFORMATETC FAR * FAR*ppCloneEnumFormatEtc)
{
	//ATLTRACE ("IEnumFormatEtc::Clone()\n");
	if (!ppCloneEnumFormatEtc)
		return E_POINTER;

	IEnumFormatEtc *newEnum = new IEnumFormatEtc(m_pFmtEtc);
	if (!newEnum)
		return E_OUTOFMEMORY;

	newEnum->AddRef();
	newEnum->m_iCur = m_iCur;
	*ppCloneEnumFormatEtc = newEnum;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CMMDataObject::CMMDataObject(CMMDropSource *pDropSource)
	: m_cRefCount(0)
	, m_pDropSource(pDropSource) 
{
}

CMMDataObject::~CMMDataObject() 
{
	for (size_t i = 0; i < m_StgMedium.size(); ++i) 
	{
		ReleaseStgMedium(m_StgMedium[i]);
		delete m_StgMedium[i];
	}
	for (size_t j = 0; j < m_ArrFormatEtc.size(); ++j)
	{
		delete m_ArrFormatEtc[j];
	}

	return;
}

STDMETHODIMP CMMDataObject::QueryInterface(/* [in] */ REFIID riid,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = nullptr;
	if (IID_IUnknown == riid || IID_IDataObject == riid)
		*ppvObject = this;

	if (nullptr != *ppvObject) 
	{
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CMMDataObject::AddRef(void) 
{
	//ATLTRACE ("CMMDataObject::AddRef\n");
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CMMDataObject::Release(void) 
{
	//ATLTRACE ("CMMDataObject::Release\n");
	long nTemp;
	nTemp = --m_cRefCount;
	if (nTemp == 0)
		delete this;

	return nTemp;
}

STDMETHODIMP CMMDataObject::GetData(
	/* [unique][in] */ FORMATETC __RPC_FAR *pformatetcIn,
	/* [out] */ STGMEDIUM __RPC_FAR *pmedium)
{
	//ATLTRACE ("CMMDataObject::c_str\n");
	if (!pformatetcIn || !pmedium)
		return E_INVALIDARG;

	pmedium->hGlobal = nullptr;
	ATLASSERT(m_StgMedium.size() == m_ArrFormatEtc.size());
	for (size_t i = 0; i < m_ArrFormatEtc.size(); ++i) 
	{
		if (pformatetcIn->tymed & m_ArrFormatEtc[i]->tymed &&
			pformatetcIn->dwAspect == m_ArrFormatEtc[i]->dwAspect &&
			pformatetcIn->cfFormat == m_ArrFormatEtc[i]->cfFormat) 
		{
			CopyMedium(pmedium, m_StgMedium[i], m_ArrFormatEtc[i]);
			return S_OK;
		}
	}

	return DV_E_FORMATETC;
}

STDMETHODIMP CMMDataObject::GetDataHere(
	/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
	/* [out][in] */ STGMEDIUM __RPC_FAR *pmedium) 
{
	//ATLTRACE ("CMMDataObject::GetDataHere\n");

	return E_NOTIMPL;
}

STDMETHODIMP CMMDataObject::QueryGetData(
	/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc) 
{
	//ATLTRACE ("CMMDataObject::QueryGetData\n");
	if (!pformatetc)
		return E_INVALIDARG;

	//support others if needed DVASPECT_THUMBNAIL  //DVASPECT_ICON   //DVASPECT_DOCPRINT
	if (!(DVASPECT_CONTENT & pformatetc->dwAspect))
		return (DV_E_DVASPECT);
	HRESULT  hr = DV_E_TYMED;
	for (size_t i = 0; i < m_ArrFormatEtc.size(); ++i) 
	{
		if (pformatetc->tymed & m_ArrFormatEtc[i]->tymed) 
		{
			if (pformatetc->cfFormat == m_ArrFormatEtc[i]->cfFormat)
				return S_OK;
			else
				hr = DV_E_CLIPFORMAT;
		}
		else
			hr = DV_E_TYMED;
	}

	return hr;
}

STDMETHODIMP CMMDataObject::GetCanonicalFormatEtc(
	/* [unique][in] */ FORMATETC __RPC_FAR *pformatectIn,
	/* [out] */ FORMATETC __RPC_FAR *pformatetcOut) 
{
	//ATLTRACE ("CMMDataObject::GetCanonicalFormatEtc\n");
	if (!pformatetcOut)
		return E_INVALIDARG;

	return DATA_S_SAMEFORMATETC;
}

STDMETHODIMP CMMDataObject::SetData(
	/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
	/* [unique][in] */ STGMEDIUM __RPC_FAR *pmedium,
	/* [in] */ BOOL fRelease) 
{
	//ATLTRACE ("CMMDataObject::SetData\n");
	if (!pformatetc || !pmedium)
		return E_INVALIDARG;

	ATLASSERT(pformatetc->tymed == pmedium->tymed);

	for (size_t i = 0; i < m_ArrFormatEtc.size(); ++i)
	{
		if (pformatetc->tymed & m_ArrFormatEtc[i]->tymed &&
			pformatetc->dwAspect == m_ArrFormatEtc[i]->dwAspect &&
			pformatetc->cfFormat == m_ArrFormatEtc[i]->cfFormat)
		{
			::ReleaseStgMedium(m_StgMedium[i]);
			if (fRelease)
				*m_StgMedium[i] = *pmedium;
			else
				CopyMedium(m_StgMedium[i], pmedium, pformatetc);

			return S_OK;
		}
	}
	FORMATETC* fetc = new FORMATETC;
	STGMEDIUM* pStgMed = new STGMEDIUM;

	if (!fetc || !pStgMed)
		return E_OUTOFMEMORY;

	ZeroMemory(fetc, sizeof(FORMATETC));
	ZeroMemory(pStgMed, sizeof(STGMEDIUM));

	*fetc = *pformatetc;
	m_ArrFormatEtc.push_back(fetc);

	if (fRelease)
		*pStgMed = *pmedium;
	else
		CopyMedium(pStgMed, pmedium, pformatetc);

	m_StgMedium.push_back(pStgMed);

	return S_OK;
}

void CMMDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc) 
{
	int nSize = GlobalSize(pMedSrc->hGlobal);
	switch (pMedSrc->tymed) 
	{
		case TYMED_HGLOBAL:
			pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, 0);
			break;
		case TYMED_GDI:
			pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, 0);
			break;
		case TYMED_MFPICT:
			pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, 0);
			break;
		case TYMED_ENHMF:
			pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, 0);
			break;
		case TYMED_FILE:
			pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, 0);
			break;
		case TYMED_ISTREAM:
			pMedDest->pstm = pMedSrc->pstm;
			pMedSrc->pstm->AddRef();
			break;
		case TYMED_ISTORAGE:
			pMedDest->pstg = pMedSrc->pstg;
			pMedSrc->pstg->AddRef();
			break;
		case TYMED_NULL:
		default:
			break;
	}

	pMedDest->tymed = pMedSrc->tymed;
	pMedDest->pUnkForRelease = nullptr;
	if (pMedSrc->pUnkForRelease) 
	{
		pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
		pMedSrc->pUnkForRelease->AddRef();
	}
}

STDMETHODIMP CMMDataObject::EnumFormatEtc(
	/* [in] */ DWORD dwDirection,
	/* [out] */ IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc)
{
	//ATLTRACE ("CMMDataObject::EnumFormatEtc\n");
	if (!ppenumFormatEtc)
		return E_POINTER;

	*ppenumFormatEtc = nullptr;
	switch (dwDirection) 
	{
		case DATADIR_GET:
			*ppenumFormatEtc = new IEnumFormatEtc(m_ArrFormatEtc);
			if (!*ppenumFormatEtc)
				return E_OUTOFMEMORY;
			(*ppenumFormatEtc)->AddRef();
			break;

		case DATADIR_SET:
		default:
			return E_NOTIMPL;
			break;
	}

	return S_OK;
}

STDMETHODIMP CMMDataObject::DAdvise(
	/* [in] */ FORMATETC __RPC_FAR *pformatetc,
	/* [in] */ DWORD advf,
	/* [unique][in] */ IAdviseSink __RPC_FAR *pAdvSink,
	/* [out] */ DWORD __RPC_FAR *pdwConnection) 
{
	//ATLTRACE ("CMMDataObject::DAdvise\n");
	return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP CMMDataObject::DUnadvise(
	/* [in] */ DWORD dwConnection) 
{
	//ATLTRACE ("CMMDataObject::DUnadvise\n");
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CMMDataObject::EnumDAdvise(
	/* [out] */ IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise)
{
	//ATLTRACE ("CMMDataObject::EnumDAdvise\n");
	return OLE_E_ADVISENOTSUPPORTED;
}

#endif