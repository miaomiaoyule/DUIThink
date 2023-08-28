#include "stdafx.h"
#include "MMSvg.h"

//////////////////////////////////////////////////////////////////////////
resvg_options *							g_pResvgOptions = NULL;

//////////////////////////////////////////////////////////////////////////
CMMSvg::CMMSvg()
{

}

CMMSvg::~CMMSvg()
{

}

CMMSvg * CMMSvg::GetInstance()
{
	static CMMSvg Obj;
	return &Obj;
}

bool CMMSvg::Init()
{
#ifdef _DEBUG
	resvg_init_log();
#endif

	g_pResvgOptions = resvg_options_create();
	resvg_options_load_system_fonts(g_pResvgOptions);

	return true;
}

bool CMMSvg::UnInit()
{
	if (g_pResvgOptions)
	{
		resvg_options_destroy(g_pResvgOptions);
		g_pResvgOptions = NULL;
	}

	return true;
}

bool CMMSvg::ParseImage(IN LPCTSTR lpszFile, IN int nScale, OUT std::vector<BYTE> &vecPixel, OUT int &nWidth, OUT int &nHeight)
{
	if (NULL == g_pResvgOptions || MMInvalidString(lpszFile)) return false;

	std::vector<BYTE> vecData;
	CMMFile::GetFileData(lpszFile, vecData);
	if (vecData.empty()) return false;

	return ParseImage(vecData, nScale, vecPixel, nWidth, nHeight);
}

bool CMMSvg::ParseImage(IN LPCTSTR lpszFile, IN int nScale, OUT HBITMAP &hBitmap, OUT int &nWidth, OUT int &nHeight, OUT LPBYTE *pBits)
{
	std::vector<BYTE> vecData;
	CMMFile::GetFileData(lpszFile, vecData);
	if (vecData.empty()) return false;

	return ParseImage(vecData, nScale, hBitmap, nWidth, nHeight, pBits);
}

bool CMMSvg::ParseImage(IN const std::vector<BYTE> &vecData, IN int nScale, OUT std::vector<BYTE> &vecPixel, OUT int &nWidth, OUT int &nHeight)
{
	if (NULL == g_pResvgOptions || vecData.empty()) return false;

	resvg_render_tree *tree = NULL;
	int err = resvg_parse_tree_from_data((const char*)vecData.data(), vecData.size(), g_pResvgOptions, &tree);
	if (NULL == tree || RESVG_OK != err)
	{
		printf("parse svg Error id: %i\n", err);
		assert(false);
		return false;
	}

	float fScale = nScale * 1.0 / 100;
	resvg_size size = resvg_get_image_size(tree);
	nWidth = MulDiv((int)size.width, nScale, 100);
	nHeight = MulDiv((int)size.height, nScale, 100);

	vecPixel.resize(nWidth * nHeight * 4);
	resvg_fit_to fit_to = { RESVG_FIT_TO_TYPE_ZOOM, fScale };
	resvg_render(tree, fit_to, resvg_transform_identity(), nWidth, nHeight, (char*)vecPixel.data());

	resvg_tree_destroy(tree);

	return true;
}

bool CMMSvg::ParseImage(IN const std::vector<BYTE> &vecData, IN int nScale, OUT HBITMAP &hBitmap, OUT int &nWidth, OUT int &nHeight, OUT LPBYTE *pBits)
{
	std::vector<BYTE> vecPixel;
	if (false == ParseImage(vecData, nScale, vecPixel, nWidth, nHeight)) return false;
	if (vecPixel.empty()) return false;

	BITMAPINFO BmpSrc = {};
	BmpSrc.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmpSrc.bmiHeader.biWidth = nWidth;
	BmpSrc.bmiHeader.biHeight = -nHeight;
	BmpSrc.bmiHeader.biPlanes = 1;
	BmpSrc.bmiHeader.biBitCount = 32;
	BmpSrc.bmiHeader.biCompression = BI_RGB;
	BmpSrc.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(DWORD);
	BmpSrc.bmiHeader.biXPelsPerMeter = 0;
	BmpSrc.bmiHeader.biYPelsPerMeter = 0;
	BmpSrc.bmiHeader.biClrUsed = 0;
	BmpSrc.bmiHeader.biClrImportant = 0;

	HDC hDCScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	hBitmap = CreateDIBSection(hDCScreen, &BmpSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);
	DeleteDC(hDCScreen);

	if (NULL == hBitmap)
	{
		assert(false);
		return false;
	}

	LPBYTE pPixel = vecPixel.data();
	for (int i = 0; i < nWidth * nHeight; i++)
	{
		(*pBits)[i * 4] = pPixel[i * 4 + 2];
		(*pBits)[i * 4 + 1] = pPixel[i * 4 + 1];
		(*pBits)[i * 4 + 2] = pPixel[i * 4];
		(*pBits)[i * 4 + 3] = pPixel[i * 4 + 3];
	}

	return true;
}