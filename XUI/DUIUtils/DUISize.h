#ifndef __DUISIZE_H__
#define __DUISIZE_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUISize : public CMMSize
{
public:
	CDUISize();
	CDUISize(long cx, long cy);
	CDUISize(SIZE sz);
	~CDUISize();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
