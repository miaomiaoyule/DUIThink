#ifndef __MAGNETBOX_H__
#define __MAGNETBOX_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
//code by 城南花已开 2020-1-25 QQ:284168136
//////////////////////////////////////////////////////////////////////////
#define MagnetType_None					(0x00)
#define MagnetType_Left					(0x01)
#define MagnetType_Top					(0x02)
#define MagnetType_Right				(0x04)
#define MagnetType_Bottom				(0x08)

typedef std::unordered_map<int, CRect> MapMagnetBox;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMMagnetBox
{
	struct tagMagnetBox : public CRect
	{
		int							nIndex = -1;
		int							MagnetType = MagnetType_None;

		virtual tagMagnetBox & operator = (_In_ const CRect &rcRight)
		{
			__super::operator = (rcRight);
			
			return *this;
		}
	};
	struct tagFindMagnetBox
	{
		tagMagnetBox				rcBoxLeft;
		tagMagnetBox				rcBoxTop;
		tagMagnetBox				rcBoxRight;
		tagMagnetBox				rcBoxBottom;
	};

public:
	CMMMagnetBox();
	virtual ~CMMMagnetBox() {}

protected:
	MapMagnetBox					m_mapMagnetBox;
	tagFindMagnetBox				m_CurBoxMagnetInfo;
	int								m_nMagnetType = MagnetType_Left | MagnetType_Top | MagnetType_Right | MagnetType_Bottom;
	int								m_nMagnetValue = 12;
	bool							m_bBoxMovingX = false;
	bool							m_bBoxMovingY = false;
	CPoint							m_ptPullTotal = {};
	CRect							m_rcPullTotal = {};

public:
	void PushBox(int nIndex, CRect rcBox);
	void RemoveAt(int nIndex);
	void RemoveAll();
	void SceneInit(int nMagnetType = MagnetType_Left | MagnetType_Top | MagnetType_Right | MagnetType_Bottom, int nMagnetValue = 12);

	CRect MoveBox(int nIndex, IN CPoint ptOffset);
	CRect StretchBox(int nIndex, IN CRect rcStretch, OUT bool &bMagnet);

	//help
protected:
	void InitBoxMagnetInfo(int nIndex);
	bool MoveOffset(int nIndex, IN CPoint ptOffset, OUT CPoint &ptOffsetRealy);
	bool StretchOffset(int nIndex, IN CRect rcStretch, OUT CRect &rcStretchRealy);
	void FindNearestMagnetBox(int nIndex, int nMoveDirection, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxLeftDirection(int nIndex, CRect rcBoxItem, CRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxRightDirection(int nIndex, CRect rcBoxItem, CRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxTopDirection(int nIndex, CRect rcBoxItem, CRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxBottomDirection(int nIndex, CRect rcBoxItem, CRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
};

//////////////////////////////////////////////////////////////////////////
#endif
