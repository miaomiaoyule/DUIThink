#ifndef __DUIDEFINEBASE_H__
#define __DUIDEFINEBASE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define Key_ResourceEncrypt				(0x19890527)

//////////////////////////////////////////////////////////////////////////
#define DUIRegisterCreateResource(class_name)\
	CDUIFactory::GetInstance()->RegistResourceClass((#class_name), (DUICreateResourceObj)class_name::CreateResourceObj);

#define DUIRegisterCreateAttribute(class_name)\
	CDUIFactory::GetInstance()->RegistAttributeClass((#class_name), (DUICreateAttributeObj)class_name::CreateAttributeObj);

#define DUIRegisterCreateWndManager(class_name)\
	CDUIFactory::GetInstance()->RegistWndManagerClass((#class_name), (DUICreateWndManagerObj)class_name::CreateWndManagerObj);

//////////////////////////////////////////////////////////////////////////
#define DUIInner_RegisterCreateControl(class_name)\
	RegistControlClass((#class_name), (DUICreateControlObj)class_name::CreateControlObj);

#define DUIInner_RegisterCreateResource(class_name)\
	RegistResourceClass((#class_name), (DUICreateResourceObj)class_name::CreateResourceObj);

#define DUIInner_RegisterCreateAttribute(class_name)\
	RegistAttributeClass((#class_name), (DUICreateAttributeObj)class_name::CreateAttributeObj);

#define DUIInner_RegisterCreateWndManager(class_name)\
	RegistWndManagerClass((#class_name), (DUICreateWndManagerObj)class_name::CreateWndManagerObj);

//////////////////////////////////////////////////////////////////////////

#endif