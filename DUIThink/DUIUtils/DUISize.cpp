#include "StdAfx.h"
#include "DUISize.h"

//////////////////////////////////////////////////////////////////////////
CDUISize::CDUISize(void)
{
	cx = cy = 0;
}

CDUISize::CDUISize(long cx, long cy)
{
	this->cx = cx, this->cy = cy;
}

CDUISize::CDUISize(SIZE sz)
{
	cx = sz.cx;
	cy = sz.cy;

	return;
}

CDUISize::~CDUISize(void)
{
}