#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifndef __MM_REDIRECTION_H__
#define __MM_REDIRECTION_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
/*
 * for run 32 program on os64 in some scene.
 */
class MMHELPER_API CMMReDirection
{
public:
	CMMReDirection();
	~CMMReDirection();

protected:
	bool								m_bDisabledReDirection = false;
	PVOID								m_pOldValue = NULL;

	//method
public:
	void PerformDisableReDirection();
};

//////////////////////////////////////////////////////////////////////////

#endif
#endif