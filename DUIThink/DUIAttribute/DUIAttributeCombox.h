#ifndef __DUIATTRIBUTECOMBOX_H__
#define __DUIATTRIBUTECOMBOX_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<uint32_t, tagDuiCombox> UnorderMapDuiAttributeCombox;
typedef std::map<uint32_t, tagDuiCombox> MapDuiAttributeCombox;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeCombox INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeCombox = { 0xDA1F9EC1,0x02EF,0x4F42,0x83,0x12,0x23,0x1F,0xC6,0x03,0x3D,0xF6 };
class DUITHINK_API CDUIAttributeCombox
	: public CDUIAttributeObject
{
	DuiDeclare_CreateAttribute(CDUIAttributeCombox)
	MMDeclare_ClassName(CDUIAttributeCombox)

public:
	CDUIAttributeCombox(void);
	~CDUIAttributeCombox(void);

	//variant
protected:
	uint32_t							m_uValueHash = 0;
	int									m_nCurSel = -1;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

public:
	CDUIAttributeCombox & operator = (CDUIAttributeObject &Right) override;

	//item
	virtual void InitSelectItem();
	virtual int GetCurSelItem();
	virtual int GetCurSelIndex();
	virtual CMMString GetCurSelDescribe();
	virtual void SetCurSelIndex(int nIndex);
	virtual void SelectItem(int nItem);
	virtual void SelectItem(CMMString strDescribe);
	virtual int GetItemCount();
	virtual tagDuiCombox GetCombox();
	virtual void SetCombox(const tagDuiCombox &AttriCombox);
	virtual tagDuiComboxItem GetItemAt(int nIndex);
	virtual void RemoveAll();

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;

	//help
protected:
	uint32_t GetValueID() override;
	void SetValueID(uint32_t uValueID) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif