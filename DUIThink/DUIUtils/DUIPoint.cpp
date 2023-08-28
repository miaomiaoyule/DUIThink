#include "StdAfx.h"
#include "DUIPoint.h"

//////////////////////////////////////////////////////////////////////////
CDUIPoint::CDUIPoint()
{
	x = y = 0;
}

CDUIPoint::CDUIPoint(const POINT &src)
{
	x = src.x;
	y = src.y;
}

CDUIPoint::CDUIPoint(int _x, int _y)
{
	x = _x;
	y = _y;
}

CDUIPoint::CDUIPoint(LPARAM lParam)
{
	x = GET_X_LPARAM(lParam);
	y = GET_Y_LPARAM(lParam);
}

void CDUIPoint::Offset(int xOffset, int yOffset)
{
	x += xOffset;
	y += yOffset;

	return;
}

bool CDUIPoint::operator == (const CDUIPoint &pt) const
{
	return x == pt.x && y == pt.y;
}