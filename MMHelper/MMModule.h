// Written by 城南花已开 QQ:284168136 QQGroup:885314879
// Copyright (c) 城南花已开
//
// This code may be used in compiled form in any way you desire. These
// source files may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef __MM_MODULE_H__
#define __MM_MODULE_H__

//////////////////////////////////////////////////////////////////////////////////
//base interface
#define VER_IMMUnknown INTERFACE_VERSION(1,1)
static const GUID IID_IMMUnknown = { 0x5feec21e,0xdbf3,0x46f0,0x9f,0x57,0xd1,0xcd,0x71,0x1c,0x46,0xde };
interface IMMUnknown
{
	virtual ~IMMUnknown() {}
	virtual LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//版本比较

//产品版本
#define BULID_VER						3								//授权版本
#define PRODUCT_VER						4								//产品版本

//接口版本
#define INTERFACE_VERSION(cbMainVer,cbSubVer)							\
		(DWORD)(														\
		(((BYTE)(PRODUCT_VER))<<24)+									\
		(((BYTE)(cbMainVer))<<16)+										\
		((BYTE)(cbSubVer)<<8))+											\
		((BYTE)(BULID_VER))

//模块版本
#define PROCESS_VERSION(cbMainVer,cbSubVer,cbBuildVer)					\
		(DWORD)(														\
		(((BYTE)(PRODUCT_VER))<<24)+									\
		(((BYTE)(cbMainVer))<<16)+										\
		((BYTE)(cbSubVer)<<8)+											\
		(BYTE)(cbBuildVer))

//产品版本
inline BYTE MMHELPER_API GetProductVer(DWORD dwVersion);
inline BYTE MMHELPER_API GetMainVer(DWORD dwVersion);
inline BYTE MMHELPER_API GetSubVer(DWORD dwVersion);
inline BYTE MMHELPER_API GetBuildVer(DWORD dwVersion);
bool MMHELPER_API InterfaceVersionCompare(DWORD dwQueryVer, DWORD dwInterfaceVer);

//////////////////////////////////////////////////////////////////////////////////
//查询接口
#define QUERYINTERFACE(Interface,Guid,dwQueryVer)															\
	if ((Guid==IID_##Interface)&&(InterfaceVersionCompare(dwQueryVer,VER_##Interface)))						\
		return static_cast<Interface *>(this);											

//查询接口
#define QUERYINTERFACE_IMMUNKNOWN(BaseInterface,Guid,dwQueryVer)											\
	if ((Guid==IID_IMMUnknown)&&(InterfaceVersionCompare(dwQueryVer,VER_IMMUnknown)))						\
		return static_cast<IMMUnknown *>(static_cast<BaseInterface *>(this));			

//////////////////////////////////////////////////////////////////////////////////
//查询接口
#define QUERY_ME_INTERFACE(Interface)																		\
	((Interface *)QueryInterface(IID_##Interface,VER_##Interface))

//查询接口
#define QUERY_OBJECT_INTERFACE(Object,Interface)															\
	((Interface *)Object.QueryInterface(IID_##Interface,VER_##Interface))

//查询接口
#define QUERY_OBJECT_PTR_INTERFACE(pObject,Interface)														\
	((pObject==NULL)?NULL:((Interface *)pObject->QueryInterface(IID_##Interface,VER_##Interface)))

//////////////////////////////////////////////////////////////////////////////////
typedef VOID * (MMModuleCreateProc)(REFGUID Gudi, DWORD dwInterfaceVer);
template <typename IMMModelInterface> class CMMTempldateHelper
{
public:
	CMMTempldateHelper(REFGUID Guid, DWORD dwVersion, LPCTSTR lpszModuleDll, LPCSTR lpszCreateProc);
	virtual ~CMMTempldateHelper();

	//variant
protected:
	IMMModelInterface *					m_pIModelInterface = NULL;

	//config
public:
	REFGUID								m_Guid = {};
	const DWORD							m_dwVersion = 0;
	HINSTANCE							m_hDllInstance = NULL;
	std::string							m_strCreateProc;
	CMMString							m_strModuleDllName;
	CMMString							m_strDescribe;

	//method
public:
	bool CloseInstance();
	bool CreateInstance();

	//operator
public:
	inline LPCTSTR GetErrorDescribe() const;
	inline IMMModelInterface * operator->() const;
	inline operator IMMModelInterface *() const;
	inline IMMModelInterface * GetInterface() const;
};

template <typename IMMModelInterface>
CMMTempldateHelper<IMMModelInterface>::CMMTempldateHelper(REFGUID Guid, DWORD dwVersion, LPCTSTR lpszModuleDll, LPCSTR lpszCreateProc)
	: m_dwVersion(dwVersion)
	, m_Guid(Guid)
	, m_strCreateProc(lpszCreateProc)
	, m_strModuleDllName(lpszModuleDll)
{
	return;
}

template <typename IMMModelInterface>
CMMTempldateHelper<IMMModelInterface>::~CMMTempldateHelper()
{
	CloseInstance();
}

template <typename IMMModelInterface>
bool CMMTempldateHelper<IMMModelInterface>::CreateInstance()
{
	CloseInstance();

	try
	{
		assert(false == m_strCreateProc.empty());
		assert(false == m_strModuleDllName.empty());

		m_hDllInstance = ::LoadLibrary(m_strModuleDllName);
		if (NULL == m_hDllInstance)
		{
			m_hDllInstance = GetModuleHandle(NULL);
			m_strDescribe.Format(_T("“%s”模块加载失败，转当前模块"), m_strModuleDllName);
		}

		MMModuleCreateProc *CreateProc = (MMModuleCreateProc*)GetProcAddress(m_hDllInstance, m_strCreateProc.c_str());
		if (NULL == CreateProc)
		{
			m_strDescribe.Format(_T("找不到组件创建函数“%s”"), (LPCTSTR)CA2CT(m_strCreateProc.c_str()));
			return false;
		}

		m_pIModelInterface = (IMMModelInterface*)CreateProc(m_Guid, m_dwVersion);
		if (NULL == m_pIModelInterface)
		{
			m_strDescribe.Format(_T("调用函数“%s”生成对象失败"), (LPCTSTR)CA2CT(m_strCreateProc.c_str()));
			return false;
		}
	}
	catch (LPCTSTR lpszError)
	{
		m_strDescribe.Format(_T("由于“%s”，组件创建失败"), lpszError);
		return false;
	}
	catch (...)
	{
		m_strDescribe.Format(_T("组件创建函数“%s”产生未知异常错误，组件创建失败"), (LPCTSTR)CA2CT(m_strCreateProc.c_str()));
		return false;
	}

	return true;
}

template <typename IMMModelInterface>
bool CMMTempldateHelper<IMMModelInterface>::CloseInstance()
{
	m_strDescribe.clear();

	if (m_pIModelInterface != NULL)
	{
		m_pIModelInterface = NULL;
	}

	if (m_hDllInstance != NULL)
	{
		::FreeLibrary(m_hDllInstance);
		m_hDllInstance = NULL;
	}

	return true;
}

template <typename IMMModelInterface>
inline LPCTSTR CMMTempldateHelper<IMMModelInterface>::GetErrorDescribe() const
{
	return m_strDescribe;
}

template <typename IMMModelInterface>
inline IMMModelInterface * CMMTempldateHelper<IMMModelInterface>::operator->() const
{
	return GetInterface();
}

template <typename IMMModelInterface>
inline CMMTempldateHelper<IMMModelInterface>::operator IMMModelInterface *() const
{
	return GetInterface();
}

template <typename IMMModelInterface>
inline IMMModelInterface * CMMTempldateHelper<IMMModelInterface>::GetInterface() const
{
	return m_pIModelInterface;
}

//////////////////////////////////////////////////////////////////////////////////
#define Declare_MMModuleHelper(ObjectName, ModuleDllName, CreateFunctionName)								\
class C##ObjectName##Helper : public CMMTempldateHelper<I##ObjectName>										\
{																											\
public:																										\
	C##ObjectName##Helper() : CMMTempldateHelper<I##ObjectName>(IID_I##ObjectName,							\
		VER_I##ObjectName, ModuleDllName, #CreateFunctionName) { }										\
};

#define Implement_MMCreateModule(ObjectName)																	\
extern "C" __declspec(dllexport) VOID * Create##ObjectName(REFGUID Guid, DWORD dwInterfaceVer)				\
{																											\
	C##ObjectName * p##ObjectName=NULL;																	\
	try																										\
	{																										\
		p##ObjectName = new C##ObjectName();																\
		if (p##ObjectName == NULL) throw _T("创建失败");													\
		VOID * pObject = p##ObjectName->QueryInterface(Guid, dwInterfaceVer);									\
		if (pObject == NULL) throw _T("接口查询失败");														\
		return pObject;																						\
	}																										\
	catch (...) {}																							\
	MMSafeDelete(p##ObjectName);																				\
	return NULL;																							\
}

#define Implement_MMStaticModule(ObjectName)																	\
extern "C" __declspec(dllexport) VOID * Create##ObjectName(REFGUID Guid, DWORD dwInterfaceVer)		\
{																											\
	C##ObjectName * p##ObjectName=NULL;																	\
	try																										\
	{																										\
		p##ObjectName = C##ObjectName::GetInstance();																\
		if (p##ObjectName == NULL) throw _T("创建失败");													\
		VOID * pObject = p##ObjectName->QueryInterface(Guid, dwInterfaceVer);									\
		if (pObject == NULL) throw _T("接口查询失败");														\
		return pObject;																						\
	}																										\
	catch (...) {}																							\
	return NULL;																							\
}

//////////////////////////////////////////////////////////////////////////
template <typename IMMModelInterface>
class CMMInterfaceHelper
{
public:
	CMMInterfaceHelper(REFGUID Guid, DWORD dwVersion, IMMUnknown *pInterfaceSrc = NULL);
	virtual ~CMMInterfaceHelper();

protected:
	IMMModelInterface *					m_pInterfaceDest = NULL;
	IMMUnknown *						m_pInterfaceSrc = NULL;
	GUID								m_InterfaceGuid = {};
	DWORD								m_InterfaceVer = 0;

	//method
public:
	inline IMMModelInterface * operator ->();
	inline IMMModelInterface * operator = (IMMUnknown *pInterfaceSrc);
	inline operator IMMModelInterface *();
	inline IMMUnknown * Get();
};

template <typename IMMModelInterface>
CMMInterfaceHelper<IMMModelInterface>::CMMInterfaceHelper(REFGUID Guid, DWORD dwVersion, IMMUnknown *pInterfaceSrc /* = NULL */)
	: m_pInterfaceSrc(pInterfaceSrc)
	, m_InterfaceGuid(Guid)
	, m_InterfaceVer(dwVersion)
{
	if (m_pInterfaceSrc)
	{
		m_pInterfaceDest = static_cast<IMMModelInterface*>(m_pInterfaceSrc->QueryInterface(m_InterfaceGuid, m_InterfaceVer));
	}

	return;
}

template <typename IMMModelInterface>
CMMInterfaceHelper<IMMModelInterface>::~CMMInterfaceHelper()
{
	m_pInterfaceSrc = NULL;
	m_pInterfaceDest = NULL;

	return;
}

template <typename IMMModelInterface>
IMMModelInterface * CMMInterfaceHelper<IMMModelInterface>::operator ->()
{
	return m_pInterfaceDest;
}

template <typename IMMModelInterface>
IMMModelInterface * CMMInterfaceHelper<IMMModelInterface>::operator = (IMMUnknown *pInterfaceSrc)
{
	m_pInterfaceSrc = pInterfaceSrc;
	m_pInterfaceDest = NULL;

	if (m_pInterfaceSrc)
	{
		m_pInterfaceDest = static_cast<IMMModelInterface*>(m_pInterfaceSrc->QueryInterface(m_InterfaceGuid, m_InterfaceVer));
	}

	return m_pInterfaceDest;
}

template <typename IMMModelInterface>
CMMInterfaceHelper<IMMModelInterface>::operator IMMModelInterface *()
{
	return m_pInterfaceDest;
}

template <typename IMMModelInterface>
IMMUnknown * CMMInterfaceHelper<IMMModelInterface>::Get()
{
	return m_pInterfaceSrc;
}

//////////////////////////////////////////////////////////////////////////
template <typename IDL>
class CMMSafeIDL
{
public:
	CMMSafeIDL(IDL pIDL)
	{
		m_pIDL = pIDL;
	}
	virtual ~CMMSafeIDL()
	{
		if (m_pIDL)
		{
			CoTaskMemFree((LPVOID)m_pIDL);
		}

		m_pIDL = NULL;
	}
	CMMSafeIDL(CMMSafeIDL &Copy)
	{
		this->operator=(Copy);
	}

	//variant
protected:
	IDL									m_pIDL = NULL;

	//method
public:
	IDL Get() { return m_pIDL; }
	operator IDL() { return m_pIDL; }
	CMMSafeIDL & operator = (IDL Right)
	{
		if (m_pIDL)
		{
			CoTaskMemFree((LPVOID)m_pIDL);
			m_pIDL = NULL;
		}
		if (Right)
		{
			m_pIDL = ILCloneFull(Right);
		}
	}
	CMMSafeIDL & operator = (CMMSafeIDL &Right)
	{
		if (m_pIDL)
		{
			CoTaskMemFree((LPVOID)m_pIDL);
			m_pIDL = NULL;
		}
		if (Right.Get())
		{
			m_pIDL = ILCloneFull(Right.Get());
		}
	}
};

//////////////////////////////////////////////////////////////////////////
#define Declare_MMInterfaceHelper(IMMModelInterface, InterfaceVariant) \
CMMInterfaceHelper<IMMModelInterface> InterfaceVariant

#define Implement_MMInterfaceHelper(IMMModelInterface, InterfaceVariant) \
InterfaceVariant(IID_##IMMModelInterface, VER_##IMMModelInterface)

#define MMInterfaceHelper(IMMModelInterface, InterfaceSrc, InterfaceVariant) \
CMMInterfaceHelper<IMMModelInterface>InterfaceVariant(IID_##IMMModelInterface, VER_##IMMModelInterface, InterfaceSrc)

//////////////////////////////////////////////////////////////////////////////////

#endif