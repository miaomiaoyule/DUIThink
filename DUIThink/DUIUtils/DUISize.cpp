#include "StdAfx.h"
#include "DUISize.h"


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

bool CDUISize::operator == (const SIZE &sz) const
{
	return cx == sz.cx && cy == sz.cy;
}

bool CDUISize::operator == (const CDUISize &sz) const
{
	return cx == sz.cx && cy == sz.cy;
}

bool CDUISize::operator != (const SIZE &sz) const
{
	return false == operator==(sz);
}

bool CDUISize::operator != (const CDUISize &sz) const
{
	return false == operator==(sz);
}

bool operator == (const SIZE &szFirst, const CDUISize &szNext)
{
	return szNext == szFirst;
}