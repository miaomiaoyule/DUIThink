#ifndef __MMSIZE_H__
#define __MMSIZE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMSize : public SIZE
{
public:
	CMMSize();
	CMMSize(long cx, long cy);
	CMMSize(SIZE sz);
	~CMMSize();

	bool operator == (const SIZE &sz) const;
	bool operator == (const CMMSize &sz) const;
	bool operator != (const SIZE &sz) const;
	bool operator != (const CMMSize &sz) const;
};

MMHELPER_API bool operator == (const SIZE &szFirst, const CMMSize &szNext);

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
