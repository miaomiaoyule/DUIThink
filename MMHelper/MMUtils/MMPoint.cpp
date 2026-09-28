#include "StdAfx.h"
#include "MMPoint.h"

//////////////////////////////////////////////////////////////////////////
CMMPoint::CMMPoint()
{
	x = y = 0;
}

CMMPoint::CMMPoint(const POINT &src)
{
	x = src.x;
	y = src.y;
}

CMMPoint::CMMPoint(int _x, int _y)
{
	x = _x;
	y = _y;
}

CMMPoint::CMMPoint(LPARAM lParam)
{
	x = GET_X_LPARAM(lParam);
	y = GET_Y_LPARAM(lParam);
}

void CMMPoint::Offset(int xOffset, int yOffset)
{
	x += xOffset;
	y += yOffset;

	return;
}

bool CMMPoint::operator == (const CMMPoint &pt) const
{
	return x == pt.x && y == pt.y;
}

bool CMMPoint::operator != (const CMMPoint &pt) const
{
	return x != pt.x || y != pt.y;
}