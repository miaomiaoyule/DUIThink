#ifndef __DUIATTRI_IMAGESECTION_H__
#define __DUIATTRI_IMAGESECTION_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, tagDuiImageSection> MapDuiAttriImageSection;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttriImageSection INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttriImageSection = { 0xE656788D,0x3217,0x4F91,0xA7,0x21,0x29,0x5B,0x6D,0xAF,0x54,0xF9 };
class DUITHINK_API CDUIAttriImageSection
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttriImageSection)
	MMDeclare_ClassName(CDUIAttriImageSection)

public:
	CDUIAttriImageSection();
	virtual ~CDUIAttriImageSection(void);

	//variant
protected:
	uint32_t							m_uValueHash = 0;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

public:
	//render
	virtual void Draw(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint);
	virtual void Draw(CDUIImageBase *pImageBase, const tagDuiImageSection &ImageSection, HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint);
	virtual void DrawAnimate(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcPaint, const tagDuiAnimateImageInfo &AnimateImageInfo, int nFrameCur);

	virtual bool IsEmpty();
	virtual tagDuiImageSection GetImageSection();
	virtual void SetImageSection(const tagDuiImageSection &ImageSection);
	virtual CDUIImageBase * GetCurImageBase();
	virtual void SetImageResSwitch(const vector<CMMString> &vecImageResSwitch);
	virtual void SetImageResSwitch(const CMMString &strImageResSwitch);
	virtual vector<CMMString> GetImageResSwitch();
	virtual LPCTSTR GetImageResName();

	virtual CDUIRect GetSource();
	virtual CDUIRect GetSource(tagDuiImageSection ImageSection);
	virtual void SetSourceNormal();
	virtual void SetSourcePart(BYTE cbPartAll, BYTE cbPartSel);
	virtual void SetSourceCustom(const CDUIRect &rcSource);

	virtual BYTE GetAlpha();
	virtual void SetAlpha(BYTE cbAlpha);

	virtual ARGB GetMask();
	virtual void SetMask(ARGB dwMask);

	virtual CDUIRect GetCorner();
	virtual void SetCorner(const CDUIRect &rcCorner);
	virtual bool IsHole();
	virtual void SetHole(bool bHole);
	virtual bool IsTiledX();
	virtual void SetTiledX(bool bTiledX);
	virtual bool IsTiledY();
	virtual void SetTiledY(bool bTiledY);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;

	//help
protected:
	uint32_t GetValueID() override;
	void SetValueID(uint32_t uValueID) override;
	CDUIRect GetDest(CDUIRect rcSource, CDUIRect rcItem);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
