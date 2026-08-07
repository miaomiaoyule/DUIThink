#include "StdAfx.h"
#include "MMSize.h"

//////////////////////////////////////////////////////////////////////////
CMMSize::CMMSize(void)
{
	cx = cy = 0;
}

CMMSize::CMMSize(long cx, long cy)
{
	this->cx = cx, this->cy = cy;
}

CMMSize::CMMSize(SIZE sz)
{
	cx = sz.cx;
	cy = sz.cy;

	return;
}

CMMSize::~CMMSize(void)
{
}

bool CMMSize::operator == (const SIZE &sz) const
{
	return cx == sz.cx && cy == sz.cy;
}

bool CMMSize::operator == (const CMMSize &sz) const
{
	return cx == sz.cx && cy == sz.cy;
}

bool CMMSize::operator != (const SIZE &sz) const
{
	return false == operator==(sz);
}

bool CMMSize::operator != (const CMMSize &sz) const
{
	return false == operator==(sz);
}

bool operator == (const SIZE &szFirst, const CMMSize &szNext)
{
	return szNext == szFirst;
}