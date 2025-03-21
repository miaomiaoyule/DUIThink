#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_SVG_H__
#define __MM_SVG_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMSvg
{
protected:
	CMMSvg();
	~CMMSvg();

protected:

public:
	static CMMSvg * GetInstance();
	bool Init();
	bool UnInit();

	//method
public:
	//************************************
	// Description:	parse image argb from svg, vecPixel is RGBA8888
	// Parameter: 	LPCTSTR lpszFile Àý£ºC:\Test.svg
	//************************************
	bool ParseImage(IN LPCTSTR lpszFile, IN int nScale, OUT std::vector<BYTE> &vecPixel, OUT int &nWidth, OUT int &nHeight);

	//************************************
	// Description:	parse image HBITMAP from svg
	// Parameter: 	LPCTSTR lpszFile Àý£ºC:\Test.svg
	//************************************
	bool ParseImage(IN LPCTSTR lpszFile, IN int nScale, OUT HBITMAP &hBitmap, OUT int &nWidth, OUT int &nHeight, OUT LPBYTE *pBits);

	//************************************
	// Description:	parse image argb from svg, vecPixel is RGBA8888
	// Parameter: 	std::vector<BYTE> vecData £ºsvg file data
	//************************************
	bool ParseImage(IN const std::vector<BYTE> &vecData, IN int nScale, OUT std::vector<BYTE> &vecPixel, OUT int &nWidth, OUT int &nHeight);

	//************************************
	// Description:	parse image HBITMAP from svg
	// Parameter: 	std::vector<BYTE> vecData £ºsvg file data
	//************************************
	bool ParseImage(IN const std::vector<BYTE> &vecData, IN int nScale, OUT HBITMAP &hBitmap, OUT int &nWidth, OUT int &nHeight, OUT LPBYTE *pBits);
};

//////////////////////////////////////////////////////////////////////////

#endif
#endif
