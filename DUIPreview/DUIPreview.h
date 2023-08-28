#ifndef __DUIPREVIEW_H__
#define __DUIPREVIEW_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
class CDUIPreview
{
protected:
	CDUIPreview();

	//variant
protected:

	//method
public:
	static CDUIPreview * GetInstance();
	void Init(HINSTANCE hInstance, CString strCommandLine);
	void UnInit();
};

//////////////////////////////////////////////////////////////////////////

#endif