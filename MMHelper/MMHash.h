#ifndef __MM_HASH_H__
#define __MM_HASH_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
const static uint32_t MM_FNV_offset_basis = 2166136261U;
const static uint32_t MM_FNV_prime = 16777619U;

inline uint32_t MM_Fnv1a_append_bytes(uint32_t _Val,
	const unsigned char * const _First, const uint32_t _Count)
{	// accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
	for (uint32_t _Idx = 0; _Idx < _Count; ++_Idx)
	{
		_Val ^= static_cast<uint32_t>(_First[_Idx]);
		_Val *= MM_FNV_prime;
	}

	return (_Val);
}

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMHash
{
public:
	static uint32_t GetHash(CMMString &str);
	static uint32_t GetHash(LPCSTR lpszStr);
	static uint32_t GetHash(LPCTSTR lpszStr);
	static uint32_t GetHash(const std::vector<CMMString> &vecStr);
};

//////////////////////////////////////////////////////////////////////////

#endif

