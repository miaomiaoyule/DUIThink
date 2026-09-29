#ifndef __DUIRECT_H__
#define __DUIRECT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUIRect : public CMMRect
{
public:
	CDUIRect();
	CDUIRect(const RECT& src);
	CDUIRect(long left, long top, long right, long bottom);
	~CDUIRect();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif