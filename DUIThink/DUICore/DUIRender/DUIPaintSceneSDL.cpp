#include "StdAfx.h"
#include "DUIPaintSceneSDL.h"
#include "DUICanvasRaster.h"

#if defined(DuiPlatform_SDL)

//////////////////////////////////////////////////////////////////////////
CDUIPaintSceneSDL::CDUIPaintSceneSDL(HWND hWnd, int nWidth, int nHeight)
	: m_hWnd(hWnd)
{
	CreateGpu(nWidth, nHeight);
}

CDUIPaintSceneSDL::~CDUIPaintSceneSDL()
{
	ReleaseGpu();
}

IDuiCanvas * CDUIPaintSceneSDL::GetCanvas()
{
	return m_pCanvas;
}

HDC CDUIPaintSceneSDL::GetCanvasHDC()
{
	return DuiHDCFromCanvas(m_pCanvas);
}

LPBYTE CDUIPaintSceneSDL::GetBits()
{
	return m_pCanvas ? m_pCanvas->GetBits() : NULL;
}

int CDUIPaintSceneSDL::GetWidth() const
{
	return m_nWidth;
}

int CDUIPaintSceneSDL::GetHeight() const
{
	return m_nHeight;
}

bool CDUIPaintSceneSDL::Resize(int nWidth, int nHeight)
{
	if (nWidth == m_nWidth && nHeight == m_nHeight && m_pCanvas && m_pTexture)
	{
		return true;
	}

	return CreateGpu(nWidth, nHeight);
}

bool CDUIPaintSceneSDL::CreateGpu(int nWidth, int nHeight)
{
	ReleaseGpu();

	m_nWidth = max(1, nWidth);
	m_nHeight = max(1, nHeight);
	m_pCanvas = new CDUICanvasRaster(m_nWidth, m_nHeight);

	if (NULL == m_hWnd) return false;

	m_pRenderer = SDL_CreateRenderer(m_hWnd, NULL);
	if (NULL == m_pRenderer)
	{
		return false;
	}

	SDL_SetRenderVSync(m_pRenderer, 1);
	m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, m_nWidth, m_nHeight);
	if (NULL == m_pTexture)
	{
		return false;
	}

	SDL_SetTextureBlendMode(m_pTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureScaleMode(m_pTexture, SDL_SCALEMODE_LINEAR);
	return true;
}

void CDUIPaintSceneSDL::ReleaseGpu()
{
	if (m_pTexture)
	{
		SDL_DestroyTexture(m_pTexture);
		m_pTexture = NULL;
	}
	if (m_pRenderer)
	{
		SDL_DestroyRenderer(m_pRenderer);
		m_pRenderer = NULL;
	}
	MMSafeDelete(m_pCanvas);
	m_nWidth = 0;
	m_nHeight = 0;
}

void CDUIPaintSceneSDL::Present()
{
	RECT rc = { 0, 0, m_nWidth, m_nHeight };
	Present(rc);
}

void CDUIPaintSceneSDL::Present(const RECT &rcDirty)
{
	if (NULL == m_pRenderer || NULL == m_pTexture || NULL == m_pCanvas || NULL == m_pCanvas->GetBits())
	{
		return;
	}

	SDL_Rect rcUpdate = { 0, 0, m_nWidth, m_nHeight };
	if (rcDirty.right > rcDirty.left && rcDirty.bottom > rcDirty.top)
	{
		rcUpdate.x = max(0, rcDirty.left);
		rcUpdate.y = max(0, rcDirty.top);
		rcUpdate.w = min(m_nWidth - rcUpdate.x, rcDirty.right - rcDirty.left);
		rcUpdate.h = min(m_nHeight - rcUpdate.y, rcDirty.bottom - rcDirty.top);
	}

	const int nPitch = m_pCanvas->GetPitch();
	BYTE *pBits = m_pCanvas->GetBits() + rcUpdate.y * nPitch + rcUpdate.x * 4;
	SDL_UpdateTexture(m_pTexture, &rcUpdate, pBits, nPitch);

	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer);
	SDL_RenderTexture(m_pRenderer, m_pTexture, NULL, NULL);
	SDL_RenderPresent(m_pRenderer);
}

#endif
