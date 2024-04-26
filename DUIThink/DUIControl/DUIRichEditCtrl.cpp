#include "StdAfx.h"
#include "DUIRichEditCtrl.h"
#include <textserv.h>

//////////////////////////////////////////////////////////////////////////
#define ID_RICH_UNDO					101
#define ID_RICH_CUT						102
#define ID_RICH_COPY					103
#define ID_RICH_PASTE					104
#define ID_RICH_CLEAR					105
#define ID_RICH_SELECTALL				106
#define ID_RICH_REDO					107

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
	0x8d33f740,
	0xcf58,
	0x11ce,
	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG lPerInchX)
{
	return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, lPerInchX);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG lPerInchY)
{
	return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, lPerInchY);
}

HRESULT InitDefaultCharFormat(CDUIRichEditCtrl *pOwnerCtrl, CHARFORMAT2W *pCharFormat)
{
	if (NULL == pOwnerCtrl || NULL == pOwnerCtrl->GetWndManager() || NULL == pCharFormat) return S_FALSE;

	ZeroMemory(pCharFormat, sizeof(CHARFORMAT2W));

	//font
	CDUIFontBase *pFontDefault = CDUIGlobal::GetInstance()->GetFontResDefault();
	HFONT hFont = pOwnerCtrl->GetFontNormal();
	NULL == hFont ? hFont = pOwnerCtrl->GetFont() : NULL;
	NULL == hFont && pFontDefault ? hFont = pFontDefault->GetHandle() : NULL;

	LOGFONT lf = {};
	::GetObject(hFont, sizeof(LOGFONT), &lf);

	//color
	ARGB dwColor = pOwnerCtrl->GetTextColorNormal();
	0 == dwColor ? dwColor = pOwnerCtrl->GetTextColor() : 0;

	pCharFormat->cbSize = sizeof(CHARFORMAT2W);
	pCharFormat->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
	LONG yPixPerInch = GetDeviceCaps(pOwnerCtrl->GetWndManager()->GetWndDC(), LOGPIXELSY);
	pCharFormat->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
	pCharFormat->yOffset = 0;
	pCharFormat->dwEffects = 0;
	pCharFormat->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
	if (lf.lfWeight >= FW_BOLD) pCharFormat->dwEffects |= CFE_BOLD;
	if (lf.lfItalic) pCharFormat->dwEffects |= CFE_ITALIC;
	if (lf.lfUnderline) pCharFormat->dwEffects |= CFE_UNDERLINE;
	pCharFormat->bCharSet = lf.lfCharSet;
	pCharFormat->bPitchAndFamily = lf.lfPitchAndFamily;

#ifdef _UNICODE
	_tcscpy(pCharFormat->szFaceName, lf.lfFaceName);
#else
	//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
	MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pCharFormat->szFaceName, LF_FACESIZE);
#endif

	return S_OK;
}

HRESULT InitDefaultParaFormat(CDUIRichEditCtrl *pOwnerCtrl, PARAFORMAT2 *pParaFormat)
{
	ZeroMemory(pParaFormat, sizeof(PARAFORMAT2));
	pParaFormat->cbSize = sizeof(PARAFORMAT2);
	pParaFormat->dwMask = PFM_ALL;
	pParaFormat->wAlignment = PFA_LEFT;
	pParaFormat->cTabCount = 1;
	pParaFormat->rgxTabs[0] = lDefaultTab;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
class CDUITextHost : public ITextHost
{
public:
	CDUITextHost();
	virtual ~CDUITextHost();

	//variant
private:
	CDUIRichEditCtrl *					m_pOwnerCtrl = NULL;
	ITextServices *						m_pTextService = NULL;			//pointer to Text Services object
	ULONG								m_uRefCount = 0;				//Reference Count

	bool								m_bInplaceActive = true;		//Whether control is inplace active
	CDUIRect							m_rcClient;						//Client Rect for this control
	CDUISize							m_szExtent;						//Extent array
	CHARFORMAT2W						m_CharFormat = {};				//Default character format
	PARAFORMAT2							m_ParaFormat = {};				//Default paragraph format
	LONG								m_laccelpos = -1;				//Accelerator position

	//method
public:
	HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject) override;
	ULONG _stdcall AddRef(void) override;
	ULONG _stdcall Release(void) override;

	bool Init(CDUIRichEditCtrl *pOwnerCtrl);
	ITextServices * GetTextServices(void) { return m_pTextService; }
	CDUIRect GetClientRect() { return m_rcClient; }
	void SetClientRect(CDUIRect rcClient);
	void SetFont(HFONT hFont);
	void SetColor(DWORD dwColor);

	WORD GetDefaultAlign();
	void SetDefaultAlign(WORD wNewAlign);
	LONG GetDefaultLeftIndent();
	void SetDefaultLeftIndent(LONG lNewIndent);
	bool GetActiveState() { return m_bInplaceActive; }
	LONG SetAccelPos(LONG laccelpos);
	void SetEnabled(bool bEnabled);
	void SetCharFormat(CHARFORMAT2W &cf);
	void SetParaFormat(PARAFORMAT2 &pf);
	HRESULT OnTxInPlaceDeactivate();
	HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
	bool OnSetCursor(CDUIPoint pt);

	// -----------------------------
	//	ITextHost interface
	// -----------------------------
	virtual HDC TxGetDC();
	virtual INT TxReleaseDC(HDC hdc);
	virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags);
	virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
	virtual BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw);
	virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual void TxViewChange(BOOL fUpdate);
	virtual BOOL TxCreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
	virtual BOOL TxShowCaret(BOOL fShow);
	virtual BOOL TxSetCaretPos(INT x, INT y);
	virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
	virtual void TxKillTimer(UINT idTimer);
	virtual void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
	virtual void TxSetCapture(BOOL fCapture);
	virtual void TxSetFocus();
	virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
	virtual BOOL TxScreenToClient(LPPOINT lppt);
	virtual BOOL TxClientToScreen(LPPOINT lppt);
	virtual HRESULT TxActivate(LONG * plOldState);
	virtual HRESULT TxDeactivate(LONG lNewState);
	virtual HRESULT TxGetClientRect(LPRECT prc);
	virtual HRESULT TxGetViewInset(LPRECT prc);
	virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF);
	virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
	virtual COLORREF TxGetSysColor(int nIndex);
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT TxGetMaxLength(DWORD *plength);
	virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
	virtual HRESULT TxGetPasswordChar(TCHAR *pch);
	virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
	virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
	virtual HRESULT OnTxCharFormatChange(const CHARFORMATW * pcf);
	virtual HRESULT OnTxParaFormatChange(const PARAFORMAT * ppf);
	virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	virtual HRESULT TxNotify(DWORD iNotify, void *pv);
	virtual HIMC TxImmGetContext(void);
	virtual void TxImmReleaseContext(HIMC himc);
	virtual HRESULT TxGetSelectionBarWidth(LONG *m_lSelBarWidth);
};

CDUITextHost::CDUITextHost()
{
	return;
}

CDUITextHost::~CDUITextHost()
{
	if (m_pTextService)
	{
		m_pTextService->OnTxInPlaceDeactivate();
		m_pTextService->Release();
	}

	m_pTextService = NULL;

	return;
}

HRESULT CDUITextHost::QueryInterface(REFIID riid, void **ppvObject)
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown)
		|| IsEqualIID(riid, IID_ITextHost))
	{
		AddRef();
		*ppvObject = (ITextHost *)this;
		hr = S_OK;
	}

	return hr;
}

ULONG CDUITextHost::AddRef(void)
{
	return ++m_uRefCount;
}

ULONG CDUITextHost::Release(void)
{
	ULONG c_Refs = --m_uRefCount;

	if (c_Refs == 0)
	{
		delete this;
	}

	return c_Refs;
}

bool CDUITextHost::Init(CDUIRichEditCtrl *pOwnerCtrl)
{
	if (NULL == pOwnerCtrl) return false;

	IUnknown *pUnk = NULL;
	HRESULT hRes = S_FALSE;

	m_pOwnerCtrl = pOwnerCtrl;

	// Initialize Reference count
	m_uRefCount = 1;

	do
	{
		//Create and cache CHARFORMAT for this control
		if (FAILED(InitDefaultCharFormat(pOwnerCtrl, &m_CharFormat))) return false;

		//Create and cache PARAFORMAT for this control
		if (FAILED(InitDefaultParaFormat(pOwnerCtrl, &m_ParaFormat))) return false;

		//edit controls are rich by default
		m_bInplaceActive = TRUE;

		long lStyle = m_pOwnerCtrl->GetStyle();
		if (0 == (lStyle & ES_LEFT))
		{
			(lStyle & ES_CENTER) ? m_ParaFormat.wAlignment = PFA_CENTER : NULL;
			(lStyle & ES_RIGHT) ? m_ParaFormat.wAlignment = PFA_RIGHT : NULL;
		}

		//create
		PCreateTextServices pFuncCreateTextServices = NULL;
#ifdef _UNICODE
		HMODULE hModule = LoadLibrary(_T("Msftedit.dll"));
#else
		HMODULE hModule = LoadLibrary(_T("Riched20.dll"));
#endif
		if (hModule)
		{
			pFuncCreateTextServices = (PCreateTextServices)GetProcAddress(hModule, "CreateTextServices");
		}
		if (pFuncCreateTextServices)
		{
			hRes = pFuncCreateTextServices(NULL, this, &pUnk);
		}
		if (FAILED(hRes)) return false;

		//interface
		hRes = pUnk->QueryInterface(IID_ITextServices, (void **)&m_pTextService);
		pUnk->Release();

		if (FAILED(hRes)) return false;

		return true;

	} while (false);

	return false;
}

void CDUITextHost::SetClientRect(CDUIRect rcClient)
{
	if (NULL == m_pOwnerCtrl) return;

	m_rcClient = rcClient;

	LONG lPerInchX = ::GetDeviceCaps(m_pOwnerCtrl->GetWndManager()->GetWndDC(), LOGPIXELSX);
	LONG lPerInchY = ::GetDeviceCaps(m_pOwnerCtrl->GetWndManager()->GetWndDC(), LOGPIXELSY);
	m_szExtent.cx = DXtoHimetricX(m_rcClient.GetWidth(), lPerInchX);
	m_szExtent.cy = DYtoHimetricY(m_rcClient.GetHeight(), lPerInchY);

	m_pTextService->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
	m_pTextService->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);

	return;
}

void CDUITextHost::SetFont(HFONT hFont)
{
	if (NULL == m_pOwnerCtrl
		|| NULL == m_pOwnerCtrl->GetWndManager()
		|| NULL == hFont
		|| NULL == m_pTextService) return;

	LOGFONT lf = {};
	::GetObject(hFont, sizeof(LOGFONT), &lf);

	LONG yPixPerInch = ::GetDeviceCaps(m_pOwnerCtrl->GetWndManager()->GetWndDC(), LOGPIXELSY);
	m_CharFormat.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;

	if (lf.lfWeight >= FW_BOLD) m_CharFormat.dwEffects |= CFE_BOLD;
	if (lf.lfItalic) m_CharFormat.dwEffects |= CFE_ITALIC;
	if (lf.lfUnderline) m_CharFormat.dwEffects |= CFE_UNDERLINE;
	m_CharFormat.bCharSet = lf.lfCharSet;
	m_CharFormat.bPitchAndFamily = lf.lfPitchAndFamily;

#ifdef _UNICODE
	_tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
#else
	//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
	MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE);
#endif

	m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);

	return;
}

void CDUITextHost::SetColor(DWORD dwColor)
{
	if (NULL == m_pTextService) return;

	m_CharFormat.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));

	m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);

	return;
}

WORD CDUITextHost::GetDefaultAlign()
{
	return m_ParaFormat.wAlignment;
}

void CDUITextHost::SetDefaultAlign(WORD wNewAlign)
{
	if (NULL == m_pTextService) return;

	m_ParaFormat.wAlignment = wNewAlign;

	// Notify control of property change
	m_pTextService->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);

	return;
}

LONG CDUITextHost::GetDefaultLeftIndent()
{
	return m_ParaFormat.dxOffset;
}

void CDUITextHost::SetDefaultLeftIndent(LONG lNewIndent)
{
	if (NULL == m_pTextService)return;

	m_ParaFormat.dxOffset = lNewIndent;

	m_pTextService->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);

	return;
}

LONG CDUITextHost::SetAccelPos(LONG l_accelpos)
{
	LONG laccelposOld = l_accelpos;

	m_laccelpos = l_accelpos;

	// notify text services of property change
	m_pTextService->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

	return laccelposOld;
}

void CDUITextHost::SetEnabled(bool bEnabled)
{
	if (NULL == m_pTextService) return;

	m_CharFormat.dwMask |= CFM_COLOR | CFM_DISABLED;
	m_CharFormat.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

	if (bEnabled)
	{
		m_CharFormat.dwEffects &= ~CFE_DISABLED;
	}

	m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);

	return;
}

void CDUITextHost::SetCharFormat(CHARFORMAT2W &cf)
{
	m_CharFormat = cf;

	return;
}

void CDUITextHost::SetParaFormat(PARAFORMAT2 &pf)
{
	m_ParaFormat = pf;

	return;
}

HRESULT	CDUITextHost::OnTxInPlaceDeactivate()
{
	HRESULT hr = m_pTextService->OnTxInPlaceDeactivate();

	if (SUCCEEDED(hr))
	{
		m_bInplaceActive = FALSE;
	}

	return hr;
}

HRESULT	CDUITextHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
	m_bInplaceActive = TRUE;

	HRESULT hr = m_pTextService->OnTxInPlaceActivate(prcClient);

	if (FAILED(hr))
	{
		m_bInplaceActive = FALSE;
	}

	return hr;
}

bool CDUITextHost::OnSetCursor(CDUIPoint pt)
{
	if (NULL == m_pOwnerCtrl) return false;

	// Is this in our rectangle?
	if (m_rcClient.PtInRect(pt))
	{
		HDC hDC = m_pOwnerCtrl->GetWndManager()->GetWndDC();

		return SUCCEEDED(m_pTextService->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, hDC, NULL, &m_rcClient, pt.x, pt.y));
	}

	return false;
}

HDC CDUITextHost::TxGetDC()
{
	return m_pOwnerCtrl ? m_pOwnerCtrl->GetWndManager()->GetWndDC() : NULL;
}

int CDUITextHost::TxReleaseDC(HDC hdc)
{
	return 1;
}

BOOL CDUITextHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
	if (NULL == m_pOwnerCtrl) return false;

	if (SB_VERT == fnBar)
	{
		m_pOwnerCtrl->SetUseVertScrollBar(fShow == TRUE);

		return true;
	}
	if (SB_HORZ == fnBar)
	{
		m_pOwnerCtrl->SetUseHorizScrollBar(fShow == TRUE);

		return true;
	}
	if (SB_BOTH == fnBar)
	{
		m_pOwnerCtrl->EnableScrollBar(fShow == TRUE, fShow == TRUE);
	}

	return true;
}

BOOL CDUITextHost::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
	if (SB_VERT == fuSBFlags)
	{
		m_pOwnerCtrl->SetUseVertScrollBar(fuArrowflags != ESB_DISABLE_BOTH);

		return true;
	}
	if (SB_HORZ == fuSBFlags)
	{
		m_pOwnerCtrl->SetUseHorizScrollBar(fuArrowflags != ESB_DISABLE_BOTH);

		return true;
	}
	if (SB_BOTH == fuSBFlags)
	{
		m_pOwnerCtrl->EnableScrollBar(fuArrowflags != ESB_DISABLE_BOTH, fuArrowflags != ESB_DISABLE_BOTH);
	}

	return TRUE;
}

BOOL CDUITextHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	if (NULL == m_pOwnerCtrl) return false;

	m_pOwnerCtrl->NeedRefreshView();

	return TRUE;
}

BOOL CDUITextHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
	if (NULL == m_pOwnerCtrl) return false;

	CDUISize szScrollPos = m_pOwnerCtrl->GetScrollPos();

	if (SB_VERT == fnBar)
	{
		szScrollPos.cx = nPos;
	}
	else if (SB_HORZ == fnBar)
	{
		szScrollPos.cy = nPos;
	}

	m_pOwnerCtrl->SetScrollPos(szScrollPos);

	return TRUE;
}

void CDUITextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	if (NULL == m_pOwnerCtrl) return;

	m_pOwnerCtrl->Invalidate();

	return;
}

void CDUITextHost::TxViewChange(BOOL fUpdate)
{
	if (NULL == m_pOwnerCtrl) return;

	m_pOwnerCtrl->Invalidate();

	return;
}

BOOL CDUITextHost::TxCreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return false;

	return m_pOwnerCtrl->GetWndManager()->CreateCaret(hBmp, nWidth, nHeight);
}

BOOL CDUITextHost::TxShowCaret(BOOL fShow)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return false;

	m_pOwnerCtrl->GetWndManager()->ShowCaret(TRUE == fShow);

	return true;
}

BOOL CDUITextHost::TxSetCaretPos(INT x, INT y)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return false;

	m_pOwnerCtrl->GetWndManager()->SetCaretPos(CDUIPoint(x, y));

	return true;
}

BOOL CDUITextHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return false;

	return m_pOwnerCtrl->SetTimer(idTimer, uTimeout);
}

void CDUITextHost::TxKillTimer(UINT idTimer)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return;

	m_pOwnerCtrl->KillTimer(idTimer);

	return;
}

void CDUITextHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
{
	return;
}

void CDUITextHost::TxSetCapture(BOOL fCapture)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return;

	if (fCapture)
	{
		m_pOwnerCtrl->GetWndManager()->SetCapture();
	}
	else
	{
		m_pOwnerCtrl->GetWndManager()->ReleaseCapture();
	}

	return;
}

void CDUITextHost::TxSetFocus()
{
	if (NULL == m_pOwnerCtrl) return;

	m_pOwnerCtrl->SetFocus();

	return;
}

void CDUITextHost::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	::SetCursor(hcur);

	return;
}

BOOL CDUITextHost::TxScreenToClient(LPPOINT lppt)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return false;

	return ::ScreenToClient(m_pOwnerCtrl->GetWndManager()->GetWndHandle(), lppt);
}

BOOL CDUITextHost::TxClientToScreen(LPPOINT lppt)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetWndManager()) return false;

	return ::ClientToScreen(m_pOwnerCtrl->GetWndManager()->GetWndHandle(), lppt);
}

HRESULT CDUITextHost::TxActivate(LONG *plOldState)
{
	return S_OK;
}

HRESULT CDUITextHost::TxDeactivate(LONG lNewState)
{
	return S_OK;
}

HRESULT CDUITextHost::TxGetClientRect(LPRECT prc)
{
	*prc = GetClientRect();

	return NOERROR;
}

HRESULT CDUITextHost::TxGetViewInset(LPRECT prc)
{
	prc->left = prc->right = prc->top = prc->bottom = 0;

	return NOERROR;
}

HRESULT CDUITextHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	*ppCF = &m_CharFormat;

	return NOERROR;
}

HRESULT CDUITextHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	*ppPF = &m_ParaFormat;

	return NOERROR;
}

COLORREF CDUITextHost::TxGetSysColor(int nIndex)
{
	DWORD dwColor = ::GetSysColor(nIndex);

	return dwColor;
}

HRESULT CDUITextHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	if (NULL == m_pOwnerCtrl) return NOERROR;

	*pstyle = m_pOwnerCtrl->IsTransparent() ? TXTBACK_TRANSPARENT : TXTBACK_OPAQUE;

	return NOERROR;
}

HRESULT CDUITextHost::TxGetMaxLength(DWORD *pLength)
{
	if (NULL == m_pOwnerCtrl) return NOERROR;

	*pLength = m_pOwnerCtrl->GetMaxChar();

	return NOERROR;
}

HRESULT CDUITextHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
	if (NULL == m_pOwnerCtrl) return NOERROR;

	DWORD dwStyle = WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_DISABLENOSCROLL;

	*pdwScrollBar = (m_pOwnerCtrl->GetStyle() & dwStyle);

	return NOERROR;
}

HRESULT CDUITextHost::TxGetPasswordChar(TCHAR *pch)
{
	if (NULL == m_pOwnerCtrl) return NOERROR;

	*pch = m_pOwnerCtrl->GetPasswordChar();

	return NOERROR;
}

HRESULT CDUITextHost::TxGetAcceleratorPos(LONG *pcp)
{
	*pcp = m_laccelpos;

	return S_OK;
}

HRESULT CDUITextHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
	return S_OK;
}

HRESULT CDUITextHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
	return S_OK;
}

HRESULT CDUITextHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
	if (NULL == m_pOwnerCtrl) return NOERROR;

	DWORD dwProperties = TXTBIT_AUTOWORDSEL | ES_AUTOHSCROLL;
	long lStyle = m_pOwnerCtrl->GetStyle();

	if (m_pOwnerCtrl->IsRichText()) dwProperties = TXTBIT_RICHTEXT;
	if (lStyle & ES_READONLY) dwProperties |= TXTBIT_READONLY;
	if (lStyle & ES_PASSWORD) dwProperties |= TXTBIT_USEPASSWORD;

	if (lStyle & ES_MULTILINE)
	{
		dwProperties |= (ES_AUTOVSCROLL | TXTBIT_MULTILINE | TXTBIT_WORDWRAP);
		dwProperties &= ~ES_AUTOHSCROLL;
	}
	if (0 == (lStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}
	if (m_pOwnerCtrl->IsAllowBeep())
	{
		dwProperties |= TXTBIT_ALLOWBEEP;
	}
	if (m_pOwnerCtrl->IsSaveSelection())
	{
		dwProperties |= TXTBIT_SAVESELECTION;
	}

	*pdwBits = (dwProperties & dwMask);

	return NOERROR;
}

HRESULT CDUITextHost::TxNotify(DWORD iNotify, void *pv)
{
	if (iNotify == EN_REQUESTRESIZE)
	{
		REQRESIZE *preqsz = (REQRESIZE *)pv;
		CDUIRect rcClient = GetClientRect();
		rcClient.right = rcClient.left + preqsz->rc.right;
		rcClient.bottom = rcClient.top + preqsz->rc.bottom;
		SetClientRect(rcClient);
	}

	m_pOwnerCtrl->OnTxNotify(iNotify, pv);

	return S_OK;
}

HIMC CDUITextHost::TxImmGetContext(void)
{
	return NULL;
}

void CDUITextHost::TxImmReleaseContext(HIMC himc)
{
	//::ImmReleaseContext( hwnd, himc );
}

HRESULT CDUITextHost::TxGetExtent(LPSIZEL lpExtent)
{
	*lpExtent = m_szExtent;

	return S_OK;
}

HRESULT	CDUITextHost::TxGetSelectionBarWidth(LONG *plSelBarWidth)
{
	if (NULL == m_pOwnerCtrl || NULL == m_pOwnerCtrl->GetVertScrollBar()) return S_FALSE;

	*plSelBarWidth = m_pOwnerCtrl->GetVertScrollBar()->GetFixedWidth();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIRichEditCtrl)
MMImplement_ClassName(CDUIRichEditCtrl)

CDUIRichEditCtrl::CDUIRichEditCtrl(void)
{
}

CDUIRichEditCtrl::~CDUIRichEditCtrl(void)
{
	if (m_pWndManager)
	{
		m_pWndManager->RemovePreMessagePtr(this);
	}
	if (m_pTextHost)
	{
		m_pTextHost->Release();
	}

	m_pTextHost = NULL;

	return;
}

LRESULT CDUIRichEditCtrl::OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (NULL == m_pWndManager) return 0;

	if (uMsg == WM_IME_COMPOSITION)
	{
		//解决微软输入法位置异常的问题
		HIMC hIMC = ImmGetContext(GetWndManager()->GetWndHandle());
		if (hIMC)
		{
			//Set composition window position near caret position
			POINT point;
			GetCaretPos(&point);

			COMPOSITIONFORM Composition;
			Composition.dwStyle = CFS_POINT;
			Composition.ptCurrentPos.x = point.x;
			Composition.ptCurrentPos.y = point.y;
			ImmSetCompositionWindow(hIMC, &Composition);

			ImmReleaseContext(GetWndManager()->GetWndHandle(), hIMC);
		}

		return 0;
	}
	if (IsCaptured() || IsFocused())
	{
		LRESULT lResult = 0;
		bool bDispath = (WM_KEYFIRST <= uMsg && uMsg <= WM_KEYLAST)
			|| uMsg == WM_CHAR
			|| uMsg == WM_IME_CHAR
			|| IsCaptured();

		if (false == bDispath && (WM_MOUSEFIRST <= uMsg && uMsg <= WM_MOUSELAST))
		{
			CDUIPoint pt(lParam);
			CDUIRect rcPos = GetAbsoluteRect();
			bDispath = rcPos.PtInRect(pt);
		}
		if (bDispath)
		{
			HRESULT hRes = TxSendMessage(uMsg, wParam, lParam, &lResult);
		}

		return lResult;
	}

#ifdef _USEIMM
	else if (uMsg == WM_IME_STARTCOMPOSITION)
	{
		if (IsFocused()) {
			POINT ptCaret = { 0 };
			::GetCaretPos(&ptCaret);
			HIMC hMic = ::ImmGetContext(GetManager()->GetPaintWindow());
			COMPOSITIONFORM cpf;
			cpf.dwStyle = CFS_FORCE_POSITION;
			cpf.ptCurrentPos.x = ptCaret.x + m_pTwh->GetCaretWidth();
			cpf.ptCurrentPos.y = ptCaret.y;
			::ImmSetCompositionWindow(hMic, &cpf);

			HFONT hFont = GetManager()->GetFont(m_iFont);
			LOGFONT lf;
			::GetObject(hFont, sizeof(LOGFONT), &lf);
			::ImmSetCompositionFont(hMic, &lf);

			::ImmReleaseContext(GetManager()->GetPaintWindow(), hMic);
		}
		bWasHandled = false;
		return 0;
	}
#endif

	return 0;
}

bool CDUIRichEditCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;
	
	if (pAttributeObj == &m_AttributeText)
	{
		SetText(m_AttributeText.GetValue());

		return true;
	}
	if (pAttributeObj == &m_AttributeRichText)
	{
		SetRichText(IsRichText());

		return true;
	}
	if (pAttributeObj == &m_AttributeReadOnly)
	{
		SetReadOnly(IsReadOnly());

		return true;
	}
	if (pAttributeObj == &m_AttributePasswordChar)
	{
		SetPasswordChar(GetPasswordChar());

		return true;
	}
	if (pAttributeObj == &m_AttributeAllowBeep)
	{
		SetAllowBeep(IsAllowBeep());

		return true;
	}
	if (pAttributeObj == &m_AttributeSaveSelection)
	{
		SetSaveSelection(IsSaveSelection());

		return true;
	}
	if (pAttributeObj == &m_AttributeTransparent)
	{
		SetTransparent(IsTransparent());

		return true;
	}
	if (pAttributeObj == &m_AttributeMaxChar)
	{
		SetMaxChar(GetMaxChar());

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStyleNormal
		|| pAttributeObj == &m_AttributeTextStyleHot
		|| pAttributeObj == &m_AttributeTextStyleFocus
		|| pAttributeObj == &m_AttributeTextStyleDisabled)
	{
		ConstructTextStyle();
	}

	return false;
}

LPVOID CDUIRichEditCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIRichEditCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIRichEditCtrl::GetDescribe() const
{
	return Dui_Ctrl_RichEdit;
}

void CDUIRichEditCtrl::RefreshView()
{
	//child
	RefreshScrollBar();

	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcRange = GetLayoutRangeOfItem();
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || false == pChild->IsVisible()) continue;

		pChild->OnDuiSize(rcRange);
	}

	CDUIControlBase::RefreshView();

	//rich
	if (m_pTextHost)
	{
		CDUIRect rcTextPadding = GetTextPadding();

		rcRange.left += rcTextPadding.left;
		rcRange.right -= rcTextPadding.right;
		rcRange.top += rcTextPadding.top;
		rcRange.bottom -= rcTextPadding.bottom;
		rcRange.CheckRect();
		m_pTextHost->SetClientRect(rcRange);

		if (m_pVertScrollBarCtrl)
		{
			LONG lWidth = rcRange.GetWidth();
			LONG lHeight = 0;
			SIZEL szExtent = { -1, -1 };
			m_pTextHost->GetTextServices()->TxGetNaturalSize(
				DVASPECT_CONTENT,
				GetWndManager()->GetWndDC(),
				NULL,
				NULL,
				TXTNS_FITTOCONTENT,
				&szExtent,
				&lWidth,
				&lHeight);

			UpdateScrollBar(CDUISize(0, max(0, lHeight - rcRange.GetHeight())));
		}
	}

	return;
}

void CDUIRichEditCtrl::SetEnabled(bool bEnable)
{
	CDUIContainerCtrl::SetEnabled(bEnable);

	if (m_pTextHost)
	{
		m_pTextHost->SetColor(IsEnabled() ? GetTextColor() : GetTextColorDisabled());
	}

	return;
}

void CDUIRichEditCtrl::SetScrollPos(SIZE szPos, bool bMsg)
{
	CDUIContainerCtrl::SetScrollPos(szPos, bMsg);

	szPos = GetScrollPos();

	WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, szPos.cx);
	TxSendMessage(WM_HSCROLL, wParam, 0L, 0);

	wParam = MAKEWPARAM(SB_THUMBPOSITION, szPos.cy);
	TxSendMessage(WM_VSCROLL, wParam, 0L, 0);

	return;
}

void CDUIRichEditCtrl::LineUp()
{
	TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);

	return;
}

void CDUIRichEditCtrl::LineDown()
{
	TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);

	return;
}

void CDUIRichEditCtrl::PageUp()
{
	TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);

	return;
}

void CDUIRichEditCtrl::PageDown()
{
	TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);

	return;
}

void CDUIRichEditCtrl::HomeUp()
{
	TxSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);

	return;
}

void CDUIRichEditCtrl::EndDown()
{
	TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);

	return;
}

void CDUIRichEditCtrl::LineLeft()
{
	TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);

	return;
}

void CDUIRichEditCtrl::LineRight()
{
	TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);

	return;
}

void CDUIRichEditCtrl::PageLeft()
{
	TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);

	return;
}

void CDUIRichEditCtrl::PageRight()
{
	TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);

	return;
}

void CDUIRichEditCtrl::HomeLeft()
{
	TxSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);

	return;
}

void CDUIRichEditCtrl::EndRight()
{
	TxSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);

	return;
}

bool CDUIRichEditCtrl::IsRichText()
{
	return m_AttributeRichText.GetValue();
}

void CDUIRichEditCtrl::SetRichText(bool bRichText)
{
#ifndef DUI_DESIGN
	if (bRichText == IsRichText()) return;
#endif

	m_AttributeRichText.SetValue(bRichText);

	if (m_pTextHost && m_pTextHost->GetTextServices())
	{
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, IsRichText() ? TXTBIT_RICHTEXT : 0);
	}

	return;
}

bool CDUIRichEditCtrl::IsReadOnly()
{
	return m_AttributeReadOnly.GetValue();
}

void CDUIRichEditCtrl::SetReadOnly(bool bReadOnly)
{
#ifndef DUI_DESIGN
	if (bReadOnly == IsReadOnly()) return;
#endif

	m_AttributeReadOnly.SetValue(bReadOnly);

	if (m_pTextHost && m_pTextHost->GetTextServices())
	{
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_READONLY, IsReadOnly() ? TXTBIT_READONLY : 0);
	}

	return;
}

bool CDUIRichEditCtrl::IsNumberOnly()
{
	return m_AttributeNumberOnly.GetValue();
}

void CDUIRichEditCtrl::SetNumberOnly(bool bNumberOnly)
{
	if (bNumberOnly == IsNumberOnly()) return;

	m_AttributeNumberOnly.SetValue(bNumberOnly);

	return;
}

bool CDUIRichEditCtrl::IsPasswordMode()
{
	return m_AttributePasswordMode.GetValue();
}

void CDUIRichEditCtrl::SetPasswordMode(bool bPasswordMode)
{
	if (bPasswordMode == IsPasswordMode()) return;

	m_AttributePasswordMode.SetValue(bPasswordMode);

	return;
}

bool CDUIRichEditCtrl::IsAllowBeep()
{
	return m_AttributeAllowBeep.GetValue();
}

void CDUIRichEditCtrl::SetAllowBeep(bool bAllowBeep)
{
#ifndef DUI_DESIGN
	if (bAllowBeep == IsAllowBeep()) return;
#endif

	m_AttributeAllowBeep.SetValue(bAllowBeep);

	if (m_pTextHost && m_pTextHost->GetTextServices())
	{
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, IsAllowBeep() ? TXTBIT_ALLOWBEEP : 0);
	}

	return;
}

bool CDUIRichEditCtrl::IsSaveSelection()
{
	return m_AttributeSaveSelection.GetValue();
}

void CDUIRichEditCtrl::SetSaveSelection(bool bSaveSelection)
{
#ifndef DUI_DESIGN
	if (bSaveSelection == IsSaveSelection()) return;
#endif

	m_AttributeSaveSelection.SetValue(bSaveSelection);

	if (m_pTextHost && m_pTextHost->GetTextServices())
	{
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, IsSaveSelection() ? TXTBIT_SAVESELECTION : 0);
	}

	return;
}

bool CDUIRichEditCtrl::IsTransparent()
{
	return m_AttributeTransparent.GetValue();
}

void CDUIRichEditCtrl::SetTransparent(bool bTransparent)
{
#ifndef DUI_DESIGN
	if (bTransparent == IsTransparent()) return;
#endif

	m_AttributeTransparent.SetValue(bTransparent);

	if (m_pTextHost && m_pTextHost->GetTextServices())
	{
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
	}

	return;
}

bool CDUIRichEditCtrl::IsAutoSelAll()
{
	return m_AttributeAutoSelAll.GetValue();
}

void CDUIRichEditCtrl::SetAutoSelAll(bool bAutoSelAll)
{
	if (bAutoSelAll == IsAutoSelAll()) return;

	m_AttributeAutoSelAll.SetValue(bAutoSelAll);

	return;
}

UINT CDUIRichEditCtrl::GetMaxChar()
{
	return m_AttributeMaxChar.GetValue();
}

void CDUIRichEditCtrl::SetMaxChar(UINT uMax)
{
#ifndef DUI_DESIGN
	if (uMax == GetMaxChar()) return;
#endif

	m_AttributeMaxChar.SetValue(uMax);

	if (m_pTextHost && m_pTextHost->GetTextServices())
	{
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
	}

	return;
}

int CDUIRichEditCtrl::GetNumberMinLimit()
{
	return m_AttributeNumberMinLimit.GetValue();
}

void CDUIRichEditCtrl::SetNumberMinLimit(int nMinNum)
{
	if (nMinNum == GetNumberMinLimit()) return;

	m_AttributeNumberMinLimit.SetValue(nMinNum);

	return;
}

int CDUIRichEditCtrl::GetNumberMaxLimit()
{
	return m_AttributeNumberMaxLimit.GetValue();
}

void CDUIRichEditCtrl::SetNumberMaxLimit(int nMaxNum)
{
	if (nMaxNum == GetNumberMaxLimit()) return;

	m_AttributeNumberMaxLimit.SetValue(nMaxNum);

	return;
}

TCHAR CDUIRichEditCtrl::GetPasswordChar()
{
	return *m_AttributePasswordChar.GetValue();
}

void CDUIRichEditCtrl::SetPasswordChar(TCHAR chPasswordChar)
{
#ifndef DUI_DESIGN
	if (chPasswordChar == GetPasswordChar()) return;
#endif

	m_AttributePasswordChar.SetValue(CMMString(chPasswordChar));

	if (m_pTextHost && m_pTextHost->GetTextServices())
	{
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, (GetPasswordChar() != 0) ? TXTBIT_USEPASSWORD : 0);
	}

	Invalidate();

	return;
}

CMMString CDUIRichEditCtrl::GetTipText()
{
	return m_AttributeTipText.GetValue();
}

void CDUIRichEditCtrl::SetTipText(LPCTSTR lpszTipText)
{
	if (lpszTipText == GetTipText()) return;

	m_AttributeTipText.SetValue(lpszTipText);

	return;
}

CMMString CDUIRichEditCtrl::GetText()
{
	long lLen = GetTextLength(GTL_DEFAULT);
	GETTEXTEX gt;
	gt.flags = GT_DEFAULT;

#ifdef _UNICODE
	gt.cb = sizeof(TCHAR) * (lLen + 1);
	gt.codepage = 1200;
	CMMString strText('\0', lLen + 1);
#else
	gt.cb = sizeof(TCHAR) * (lLen * 2 + 1);
	gt.codepage = CP_ACP;
	CMMString strText('\0', lLen * 2 + 1);
#endif

	gt.lpDefaultChar = NULL;
	gt.lpUsedDefChar = NULL;
	TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)strText.c_str(), 0);

	return strText.c_str();
}

CMMString CDUIRichEditCtrl::GetText(long nStartChar, long nEndChar) const
{
	TEXTRANGEW tr = { 0 };
	tr.chrg.cpMin = nStartChar;
	tr.chrg.cpMax = nEndChar;

	CMMString strText('\0', nEndChar - nStartChar + 1);
	tr.lpstrText = (LPTSTR)strText.c_str();
	TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);

	return strText;
}

bool CDUIRichEditCtrl::SetText(LPCTSTR lpszText)
{
	if (NULL == m_pTextHost) return false;

#ifndef DUI_DESIGN
	if (0 == lstrcmp(lpszText, m_AttributeText.GetValue())) return false;
#endif

	m_AttributeText.SetValue(lpszText);

	SetSel(0, -1);
	ReplaceSel(lpszText, FALSE);

	return true;
}

HFONT CDUIRichEditCtrl::GetFont()
{
	return m_AttributeTextStyleNormal.GetFont();
}

ARGB CDUIRichEditCtrl::GetTextColor()
{
	return m_AttributeTextStyleNormal.GetTextColor();
}

RECT CDUIRichEditCtrl::GetTextPadding()
{
	return DuiDpiScaleCtrl(m_AttributeTextPadding.GetValue());
}

void CDUIRichEditCtrl::SetTextPadding(RECT rcPadding)
{
	if (rcPadding == GetTextPadding()) return;

	m_AttributeTextPadding.SetValue(rcPadding);

	Invalidate();

	return;
}

ARGB CDUIRichEditCtrl::GetTextColorNormal()
{
	return m_AttributeTextStyleNormal.GetTextColor();
}

ARGB CDUIRichEditCtrl::GetTextColorHot()
{
	return m_AttributeTextStyleHot.GetTextColor();
}

ARGB CDUIRichEditCtrl::GetTextColorFocus()
{
	return m_AttributeTextStyleFocus.GetTextColor();
}

ARGB CDUIRichEditCtrl::GetTextColorDisabled()
{
	return m_AttributeTextStyleDisabled.GetTextColor();
}

ARGB CDUIRichEditCtrl::GetTextColorTipNormal()
{
	return m_AttributeTextStyleTipTextNormal.GetTextColor();
}

ARGB CDUIRichEditCtrl::GetTextColorTipHot()
{
	return m_AttributeTextStyleTipTextHot.GetTextColor();
}

HFONT CDUIRichEditCtrl::GetFontNormal()
{
	return m_AttributeTextStyleNormal.GetFont();
}

HFONT CDUIRichEditCtrl::GetFontHot()
{
	return m_AttributeTextStyleHot.GetFont();
}

HFONT CDUIRichEditCtrl::GetFontFocus()
{
	return m_AttributeTextStyleFocus.GetFont();
}

HFONT CDUIRichEditCtrl::GetFontDisabled()
{
	return m_AttributeTextStyleDisabled.GetFont();
}

HFONT CDUIRichEditCtrl::GetFontTipNormal()
{
	return m_AttributeTextStyleTipTextNormal.GetFont();
}

HFONT CDUIRichEditCtrl::GetFontTipHot()
{
	return m_AttributeTextStyleTipTextHot.GetFont();
}

tagDuiTextStyle CDUIRichEditCtrl::GetTextStyleNormal()
{
	return m_AttributeTextStyleNormal.GetTextStyle();
}

tagDuiTextStyle CDUIRichEditCtrl::GetTextStyleHot()
{
	return m_AttributeTextStyleHot.GetTextStyle();
}

tagDuiTextStyle CDUIRichEditCtrl::GetTextStyleFocus()
{
	return m_AttributeTextStyleFocus.GetTextStyle();
}

tagDuiTextStyle CDUIRichEditCtrl::GetTextStyleDisabled()
{
	return m_AttributeTextStyleDisabled.GetTextStyle();
}

tagDuiTextStyle CDUIRichEditCtrl::GetTextStyleTipNormal()
{
	return m_AttributeTextStyleTipTextNormal.GetTextStyle();
}

tagDuiTextStyle CDUIRichEditCtrl::GetTextStyleTipHot()
{
	return m_AttributeTextStyleTipTextHot.GetTextStyle();
}

void CDUIRichEditCtrl::SetTextStyleNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleNormal.SetTextStyle(TextStyle);

	return;
}

void CDUIRichEditCtrl::SetTextStyleHot(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleHot.SetTextStyle(TextStyle);

	return;
}

void CDUIRichEditCtrl::SetTextStyleFocus(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleFocus.SetTextStyle(TextStyle);

	return;
}

void CDUIRichEditCtrl::SetTextStyleDisabled(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleDisabled.SetTextStyle(TextStyle);

	return;
}

void CDUIRichEditCtrl::SetTextStyleTipNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleTipTextNormal.SetTextStyle(TextStyle);

	return;
}

void CDUIRichEditCtrl::SetTextStyleTipHot(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleTipTextHot.SetTextStyle(TextStyle);

	return;
}

tagDuiImageSection CDUIRichEditCtrl::GetImageSectionNormal()
{
	return m_AttributeImageNormal.GetImageSection();
}

tagDuiImageSection CDUIRichEditCtrl::GetImageSectionHot()
{
	return m_AttributeImageHot.GetImageSection();
}

tagDuiImageSection CDUIRichEditCtrl::GetImageSectionFocus()
{
	return m_AttributeImageFocus.GetImageSection();
}

tagDuiImageSection CDUIRichEditCtrl::GetImageSectionDisabled()
{
	return m_AttributeImageDisabled.GetImageSection();
}

void CDUIRichEditCtrl::SetImageSectionNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIRichEditCtrl::SetImageSectionHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIRichEditCtrl::SetImageSectionFocus(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageFocus.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIRichEditCtrl::SetImageSectionDisabled(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDisabled.SetImageSection(ImageSection);

	Invalidate();

	return;
}

long CDUIRichEditCtrl::GetStyle()
{
	long lStyle = ES_AUTOHSCROLL;
	if (IsUseHorizScrollBar()) lStyle |= ES_DISABLENOSCROLL | WS_HSCROLL;
	if (IsUseVertScrollBar()) lStyle |= ES_DISABLENOSCROLL | WS_VSCROLL;
	if (IsReadOnly()) lStyle |= ES_READONLY;
	if (IsPasswordMode()) lStyle |= ES_PASSWORD;

	tagDuiTextStyle TextStyle = {};
	if (false == m_AttributeTextStyleNormal.empty()) TextStyle = GetTextStyleNormal();
	if (TextStyle.dwTextStyle & DT_LEFT)
	{
		lStyle &= ~(ES_CENTER | ES_RIGHT);
		lStyle |= ES_LEFT;
	}
	if (TextStyle.dwTextStyle & DT_CENTER)
	{
		lStyle &= ~(ES_LEFT | ES_RIGHT);
		lStyle |= ES_CENTER;
	}
	if (TextStyle.dwTextStyle & DT_CENTER)
	{
		lStyle &= ~(ES_LEFT | ES_CENTER);
		lStyle |= ES_RIGHT;
	}
	if (TextStyle.IsWordBreak())
	{
		lStyle |= ES_MULTILINE | ES_AUTOVSCROLL;
		lStyle &= ~ES_AUTOHSCROLL;
	}

	return lStyle;
}

void CDUIRichEditCtrl::SetFont(LPCTSTR lpszFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	if (NULL == m_pTextHost) return;

	LOGFONT lf = {};
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcsncpy(lf.lfFaceName, lpszFontName, LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if (bBold) lf.lfWeight += FW_BOLD;
	if (bUnderline) lf.lfUnderline = TRUE;
	if (bItalic) lf.lfItalic = TRUE;

	HFONT hFont = ::CreateFontIndirect(&lf);
	if (NULL == hFont) return;

	m_pTextHost->SetFont(hFont);

	MMSafeDeleteObject(hFont);

	return;
}

long CDUIRichEditCtrl::GetTextLength(DWORD dwFlags) const
{
	GETTEXTLENGTHEX textLenEx;
	textLenEx.flags = dwFlags;

#ifdef _UNICODE
	textLenEx.codepage = 1200;
#else
	textLenEx.codepage = CP_ACP;
#endif

	LRESULT lResult = 0;
	TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);

	return (long)lResult;
}

bool CDUIRichEditCtrl::GetModify() const
{
	if (NULL == m_pTextHost) return false;

	LRESULT lResult;
	TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

void CDUIRichEditCtrl::SetModify(bool bModified) const
{
	TxSendMessage(EM_SETMODIFY, bModified, 0, 0);

	return;
}

void CDUIRichEditCtrl::GetSel(CHARRANGE &cr) const
{
	TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);

	return;
}

void CDUIRichEditCtrl::GetSel(long &nStartChar, long &nEndChar) const
{
	CHARRANGE cr;
	TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);

	nStartChar = cr.cpMin;
	nEndChar = cr.cpMax;

	return;
}

CMMString CDUIRichEditCtrl::GetSelText() const
{
	if (NULL == m_pTextHost) return _T("");

	CHARRANGE cr;
	cr.cpMin = cr.cpMax = 0;
	TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);

	CMMString strText('\0', cr.cpMax - cr.cpMin + 1);
	TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)strText.c_str(), 0);

	return strText;
}

int CDUIRichEditCtrl::SetSel(CHARRANGE &cr)
{
	LRESULT lResult;
	TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult);

	return (int)lResult;
}

int CDUIRichEditCtrl::SetSel(long nStartChar, long nEndChar)
{
	CHARRANGE cr;
	cr.cpMin = nStartChar;
	cr.cpMax = nEndChar;
	LRESULT lResult;
	TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult);

	return (int)lResult;
}

void CDUIRichEditCtrl::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
{
#ifdef _UNICODE		
	TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText, 0);
#else
	CMMString strText = CA2CT(lsszNewText);
	TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)strText.c_str(), 0);
#endif

	return;
}

void CDUIRichEditCtrl::ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
{
	TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText, 0);

	return;
}

int CDUIRichEditCtrl::SetSelAll()
{
	return SetSel(0, -1);
}

int CDUIRichEditCtrl::SetSelNone()
{
	return SetSel(-1, 0);
}

bool CDUIRichEditCtrl::GetZoom(int &nNum, int &nDen) const
{
	LRESULT lResult;
	TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::SetZoom(int nNum, int nDen)
{
	if (nNum < 0 || nNum > 64) return false;
	if (nDen < 0 || nDen > 64) return false;

	LRESULT lResult;
	TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::SetZoomOff()
{
	LRESULT lResult;
	TxSendMessage(EM_SETZOOM, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

WORD CDUIRichEditCtrl::GetSelectionType() const
{
	LRESULT lResult;
	TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);

	return (WORD)lResult;
}

bool CDUIRichEditCtrl::GetAutoUrlDetect() const
{
	LRESULT lResult;
	TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::SetAutoUrlDetect(bool bAutoDetect)
{
	LRESULT lResult;
	TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);

	return (BOOL)lResult == FALSE;
}

DWORD CDUIRichEditCtrl::GetEventMask() const
{
	LRESULT lResult;
	TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);

	return (DWORD)lResult;
}

DWORD CDUIRichEditCtrl::SetEventMask(DWORD dwEventMask)
{
	LRESULT lResult;
	TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);

	return (DWORD)lResult;
}

void CDUIRichEditCtrl::HideSelection(bool bHide, bool bChangeStyle)
{
	TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);

	return;
}

void CDUIRichEditCtrl::ScrollCaret()
{
	TxSendMessage(EM_SCROLLCARET, 0, 0, 0);

	return;
}

int CDUIRichEditCtrl::InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo)
{
	int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
	ReplaceSel(lpstrText, bCanUndo);

	return nRet;
}

int CDUIRichEditCtrl::AppendText(LPCTSTR lpstrText, bool bCanUndo)
{
	int nRet = SetSel(-1, -1);
	ReplaceSel(lpstrText, bCanUndo);

	return nRet;
}

DWORD CDUIRichEditCtrl::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);

	return (DWORD)lResult;
}

bool CDUIRichEditCtrl::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
	if (NULL == m_pTextHost) return false;

	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);

	if ((BOOL)lResult == TRUE)
	{
		CHARFORMAT2W cfw;
		cfw.cbSize = sizeof(CHARFORMAT2W);
		TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);

		m_pTextHost->SetCharFormat(cfw);

		return true;
	}

	return false;
}

DWORD CDUIRichEditCtrl::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);

	return (DWORD)lResult;
}

bool CDUIRichEditCtrl::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
	if (NULL == m_pTextHost) return false;

	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::SetWordCharFormat(CHARFORMAT2 &cf)
{
	if (NULL == m_pTextHost) return false;

	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lResult);

	return (BOOL)lResult == TRUE;
}

DWORD CDUIRichEditCtrl::GetParaFormat(PARAFORMAT2 &pf) const
{
	pf.cbSize = sizeof(PARAFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);

	return (DWORD)lResult;
}

bool CDUIRichEditCtrl::SetParaFormat(PARAFORMAT2 &pf)
{
	if (NULL == m_pTextHost) return false;

	pf.cbSize = sizeof(PARAFORMAT2);
	LRESULT lResult;
	TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);

	if ((BOOL)lResult == TRUE)
	{
		m_pTextHost->SetParaFormat(pf);

		return true;
	}

	return false;
}

bool CDUIRichEditCtrl::CanUndo()
{
	if (NULL == m_pTextHost) return false;

	LRESULT lResult;
	TxSendMessage(EM_CANUNDO, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::CanRedo()
{
	if (NULL == m_pTextHost) return false;

	LRESULT lResult;
	TxSendMessage(EM_CANREDO, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::CanPaste()
{
	if (NULL == m_pTextHost) return false;

	LRESULT lResult;
	TxSendMessage(EM_CANPASTE, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::Redo()
{
	if (NULL == m_pTextHost) return false;

	LRESULT lResult;
	TxSendMessage(EM_REDO, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

bool CDUIRichEditCtrl::Undo()
{
	if (NULL == m_pTextHost) return false;

	LRESULT lResult;
	TxSendMessage(EM_UNDO, 0, 0, &lResult);

	return (BOOL)lResult == TRUE;
}

void CDUIRichEditCtrl::Clear()
{
	TxSendMessage(WM_CLEAR, 0, 0, 0);

	return;
}

void CDUIRichEditCtrl::Copy()
{
	TxSendMessage(WM_COPY, 0, 0, 0);

	return;
}

void CDUIRichEditCtrl::Cut()
{
	TxSendMessage(WM_CUT, 0, 0, 0);

	return;
}

void CDUIRichEditCtrl::Paste()
{
	TxSendMessage(WM_PASTE, 0, 0, 0);

	return;
}

int CDUIRichEditCtrl::GetLineCount() const
{
	if (NULL == m_pTextHost) return 0;

	LRESULT lResult;
	TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);

	return (int)lResult;
}

CMMString CDUIRichEditCtrl::GetLine(int nIndex, int nMaxLength) const
{
	CMMString strText('\0', nMaxLength + 1);

	LPTSTR lpszText = (LPTSTR)strText.c_str();
	*(LPWORD)lpszText = (WORD)nMaxLength;
	TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpszText, 0);

	return strText;
}

int CDUIRichEditCtrl::LineIndex(int nLine) const
{
	LRESULT lResult;
	TxSendMessage(EM_LINEINDEX, nLine, 0, &lResult);

	return (int)lResult;
}

int CDUIRichEditCtrl::LineLength(int nLine) const
{
	LRESULT lResult;
	TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);

	return (int)lResult;
}

bool CDUIRichEditCtrl::LineScroll(int nLines, int nChars)
{
	LRESULT lResult;
	TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);

	return (BOOL)lResult == TRUE;
}

CDUIPoint CDUIRichEditCtrl::GetCharPos(long lChar) const
{
	CDUIPoint pt;
	TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0);

	return pt;
}

long CDUIRichEditCtrl::LineFromChar(long nIndex) const
{
	if (NULL == m_pTextHost) return 0L;

	LRESULT lResult;
	TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);

	return (long)lResult;
}

CDUIPoint CDUIRichEditCtrl::PosFromChar(UINT nChar) const
{
	POINTL pt;
	TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0);

	return CDUIPoint(pt.x, pt.y);
}

int CDUIRichEditCtrl::CharFromPos(CDUIPoint pt) const
{
	POINTL ptl = { pt.x, pt.y };
	if (NULL == m_pTextHost) return 0;

	LRESULT lResult;
	TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);

	return (int)lResult;
}

void CDUIRichEditCtrl::EmptyUndoBuffer()
{
	TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0);

	return;
}

UINT CDUIRichEditCtrl::SetUndoLimit(UINT nLimit)
{
	if (NULL == m_pTextHost) return 0;

	LRESULT lResult;
	TxSendMessage(EM_SETUNDOLIMIT, (WPARAM)nLimit, 0, &lResult);

	return (UINT)lResult;
}

long CDUIRichEditCtrl::StreamIn(int nFormat, EDITSTREAM &es)
{
	if (NULL == m_pTextHost) return 0L;

	LRESULT lResult;
	TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);

	return (long)lResult;
}

long CDUIRichEditCtrl::StreamOut(int nFormat, EDITSTREAM &es)
{
	if (NULL == m_pTextHost) return 0L;

	LRESULT lResult;
	TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);

	return (long)lResult;
}

bool CDUIRichEditCtrl::SetDropAcceptFile(bool bAccept)
{
	LRESULT lResult;
	TxSendMessage(EM_SETEVENTMASK, 0, ENM_DROPFILES | ENM_LINK, // ENM_CHANGE| ENM_CORRECTTEXT | ENM_DRAGDROPDONE | ENM_DROPFILES | ENM_IMECHANGE | ENM_LINK | ENM_OBJECTPOSITIONS | ENM_PROTECTED | ENM_REQUESTRESIZE | ENM_SCROLL | ENM_SELCHANGE | ENM_UPDATE,
		&lResult);

	return (BOOL)lResult == FALSE;
}

IDropTarget * CDUIRichEditCtrl::GetTxDropTarget()
{
	if (NULL == m_pTextHost) return NULL;

	IDropTarget *pdt = NULL;
	if (m_pTextHost->GetTextServices()->TxGetDropTarget(&pdt) == NOERROR) return pdt;

	return NULL;
}

IRichEditOle * CDUIRichEditCtrl::GetIRichEditOle()
{
	IRichEditOle *pRichItem = NULL;
	TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichItem, 0);

	return pRichItem;
}

bool CDUIRichEditCtrl::SetOLECallback(IRichEditOleCallback *pCallback)
{
	TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)pCallback, 0);

	return true;
}

bool CDUIRichEditCtrl::InsertNotGIFImage(LPCTSTR szFilePath, DWORD dwUserData)
{
	if (szFilePath == NULL) return false;

	IRichEditOle *pRichEditOle = GetIRichEditOle();
	if (pRichEditOle)
	{
		//CImageDataObject imageDataObject;
		//imageDataObject.InsertNotGIFImage(pRichEditOle, szFilePath, dwUserData);

		pRichEditOle->Release();
		pRichEditOle = NULL;

		return true;
	}

	return false;
}

bool CDUIRichEditCtrl::InsertGIFImage(LPCTSTR szFilePath, DWORD dwUserData)
{
	IRichEditOle *pRichEditOle = GetIRichEditOle();
	if (pRichEditOle && szFilePath != NULL)
	{
		//CGifOleObject::InsertGifRichEdit(this, GetManager()->GetPaintWindow(), pRichEditOle, szFilePath, dwUserData);
		pRichEditOle->Release();
		pRichEditOle = NULL;

		return true;
	}

	return false;
}


HRESULT CDUIRichEditCtrl::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
	if (m_pTextHost)
	{
		return m_pTextHost->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
	}

	return S_FALSE;
}

bool CDUIRichEditCtrl::OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (m_pTextHost)
	{
		m_pTextHost->OnSetCursor(pt);
	}

	return true;
}

bool CDUIRichEditCtrl::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseEnter(pt, Msg)) return false;
	if (NULL == m_pTextHost) return false;
	if (IsFocused()) return true;

	ConstructTextStyle();

	Invalidate();

	return true;
}

void CDUIRichEditCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseLeave(pt, Msg);

	if (NULL == m_pTextHost || IsFocused()) return;

	ConstructTextStyle();

	Invalidate();

	return;
}

bool CDUIRichEditCtrl::OnDuiSetFocus()
{
	if (false == __super::OnDuiSetFocus()) return false;
	if (NULL == m_pTextHost) return false;

	m_pTextHost->OnTxInPlaceActivate(NULL);

	//font color
	ConstructTextStyle();

	m_pTextHost->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);

	//autoselall
	if (IsAutoSelAll())
	{
		SetSelAll();
	}

	return true;
}

bool CDUIRichEditCtrl::OnDuiKillFocus()
{
	if (false == __super::OnDuiKillFocus()) return false;
	if (NULL == m_pTextHost) return false;

	m_pTextHost->OnTxInPlaceActivate(NULL);

	ConstructTextStyle();

	m_pTextHost->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);

	return true;
}

void CDUIRichEditCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (NULL == GetWndManager() || m_pTextHost) return;

	//create
	CMMString strText = m_AttributeText.GetValue();
	m_pTextHost = new CDUITextHost();

	if (m_pTextHost)
	{
		m_pTextHost->Init(this);
		m_pTextHost->GetTextServices()->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);

		LRESULT lResult;
		m_pTextHost->GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
		m_pTextHost->OnTxInPlaceActivate(NULL);
		m_pWndManager->AddPreMessagePtr(this);

		ConstructTextStyle();

		SetText(strText);
	}

	return;
}

void CDUIRichEditCtrl::OnDuiWndManagerDetach()
{
	__super::OnDuiWndManagerDetach();

	if (m_pWndManager)
	{
		m_pWndManager->RemovePreMessagePtr(this);
	}

	return;
}

void CDUIRichEditCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	__super::OnDuiTimer(uTimerID, Msg);

	if (m_pTextHost)
	{
		m_pTextHost->GetTextServices()->TxSendMessage(WM_TIMER, Msg.wParam, Msg.lParam, 0);
	}

	return;
}

LRESULT CDUIRichEditCtrl::OnDuiContextMenu(const DuiMessage &Msg)
{
	if (NULL == m_pWndManager) return 0;

	__super::OnDuiContextMenu(Msg);

	LRESULT lRes = 0;
	TxSendMessage(WM_CONTEXTMENU, Msg.wParam, Msg.lParam, &lRes);

	//menu
	HMENU hPopMenu = CreatePopupMenu();
	AppendMenu(hPopMenu, 0, ID_RICH_UNDO, _T("撤销(&U)"));
	AppendMenu(hPopMenu, 0, ID_RICH_REDO, _T("重做(&R)"));
	AppendMenu(hPopMenu, MF_SEPARATOR, 0, _T(""));
	AppendMenu(hPopMenu, 0, ID_RICH_CUT, _T("剪切(&X)"));
	AppendMenu(hPopMenu, 0, ID_RICH_COPY, _T("复制(&C)"));
	AppendMenu(hPopMenu, 0, ID_RICH_PASTE, _T("粘帖(&V)"));
	AppendMenu(hPopMenu, 0, ID_RICH_CLEAR, _T("清空(&L)"));
	AppendMenu(hPopMenu, MF_SEPARATOR, 0, _T(""));
	AppendMenu(hPopMenu, 0, ID_RICH_SELECTALL, _T("全选(&A)"));

	//初始化菜单项
	UINT uUndo = (CanUndo() ? 0 : MF_GRAYED);
	::EnableMenuItem(hPopMenu, ID_RICH_UNDO, MF_BYCOMMAND | uUndo);
	UINT uRedo = (CanRedo() ? 0 : MF_GRAYED);
	EnableMenuItem(hPopMenu, ID_RICH_REDO, MF_BYCOMMAND | uRedo);
	UINT uSel = ((GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
	UINT uReadonly = IsReadOnly() ? MF_GRAYED : 0;
	EnableMenuItem(hPopMenu, ID_RICH_CUT, MF_BYCOMMAND | uSel | uReadonly);
	EnableMenuItem(hPopMenu, ID_RICH_COPY, MF_BYCOMMAND | uSel);
	EnableMenuItem(hPopMenu, ID_RICH_CLEAR, MF_BYCOMMAND | uSel | uReadonly);
	EnableMenuItem(hPopMenu, ID_RICH_PASTE, MF_BYCOMMAND | uReadonly);

	CDUIPoint ptScreen = Msg.ptMouse;
	::ClientToScreen(m_pWndManager->GetWndHandle(), &ptScreen);
	TrackPopupMenu(hPopMenu, TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, 0, m_pWndManager->GetWndHandle(), NULL);
	DestroyMenu(hPopMenu);

	return lRes;
}

LRESULT CDUIRichEditCtrl::OnDuiCommand(const DuiMessage &Msg)
{
	__super::OnDuiCommand(Msg);

	UINT uCmd = (UINT)Msg.wParam;
	switch (uCmd)
	{
		case ID_RICH_UNDO:
		{
			Undo();

			break;
		}
		case ID_RICH_REDO:
		{
			Redo();

			break;
		}
		case ID_RICH_CUT:
		{
			Cut();

			break;
		}
		case ID_RICH_COPY:
		{
			Copy();

			break;
		}
		case ID_RICH_PASTE:
		{
			Paste();

			break;
		}
		case ID_RICH_CLEAR:
		{
			Clear();

			break;
		}
		case ID_RICH_SELECTALL:
		{
			SetSelAll();

			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

void CDUIRichEditCtrl::OnTxNotify(DWORD iNotify, void *pv)
{
	if (NULL == m_pWndManager) return;

	switch (iNotify)
	{
		case EN_DROPFILES:
		case EN_MSGFILTER:
		case EN_OLEOPFAILED:
		case EN_PROTECTED:
		case EN_SAVECLIPBOARD:
		case EN_SELCHANGE:
		case EN_STOPNOUNDO:
		case EN_LINK:
		case EN_OBJECTPOSITIONS:
		case EN_DRAGDROPDONE:
		{
			if (pv)                        // Fill out NMHDR portion of pv   
			{
				LONG nId = GetWindowLong(m_pWndManager->GetWndHandle(), GWL_ID);
				NMHDR *phdr = (NMHDR *)pv;
				phdr->hwndFrom = m_pWndManager->GetWndHandle();
				phdr->idFrom = nId;
				phdr->code = iNotify;

				if (SendMessage(m_pWndManager->GetWndHandle(), WM_NOTIFY, (WPARAM)nId, (LPARAM)pv))
				{
					//hr = S_FALSE;   
				}
			}
		}

		break;
	}

	return;
}

void CDUIRichEditCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupRichEdit, _T("RichEdit"));
	DuiCreateAttribute(m_AttributeRichText, _T("RichText"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeReadOnly, _T("ReadOnly"), _T("ReadOnly"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeNumberOnly, _T("NumberOnly"), _T("NumberOnly"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributePasswordMode, _T("PasswordMode"), _T("PasswordMode"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributePasswordChar, _T("PasswordChar"), _T("PasswordChar"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeAllowBeep, _T("AllowBeep"), _T("AllowBeep"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeSaveSelection, _T("SaveSelection"), _T("WordWrap"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTransparent, _T("Transparent"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeAutoSelAll, _T("AutoSelAll"), _T("AutoSelAll"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeMaxChar, _T("MaxChar"), _T("Max Char Count"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeNumberMinLimit, _T("NumberMinLimit"), _T("NumberMinLimit"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeNumberMaxLimit, _T("NumberMaxLimit"), _T("NumberMaxLimit"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTipText, _T("TipText"), _T("Tip Info For Empty Edit"), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeText, _T("Text"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTextStyleTipTextNormal, _T("TextStyleTipTextNormal"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTextStyleTipTextHot, _T("TextStyleTipTextHot"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTextStyleNormal, _T("TextStyleNormal"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTextStyleHot, _T("TextStyleHot"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTextStyleFocus, _T("TextStyleFocus"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTextStyleDisabled, _T("TextStyleDisabled"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeImageNormal, _T("ImageNormal"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeImageHot, _T("ImageHot"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeImageFocus, _T("ImageFocus"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeImageDisabled, _T("ImageDisabled"), _T(""), m_AttributeGroupRichEdit);
	DuiCreateAttribute(m_AttributeTextPadding, _T("TextPadding"), _T(""), m_AttributeGroupRichEdit);

	return;
}

void CDUIRichEditCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	DuiInitAttriValue(m_AttributeRichText, true);
	DuiInitAttriValue(m_AttributeTransparent, true);
	DuiInitAttriValue(m_AttributePasswordChar, _T("*"));
	DuiInitAttriValue(m_AttributeMaxChar, (32 * 1024) - 1);
	DuiInitAttriValue(m_AttributeNumberMinLimit, 0x80000000);
	DuiInitAttriValue(m_AttributeNumberMaxLimit, 0x7fffffff);

	if (false == m_AttributeCursor.IsModified())
	{
		m_AttributeCursor.SelectItem(Cursor_IBeam);
	}

	return;
}

void CDUIRichEditCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeNumberOnly, false);
	DuiInitAttriVisible(m_AttributeNumberMinLimit, false);
	DuiInitAttriVisible(m_AttributeNumberMaxLimit, false);

	return;
}

void CDUIRichEditCtrl::PaintStatusImage(HDC hDC)
{
	__super::PaintStatusImage(hDC);

	CDUIAttriImageSection* pAttribute = NULL;
	if (false == IsEnabled())
	{
		pAttribute = &m_AttributeImageDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeImageHot;
	}
	else if (IsFocused())
	{
		pAttribute = &m_AttributeImageFocus;
	}
	else
	{
		pAttribute = &m_AttributeImageNormal;
	}

	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeImageNormal : pAttribute;
	if (NULL == pAttribute) return;

	pAttribute->Draw(hDC, m_rcAbsolute, m_rcPaint);

	return;
}

void CDUIRichEditCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pTextHost) return;

	//text
	CDUIRect rcRichEdit = m_pTextHost->GetClientRect();

	//tiptext
	if (GetText().empty())
	{
		CMMString strTipText = GetTipText();
		if (strTipText.empty()) return;

		CDUIAttributeTextStyle *pAttribute = (m_nControlStatus & ControlStatus_Hot) ? &m_AttributeTextStyleTipTextHot : &m_AttributeTextStyleTipTextNormal;
		NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyleTipTextNormal : pAttribute;
		NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyleNormal : pAttribute;

		if (NULL == pAttribute) return;

		pAttribute->Draw(hDC, rcRichEdit, strTipText, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), false);

		return;
	}

	// Remember wparam is actually the hdc and lparam is the update
	// rect because this message has been preprocessed by the window.
	m_pTextHost->GetTextServices()->TxDraw(
		DVASPECT_CONTENT,  		// Draw Aspect
		/*-1*/0,				// Lindex
		NULL,					// Info for drawing optimazation
		NULL,					// target device information
		hDC,					// Draw device HDC
		NULL, 				   	// Target device HDC
		(RECTL*)&rcRichEdit,	// Bounding client rectangle
		NULL, 		            // Clipping rectangle for metafiles
		(RECT*)&m_rcPaint,		// Update rectangle
		NULL, 	   				// Call back function
		NULL,					// Call back parameter
		0);				        // What view of the object

	CDUIRect rcRange = GetLayoutRangeOfItem();
	LONG lWidth = rcRange.GetWidth();
	LONG lHeight = 0;
	SIZEL szExtent = { -1, -1 };
	m_pTextHost->GetTextServices()->TxGetNaturalSize(
		DVASPECT_CONTENT,
		m_pWndManager->GetWndDC(),
		NULL,
		NULL,
		TXTNS_FITTOCONTENT,
		&szExtent,
		&lWidth,
		&lHeight);

	if (lHeight <= rcRange.GetHeight())
	{
		//NeedRefreshView();
	}

	return;
}

void CDUIRichEditCtrl::ConstructTextStyle()
{
	if (NULL == m_pTextHost) return;

	//disabled
	if (false == IsEnabled())
	{
		CDUIAttributeTextStyle *pAttribute = &m_AttributeTextStyleDisabled;
		NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyleNormal : pAttribute;
		m_pTextHost->SetFont(pAttribute ? pAttribute->GetFont() : NULL);
		m_pTextHost->SetColor(pAttribute ? pAttribute->GetTextColor() : 0);

		return;
	}

	//focus
	if (IsFocused())
	{
		CDUIAttributeTextStyle *pAttribute = &m_AttributeTextStyleFocus;
		NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeTextStyleNormal : pAttribute;
		m_pTextHost->SetFont(pAttribute ? pAttribute->GetFont() : NULL);
		m_pTextHost->SetColor(pAttribute ? pAttribute->GetTextColor() : 0);

		return;
	}

	//hover
	if (m_nControlStatus & ControlStatus_Hot)
	{
		CDUIAttributeTextStyle *pAttribute = &m_AttributeTextStyleHot;
		if (pAttribute && false == pAttribute->empty())
		{
			m_pTextHost->SetFont(pAttribute->GetFont());
			m_pTextHost->SetColor(pAttribute->GetTextColor());
		}

		return;
	}

	//normal
	CDUIAttributeTextStyle *pAttribute = &m_AttributeTextStyleNormal;
	m_pTextHost->SetFont(pAttribute ? pAttribute->GetFont() : NULL);
	m_pTextHost->SetColor(pAttribute ? pAttribute->GetTextColor() : 0);

	return;
}