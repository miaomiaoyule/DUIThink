#ifndef __MM_MD5CHECKSUM_H__
#define __MM_MD5CHECKSUM_H__

#pragma once

///////////////////////////////////////////////////////////////////////////////////////

//初始常量
#define MD5_INIT_STATE_0	0x67452301
#define MD5_INIT_STATE_1	0xefcdab89
#define MD5_INIT_STATE_2	0x98badcfe
#define MD5_INIT_STATE_3	0x10325476

//转换常量
#define MD5_S11		7
#define MD5_S12		12
#define MD5_S13		17
#define MD5_S14		22
#define MD5_S21		5
#define MD5_S22		9
#define MD5_S23		14
#define MD5_S24		20
#define MD5_S31		4
#define MD5_S32		11
#define MD5_S33		16
#define MD5_S34		23
#define MD5_S41		6
#define MD5_S42		10
#define MD5_S43		15
#define MD5_S44		21

//1轮转换常量
#define MD5_T01		0xd76aa478 
#define MD5_T02		0xe8c7b756 
#define MD5_T03		0x242070db 
#define MD5_T04		0xc1bdceee 
#define MD5_T05		0xf57c0faf 
#define MD5_T06		0x4787c62a 
#define MD5_T07		0xa8304613 
#define MD5_T08		0xfd469501 
#define MD5_T09		0x698098d8 
#define MD5_T10		0x8b44f7af 
#define MD5_T11		0xffff5bb1 
#define MD5_T12		0x895cd7be 
#define MD5_T13		0x6b901122 
#define MD5_T14		0xfd987193 
#define MD5_T15		0xa679438e 
#define MD5_T16		0x49b40821 

//2轮转换常量
#define MD5_T17		0xf61e2562 
#define MD5_T18		0xc040b340 
#define MD5_T19		0x265e5a51 
#define MD5_T20		0xe9b6c7aa 
#define MD5_T21		0xd62f105d 
#define MD5_T22		0x02441453 
#define MD5_T23		0xd8a1e681 
#define MD5_T24		0xe7d3fbc8 
#define MD5_T25		0x21e1cde6 
#define MD5_T26		0xc33707d6 
#define MD5_T27		0xf4d50d87 
#define MD5_T28		0x455a14ed 
#define MD5_T29		0xa9e3e905 
#define MD5_T30		0xfcefa3f8 
#define MD5_T31		0x676f02d9 
#define MD5_T32		0x8d2a4c8a 

//3轮转换常量
#define MD5_T33		0xfffa3942 
#define MD5_T34		0x8771f681 
#define MD5_T35		0x6d9d6122 
#define MD5_T36		0xfde5380c 
#define MD5_T37		0xa4beea44 
#define MD5_T38		0x4bdecfa9 
#define MD5_T39		0xf6bb4b60 
#define MD5_T40		0xbebfbc70 
#define MD5_T41		0x289b7ec6 
#define MD5_T42		0xeaa127fa 
#define MD5_T43		0xd4ef3085 
#define MD5_T44		0x04881d05 
#define MD5_T45		0xd9d4d039 
#define MD5_T46		0xe6db99e5 
#define MD5_T47		0x1fa27cf8 
#define MD5_T48		0xc4ac5665 

//4轮转换常量
#define MD5_T49		0xf4292244 
#define MD5_T50		0x432aff97 
#define MD5_T51		0xab9423a7 
#define MD5_T52		0xfc93a039 
#define MD5_T53		0x655b59c3 
#define MD5_T54		0x8f0ccc92 
#define MD5_T55		0xffeff47d 
#define MD5_T56		0x85845dd1 
#define MD5_T57		0x6fa87e4f 
#define MD5_T58		0xfe2ce6e0 
#define MD5_T59		0xa3014314 
#define MD5_T60		0x4e0811a1 
#define MD5_T61		0xf7537e82 
#define MD5_T62		0xbd3af235 
#define MD5_T63		0x2ad7d2bb 
#define MD5_T64		0xeb86d391 


//填充数据
static unsigned char PADDING[64] = 
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

///////////////////////////////////////////////////////////////////////////////////////

//MD5校验
class MMHELPER_API CMMMD5Checksum
{
	//存储变量
private:
	BYTE								m_lpszBuffer[64];             //输入缓冲
	ULONG								m_nCount[2];                  //位数计数
	ULONG								m_lMD5[4];                    //MD5校验和

	//函数定义
protected:
	CMMMD5Checksum();
	virtual ~CMMMD5Checksum();

	//静态函数
public:
	static CMMString GetMD5(BYTE *pBuf, UINT nLength);
	static CMMString GetMD5(FILE *pFile);
	static CMMString GetMD5(const CMMString &strFilePath);

	//辅助函数
protected:
	CMMString Final();

	//辅助函数
protected:
	VOID Transform(BYTE Block[64]);
	VOID Update(BYTE *Input, ULONG nInputLen);
	VOID DWordToByte(BYTE *Output, DWORD *Input, UINT nLength);
	VOID ByteToDWord(DWORD *Output, BYTE *Input, UINT nLength);

	//内联函数
protected:
	inline DWORD RotateLeft(DWORD x, int n);
	inline VOID FF(DWORD &A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline VOID GG(DWORD &A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline VOID HH(DWORD &A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline VOID II(DWORD &A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
};

///////////////////////////////////////////////////////////////////////////////////////
#endif