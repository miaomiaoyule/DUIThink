#include "StdAfx.h"
#include "DUIRect.h"


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

CDUIRect CDUIRect::RectRelToAbs(CDUIRect& rcParentAbs, CDUIRect& rcChildRel)
{
	CDUIRect rcAbs;

	rcAbs.left = rcParentAbs.left + rcChildRel.left;
	rcAbs.top = rcParentAbs.top + rcChildRel.top;
	rcAbs.right = rcAbs.left + rcChildRel.GetWidth();
	rcAbs.bottom = rcAbs.top + rcChildRel.GetHeight();

	return rcAbs;
}

CDUIRect CDUIRect::RectAbsToRel(CDUIRect& rcParentAbs, CDUIRect& rcChildAbs)
{
	CDUIRect rcRel;
	rcRel.left = rcChildAbs.left - rcParentAbs.left;
	rcRel.top = rcChildAbs.top - rcParentAbs.top;
	rcRel.right = rcRel.left + rcChildAbs.GetWidth();
	rcRel.bottom = rcRel.top + rcChildAbs.GetHeight();

	return rcRel;
}

int CDUIRect::GetWidth() const
{
	return right - left;
}

int CDUIRect::GetHeight() const
{
	return bottom - top;
}

uint32_t CDUIRect::GetID() const
{
	CMMString strInfo = CMMStrHelp::Format(_T("%d-%d-%d-%d"), left, top, right, bottom);
	
	return CMMHash().GetHash(strInfo);
}

CDUIRect CDUIRect::RectClip(const CDUIRect& rcModal, const CDUIRect& rect)
{
	CDUIRect rcResult;
	::IntersectRect(&rcResult, &rcModal, &rect);

	return rcResult;
}

bool CDUIRect::IsEmpty()
{
	return (GetWidth() <= 0 || GetHeight() <= 0);
}

void CDUIRect::Empty()
{
	left = top = right = bottom = 0;

	return;
}

bool CDUIRect::PtInRect(const CDUIPoint &pt)
{
	return ::PtInRect(this, pt);
}

bool CDUIRect::RcInRect(const CDUIRect &rc)
{
	return rc.left >= left && rc.right <= right && rc.top >= top && rc.bottom <= bottom;
}

void CDUIRect::Join(const RECT& rc)
{
	if (rc.left < left) left = rc.left;
	if (rc.top < top) top = rc.top;
	if (rc.right > right) right = rc.right;
	if (rc.bottom > bottom) bottom = rc.bottom;

	return;
}

void CDUIRect::Inflate(int cx, int cy)
{
	::InflateRect(this, cx, cy);
}

void CDUIRect::Inflate(int l, int t, int r, int b)
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}

void CDUIRect::Deflate(int cx, int cy)
{
	::InflateRect(this, -cx, -cy);
}

void CDUIRect::Deflate(int l, int t, int r, int b)
{
	left += l;
	top += t;
	right -= r;
	bottom -= b;
}

void CDUIRect::Union(CDUIRect& rc)
{
	::UnionRect(this, this, &rc);
}

void CDUIRect::CheckRect()
{
	if (right < left)
		right = left;

	if (bottom < top)
		bottom = top;
}

void CDUIRect::Offset(CDUISize sz)
{
	left += sz.cx;
	right += sz.cx;
	top += sz.cy;
	bottom += sz.cy;
}

void CDUIRect::Offset(int cx, int cy)
{
	left += cx;
	right += cx;
	top += cy;
	bottom += cy;
}

bool CDUIRect::operator == (const CDUIRect& rect) const
{
	if (left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom)
		return true;

	return false;
}

bool CDUIRect::operator != (const CDUIRect& rect) const
{
	if (left != rect.left || top != rect.top || right != rect.right || bottom != rect.bottom)
		return true;

	return false;
}

bool operator == (const RECT &rcFirst, const CDUIRect &rcNext)
{
	return rcNext == rcFirst;
}