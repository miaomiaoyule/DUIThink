#include "StdAfx.h"
#include "DUIInstrumentCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIInstrumentCtrl)
MMImplement_ClassName(CDUIInstrumentCtrl)

CDUIInstrumentCtrl::CDUIInstrumentCtrl(void)
{
	return;
}

CDUIInstrumentCtrl::~CDUIInstrumentCtrl(void)
{
	return;
}

bool CDUIInstrumentCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	return false;
}

LPVOID CDUIInstrumentCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIInstrumentCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIInstrumentCtrl::GetDescribe() const
{
	return Dui_Ctrl_Instrument;
}

void CDUIInstrumentCtrl::InitProperty()
{
	__super::InitProperty();

	//DuiCreateGroupAttribute(m_AttributeGroupText, _T("Text"));
	//DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupText);
	//DuiCreateAttribute(m_AttributeTextStyleIPDot, _T("TextStyleIPDot"), _T(""), m_AttributeGroupText);

	return;
}

void CDUIInstrumentCtrl::InitComplete()
{
	__super::InitComplete();

	EnableScrollBar(false, false);

	return;
}