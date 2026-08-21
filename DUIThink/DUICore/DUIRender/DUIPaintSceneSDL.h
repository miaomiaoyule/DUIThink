#ifndef __DUIPAINTSCENESDL_H__
#define __DUIPAINTSCENESDL_H__

#pragma once

#include "IDuiCanvas.h"

#ifdef DuiPlatform_SDL

class CDUICanvasRaster;

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIPaintSceneSDL : public IDuiPaintScene
{
public:
	CDUIPaintSceneSDL(HWND hWnd, int nWidth, int nHeight);
	virtual ~CDUIPaintSceneSDL();

	IDuiCanvas * GetCanvas() override;
	HDC GetCanvasHDC() override;
	LPBYTE GetBits() override;
	int GetWidth() const override;
	int GetHeight() const override;
	bool Resize(int nWidth, int nHeight) override;
	void Present() override;
	void Present(const RECT &rcDirty) override;

protected:
	bool CreateGpu(int nWidth, int nHeight);
	void ReleaseGpu();

	HWND m_hWnd = NULL;
	SDL_Renderer *m_pRenderer = NULL;
	SDL_Texture *m_pTexture = NULL;
	CDUICanvasRaster *m_pCanvas = NULL;
	int m_nWidth = 0;
	int m_nHeight = 0;
};

//////////////////////////////////////////////////////////////////////////
#endif
#endif
