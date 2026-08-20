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
#if defined DuiPlatform_SDL
class SizeF
{
public:
    SizeF()
    {
        Width = Height = 0.0f;
    }

    SizeF(IN const SizeF& size)
    {
        Width = size.Width;
        Height = size.Height;
    }

    SizeF(IN REAL width,
        IN REAL height)
    {
        Width = width;
        Height = height;
    }

    SizeF operator+(IN const SizeF& sz) const
    {
        return SizeF(Width + sz.Width,
            Height + sz.Height);
    }

    SizeF operator-(IN const SizeF& sz) const
    {
        return SizeF(Width - sz.Width,
            Height - sz.Height);
    }

    BOOL Equals(IN const SizeF& sz) const
    {
        return (Width == sz.Width) && (Height == sz.Height);
    }

    BOOL Empty() const
    {
        return (Width == 0.0f && Height == 0.0f);
    }

public:

    REAL Width;
    REAL Height;
};
#endif

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
