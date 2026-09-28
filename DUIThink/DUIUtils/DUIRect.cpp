#include "StdAfx.h"
#include "DUIRect.h"

//////////////////////////////////////////////////////////////////////////
CDUIRect::CDUIRect()
{
	left = right = top = bottom = 0;
}

CDUIRect::CDUIRect(const RECT& src)
{
	left = src.left;
	top = src.top;
	right = src.right;
	bottom = src.bottom;
}

CDUIRect::CDUIRect(long left, long top, long right, long bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

CDUIRect::~CDUIRect()
{

}