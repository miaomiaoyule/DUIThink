#include "stdafx.h"
#include "MMDisplayer.h"

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hDCMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	if (NULL == hMonitor) return false;

	tagMMFindMonitor &FindMonitor = *(tagMMFindMonitor*)dwData;

	MONITORINFOEX MonitorInfo = {};
	MonitorInfo.cbSize = sizeof(MONITORINFOEX);
	::GetMonitorInfo(hMonitor, &MonitorInfo);

	if (MonitorInfo.rcMonitor.left < FindMonitor.MonitorInfoFarLeft.rcMonitor.left || NULL == FindMonitor.hMonitorFarLeft)
	{
		FindMonitor.hMonitorFarLeft = hMonitor;
		FindMonitor.MonitorInfoFarLeft = MonitorInfo;
	}
	if (MonitorInfo.rcMonitor.top < FindMonitor.MonitorInfoFarTop.rcMonitor.top || NULL == FindMonitor.hMonitorFarTop)
	{
		FindMonitor.hMonitorFarTop = hMonitor;
		FindMonitor.MonitorInfoFarTop = MonitorInfo;
	}
	if (MonitorInfo.rcMonitor.right > FindMonitor.MonitorInfoFarRight.rcMonitor.right || NULL == FindMonitor.hMonitorFarRight)
	{
		FindMonitor.hMonitorFarRight = hMonitor;
		FindMonitor.MonitorInfoFarRight = MonitorInfo;
	}
	if (MonitorInfo.rcMonitor.bottom > FindMonitor.MonitorInfoFarBottom.rcMonitor.bottom || NULL == FindMonitor.hMonitorFarBottom)
	{
		FindMonitor.hMonitorFarBottom = hMonitor;
		FindMonitor.MonitorInfoFarBottom = MonitorInfo;
	}
	if (FindMonitor.strName == MonitorInfo.szDevice)
	{
		FindMonitor.hMonitor = hMonitor;
	}
	if (MONITORINFOF_PRIMARY == MonitorInfo.dwFlags)
	{
		FindMonitor.hMonitorPrimary = hMonitor;
		FindMonitor.MonitorInfoPrimary = MonitorInfo;
	}

	FindMonitor.vecMonitor.push_back(MonitorInfo);

	return true;
}

struct tagEnumWnd
{
	HMONITOR					hMonitorVerify = NULL;
	HWND						hWndFind = NULL;
};
static BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam)
{
	if (NULL == hWnd) return false;

	TCHAR szClassName[MAX_PATH] = {};
	::RealGetWindowClass(hWnd, szClassName, MAX_PATH);
	if (-1 == CMMString(szClassName).find(_T("Shell_"))
		|| -1 == CMMString(szClassName).find(_T("TrayWnd"))) return true;

	tagEnumWnd &EnumWnd = *(tagEnumWnd*)(lParam);
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
	if (hMonitor == EnumWnd.hMonitorVerify)
	{
		EnumWnd.hWndFind = hWnd;
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
CMMDisplayer::CMMDisplayer()
{
}

CMMDisplayer::~CMMDisplayer()
{
}

void CMMDisplayer::SetResolution(SIZE szResolution)
{
	m_szResolutionCur = szResolution;

	return;
}

float CMMDisplayer::Scale(float fValue, bool bHoriz)
{
	if (false == VerifyResolutionCur()) return fValue;

	double lfX = m_szResolutionCur.cx * 1.0 / RESOLUTION_HORIZ;
	double lfY = m_szResolutionCur.cy * 1.0 / RESOLUTION_VERT;

	return fValue * (bHoriz ? lfX : lfY);
}

POINTF CMMDisplayer::Scale(POINTF ptValue)
{
	if (false == VerifyResolutionCur()) return ptValue;

	double lfX = m_szResolutionCur.cx * 1.0 / RESOLUTION_HORIZ;
	double lfY = m_szResolutionCur.cy * 1.0 / RESOLUTION_VERT;

	return { float(ptValue.x * lfX), float(ptValue.y * lfY) };
}

SIZEF CMMDisplayer::Scale(SIZEF szValue)
{
	POINTF ptValue = { szValue.cx, szValue.cy };
	ptValue = Scale(ptValue);
	return { ptValue.x, ptValue.y };
}

POINTF CMMDisplayer::ScaleSuitable(POINTF ptValue)
{
	if (false == VerifyResolutionCur()) return ptValue;
	if (0 == ptValue.x || 0 == ptValue.y) return ptValue;

	double lfScale = ptValue.x * 1.0 / ptValue.y;

	if (ptValue.x > ptValue.y)
	{
		double lfX = m_szResolutionCur.cx * 1.0 / RESOLUTION_HORIZ;
		float fX = ptValue.x * lfX;
		float fY = fX / lfScale;
		return { fX, fY };
	}

	double lfY = m_szResolutionCur.cy * 1.0 / RESOLUTION_VERT;
	float fY = ptValue.y * lfY;
	float fX = fY * lfScale;
	return { fX, fY };
}

SIZEF CMMDisplayer::ScaleSuitable(SIZEF szValue)
{
	POINTF ptValue = { szValue.cx, szValue.cy };
	ptValue = ScaleSuitable(ptValue);
	return { ptValue.x, ptValue.y };
}

float CMMDisplayer::ScaleBack(float fValue, bool bHoriz)
{
	if (false == VerifyResolutionCur()) return fValue;

	double lfX = m_szResolutionCur.cx * 1.0 / RESOLUTION_HORIZ;
	double lfY = m_szResolutionCur.cy * 1.0 / RESOLUTION_VERT;

	return fValue / (bHoriz ? lfX : lfY);
}

POINTF CMMDisplayer::ScaleBack(POINTF ptValue)
{
	if (false == VerifyResolutionCur()) return ptValue;

	double lfX = m_szResolutionCur.cx * 1.0 / RESOLUTION_HORIZ;
	double lfY = m_szResolutionCur.cy * 1.0 / RESOLUTION_VERT;

	return { float(ptValue.x / lfX), float(ptValue.y / lfY) };
}

SIZEF CMMDisplayer::ScaleBack(SIZEF szValue)
{
	POINTF ptValue = { szValue.cx, szValue.cy };
	ptValue = ScaleBack(ptValue);
	return { ptValue.x, ptValue.y };
}

POINTF CMMDisplayer::ScaleSuitableBack(POINTF ptValue)
{
	if (false == VerifyResolutionCur()) return ptValue;
	if (0 == ptValue.x || 0 == ptValue.y) return ptValue;

	double lfScale = ptValue.x * 1.0 / ptValue.y;

	if (ptValue.x > ptValue.y)
	{
		double lfX = m_szResolutionCur.cx * 1.0 / RESOLUTION_HORIZ;
		float fX = ptValue.x / lfX;
		float fY = fX / lfScale;
		return { fX, fY };
	}

	double lfY = m_szResolutionCur.cy * 1.0 / RESOLUTION_VERT;
	float fY = ptValue.y / lfY;
	float fX = fY * lfScale;
	return { fX, fY };
}

SIZEF CMMDisplayer::ScaleSuitableBack(SIZEF szValue)
{
	POINTF ptValue = { szValue.cx, szValue.cy };
	ptValue = ScaleSuitableBack(ptValue);
	return { ptValue.x, ptValue.y };
}

bool CMMDisplayer::VerifyResolutionCur()
{
	return false == (m_szResolutionCur.cx == m_szResolutionCur.cy && 0 == m_szResolutionCur.cx);
}

int CMMDisplayer::GetTaskBarInfo(RECT &rcTaskBar, BOOL &bTrayAutoHide)
{
	bTrayAutoHide = false;
	SetRectEmpty(&rcTaskBar);

	HWND hWndShellTray = ::FindWindow(L"Shell_TrayWnd", NULL);
	if (hWndShellTray)
	{
		INT nMetricsX = GetSystemMetrics(SM_CXSCREEN);
		INT nMetricsY = GetSystemMetrics(SM_CYSCREEN);

		::GetWindowRect(hWndShellTray, &rcTaskBar);

		//int nTrayWndWidth = rcTaskBar.right - rcTaskBar.left;
		//int nTrayWndHight = rcTaskBar.bottom - rcTaskBar.top;

		APPBARDATA abd;
		memset(&abd, 0, sizeof(APPBARDATA));
		abd.cbSize = sizeof(abd);
		abd.hWnd = hWndShellTray;
		if ((ABS_AUTOHIDE & ::SHAppBarMessage(ABM_GETSTATE, &abd)))
		{
			bTrayAutoHide = true;
		}

		APPBARDATA BarData = {};
		BarData.cbSize = sizeof(BarData);
		if (::SHAppBarMessage(ABM_GETTASKBARPOS, &BarData))
		{
			return BarData.uEdge;
		}
	}

	return ABE_BOTTOM;
}

bool CMMDisplayer::GetTaskBarType(int &nDockType)
{
	HWND hWndShellTray = ::FindWindow(L"Shell_TrayWnd", NULL);
	if (hWndShellTray)
	{
		APPBARDATA BarData = {};
		BarData.cbSize = sizeof(BarData);
		if (::SHAppBarMessage(ABM_GETTASKBARPOS, &BarData))
		{
			nDockType == BarData.uEdge;

			return true;
		}			
	}

	return false;
}

int CMMDisplayer::GetTaskbarDockType(HMONITOR hMonitor)
{
	NULL == hMonitor ? hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY) : hMonitor;
	if (NULL == hMonitor) return ABE_BOTTOM;

	tagEnumWnd EnumWnd = {};
	EnumWnd.hMonitorVerify = hMonitor;
	::EnumWindows(EnumWindowProc, (LPARAM)&EnumWnd);
	if (NULL == EnumWnd.hWndFind) return ABE_BOTTOM;

	MONITORINFO Monitor = {};
	Monitor.cbSize = sizeof(Monitor);
	::GetMonitorInfo(hMonitor, &Monitor);
	CRect rcMonitor = Monitor.rcMonitor;

	CRect rcWnd;
	GetWindowRect(EnumWnd.hWndFind, &rcWnd);

	if (rcWnd.left == rcMonitor.left
		&& rcWnd.right == rcMonitor.right
		&& rcWnd.top <= rcMonitor.bottom
		&& rcWnd.bottom >= rcMonitor.bottom)
	{
		return ABE_BOTTOM;
	}
	if (rcWnd.left == rcMonitor.left
		&& rcWnd.right == rcMonitor.right
		&& rcWnd.top <= rcMonitor.top
		&& rcWnd.bottom >= rcMonitor.top)
	{
		return ABE_TOP;
	}
	if (rcWnd.left <= rcMonitor.left
		&& rcWnd.right >= rcMonitor.left
		&& rcWnd.top == rcMonitor.top
		&& rcWnd.bottom == rcMonitor.bottom)
	{
		return ABE_LEFT;
	}
	if (rcWnd.left <= rcMonitor.right
		&& rcWnd.right >= rcMonitor.right
		&& rcWnd.top == rcMonitor.top
		&& rcWnd.bottom == rcMonitor.bottom)
	{
		return ABE_RIGHT;
	}

	return ABE_BOTTOM;
}

bool CMMDisplayer::GetComputerMonitor(tagMMFindMonitor &ComputerMonitor)
{
	::EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&ComputerMonitor);
	if (NULL == ComputerMonitor.hMonitorFarLeft
		|| NULL == ComputerMonitor.hMonitorFarTop
		|| NULL == ComputerMonitor.hMonitorFarRight
		|| NULL == ComputerMonitor.hMonitorFarBottom
		|| NULL == ComputerMonitor.hMonitorPrimary)
		return false;

	return true;
}

int CMMDisplayer::GetMultScreenNumber()
{
	return ::GetSystemMetrics(SM_CMONITORS);
}

MONITORINFOEX CMMDisplayer::GetMonitorFromPoint(const POINT &pt)
{
	HMONITOR hMonotor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	if (NULL == hMonotor) return {};

	MONITORINFOEX MonitorInfo = {};
	MonitorInfo.cbSize = sizeof(MonitorInfo);
	GetMonitorInfo(hMonotor, &MonitorInfo);
	return MonitorInfo;
}

MONITORINFOEX CMMDisplayer::GetPrimaryMonitor()
{
	tagMMFindMonitor FindMonitor = {};
	::EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&FindMonitor);

	return FindMonitor.MonitorInfoPrimary;
}

bool CMMDisplayer::MonitorRelToMonitorAbs(IN CMMRectF rcRelOfMonitor,
	IN CMMRectF rcRelOfWork,
	IN OUT CMMString &strMonitorName,
	IN OUT CMMDpi &DpiInfo,
	IN OUT CMMRectF &rcAbsOfMonitor,
	IN OUT CMMRectF &rcMonitorLast)
{
	tagMMFindMonitor FindMonitor = {};
	FindMonitor.strName = strMonitorName;
	::EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&FindMonitor);

	POINT ptLT = { rcAbsOfMonitor.left, rcAbsOfMonitor.top };
	POINT ptRB = { rcAbsOfMonitor.right, rcAbsOfMonitor.bottom };

	HMONITOR hMonitor = FindMonitor.hMonitor;
	HMONITOR hMonitorFarLeft = FindMonitor.hMonitorFarLeft;
	HMONITOR hMonitorFarTop = FindMonitor.hMonitorFarTop;
	NULL == hMonitor ? hMonitor = MonitorFromPoint(ptLT, MONITOR_DEFAULTTONULL) : hMonitor;
	NULL == hMonitor ? hMonitor = MonitorFromPoint(ptRB, MONITOR_DEFAULTTONEAREST) : hMonitor;
	NULL == hMonitorFarLeft ? hMonitorFarLeft = hMonitor : hMonitorFarLeft;
	NULL == hMonitorFarTop ? hMonitorFarTop = hMonitor : hMonitorFarTop;

	MONITORINFOEX MonitorInfo = {}, MonitorInfoFarLeft = {}, MonitorInfoFarTop = {};
	MonitorInfo.cbSize = sizeof(MonitorInfo);
	MonitorInfoFarLeft.cbSize = sizeof(MonitorInfoFarLeft);
	MonitorInfoFarTop.cbSize = sizeof(MonitorInfoFarTop);
	GetMonitorInfo(hMonitor, &MonitorInfo);
	GetMonitorInfo(hMonitorFarLeft, &MonitorInfoFarLeft);
	GetMonitorInfo(hMonitorFarTop, &MonitorInfoFarTop);
	CRect rcWork = MonitorInfo.rcWork;
	CRect rcMonitor = MonitorInfo.rcMonitor;
	CRect rcMonitorFarLeft = MonitorInfoFarLeft.rcMonitor;
	CRect rcMonitorFarTop = MonitorInfoFarTop.rcMonitor;
	bool bRelOfMonitorOrWork = rcMonitorLast.GetWidth() == rcMonitor.Width()
		&& rcMonitorLast.GetHeight() == rcMonitor.Height()
		&& strMonitorName == MonitorInfo.szDevice;
	rcMonitorLast = rcMonitor;
	strMonitorName = MonitorInfo.szDevice;

	CMMDisplayer DisplayMgr;
	DisplayMgr.SetResolution(bRelOfMonitorOrWork ? SIZE{ rcMonitor.Width(), rcMonitor.Height() } : SIZE{ rcWork.Width(), rcWork.Height() });
	//if (false == DPIInfo.bSetDPI)
	//{
	//	DPIInfo.bSetDPI = true;
	//	DPIInfo.DPI.SetScale(CXZDPI::GetDPIVerifyRange(CXZDPI::GetDPIOfMonitor(hMonitor)));
	//}

	CMMRectF rcRelative = bRelOfMonitorOrWork ? rcRelOfMonitor : rcRelOfWork;

	//resolution
	//rcAbsOfMonitor.left = DisplayMgr.Scale(rcRelative.left, true);
	//rcAbsOfMonitor.top = DisplayMgr.Scale(rcRelative.top, false);
	//rcAbsOfMonitor.right = DisplayMgr.Scale(rcRelative.right, true);
	//rcAbsOfMonitor.bottom = DisplayMgr.Scale(rcRelative.bottom, false);
	//
	////DPI
	//rcAbsOfMonitor.left = DPIInfo.DPI.Scale(rcAbsOfMonitor.left);
	//rcAbsOfMonitor.top = DPIInfo.DPI.Scale(rcAbsOfMonitor.top);
	//rcAbsOfMonitor.right = DPIInfo.DPI.Scale(rcAbsOfMonitor.right);
	//rcAbsOfMonitor.bottom = DPIInfo.DPI.Scale(rcAbsOfMonitor.bottom);
	//rcAbsOfMonitor.OffsetRect(rcMonitor.left, rcMonitor.top);
	//
	////verify
	//if (rcAbsOfMonitor.right > rcWork.right)
	//{
	//	rcAbsOfMonitor.OffsetRect(rcWork.right - rcAbsOfMonitor.right, 0);
	//}
	//if (rcAbsOfMonitor.left < rcWork.left)
	//{
	//	rcAbsOfMonitor.OffsetRect(rcWork.left - rcAbsOfMonitor.left, 0);
	//}
	//if (rcAbsOfMonitor.bottom > rcWork.bottom)
	//{
	//	rcAbsOfMonitor.OffsetRect(0, rcWork.bottom - rcAbsOfMonitor.bottom);
	//}
	//if (rcAbsOfMonitor.top < rcWork.top)
	//{
	//	rcAbsOfMonitor.OffsetRect(0, rcWork.top - rcAbsOfMonitor.top);
	//}
	//
	//DPIInfo.DPI.SetScale(96);

	return true;
}

bool CMMDisplayer::MonitorAbsToMonitorRel(IN CMMRectF rcAbsOfMonitor,
	IN OUT CMMString &strMonitorName,
	IN OUT CMMDpi &DpiInfo,
	IN OUT CMMRectF &rcRelOfMonitor,
	IN OUT CMMRectF &rcRelOfWork,
	IN OUT CMMRectF &rcMonitorLast)
{
	tagMMFindMonitor FindMonitor = {};
	::EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&FindMonitor);
	if (NULL == FindMonitor.hMonitorFarLeft || NULL == FindMonitor.hMonitorFarTop) return false;

	HMONITOR hMonitorFarLeft = FindMonitor.hMonitorFarLeft;
	HMONITOR hMonitorFarTop = FindMonitor.hMonitorFarTop;
	MONITORINFOEX MonitorInfoFarLeft = FindMonitor.MonitorInfoFarLeft;
	MONITORINFOEX MonitorInfoFarTop = FindMonitor.MonitorInfoFarTop;
	CRect rcMonitorFarLeft = MonitorInfoFarLeft.rcMonitor;
	CRect rcMonitorFarTop = MonitorInfoFarTop.rcMonitor;
	POINT ptLT = { rcAbsOfMonitor.left, rcAbsOfMonitor.top };
	POINT ptRT = { rcAbsOfMonitor.right, rcAbsOfMonitor.top };

	HMONITOR hMonitor = MonitorFromPoint(ptLT, MONITOR_DEFAULTTONULL);
	NULL == hMonitor ? hMonitor = MonitorFromPoint(ptRT, MONITOR_DEFAULTTONEAREST) : hMonitor;
	if (NULL == hMonitor) return false;

	MONITORINFOEX MonitorInfo = {};
	MonitorInfo.cbSize = sizeof(MonitorInfo);
	GetMonitorInfo(hMonitor, &MonitorInfo);
	strMonitorName = MonitorInfo.szDevice;
	CRect rcWork = MonitorInfo.rcWork;
	CRect rcMonitor = MonitorInfo.rcMonitor;
	rcMonitorLast = rcMonitor;

	//relofmonitor
	//{
	//	CMMDisplayer DisplayMgr;
	//	DisplayMgr.SetResolution({ rcMonitor.Width(), rcMonitor.Height() });
	//	if (false == DPIInfo.bSetDPI)
	//	{
	//		DPIInfo.bSetDPI = true;
	//		DPIInfo.DPI.SetScale(CXZDPI::GetDPIVerifyRange(CXZDPI::GetDPIOfMonitor(hMonitor)));
	//	}

	//	rcRelOfMonitor = rcAbsOfMonitor;
	//	rcRelOfMonitor.Offset(-rcMonitor.left, -rcMonitor.top);

	//	rcRelOfMonitor.left = DPIInfo.DPI.ScaleVerify(rcRelOfMonitor.left, DPIInfo.DPI.ScaleBack(rcRelOfMonitor.left));
	//	rcRelOfMonitor.left = DisplayMgr.ScaleBack(rcRelOfMonitor.left, true);
	//	rcRelOfMonitor.top = DPIInfo.DPI.ScaleVerify(rcRelOfMonitor.top, DPIInfo.DPI.ScaleBack(rcRelOfMonitor.top));
	//	rcRelOfMonitor.top = DisplayMgr.ScaleBack(rcRelOfMonitor.top, false);
	//	rcRelOfMonitor.right = DPIInfo.DPI.ScaleVerify(rcRelOfMonitor.right, DPIInfo.DPI.ScaleBack(rcRelOfMonitor.right));
	//	rcRelOfMonitor.right = DisplayMgr.ScaleBack(rcRelOfMonitor.right, true);
	//	rcRelOfMonitor.bottom = DPIInfo.DPI.ScaleVerify(rcRelOfMonitor.bottom, DPIInfo.DPI.ScaleBack(rcRelOfMonitor.bottom));
	//	rcRelOfMonitor.bottom = DisplayMgr.ScaleBack(rcRelOfMonitor.bottom, false);
	//}
	////relofwork
	//{
	//	CMMDisplayer DisplayMgr;
	//	DisplayMgr.SetResolution({ rcWork.Width(), rcWork.Height() });
	//	if (false == DPIInfo.bSetDPI)
	//	{
	//		DPIInfo.bSetDPI = true;
	//		DPIInfo.DPI.SetScale(CXZDPI::GetDPIVerifyRange(CXZDPI::GetDPIOfMonitor(hMonitor)));
	//	}

	//	rcRelOfWork = rcAbsOfMonitor;
	//	rcRelOfWork.Offset(-rcWork.left, -rcWork.top);

	//	rcRelOfWork.left = DPIInfo.DPI.ScaleVerify(rcRelOfWork.left, DPIInfo.DPI.ScaleBack(rcRelOfWork.left));
	//	rcRelOfWork.left = DisplayMgr.ScaleBack(rcRelOfWork.left, true);
	//	rcRelOfWork.top = DPIInfo.DPI.ScaleVerify(rcRelOfWork.top, DPIInfo.DPI.ScaleBack(rcRelOfWork.top));
	//	rcRelOfWork.top = DisplayMgr.ScaleBack(rcRelOfWork.top, false);
	//	rcRelOfWork.right = DPIInfo.DPI.ScaleVerify(rcRelOfWork.right, DPIInfo.DPI.ScaleBack(rcRelOfWork.right));
	//	rcRelOfWork.right = DisplayMgr.ScaleBack(rcRelOfWork.right, true);
	//	rcRelOfWork.bottom = DPIInfo.DPI.ScaleVerify(rcRelOfWork.bottom, DPIInfo.DPI.ScaleBack(rcRelOfWork.bottom));
	//	rcRelOfWork.bottom = DisplayMgr.ScaleBack(rcRelOfWork.bottom, false);
	//}

	return true;
}

CMMRectF CMMDisplayer::MonitorAbsToScreenAbs(CMMRectF rcAbsolute)
{
	tagMMFindMonitor FindMonitor = {};
	::EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&FindMonitor);
	POINT ptLT = { rcAbsolute.left, rcAbsolute.top };
	POINT ptRB = { rcAbsolute.right, rcAbsolute.bottom };

	HMONITOR hMonitor = FindMonitor.hMonitor;
	HMONITOR hMonitorFarLeft = FindMonitor.hMonitorFarLeft;
	HMONITOR hMonitorFarTop = FindMonitor.hMonitorFarTop;
	NULL == hMonitor ? hMonitor = MonitorFromPoint(ptLT, MONITOR_DEFAULTTONULL) : hMonitor;
	NULL == hMonitor ? hMonitor = MonitorFromPoint(ptRB, MONITOR_DEFAULTTONEAREST) : hMonitor;
	NULL == hMonitorFarLeft ? hMonitorFarLeft = hMonitor : hMonitorFarLeft;
	NULL == hMonitorFarTop ? hMonitorFarTop = hMonitor : hMonitorFarTop;

	MONITORINFOEX MonitorInfo = {}, MonitorInfoFarLeft = {}, MonitorInfoFarTop = {};
	MonitorInfo.cbSize = sizeof(MonitorInfo);
	MonitorInfoFarLeft.cbSize = sizeof(MonitorInfoFarLeft);
	MonitorInfoFarTop.cbSize = sizeof(MonitorInfoFarTop);
	GetMonitorInfo(hMonitor, &MonitorInfo);
	GetMonitorInfo(hMonitorFarLeft, &MonitorInfoFarLeft);
	GetMonitorInfo(hMonitorFarTop, &MonitorInfoFarTop);
	CRect rcMonitor = MonitorInfo.rcMonitor;
	CRect rcMonitorFarLeft = MonitorInfoFarLeft.rcMonitor;
	CRect rcMonitorFarTop = MonitorInfoFarTop.rcMonitor;

	//convert to abs
	rcAbsolute.Offset(-rcMonitor.left, -rcMonitor.top);
	rcAbsolute.Offset(rcMonitor.left - rcMonitorFarLeft.left, rcMonitor.top - rcMonitorFarTop.top);
	
	return rcAbsolute;
}

CMMRectF CMMDisplayer::ScreenAbsToMonitorAbs(CMMRectF rcAbsolute)
{
	tagMMFindMonitor FindMonitor = {};
	::EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&FindMonitor);
	if (NULL == FindMonitor.hMonitorFarLeft || NULL == FindMonitor.hMonitorFarTop) return rcAbsolute;

	HMONITOR hMonitorFarLeft = FindMonitor.hMonitorFarLeft;
	HMONITOR hMonitorFarTop = FindMonitor.hMonitorFarTop;
	MONITORINFOEX MonitorInfoFarLeft = FindMonitor.MonitorInfoFarLeft;
	MONITORINFOEX MonitorInfoFarTop = FindMonitor.MonitorInfoFarTop;
	CRect rcMonitorFarLeft = MonitorInfoFarLeft.rcMonitor;
	CRect rcMonitorFarTop = MonitorInfoFarTop.rcMonitor;
	CMMRectF rcAbsoluteOfMonitor = rcAbsolute;
	rcAbsoluteOfMonitor.Offset(rcMonitorFarLeft.left, rcMonitorFarTop.top);

	return rcAbsoluteOfMonitor;
}

void CMMDisplayer::GetScreenInfo(OUT CMMRectF &rcScreenWork, OUT CMMRectF &rcScreenDisplay, OUT CMMRectF &rcPrimary, std::vector<MONITORINFOEX> &vecMonitor)
{
	tagMMFindMonitor FindMonitor = {};
	::EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&FindMonitor);
	if (NULL == FindMonitor.hMonitorFarLeft 
		|| NULL == FindMonitor.hMonitorFarTop
		|| NULL == FindMonitor.hMonitorFarRight
		|| NULL == FindMonitor.hMonitorFarBottom
		|| NULL == FindMonitor.hMonitorPrimary) return;

	rcScreenWork.left = FindMonitor.MonitorInfoFarLeft.rcWork.left;
	rcScreenWork.top = FindMonitor.MonitorInfoFarTop.rcWork.top;
	rcScreenWork.right = FindMonitor.MonitorInfoFarRight.rcWork.right;
	rcScreenWork.bottom = FindMonitor.MonitorInfoFarBottom.rcWork.bottom;

	rcScreenDisplay.left = FindMonitor.MonitorInfoFarLeft.rcMonitor.left;
	rcScreenDisplay.top = FindMonitor.MonitorInfoFarTop.rcMonitor.top;
	rcScreenDisplay.right = FindMonitor.MonitorInfoFarRight.rcMonitor.right;
	rcScreenDisplay.bottom = FindMonitor.MonitorInfoFarBottom.rcMonitor.bottom;

	rcPrimary = CMMRectF(FindMonitor.MonitorInfoPrimary.rcMonitor);

	vecMonitor = FindMonitor.vecMonitor;

	return;
}

CMMRectF CMMDisplayer::PerformAdjustPosition(IN CMMRectF rcAbsOfMonitor)
{
	//right bottom
	{
		HMONITOR hMonitor = MonitorFromPoint({ (int)rcAbsOfMonitor.right, (int)rcAbsOfMonitor.bottom }, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX MonitorInfo = {};
		MonitorInfo.cbSize = sizeof(MonitorInfo);
		GetMonitorInfo(hMonitor, &MonitorInfo);
		CMMRectF rcWork = MonitorInfo.rcWork;
		if (false == rcWork.PtInRect(POINTF{ rcAbsOfMonitor.right, rcWork.bottom }))
		{
			if (abs(rcWork.right - rcAbsOfMonitor.right) < abs(rcWork.left - rcAbsOfMonitor.right))
			{
				rcAbsOfMonitor.Offset(rcWork.right - rcAbsOfMonitor.right, 0);
			}
			else
			{
				rcAbsOfMonitor.Offset(rcWork.left + rcAbsOfMonitor.GetWidth() - rcAbsOfMonitor.right, 0);
			}
		}
		if (false == rcWork.PtInRect(POINTF{ rcWork.right, rcAbsOfMonitor.bottom }))
		{
			if (abs(rcWork.bottom - rcAbsOfMonitor.bottom) < abs(rcWork.top - rcAbsOfMonitor.bottom))
			{
				rcAbsOfMonitor.Offset(0, rcWork.bottom - rcAbsOfMonitor.bottom);
			}
			else
			{
				rcAbsOfMonitor.Offset(0, rcWork.top + rcAbsOfMonitor.GetHeight() - rcAbsOfMonitor.bottom);
			}
		}
	}
	//left top
	{
		HMONITOR hMonitor = MonitorFromPoint(POINT{ (int)rcAbsOfMonitor.left, (int)rcAbsOfMonitor.top }, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX MonitorInfo = {};
		MonitorInfo.cbSize = sizeof(MonitorInfo);
		GetMonitorInfo(hMonitor, &MonitorInfo);
		CMMRectF rcWork = MonitorInfo.rcWork;
		if (false == rcWork.PtInRect(POINTF{ rcAbsOfMonitor.left, rcWork.top }))
		{
			if (abs(rcWork.left - rcAbsOfMonitor.left) < abs(rcWork.right - rcAbsOfMonitor.left))
			{
				rcAbsOfMonitor.Offset(rcWork.left - rcAbsOfMonitor.left, 0);
			}
			else
			{
				rcAbsOfMonitor.Offset(rcWork.right - rcAbsOfMonitor.GetWidth() - rcAbsOfMonitor.left, 0);
			}
		}
		if (false == rcWork.PtInRect(POINTF{ rcWork.left, rcAbsOfMonitor.top }))
		{
			if (abs(rcWork.top - rcAbsOfMonitor.top) < abs(rcWork.bottom - rcAbsOfMonitor.top))
			{
				rcAbsOfMonitor.Offset(0, rcWork.top - rcAbsOfMonitor.top);
			}
			else
			{
				rcAbsOfMonitor.Offset(0, rcWork.bottom - rcAbsOfMonitor.GetHeight() - rcAbsOfMonitor.top);
			}
		}
	}

	return rcAbsOfMonitor;
}


