#ifndef __DUIATTRIBUTECOLORSWITCH_H__
#define __DUIATTRIBUTECOLORSWITCH_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, std::vector<CMMString>> UnorderMapDuiAttributeColorRes;
typedef std::map<uint32_t, std::vector<CMMString>> MapDuiAttributeColorRes;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeColorSwitch INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeColorSwitch = { 0x7C5B1872,0x6A2A,0x40EC,0x8C,0x28,0x8E,0x67,0x00,0xC8,0x5F,0x9C };
class DUITHINK_API CDUIAttributeColorSwitch
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeColorSwitch)
	MMDeclare_ClassName(CDUIAttributeColorSwitch)

public:
	CDUIAttributeColorSwitch(void);
	~CDUIAttributeColorSwitch(void);

	//variant
protected:
	uint32_t							m_uValueHash = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeColorSwitch & operator = (CDUIAttributeObject &Right) override;

	//sign
	CMMString FormatInfo() override;

	//draw
	virtual bool DrawRect(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, CDUISize szBreakTop = {});
	virtual bool DrawPath(HDC hDC, const std::vector<CDUIPoint> &vecPtList, int nBorderSize);
	virtual bool DrawRound(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, int nXRound, int nYRound, bool bHSLAdjust = false);
	virtual bool DrawRoundRect(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, int nXRound, int nYRound, bool bHSLAdjust = false, CDUISize szBreakTop = {});
	virtual bool DrawLine(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, enDuiLineStyle LineStyle = LineStyle_Solid, bool bHSLAdjust = false);
	virtual bool FillRect(HDC hDC, const CDUIRect &rcPaint, bool bHSLAdjust = false);
	virtual bool FillRoundRect(HDC hDC, const CDUIRect &rcPaint, int nBorderSize, int nXRound, int nYRound, bool bHSLAdjust = false);

	//color
	virtual bool IsEmpty();
	virtual ARGB GetColorValue();
	virtual Color GetColorObject();

	virtual vector<CMMString> GetColorResSwitch();
	virtual bool SetColorResSwitch(const vector<CMMString> &vecColorRes);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;

	//help
protected:
	uint32_t GetValueID() override;
	void SetValueID(uint32_t uValueID) override;
	CDUIColorBase * GetColorBaseCur();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif