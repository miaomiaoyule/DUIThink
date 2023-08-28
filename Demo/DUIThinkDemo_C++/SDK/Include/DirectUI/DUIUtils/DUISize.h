#ifndef __DUISIZE_H__
#define __DUISIZE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUISize : public SIZE
{
public:
	CDUISize();
	CDUISize(long cx, long cy);
	CDUISize(SIZE sz);
	~CDUISize();

	bool operator == (const SIZE &sz) const;
	bool operator == (const CDUISize &sz) const;
	bool operator != (const SIZE &sz) const;
	bool operator != (const CDUISize &sz) const;
};

DUITHINK_API bool operator == (const SIZE &szFirst, const CDUISize &szNext);

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
