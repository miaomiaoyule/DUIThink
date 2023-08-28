#ifndef __IDUIFACTORY_H__
#define __IDUIFACTORY_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
typedef CDUIControlBase * (*DUICreateControlObj)();

//////////////////////////////////////////////////////////////////////////
#define VER_IDUIFactory INTERFACE_VERSION(1,1)
static const GUID IID_IDUIFactory = { 0xC7FDF1E7,0x6074,0x4BD5,0xB7,0x0A,0x92,0x95,0x05,0x9F,0x21,0x02 };
interface DUITHINK_API IDUIFactory : public IDUIInterface
{
	virtual void RegistControlClass(LPCSTR lpszClassName, DUICreateControlObj pFunc) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#endif