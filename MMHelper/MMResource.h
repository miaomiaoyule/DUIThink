#ifndef __MM_RESOURCE_H__
#define __MM_RESOURCE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMResource
{
public:
	CMMResource();
	virtual ~CMMResource();

	//variant
protected:

	//method
public:
	bool Load(IN HMODULE hModule, IN CMMString strResType, IN LPCTSTR lpszResName, OUT std::vector<BYTE> &vecData);
};

//////////////////////////////////////////////////////////////////////////////////

#endif