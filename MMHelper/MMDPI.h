#ifndef __MM_DPI_H__
#define __MM_DPI_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
enum enPROCESS_DPI_AWARENESS 
{
	PROCESS_DPI_UNAWARE = 0,
	PROCESS_SYSTEM_DPI_AWARE = 1,
	PROCESS_PER_MONITOR_DPI_AWARE = 2
};

enum enMONITOR_DPI_TYPE 
{
	MDT_EFFECTIVE_DPI = 0,
	MDT_ANGULAR_DPI = 1,
	MDT_RAW_DPI = 2,
	MDT_DEFAULT = MDT_EFFECTIVE_DPI
};

class MMHELPER_API CMMDpi
{
public:
	CMMDpi(void);

	//variant
private:
	static int							m_nScaleFactor;
	static int							m_nScaleFactorSDA;
	static enPROCESS_DPI_AWARENESS		m_ProcessDPIAwareness;

	//method
public:
	static int GetDpiOfMainMonitor();
	static int GetDpiOfMonitor(HMONITOR hMonitor);
	static int GetDpiOfMonitorNearestToPoint(POINT pt);
	static enPROCESS_DPI_AWARENESS GetProcessDPIAwareness();
	static bool SetProcessDPIAwareness(enPROCESS_DPI_AWARENESS Awareness);

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