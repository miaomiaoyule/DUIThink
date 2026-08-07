#ifndef __MMPOINT_H__
#define __MMPOINT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMPoint : public POINT
{
public:
	CMMPoint();
	CMMPoint(const POINT &src);
	CMMPoint(int x, int y);
	CMMPoint(LPARAM lParam);
	void Offset(int xOffset, int yOffset);

	bool operator == (const CMMPoint &pt) const;
	bool operator != (const CMMPoint &pt) const;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
