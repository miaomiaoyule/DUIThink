#ifndef __MMRECT_H__
#define __MMRECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMRect : public RECT
{
public:
	CMMRect();
	CMMRect(const RECT& src);
	CMMRect(long left, long top, long right, long bottom);
	~CMMRect();

public:
	static CMMRect RectRelToAbs(CMMRect& rcParentAbs, CMMRect& rcChildRel);
	static CMMRect RectAbsToRel(CMMRect& rcParentAbs, CMMRect& rcChildAbs);

	int GetWidth() const;
	int GetHeight() const;
	uint32_t GetID() const;

	//²Ã¼ô
	CMMRect RectClip(const CMMRect& rcModal, const CMMRect& rect);
	bool Empty() const;
	void Clear();
	bool PtInRect(const CMMPoint &pt);
	bool RcInRect(const CMMRect &rc);
	void Join(const RECT& rc);
	void Inflate(int cx, int cy);
	void Inflate(int l, int t, int r, int b);
	void Deflate(int cx, int cy);
	void Deflate(int l, int t, int r, int b);
	void Union(CMMRect& rc);

	void CheckRect();

	void Offset(CMMPoint pt);
	void Offset(CMMSize sz);
	void Offset(int cx, int cy);

	//ÔËËã·û
	bool operator == (const CMMRect &rect) const;
	bool operator != (const CMMRect &rect) const;
};

MMHELPER_API bool operator == (const RECT &rcFirst, const CMMRect &rcNext);

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif