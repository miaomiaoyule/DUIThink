#include "StdAfx.h"
//#include <math.h>
//
/////////////////////////////////////////////////////////////////////////
////
////
//IMPLEMENT_DUICONTROL(CDUIColorPaletteCtrl)
//
//CDUIColorPaletteCtrl::CDUIColorPaletteCtrl() {}
//
//CDUIColorPaletteCtrl::~CDUIColorPaletteCtrl() {
//	if (m_pBits) free(m_pBits);
//
//	if (m_hMemBitmap) {
//		::DeleteObject(m_hMemBitmap);
//	}
//
//}
//
//DWORD CDUIColorPaletteCtrl::GetSelectColor() {
//	DWORD dwColor = _HSLToRGB(m_nCurH, m_nCurS, m_nCurB);
//	return 0xFF << 24 | GetRValue(dwColor) << 16 | GetGValue(dwColor) << 8 | GetBValue(dwColor);
//}
//
//void CDUIColorPaletteCtrl::SetSelectColor(DWORD dwColor) {
//	float H = 0, S = 0, B = 0;
//	COLORREF dwBkClr = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
//	RGBToHSL(dwBkClr, &H, &S, &B);
//	m_nCurH = (int)(H * 360);
//	m_nCurS = (int)(S * 200);
//	m_nCurB = (int)(B * 200);
//	UpdatePalletData();
//	NeedUpdate();
//}
//
//faw::string_view_t CDUIColorPaletteCtrl::GetClass() const {
//	return _T("ColorPaletteUI");
//}
//
//LPVOID CDUIColorPaletteCtrl::GetInterface(faw::string_view_t lpszName) {
//	if (lpszName == DUI_CTRL_COLORPALETTE) return static_cast<CDUIColorPaletteCtrl*>(this);
//	return CDUIControlBase::GetInterface(lpszName);
//}
//
//void CDUIColorPaletteCtrl::SetPalletHeight(int nHeight) {
//	m_nPalletHeight = nHeight;
//}
//int	 CDUIColorPaletteCtrl::GetPalletHeight() const {
//	return m_nPalletHeight;
//}
//void CDUIColorPaletteCtrl::SetBarHeight(int nHeight) {
//	if (nHeight > 150) {
//		nHeight = 150; //限制最大高度，由于当前设计，nheight超出190，程序会因越界访问崩溃
//	}
//	m_nBarHeight = nHeight;
//}
//int  CDUIColorPaletteCtrl::GetBarHeight() const {
//	return m_nBarHeight;
//}
//
//void CDUIColorPaletteCtrl::SetThumbImage(faw::string_view_t pszImage) {
//	if (m_strThumbImage != pszImage) {
//		m_strThumbImage = pszImage;
//		NeedUpdate();
//	}
//}
//
//faw::string_view_t CDUIColorPaletteCtrl::GetThumbImage() const {
//	return m_strThumbImage.str_view();
//}
//
//void CDUIColorPaletteCtrl::SetAttribute(faw::string_view_t lpszName, faw::string_view_t pstrValue) {
//	if (lpszName == _T("palletheight")) SetPalletHeight(_ttoi(pstrValue.data()));
//	else if (lpszName == _T("barheight")) SetBarHeight(_ttoi(pstrValue.data()));
//	else if (lpszName == _T("thumbimage")) SetThumbImage(pstrValue);
//	else CDUIControlBase::SetAttribute(lpszName, pstrValue);
//}
//
//void CDUIColorPaletteCtrl::DoInit() {
//	m_MemDc = CreateCompatibleDC(GetManager()->GetPaintDC());
//	m_hMemBitmap = CreateCompatibleBitmap(GetManager()->GetPaintDC(), 400, 360);
//	SelectObject(m_MemDc, m_hMemBitmap);
//
//	::GetObject(m_hMemBitmap, sizeof(m_bmInfo), &m_bmInfo);
//	DWORD dwSize = m_bmInfo.bmHeight * m_bmInfo.bmWidthBytes;
//	m_pBits = (BYTE *)malloc(dwSize);
//	::GetBitmapBits(m_hMemBitmap, dwSize, m_pBits);
//}
//
//void CDUIColorPaletteCtrl::SetPos(RECT rc, bool bNeedInvalidate) {
//	CDUIControlBase::SetPos(rc, bNeedInvalidate);
//
//	m_ptLastPalletMouse.x = m_nCurH * (m_rcItem.right - m_rcItem.left) / 360 + m_rcItem.left;
//	m_ptLastPalletMouse.y = (200 - m_nCurB) * m_nPalletHeight / 200 + m_rcItem.top;
//
//	UpdatePalletData();
//	UpdateBarData();
//}
//
//void CDUIColorPaletteCtrl::DoEvent(TEventUI& event) {
//	CDUIControlBase::DoEvent(event);
//
//	if (event.Type == UIEVENT_BUTTONDOWN) {
//		if (event.ptMouse.x >= m_rcItem.left && event.ptMouse.y >= m_rcItem.top &&
//			event.ptMouse.x < m_rcItem.right && event.ptMouse.y < m_rcItem.top + m_nPalletHeight) {
//			int x = (event.ptMouse.x - m_rcItem.left) * 360 / (m_rcItem.right - m_rcItem.left);
//			int y = (event.ptMouse.y - m_rcItem.top) * 200 / m_nPalletHeight;
//			x = min(max(x, 0), 360);
//			y = min(max(y, 0), 200);
//
//			m_ptLastPalletMouse = event.ptMouse;
//			if (m_ptLastPalletMouse.x < m_rcItem.left) m_ptLastPalletMouse.x = m_rcItem.left;
//			if (m_ptLastPalletMouse.x > m_rcItem.right) m_ptLastPalletMouse.x = m_rcItem.right;
//			if (m_ptLastPalletMouse.y < m_rcItem.top) m_ptLastPalletMouse.y = m_rcItem.top;
//			if (m_ptLastPalletMouse.y > m_rcItem.top + m_nPalletHeight) m_ptLastPalletMouse.y = m_rcItem.top + m_nPalletHeight;
//
//			m_nCurH = x;
//			m_nCurB = 200 - y;
//
//			m_uButtonState |= UISTATE_PUSHED;
//			m_bIsInPallet = true;
//			m_bIsInBar = false;
//
//			UpdateBarData();
//		}
//
//		if (event.ptMouse.x >= m_rcItem.left && event.ptMouse.y >= m_rcItem.bottom - m_nBarHeight &&
//			event.ptMouse.x < m_rcItem.right && event.ptMouse.y < m_rcItem.bottom) {
//			m_nCurS = (event.ptMouse.x - m_rcItem.left) * 200 / (m_rcItem.right - m_rcItem.left);
//			m_uButtonState |= UISTATE_PUSHED;
//			m_bIsInBar = true;
//			m_bIsInPallet = false;
//			UpdatePalletData();
//		}
//
//		Invalidate();
//		return;
//	}
//	if (event.Type == UIEVENT_BUTTONUP) {
//		if ((m_uButtonState | UISTATE_PUSHED) && (IsEnabled())) {
//			m_pManager->SendNotify(this, DUI_MSGTYPE_COLORCHANGED, GetSelectColor(), 0);
//		}
//
//		m_uButtonState &= ~UISTATE_PUSHED;
//		m_bIsInPallet = false;
//		m_bIsInBar = false;
//
//		Invalidate();
//		return;
//	}
//	if (event.Type == UIEVENT_MOUSEMOVE) {
//		if (!(m_uButtonState &UISTATE_PUSHED)) {
//			m_bIsInBar = false;
//			m_bIsInPallet = false;
//		}
//		if (m_bIsInPallet == true) {
//			POINT pt = event.ptMouse;
//			pt.x -= m_rcItem.left;
//			pt.y -= m_rcItem.top;
//
//			if (pt.x >= 0 && pt.y >= 0 && pt.x <= m_rcItem.right && pt.y <= m_rcItem.top + m_nPalletHeight) {
//				int x = pt.x * 360 / (m_rcItem.right - m_rcItem.left);
//				int y = pt.y * 200 / m_nPalletHeight;
//				x = min(max(x, 0), 360);
//				y = min(max(y, 0), 200);
//
//				m_ptLastPalletMouse = event.ptMouse;
//				if (m_ptLastPalletMouse.x < m_rcItem.left) m_ptLastPalletMouse.x = m_rcItem.left;
//				if (m_ptLastPalletMouse.x > m_rcItem.right) m_ptLastPalletMouse.x = m_rcItem.right;
//				if (m_ptLastPalletMouse.y < m_rcItem.top) m_ptLastPalletMouse.y = m_rcItem.top;
//				if (m_ptLastPalletMouse.y >= m_rcItem.top + m_nPalletHeight) m_ptLastPalletMouse.y = m_rcItem.top + m_nPalletHeight;
//
//				m_nCurH = x;
//				m_nCurB = 200 - y;
//
//				UpdateBarData();
//			}
//		}
//		else if (m_bIsInBar == true) {
//			m_nCurS = (event.ptMouse.x - m_rcItem.left) * 200 / (m_rcItem.right - m_rcItem.left);
//			m_nCurS = min(max(m_nCurS, 0), 200);
//			UpdatePalletData();
//		}
//
//		Invalidate();
//		return;
//	}
//
//}
//
//void CDUIColorPaletteCtrl::PaintBkColor(HDC hDC) {
//	PaintPallet(hDC);
//}
//
//void CDUIColorPaletteCtrl::PaintPallet(HDC hDC) {
//	int nSaveDC = ::SaveDC(hDC);
//
//	::SetStretchBltMode(hDC, HALFTONE);
//	//拉伸模式将内存图画到控件上
//	StretchBlt(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_nPalletHeight, m_MemDc, 0, 1, 360, 200, SRCCOPY);
//	StretchBlt(hDC, m_rcItem.left, m_rcItem.bottom - m_nBarHeight, m_rcItem.right - m_rcItem.left, m_nBarHeight, m_MemDc, 0, 210, 200, m_nBarHeight, SRCCOPY);
//
//	RECT rcCurSorPaint = { m_ptLastPalletMouse.x - 4, m_ptLastPalletMouse.y - 4, m_ptLastPalletMouse.x + 4, m_ptLastPalletMouse.y + 4 };
//	CDUIRenderEngine::DrawImageString(hDC, m_pManager, rcCurSorPaint, m_rcPaint, m_strThumbImage.str_view());
//
//	rcCurSorPaint.left = m_rcItem.left + m_nCurS * (m_rcItem.right - m_rcItem.left) / 200 - 4;
//	rcCurSorPaint.right = m_rcItem.left + m_nCurS * (m_rcItem.right - m_rcItem.left) / 200 + 4;
//	rcCurSorPaint.top = m_rcItem.bottom - m_nBarHeight / 2 - 4;
//	rcCurSorPaint.bottom = m_rcItem.bottom - m_nBarHeight / 2 + 4;
//	CDUIRenderEngine::DrawImageString(hDC, m_pManager, rcCurSorPaint, m_rcPaint, m_strThumbImage.str_view());
//	::RestoreDC(hDC, nSaveDC);
//}
//
//void CDUIColorPaletteCtrl::UpdatePalletData() {
//	for (int y = 0; y < 200; ++y) {
//		for (int x = 0; x < 360; ++x) {
//			BYTE *pPiexl = LPBYTE(m_pBits) + ((200 - y)*m_bmInfo.bmWidthBytes) + ((x*m_bmInfo.bmBitsPixel) / 8);
//			DWORD dwColor = _HSLToRGB(x, m_nCurS, y);
//			if (dwColor == 0xFF000000) dwColor = 0xFF000001;
//			pPiexl[0] = GetBValue(dwColor);
//			pPiexl[1] = GetGValue(dwColor);
//			pPiexl[2] = GetRValue(dwColor);
//		}
//	}
//
//	SetBitmapBits(m_hMemBitmap, m_bmInfo.bmWidthBytes * m_bmInfo.bmHeight, m_pBits);
//}
//
//
//void CDUIColorPaletteCtrl::UpdateBarData() {
//	//这里画出Bar
//	for (int y = 0; y < m_nBarHeight; ++y) {
//		for (int x = 0; x < 200; ++x) {
//			BYTE *pPiexl = LPBYTE(m_pBits) + ((210 + y)*m_bmInfo.bmWidthBytes) + ((x*m_bmInfo.bmBitsPixel) / 8);
//			DWORD dwColor = _HSLToRGB(m_nCurH, x, m_nCurB);
//			if (dwColor == 0xFF000000) dwColor = 0xFF000001;
//			pPiexl[0] = GetBValue(dwColor);
//			pPiexl[1] = GetGValue(dwColor);
//			pPiexl[2] = GetRValue(dwColor);
//		}
//	}
//
//	SetBitmapBits(m_hMemBitmap, m_bmInfo.bmWidthBytes * m_bmInfo.bmHeight, m_pBits);
//}