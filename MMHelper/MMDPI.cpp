#include "stdafx.h"
#include "MMDpi.h"

//////////////////////////////////////////////////////////////////////////
typedef HRESULT(WINAPI *LPSetProcessDpiAwareness)(_In_ enMMPROCESS_DPI_AWARENESS value);
typedef HRESULT(WINAPI *LPGetProcessDpiAwareness)(_In_  HANDLE hprocess, _Out_ enMMPROCESS_DPI_AWARENESS *value);
typedef HRESULT(WINAPI *LPGetDpiForMonitor)(_In_ HMONITOR hmonitor, _In_ enMMMONITOR_DPI_TYPE dpiType, _Out_ UINT *dpiX, _Out_ UINT *dpiY);

//////////////////////////////////////////////////////////////////////////
int CMMDpi::m_nScaleFactor = 0;
int CMMDpi::m_nScaleFactorSDA = 0;
enMMPROCESS_DPI_AWARENESS CMMDpi::m_ProcessDPIAwareness = MMPROCESS_PER_MONITOR_DPI_AWARE;

CMMDpi::CMMDpi()
{
	SetDpi(96);

	return;
}

int CMMDpi::GetDpiOfMainMonitor()
{
	HMONITOR hMonitor = ::MonitorFromPoint({ 1,1 }, MONITOR_DEFAULTTOPRIMARY);

	return GetDpiOfMonitor(hMonitor);
}

int CMMDpi::GetDpiOfMonitor(HMONITOR hMonitor)
{
	UINT dpix = 96, dpiy = 96;

	HRESULT hr = E_FAIL;
	HMODULE hModule = ::LoadLibrary(_T("Shcore.dll"));
	if (hModule)
	{
		LPGetDpiForMonitor GetDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
		if (GetDpiForMonitor)
		{
			hr = GetDpiForMonitor(hMonitor, MMMDT_EFFECTIVE_DPI, &dpix, &dpiy);

			if (S_OK != hr)
			{
				int nError = GetLastError();
				CMMString strTip = CMMStrHelp::Format(_T("GetDpiForMonitor failed, LastError:[%d]"), nError);
				MessageBox(nullptr, strTip.GetBuffer(), _T("Notification"), MB_OK);
			}
		}

		FreeLibrary(hModule);
	}
	if (S_OK == hr)
	{
		return dpix;
	}

	HDC screen = GetDC(0);
	dpix = GetDeviceCaps(screen, LOGPIXELSX);
	ReleaseDC(0, screen);

	return dpix;
}

int CMMDpi::GetDpiOfMonitorNearestToPoint(POINT pt)
{
	HMONITOR hMonitor;
	hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	return GetDpiOfMonitor(hMonitor);
}

enMMPROCESS_DPI_AWARENESS CMMDpi::GetProcessDPIAwareness()
{
	HMODULE hModule = ::LoadLibrary(_T("Shcore.dll"));
	if (hModule)
	{
		LPGetProcessDpiAwareness GetProcessDpiAwareness = (LPGetProcessDpiAwareness)GetProcAddress(hModule, "GetProcessDpiAwareness");
		if (GetProcessDpiAwareness)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
			if (S_OK != GetProcessDpiAwareness(hProcess, &m_ProcessDPIAwareness))
			{
				int nError = GetLastError();
				CMMString strTip = CMMStrHelp::Format(_T("GetProcessDpiAwareness failed, LastError:[%d]"), nError);
				MessageBox(nullptr, strTip.GetBuffer(), _T("Notification"), MB_OK);
			}
		}
	}

	return m_ProcessDPIAwareness;
}

bool CMMDpi::SetProcessDPIAwareness(enMMPROCESS_DPI_AWARENESS Awareness)
{
	HMODULE hModule = ::LoadLibrary(_T("Shcore.dll"));
	if (hModule)
	{
		LPSetProcessDpiAwareness SetProcessDpiAwareness = (LPSetProcessDpiAwareness)GetProcAddress(hModule, "SetProcessDpiAwareness");
		if (SetProcessDpiAwareness && SetProcessDpiAwareness(Awareness) == S_OK)
		{
			m_ProcessDPIAwareness = Awareness;

			return true;
		}
	}

	m_ProcessDPIAwareness = Awareness;

	return false;
}

int CMMDpi::GetScale() const
{
	if (MMPROCESS_DPI_UNAWARE == m_ProcessDPIAwareness)
	{
		return 100;
	}
	if (MMPROCESS_SYSTEM_DPI_AWARE == m_ProcessDPIAwareness)
	{
		return m_nScaleFactorSDA;
	}

	return m_nScaleFactor;
}

void CMMDpi::SetScale(int nScale)
{
	m_nScaleFactor = nScale;

	if (0 == m_nScaleFactorSDA)
	{
		m_nScaleFactorSDA = m_nScaleFactor;
	}

	return;
}

UINT CMMDpi::GetDpi() const
{
	if (MMPROCESS_DPI_UNAWARE == m_ProcessDPIAwareness)
	{
		return 96;
	}
	if (MMPROCESS_SYSTEM_DPI_AWARE == m_ProcessDPIAwareness)
	{
		return MulDiv(m_nScaleFactorSDA, 96, 100);
	}

	return MulDiv(m_nScaleFactor, 96, 100);
}

void CMMDpi::SetDpi(int nDpi)
{
	m_nScaleFactor = MulDiv(nDpi, 100, 96);

	if (0 == m_nScaleFactorSDA)
	{
		m_nScaleFactorSDA = m_nScaleFactor;
	}

	return;
}

int CMMDpi::Scale(int nValue) const
{
	if (MMPROCESS_DPI_UNAWARE == m_ProcessDPIAwareness)
	{
		return nValue;
	}
	if (MMPROCESS_SYSTEM_DPI_AWARE == m_ProcessDPIAwareness)
	{
		return MulDiv(nValue, m_nScaleFactorSDA, 100);
	}

	return MulDiv(nValue, m_nScaleFactor, 100);
}

POINT CMMDpi::Scale(POINT ptPoint) const
{
	POINT ptScale = ptPoint;
	ptScale.x = Scale(ptPoint.x);
	ptScale.y = Scale(ptPoint.y);

	return ptScale;
}

SIZE CMMDpi::Scale(SIZE szSize) const
{
	SIZE szScale = szSize;
	szScale.cx = Scale(szSize.cx);
	szScale.cy = Scale(szSize.cy);

	return szScale;
}

RECT CMMDpi::Scale(RECT rcRect) const
{
	RECT rcScale = rcRect;
	int sw = Scale(rcRect.right - rcRect.left);
	int sh = Scale(rcRect.bottom - rcRect.top);
	rcScale.left = Scale(rcRect.left);
	rcScale.top = Scale(rcRect.top);
	rcScale.right = rcScale.left + sw;
	rcScale.bottom = rcScale.top + sh;

	return rcScale;
}

int CMMDpi::ScaleBack(int nValue) const
{
	if (MMPROCESS_DPI_UNAWARE == m_ProcessDPIAwareness)
	{
		return nValue;
	}
	if (MMPROCESS_SYSTEM_DPI_AWARE == m_ProcessDPIAwareness)
	{
		return MulDiv(nValue, 100, m_nScaleFactorSDA);
	}

	return MulDiv(nValue, 100, m_nScaleFactor);
}

SIZE CMMDpi::ScaleBack(SIZE szSize) const
{
	SIZE szScale = szSize;
	szScale.cx = ScaleBack(szSize.cx);
	szScale.cy = ScaleBack(szSize.cy);

	return szScale;
}

POINT CMMDpi::ScaleBack(POINT pt) const
{
	POINT ptBack = pt;
	ptBack.x = Scale(pt.x);
	ptBack.y = Scale(pt.y);

	return ptBack;
}

RECT CMMDpi::ScaleBack(RECT rcRect) const
{
	int sw = ScaleBack(rcRect.right - rcRect.left);
	int sh = ScaleBack(rcRect.bottom - rcRect.top);
	rcRect.left = ScaleBack(rcRect.left);
	rcRect.top = ScaleBack(rcRect.top);
	rcRect.right = rcRect.left + sw;
	rcRect.bottom = rcRect.top + sh;

	return rcRect;
}

int CMMDpi::ScaleVerify(int nTarget, int nScaleBack) const
{
	int nOffset = 0;
	while (true)
	{
		if (Scale((nScaleBack) + nOffset) < (nTarget))
		{
			nOffset++;
			continue;
		}

		break;
	}

	return (nScaleBack) + nOffset;
}

SIZE CMMDpi::ScaleVerify(SIZE szTarget, SIZE szScaleBack) const
{
	szScaleBack.cx = ScaleVerify(szTarget.cx, szScaleBack.cx);
	szScaleBack.cy = ScaleVerify(szTarget.cy, szScaleBack.cy);

	return szScaleBack;
}

POINT CMMDpi::ScaleVerify(POINT ptTarget, POINT ptScaleBack) const
{
	ptScaleBack.x = ScaleVerify(ptTarget.x, ptScaleBack.x);
	ptScaleBack.y = ScaleVerify(ptTarget.y, ptScaleBack.y);

	return ptScaleBack;
}

RECT CMMDpi::ScaleVerify(RECT rcTarget, RECT rcScaleBack) const
{
	int nWidthTarget = rcTarget.right - rcTarget.left;
	int nHeightTarget = rcTarget.bottom - rcTarget.top;
	int nWidthScaleBack = rcScaleBack.right - rcScaleBack.left;
	int nHeightScaleBack = rcScaleBack.bottom - rcScaleBack.top;
	nWidthScaleBack = ScaleVerify(nWidthTarget, nWidthScaleBack);
	nHeightScaleBack = ScaleVerify(nHeightTarget, nHeightScaleBack);

	rcScaleBack.left = ScaleVerify(rcTarget.left, rcScaleBack.left);
	rcScaleBack.top = ScaleVerify(rcTarget.top, rcScaleBack.top);
	rcScaleBack.right = rcScaleBack.left + nWidthScaleBack;
	rcScaleBack.bottom = rcScaleBack.top + nHeightScaleBack;

	return rcScaleBack;
}

////////////////////////////////////////////////////////////////////////////