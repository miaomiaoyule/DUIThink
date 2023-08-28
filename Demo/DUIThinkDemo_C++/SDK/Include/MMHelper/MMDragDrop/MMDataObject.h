#ifndef __MM_DATAOBJECT_H__
#define __MM_DATAOBJECT_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API IEnumFormatEtc : public IEnumFORMATETC
{
public:
	IEnumFormatEtc(const FormatEtcArray& ArrFE);
	IEnumFormatEtc(const PFormatEtcArray& ArrFE);

private:
	ULONG								m_cRefCount;
	FormatEtcArray						m_pFmtEtc;
	int									m_iCur;

public:
	//IUnknown members
	STDMETHOD(QueryInterface)(REFIID, void FAR* FAR*);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	//IEnumFORMATETC members
	STDMETHOD(Next)(ULONG, LPFORMATETC, ULONG FAR *);
	STDMETHOD(Skip)(ULONG);
	STDMETHOD(Reset)(void);
	STDMETHOD(Clone)(IEnumFORMATETC FAR * FAR*);
};


//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMDataObject 
	: public IDataObject//,public IAsyncOperation
{
public:
	CMMDataObject(CMMDropSource *pDropSource);
	virtual ~CMMDataObject();

	//variant
private:
	CMMDropSource *						m_pDropSource;
	long								m_cRefCount;
	PFormatEtcArray						m_ArrFormatEtc;
	PStgMediumArray						m_StgMedium;

	//method
public:
	void CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc);
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDataObject
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetData(
		/* [unique][in] */ FORMATETC __RPC_FAR *pformatetcIn,
		/* [out] */ STGMEDIUM __RPC_FAR *pmedium);

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere(
		/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
		/* [out][in] */ STGMEDIUM __RPC_FAR *pmedium);

	virtual HRESULT STDMETHODCALLTYPE QueryGetData(
		/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc);

	virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
		/* [unique][in] */ FORMATETC __RPC_FAR *pformatectIn,
		/* [out] */ FORMATETC __RPC_FAR *pformatetcOut);

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetData(
		/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
		/* [unique][in] */ STGMEDIUM __RPC_FAR *pmedium,
		/* [in] */ BOOL fRelease);

	virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(
		/* [in] */ DWORD dwDirection,
		/* [out] */ IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc);

	virtual HRESULT STDMETHODCALLTYPE DAdvise(
		/* [in] */ FORMATETC __RPC_FAR *pformatetc,
		/* [in] */ DWORD advf,
		/* [unique][in] */ IAdviseSink __RPC_FAR *pAdvSink,
		/* [out] */ DWORD __RPC_FAR *pdwConnection);

	virtual HRESULT STDMETHODCALLTYPE DUnadvise(
		/* [in] */ DWORD dwConnection);

	virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(
		/* [out] */ IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise);
};

//////////////////////////////////////////////////////////////////////////

#endif