#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_DISPLAYER_H__
#define __MM_DISPLAYER_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define RESOLUTION_HORIZ				(1920)
#define RESOLUTION_VERT					(1040)

//////////////////////////////////////////////////////////////////////////
struct tagMMFindMonitor
{
	CMMString							strName;
	HMONITOR							hMonitor = NULL;
	HMONITOR							hMonitorFarLeft = NULL;
	HMONITOR							hMonitorFarTop = NULL;
	HMONITOR							hMonitorFarRight = NULL;
	HMONITOR							hMonitorFarBottom = NULL;
	HMONITOR							hMonitorPrimary = NULL;
	MONITORINFOEX						MonitorInfoFarLeft = {};
	MONITORINFOEX						MonitorInfoFarTop = {};
	MONITORINFOEX						MonitorInfoFarRight = {};
	MONITORINFOEX						MonitorInfoFarBottom = {};
	MONITORINFOEX						MonitorInfoPrimary = {};
	std::vector<MONITORINFOEX> 			vecMonitor;
};

typedef struct tagSIZEF
{
	tagSIZEF(float cx, float cy)
		: cx(cx)
		, cy(cy)
	{

	}

	float								cx = 0;
	float								cy = 0;
} SIZEF;

typedef struct tagRECTF
{
	float								left = 0;
	float								top = 0;
	float								right = 0;
	float								bottom = 0;
} RECTF;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMRectF : public RECTF
{
public:
	CMMRectF()
	{
	}
	CMMRectF(float fLeft, float fTop, float fRight, float fBottom)
	{
		left = fLeft;
		top = fTop;
		right = fRight;
		bottom = fBottom;

		return;
	}
	CMMRectF(const RECT &rc)
	{
		operator = (CRect(rc));

		return;
	}

	//method
public:
	CMMRectF operator = (const CRect &rc)
	{
		left = rc.left;
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;

		return *this;
	}
	bool operator == (const CMMRectF &Right)
	{
		return left == Right.left
			&& top == Right.top
			&& right == Right.right
			&& bottom == Right.bottom;
	}
	float GetWidth() { return right - left; }
	float GetHeight() { return bottom - top; }
	void Offset(int nX, int nY)
	{
		left += nX;
		right += nX;
		top += nY;
		bottom += nY;

		return;
	}
	bool PtInRect(POINT pt)
	{
		CRect rcThis = RECT{ (int)left, (int)top, (int)right, (int)bottom };
		return ::PtInRect(&rcThis, pt);
	}
	bool PtInRect(POINTF pt)
	{
		return PtInRect(POINT{ (int)pt.x, (int)pt.y });
	}
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMDisplayer
{
public:
	CMMDisplayer();
	~CMMDisplayer();

	//variant
protected:
	SIZE								m_szResolutionCur = {};

	//method
public:
	void SetResolution(SIZE szResolution);
	float Scale(float fValue, bool bHoriz);
	POINTF Scale(POINTF ptValue);
	SIZEF Scale(SIZEF szValue);
	POINTF ScaleSuitable(POINTF ptValue);
	SIZEF ScaleSuitable(SIZEF szValue);
	float ScaleBack(float fValue, bool bHoriz);
	POINTF ScaleBack(POINTF ptValue);
	SIZEF ScaleBack(SIZEF szValue);
	POINTF ScaleSuitableBack(POINTF ptValue);
	SIZEF ScaleSuitableBack(SIZEF szValue);

	//help
protected:
	bool VerifyResolutionCur();

	//static
public:
	/*
	* result: 0/���.1/�ϱ�.2/�ұ�.3/�ײ�
	* bTrayAutoHide-�Ƿ��Զ�����
	*/
	static int GetTaskBarInfo(RECT &rcTaskBar, BOOL &bTrayAutoHide);

	/*
	* ABE_LEFT/ABE_TOP/ABE_RIGHT/ABE_BOTTOM
	*/
	static bool GetTaskBarType(int &nDockType);
	static int GetTaskbarDockType(HMONITOR hMonitor);

	/*
	* ������ȡϵͳ��ʾ��
	*/
	bool GetComputerMonitor(tagMMFindMonitor &ComputerMonitor);

	/*
	* ��ȡ��ǰ��Ļ�ĸ���
	*/
	static int GetMultScreenNumber();

	/*
	 *���ݵ��ȡ������Ļ
	 */
	static MONITORINFOEX GetMonitorFromPoint(const POINT &pt);

	/*
	* ��ȡ����
	*/
	static MONITORINFOEX GetPrimaryMonitor();

	/*
	* ��ʾ���������ת����ʾ����������
	*/
	static bool MonitorRelToMonitorAbs(IN CMMRectF rcRelOfMonitor,
		IN CMMRectF rcRelOfWork,
		IN OUT CMMString &strMonitorName,
		IN OUT CMMDpi &DpiInfo,
		IN OUT CMMRectF &rcAbsOfMonitor,
		IN OUT CMMRectF &rcMonitorLast);

	/*
	* ��ʾ����������ת����ʾ���������
	*/
	static bool MonitorAbsToMonitorRel(IN CMMRectF rcAbsOfMonitor,
		IN OUT CMMString &strMonitorName,
		IN OUT CMMDpi &DpiInfo,
		IN OUT CMMRectF &rcRelOfMonitor,
		IN OUT CMMRectF &rcRelOfWork,
		IN OUT CMMRectF &rcMonitorLast);

	/*
	* ��ʾ����������ת����Ļ��������
	*/
	static CMMRectF MonitorAbsToScreenAbs(CMMRectF rcAbsolute);

	/*
	* ��Ļ��������ת����ʾ����������
	*/
	static CMMRectF ScreenAbsToMonitorAbs(CMMRectF rcAbsolute);

	/*
	* ��ȡ������Ļ��Ϣ
	*/
	static void GetScreenInfo(OUT CMMRectF &rcScreenWork, OUT CMMRectF &rcScreenDisplay, OUT CMMRectF &rcPrimary, OUT std::vector<MONITORINFOEX> &vecMonitor);

	/*
	* �����ǿɼ���ʾ����������Ϊ�ɼ�
	*/
	static CMMRectF PerformAdjustPosition(IN CMMRectF rcAbsOfMonitor);
};

#endif
#endif

