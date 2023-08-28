#ifndef __DUIPOINT_H__
#define __DUIPOINT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIPoint : public POINT
{
public:
	CDUIPoint();
	CDUIPoint(const POINT &src);
	CDUIPoint(int x, int y);
	CDUIPoint(LPARAM lParam);
	void Offset(int xOffset, int yOffset);

	bool operator == (const CDUIPoint &pt) const;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
