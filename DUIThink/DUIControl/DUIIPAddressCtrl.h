#ifndef __DUIIPADDRESSCTRL_H__
#define __DUIIPADDRESSCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIIPAddressCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIIPAddressCtrl = { 0x2AFC97C0,0xE6FC,0x4AB1,0x9A,0x64,0x0C,0x86,0xD6,0xF2,0x60,0x81 };
class DUITHINK_API CDUIIPAddressCtrl
	: public CDUIHorizontalLayoutCtrl
{
	DuiDeclare_CreateControl(CDUIIPAddressCtrl)
	MMDeclare_ClassName(CDUIIPAddressCtrl)

public:
	CDUIIPAddressCtrl(void);
	~CDUIIPAddressCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupIP;
	CDUIAttributeText					m_AttributeIP = _T("192.168.1.1");
	CDUIAttributeTextStyle				m_AttributeTextStyle;
	CDUIAttributeTextStyle				m_AttributeTextStyleIPDot;

	//variant
protected:
	vector<CDUIRect>					m_vecIPDot;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void RefreshView() override;

	//text
	virtual HFONT GetFont();
	virtual ARGB GetTextColor();
	virtual tagDuiTextStyle GetTextStyle();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIThinkEditCtrl * GetChildAt(int nIndex) const override;

	virtual DWORD GetIPAddrValue();
	virtual void SetIPAddrValue(DWORD dwIPAddr);
	virtual CMMString GetIPAddress();
	virtual void SetIPAddress(CMMString strIPAddr);

	//message
protected:
	void OnDuiWndManagerAttach() override;

	//proprety
protected:
	void InitProperty();
	void InitComplete();

	//paint
protected:
	void PaintText(HDC hDC);

	//help
protected:
	void RefreshEditTextStyle();
	void RefreshIPDotWidth();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif