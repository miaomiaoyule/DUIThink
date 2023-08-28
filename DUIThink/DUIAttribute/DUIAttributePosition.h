#ifndef __DUIATTRIBUTEPOSITION_H__
#define __DUIATTRIBUTEPOSITION_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, tagDuiPosition> MapDuiAttributePosition;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributePosition INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributePosition = { 0xDAA5DB06,0x1862,0x4328,0x9B,0x29,0x13,0xE4,0xFA,0xB6,0x86,0x00 };
class DUITHINK_API CDUIAttributePosition
	: public CDUIAttributeObject
{
	friend class CMFCPropertyGridPositionProperty;

	DuiDeclare_CreateAttribute(CDUIAttributePosition)
	MMDeclare_ClassName(CDUIAttributePosition)

public:
	CDUIAttributePosition();
	virtual ~CDUIAttributePosition(void);

	//variant
protected:
	uint32_t							m_uValueHash = 0;
	CDUIControlBase *					m_pOwnerCtrl = NULL;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//override
protected:
	bool SetOwner(CDUIPropertyObject *pOwner) override;

public:
	//position
	virtual tagDuiPosition GetPosition();
	virtual void SetPosition(const tagDuiPosition &Position);

	//float 
	virtual void SetFloat(bool bFloat);
	virtual bool IsFloat();

	//horiz
	virtual	enDuiHorizAlignType GetHorizAlignType();
	virtual	void SetHorizAlignType(enDuiHorizAlignType AlignType);
	virtual	int GetLeftAlignValue();
	virtual	void SetLeftAlignValue(int nOffset);
	virtual int GetRightAlignValue();
	virtual void SetRightAlignValue(int nOffset);
	virtual int GetFixedWidth();
	virtual void SetFixedWidth(int nWidth);

	//vert
	virtual enDuiVertAlignType GetVertAlignType();
	virtual void SetVertAlignType(enDuiVertAlignType AlignType);
	virtual int GetTopAlignValue();
	virtual void SetTopAlignValue(int nOffset);
	virtual int GetBottomAlignValue();
	virtual void SetBottomAlignValue(int nOffset);
	virtual int GetFixedHeight();
	virtual void SetFixedHeight(int nHeight);

	//rect
	virtual bool SetPositionFromAbsolute(const CDUIRect &rcAbsolute);
	virtual CDUIRect GetAbsoluteRect(const CDUIRect &rcParentAbs);

public:
	LPCTSTR ConvertHorizAlignTypeToString(enDuiHorizAlignType AlignType);
	LPCTSTR ConvertVertAlignTypeToString(enDuiVertAlignType AlignType);
	enDuiHorizAlignType ConvertStringToHorizAlignType(CMMString strAlignType);
	enDuiVertAlignType ConvertStringToVertAlignType(CMMString strAlignType);

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue);

	//help
protected:
	uint32_t GetValueID() override;
	void SetValueID(uint32_t uValueID) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif