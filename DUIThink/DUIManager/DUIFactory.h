#ifndef __DUIFACTORY_H__
#define __DUIFACTORY_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
typedef CDUIControlBase * (*DUICreateControlObj)();
typedef CDUIResourceBase * (*DUICreateResourceObj)();
typedef CDUIAttributeObject * (*DUICreateAttributeObj)();
typedef CDUIWndManager * (*DUICreateWndManagerObj)();

typedef std::unordered_map<std::string, DUICreateControlObj> MapDuiCreateControl;
typedef std::unordered_map<std::string, DUICreateResourceObj> MapDuiCreateResource;
typedef std::unordered_map<std::string, DUICreateAttributeObj> MapDuiCreateAttribute;
typedef std::unordered_map<std::string, DUICreateWndManagerObj> MapDuiCreateWndManager;

//////////////////////////////////////////////////////////////////////////
class DUITHINK_API CDUIFactory
{
private:
	CDUIFactory();
	virtual ~CDUIFactory();

private:
	MapDuiCreateControl					m_mapControlClass;
	MapDuiCreateResource				m_mapResourceClass;
	MapDuiCreateAttribute				m_mapAttributeClass;
	MapDuiCreateWndManager				m_mapWndManagerClass;

public:
	static CDUIFactory * GetInstance();

	CDUIControlBase * CreateControlObj(LPCSTR lpszClassName);
	CDUIResourceBase * CreateResourceObj(LPCSTR lpszClassName);
	CDUIAttributeObject * CreateAttributeObj(LPCSTR lpszClassName);
	CDUIWndManager * CreateWndManagerObj(LPCSTR lpszClassName);

	void RegistControlClass(LPCSTR lpszClassName, DUICreateControlObj pFunc);
	void RegistResourceClass(LPCSTR lpszClassName, DUICreateResourceObj pFunc);
	void RegistAttributeClass(LPCSTR lpszClassName, DUICreateAttributeObj pFunc);
	void RegistWndManagerClass(LPCSTR lpszClassName, DUICreateWndManagerObj pFunc);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif