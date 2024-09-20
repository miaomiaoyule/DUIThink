#include "StdAfx.h"
#include "DUIWaterCtrl.h"

//////////////////////////////////////////////////////////////////////////
#define Water_StoneSize					(10)
#define Water_StoneDepth				(50)

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIWaterCtrl)
MMImplement_ClassName(CDUIWaterCtrl)

CDUIWaterCtrl::CDUIWaterCtrl(void)
{
	return;
}

CDUIWaterCtrl::~CDUIWaterCtrl(void)
{
	UnInitWater();

	return;
}

LPVOID CDUIWaterCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWaterCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWaterCtrl::GetDescribe() const
{
	return Dui_Ctrl_Water;
}

void CDUIWaterCtrl::RefreshView()
{
	__super::RefreshView();

	InitWater();

	return;
}

bool CDUIWaterCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	//water
	if (false == bGenerateBmp)
	{
		PaintWater(hDC);
	}

	//have ripple
	if (m_pRippleBmp && m_pRippleBmp->Ripple.bHaveRipple)
	{
		Invalidate();

		RippleSpread();
	}

	return true;
}

bool CDUIWaterCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDown(pt, Msg)) return false;

	DropStone(pt.x - m_rcAbsolute.left, pt.y - m_rcAbsolute.top);

	return true;
}

bool CDUIWaterCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseMove(pt, Msg)) return false;

	if (Msg.wParam & MK_LBUTTON)
	{
		DropStone(pt.x - m_rcAbsolute.left, pt.y - m_rcAbsolute.top);
	}

	return true;
}

void CDUIWaterCtrl::PaintWater(HDC hDC)
{
	if (NULL == m_pRippleBmp) return;

	HBITMAP hBitmapSrc = (HBITMAP)SelectObject(hDC, m_pRippleBmp->hBitmapSwap);
	if (NULL == hBitmapSrc) return;

	BITMAP BmpInfoSrc = {};
	::GetObject(hBitmapSrc, sizeof(BmpInfoSrc), &BmpInfoSrc);

	if (BmpInfoSrc.bmBits)
	{
		//copy
		for (int i = 0; i < m_pRippleBmp->Ripple.szBmpBuffer.cy; i++)
		{
			ARGB *pSrc = (ARGB*)BmpInfoSrc.bmBits + (GetAbsoluteRect().top + i) * BmpInfoSrc.bmWidth + GetAbsoluteRect().left;
			memcpy(m_pRippleBmp->vecBmpSwap.data() + m_pRippleBmp->Ripple.szBmpBuffer.cx * i, pSrc, sizeof(ARGB) * m_pRippleBmp->Ripple.szBmpBuffer.cx);
		}

		//render
		int xoff, yoff;
		int k = m_pRippleBmp->Ripple.szBmpBuffer.cx;
		for (int i = 1; i < m_pRippleBmp->Ripple.szBmpBuffer.cy - 1; i++)
		{
			for (int j = 0; j < m_pRippleBmp->Ripple.szBmpBuffer.cx; j++)
			{
				//计算偏移量
				xoff = m_pRippleBmp->Ripple.vecBufferRipple1[k - 1] - m_pRippleBmp->Ripple.vecBufferRipple1[k + 1];
				yoff = m_pRippleBmp->Ripple.vecBufferRipple1[k - m_pRippleBmp->Ripple.szBmpBuffer.cx] - m_pRippleBmp->Ripple.vecBufferRipple1[k + m_pRippleBmp->Ripple.szBmpBuffer.cx];

				//判断坐标是否在窗口范围内
				if ((i + yoff) < 0) { k++; continue; }
				if ((i + yoff) >= m_pRippleBmp->Ripple.szBmpBuffer.cy) { k++; continue; }
				if ((j + xoff) < 0) { k++; continue; }
				if ((j + xoff) >= m_pRippleBmp->Ripple.szBmpBuffer.cx) { k++; continue; }

				//计算出偏移象素和原始象素的内存地址偏移量
				int pos1, pos2;
				pos1 = m_pRippleBmp->Ripple.szBmpBuffer.cx * (i + yoff) + (j + xoff);
				pos2 = m_pRippleBmp->Ripple.szBmpBuffer.cx * i + j;

				//复制象素
				m_pRippleBmp->vecBmpDest[pos2] = m_pRippleBmp->vecBmpSwap[pos1];

				k++;
			}
		}

		//restore
		for (int i = 0; i < m_pRippleBmp->Ripple.szBmpBuffer.cy; i++)
		{
			ARGB *pDest = (ARGB*)BmpInfoSrc.bmBits + (GetAbsoluteRect().top + i) * BmpInfoSrc.bmWidth + GetAbsoluteRect().left;
			memcpy(pDest, m_pRippleBmp->vecBmpDest.data() + m_pRippleBmp->Ripple.szBmpBuffer.cx * i, sizeof(ARGB) * m_pRippleBmp->Ripple.szBmpBuffer.cx);
		}
	}

	SelectObject(hDC, hBitmapSrc);

	return;
}

void CDUIWaterCtrl::InitWater()
{
	if (NULL == m_pWndOwner
		|| NULL == m_pWndOwner->GetRootCtrl()) return;

	UnInitWater();

	m_pRippleBmp = new tagDuiRippleBitmap();
	m_pRippleBmp->Ripple.szBmpBuffer.cx = GetWidth(), m_pRippleBmp->Ripple.szBmpBuffer.cy = GetHeight();
	m_pRippleBmp->Ripple.vecBufferRipple1.clear();
	m_pRippleBmp->Ripple.vecBufferRipple2.clear();
	m_pRippleBmp->vecBmpSwap.clear();
	m_pRippleBmp->vecBmpDest.clear();
	m_pRippleBmp->Ripple.vecBufferRipple1.resize(m_pRippleBmp->Ripple.szBmpBuffer.cx * m_pRippleBmp->Ripple.szBmpBuffer.cy);
	m_pRippleBmp->Ripple.vecBufferRipple2.resize(m_pRippleBmp->Ripple.szBmpBuffer.cx * m_pRippleBmp->Ripple.szBmpBuffer.cy);
	m_pRippleBmp->vecBmpSwap.resize(m_pRippleBmp->Ripple.szBmpBuffer.cx * m_pRippleBmp->Ripple.szBmpBuffer.cy);
	m_pRippleBmp->vecBmpDest.resize(m_pRippleBmp->Ripple.szBmpBuffer.cx * m_pRippleBmp->Ripple.szBmpBuffer.cy);
	m_pRippleBmp->hBitmapSwap = CDUIRenderEngine::CreateARGB32Bitmap(m_pWndOwner->GetWndDC(), 1, 1, NULL);

	return;
}

void CDUIWaterCtrl::UnInitWater()
{
	if (NULL == m_pRippleBmp) return;

	MMSafeDeleteObject(m_pRippleBmp->hBitmapSwap);
	MMSafeDelete(m_pRippleBmp);

	return;
}

void CDUIWaterCtrl::RippleSpread()
{
	if (false == m_pRippleBmp->Ripple.bHaveRipple) return;

	m_pRippleBmp->Ripple.bHaveRipple = false;

	tagDuiRipple &Ripple = m_pRippleBmp->Ripple;
	Ripple.bHaveRipple = false;

	for (int i = Ripple.szBmpBuffer.cx; i < Ripple.szBmpBuffer.cx * Ripple.szBmpBuffer.cy - Ripple.szBmpBuffer.cx; i++)
	{
		//波能扩散
		if (i % Ripple.szBmpBuffer.cx == 0 || (i + 1) % Ripple.szBmpBuffer.cx == 0)
		{
			continue;
		}
		else
		{
			Ripple.vecBufferRipple2[i] = ((
				Ripple.vecBufferRipple1[i - 1] +
				Ripple.vecBufferRipple1[i + 1] +
				Ripple.vecBufferRipple1[i - Ripple.szBmpBuffer.cx] +
				Ripple.vecBufferRipple1[i + Ripple.szBmpBuffer.cx]) >> 1) - Ripple.vecBufferRipple2[i];
		}

		//波能衰减
		int8_t nRippleSubtract = Ripple.vecBufferRipple2[i] >> 5;
		0 == nRippleSubtract ? nRippleSubtract = Ripple.vecBufferRipple2[i] >> 4 : nRippleSubtract;
		0 == nRippleSubtract ? nRippleSubtract = Ripple.vecBufferRipple2[i] >> 3 : nRippleSubtract;
		0 == nRippleSubtract ? nRippleSubtract = Ripple.vecBufferRipple2[i] >> 2 : nRippleSubtract;
		0 == nRippleSubtract ? nRippleSubtract = Ripple.vecBufferRipple2[i] >> 1 : nRippleSubtract;
		0 == nRippleSubtract ? nRippleSubtract = Ripple.vecBufferRipple2[i] : nRippleSubtract;
		Ripple.vecBufferRipple2[i] -= nRippleSubtract;

		false == Ripple.bHaveRipple ? Ripple.bHaveRipple = 0 != Ripple.vecBufferRipple2[i] : Ripple.bHaveRipple;
	}

	//交换波能数据缓冲区
	Ripple.vecBufferRipple1.swap(Ripple.vecBufferRipple2);

	//noise point verify
	UINT uRippleTotal1 = 0, uRippleTotal2 = 0;
	for (int n = 0; n < Ripple.vecBufferRipple1.size() && Ripple.vecBufferRipple2.size(); n++)
	{
		uRippleTotal1 += abs(Ripple.vecBufferRipple1[n]);
		uRippleTotal2 += abs(Ripple.vecBufferRipple2[n]);
	}
	if (uRippleTotal1 == uRippleTotal2)
	{
		Ripple.vecBufferRipple1.clear();
		Ripple.vecBufferRipple2.clear();
		Ripple.vecBufferRipple1.resize(Ripple.szBmpBuffer.cx * Ripple.szBmpBuffer.cy);
		Ripple.vecBufferRipple2.resize(Ripple.szBmpBuffer.cx * Ripple.szBmpBuffer.cy);
		Ripple.bHaveRipple = false;
	}

	return;
}

void CDUIWaterCtrl::DropStone(int nX, int nY)
{
	if (NULL == m_pRippleBmp || m_pRippleBmp->Ripple.vecBufferRipple1.empty()) return;

	//判断坐标是否在屏幕范围内
	if ((nX + Water_StoneSize) > m_pRippleBmp->Ripple.szBmpBuffer.cx
		|| (nY + Water_StoneSize) > m_pRippleBmp->Ripple.szBmpBuffer.cy
		|| (nX - Water_StoneSize) < 0
		|| (nY - Water_StoneSize) < 0)
		return;

	for (int posx = nX - Water_StoneSize; posx < nX + Water_StoneSize; posx++)
	{
		for (int posy = nY - Water_StoneSize; posy < nY + Water_StoneSize; posy++)
		{
			if (posy == 0
				|| (m_pRippleBmp->Ripple.szBmpBuffer.cx * posy + posx) % m_pRippleBmp->Ripple.szBmpBuffer.cx == 0
				|| (m_pRippleBmp->Ripple.szBmpBuffer.cx * posy + posx + 1) % m_pRippleBmp->Ripple.szBmpBuffer.cx == 0
				|| (m_pRippleBmp->Ripple.szBmpBuffer.cx * posy + posx) > (m_pRippleBmp->Ripple.szBmpBuffer.cx * (m_pRippleBmp->Ripple.szBmpBuffer.cy - 1)))
				continue;

			if ((posx - nX) * (posx - nX) + (posy - nY) * (posy - nY) < Water_StoneSize * Water_StoneSize)
			{
				int nStoneWeight = Water_StoneDepth;
				m_pRippleBmp->Ripple.vecBufferRipple1[m_pRippleBmp->Ripple.szBmpBuffer.cx * posy + posx] = -nStoneWeight;

				m_pRippleBmp->Ripple.bHaveRipple = true;
			}
		}
	}

	return;
}

