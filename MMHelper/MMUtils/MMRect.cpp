#include "StdAfx.h"
#include "MMRect.h"

//////////////////////////////////////////////////////////////////////////
CMMRect::CMMRect()
{
	left = right = top = bottom = 0;
}

CMMRect::CMMRect(const RECT& src)
{
	left = src.left;
	top = src.top;
	right = src.right;
	bottom = src.bottom;
}

CMMRect::CMMRect(long left, long top, long right, long bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

CMMRect::~CMMRect()
{

}

CMMRect CMMRect::RectRelToAbs(CMMRect& rcParentAbs, CMMRect& rcChildRel)
{
	CMMRect rcAbs;

	rcAbs.left = rcParentAbs.left + rcChildRel.left;
	rcAbs.top = rcParentAbs.top + rcChildRel.top;
	rcAbs.right = rcAbs.left + rcChildRel.GetWidth();
	rcAbs.bottom = rcAbs.top + rcChildRel.GetHeight();

	return rcAbs;
}

CMMRect CMMRect::RectAbsToRel(CMMRect& rcParentAbs, CMMRect& rcChildAbs)
{
	CMMRect rcRel;
	rcRel.left = rcChildAbs.left - rcParentAbs.left;
	rcRel.top = rcChildAbs.top - rcParentAbs.top;
	rcRel.right = rcRel.left + rcChildAbs.GetWidth();
	rcRel.bottom = rcRel.top + rcChildAbs.GetHeight();

	return rcRel;
}

int CMMRect::GetWidth() const
{
	return right - left;
}

int CMMRect::GetHeight() const
{
	return bottom - top;
}

uint32_t CMMRect::GetID() const
{
	CMMString strInfo = CMMStrHelp::Format(_T("%d-%d-%d-%d"), left, top, right, bottom);
	
	return CMMHash().GetHash(strInfo);
}

CMMRect CMMRect::RectClip(const CMMRect& rcModal, const CMMRect& rect)
{
	CMMRect rcResult;
	::IntersectRect(&rcResult, &rcModal, &rect);

	return rcResult;
}

bool CMMRect::Empty() const
{
	return (GetWidth() <= 0 || GetHeight() <= 0);
}

void CMMRect::Clear()
{
	left = top = right = bottom = 0;

	return;
}

bool CMMRect::PtInRect(const CMMPoint &pt)
{
	POINT point = { pt.x, pt.y };
	return ::PtInRect(this, point) == TRUE;
}

bool CMMRect::RcInRect(const CMMRect &rc)
{
	return rc.left >= left && rc.right <= right && rc.top >= top && rc.bottom <= bottom;
}

void CMMRect::Join(const RECT& rc)
{
	if (rc.left < left) left = rc.left;
	if (rc.top < top) top = rc.top;
	if (rc.right > right) right = rc.right;
	if (rc.bottom > bottom) bottom = rc.bottom;

	return;
}

void CMMRect::Inflate(int cx, int cy)
{
	::InflateRect(this, cx, cy);
}

void CMMRect::Inflate(int l, int t, int r, int b)
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}

void CMMRect::Deflate(int cx, int cy)
{
	::InflateRect(this, -cx, -cy);
}

void CMMRect::Deflate(int l, int t, int r, int b)
{
	left += l;
	top += t;
	right -= r;
	bottom -= b;
}

void CMMRect::Union(CMMRect& rc)
{
	::UnionRect(this, this, &rc);
}

void CMMRect::CheckRect()
{
	if (right < left)
		right = left;

	if (bottom < top)
		bottom = top;
}

void CMMRect::Offset(CMMPoint pt)
{
	Offset(pt.x, pt.y);

	return;
}

void CMMRect::Offset(CMMSize sz)
{
	Offset(sz.cx, sz.cy);

	return;
}

void CMMRect::Offset(int cx, int cy)
{
	left += cx;
	right += cx;
	top += cy;
	bottom += cy;
}

bool CMMRect::operator == (const CMMRect& rect) const
{
	if (left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom)
		return true;

	return false;
}

bool CMMRect::operator != (const CMMRect& rect) const
{
	if (left != rect.left || top != rect.top || right != rect.right || bottom != rect.bottom)
		return true;

	return false;
}

bool operator == (const RECT &rcFirst, const CMMRect &rcNext)
{
	return rcNext == rcFirst;
}