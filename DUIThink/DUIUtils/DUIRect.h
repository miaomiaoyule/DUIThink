#ifndef __DUIRECT_H__
#define __DUIRECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIRect : public RECT
{
public:
	CDUIRect();
	CDUIRect(const RECT& src);
	CDUIRect(long left, long top, long right, long bottom);
	~CDUIRect();

public:
	static CDUIRect RectRelToAbs(CDUIRect& rcParentAbs, CDUIRect& rcChildRel);
	static CDUIRect RectAbsToRel(CDUIRect& rcParentAbs, CDUIRect& rcChildAbs);

	int GetWidth() const;
	int GetHeight() const;
	uint32_t GetID() const;

	//²Ã¼ô
	CDUIRect RectClip(const CDUIRect& rcModal, const CDUIRect& rect);
	bool IsEmpty();
	void Empty();
	bool PtInRect(const CDUIPoint &pt);
	bool RcInRect(const CDUIRect &rc);
	void Join(const RECT& rc);
	void Inflate(int cx, int cy);
	void Inflate(int l, int t, int r, int b);
	void Deflate(int cx, int cy);
	void Deflate(int l, int t, int r, int b);
	void Union(CDUIRect& rc);

	void CheckRect();

	void Offset(CDUISize sz);
	void Offset(int cx, int cy);

	//ÔËËã·û
	bool operator == (const CDUIRect &rect) const;
	bool operator != (const CDUIRect &rect) const;
};

DUITHINK_API bool operator == (const RECT &rcFirst, const CDUIRect &rcNext);

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif