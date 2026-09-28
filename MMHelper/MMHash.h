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

// Designer Attribute.xml ValueIDs are FNV-1a of UTF-16LE code units (Windows wchar_t).
// Linux wchar_t is UTF-32; hashing native bytes would not match those IDs.
inline uint32_t MM_Fnv1a_append_utf16le_units(uint32_t _Val, unsigned int cp)
{
	if (cp > 0xFFFF)
	{
		cp -= 0x10000;
		const unsigned short high = (unsigned short)(0xD800 + (cp >> 10));
		const unsigned short low = (unsigned short)(0xDC00 + (cp & 0x3FF));
		const unsigned char buf[4] =
		{
			(unsigned char)(high & 0xFF), (unsigned char)(high >> 8),
			(unsigned char)(low & 0xFF), (unsigned char)(low >> 8)
		};
	
		return MM_Fnv1a_append_bytes(_Val, buf, 4);
	}

	const unsigned short cu = (unsigned short)cp;
	const unsigned char buf[2] = { (unsigned char)(cu & 0xFF), (unsigned char)(cu >> 8) };
	return MM_Fnv1a_append_bytes(_Val, buf, 2);
}

inline uint32_t MM_Fnv1a_append_lpctstr(uint32_t _Val, LPCTSTR psz, uint32_t nChars)
{
	if (NULL == psz || 0 == nChars) return _Val;

#ifdef UNICODE
	for (uint32_t i = 0; i < nChars; ++i)
	{
		unsigned int cp = (unsigned int)psz[i];
		if (sizeof(TCHAR) == 2)
		{
			cp &= 0xFFFFu;
		}
	
		_Val = MM_Fnv1a_append_utf16le_units(_Val, cp);
	}
	
	return _Val;
#else
	return MM_Fnv1a_append_bytes(_Val, (const unsigned char *)psz, nChars);
#endif
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
//hash
namespace std
{
	template<> struct hash<CMMString>
	{
		inline uint32_t operator()(const CMMString &str) const
		{
			//forbid CMMHash::GetHash(str) to avoid dependency, and use FNV-1a algorithm directly here
			//if CMMHash::GetHash(str) connot delay load MMHelper dll
			return MM_Fnv1a_append_lpctstr(MM_FNV_offset_basis, str.c_str(), (uint32_t)str.length());
		}
	};
}

//////////////////////////////////////////////////////////////////////////

#endif

