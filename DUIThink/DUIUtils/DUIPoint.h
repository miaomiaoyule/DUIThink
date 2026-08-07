#ifndef __DUIPOINT_H__
#define __DUIPOINT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIPoint : public CMMPoint
{
public:
	CDUIPoint();
	CDUIPoint(const POINT &src);
	CDUIPoint(int x, int y);
	CDUIPoint(LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
