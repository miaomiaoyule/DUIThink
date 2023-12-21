#ifndef __DUILISTVIEWHEADERCTRL_H__
#define __DUILISTVIEWHEADERCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIListHeaderItemCtrl;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIListHeaderCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIListHeaderCtrl = { 0x99FCAA5B,0xAD12,0x4A09,0xB6,0xF9,0x7B,0x95,0x10,0x45,0x35,0x4F };
class DUITHINK_API CDUIListHeaderCtrl
	: public CDUIHorizontalLayoutCtrl
	, public IDUIControlCallBack
{
	friend class CDUIListViewCtrl;

	DuiDeclare_CreateControl(CDUIListHeaderCtrl)
	MMDeclare_ClassName(CDUIListHeaderCtrl)

public:
	CDUIListHeaderCtrl();
	~CDUIListHeaderCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupHeader;
	CDUIAttributeBool					m_AttributeScaleItem;
	CDUIAttributeLong					m_AttributeItemInitCount;

	//variant
protected:
	CDUIListViewCtrl *					m_pOwner = NULL;
	CDUIListHeaderItemCtrl *			m_pHeaderItemSelectCtrl = NULL;
	CDUICheckBoxCtrl *					m_pCheckSelectCtrl = NULL;
	int									m_nColumn = 0;
	CDUIRect							m_rcColumn[Dui_Max_ListColumn];

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//override
protected:
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnSize(CDUIControlBase *pControl) override;
	void OnVisibleChanged(CDUIControlBase *pControl) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//listview
	virtual CDUIListViewCtrl * GetOwner();
	virtual void SetOwner(CDUIListViewCtrl *pOwner);

	//scale
	virtual void SetScaleHeader(bool bScale);
	virtual bool IsScaleHeader();
	virtual float GetScaleValue(int nWidth);

	//column
	virtual int GetColumnCount();
	virtual CDUIRect GetColumnRect(int nIndex);

	//child
	virtual void InitChild();
	virtual bool InsertChild(int nItemCount, int nPos = -1);
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIListHeaderItemCtrl * GetChildAt(int nIndex) const override;
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;

	//refresh
	void RefreshView() override;

	//message
protected:

	//listview change
protected:
	void OnVisibleChangedSelectIcon();
	void OnSelectChangedListItem();

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//help
protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif