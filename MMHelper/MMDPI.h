#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_DPI_H__
#define __MM_DPI_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
enum enMMPROCESS_DPI_AWARENESS 
{
	MMPROCESS_DPI_UNAWARE = 0,
	MMPROCESS_SYSTEM_DPI_AWARE = 1,
	MMPROCESS_PER_MONITOR_DPI_AWARE = 2
};

enum enMMMONITOR_DPI_TYPE 
{
	MMMDT_EFFECTIVE_DPI = 0,
	MMMDT_ANGULAR_DPI = 1,
	MMMDT_RAW_DPI = 2,
	MMMDT_DEFAULT = MMMDT_EFFECTIVE_DPI
};

class MMHELPER_API CMMDpi
{
public:
	CMMDpi(void);

	//variant
private:
	int									m_nScaleFactor = 0;
	int									m_nScaleFactorSDA = 0;
	static enMMPROCESS_DPI_AWARENESS	m_ProcessDPIAwareness;

	//method
public:
	static int GetDpiOfMainMonitor();
	static int GetDpiOfMonitor(HMONITOR hMonitor);
	static int GetDpiOfMonitorNearestToPoint(POINT pt);
	static enMMPROCESS_DPI_AWARENESS GetProcessDPIAwareness();
	static bool SetProcessDPIAwareness(enMMPROCESS_DPI_AWARENESS Awareness);

	//method
public:
	int GetScale() const;
	void SetScale(int nScale);
	UINT GetDpi() const;
	void SetDpi(int nDpi);

	//scale
	int Scale(int nValue) const;
	POINT Scale(POINT ptPoint) const;
	SIZE Scale(SIZE szSize) const;
	RECT Scale(RECT rcRect) const;

	//scaleback
	int ScaleBack(int nValue) const;
	SIZE ScaleBack(SIZE szSize) const;
	POINT ScaleBack(POINT pt) const;
	RECT ScaleBack(RECT rcRect) const;

	//makesure scalback param after scale is >= target
	int ScaleVerify(int nTarget, int nScaleBack) const;
	SIZE ScaleVerify(SIZE szTarget, SIZE szScaleBack) const;
	POINT ScaleVerify(POINT ptTarget, POINT ptScaleBack) const;
	RECT ScaleVerify(RECT rcTarget, RECT rcScaleBack) const;
}; 

//////////////////////////////////////////////////////////////////////////

#endif
#endif