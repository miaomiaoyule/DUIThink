#include "stdafx.h"
#include "MMReDirection.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
//////////////////////////////////////////////////////////////////////////
CMMReDirection::CMMReDirection()
{
}

CMMReDirection::~CMMReDirection()
{
	if (m_bDisabledReDirection)
	{
		Wow64RevertWow64FsRedirection(m_pOldValue);
	}

	return;
}

void CMMReDirection::PerformDisableReDirection()
{
	if (m_bDisabledReDirection) return;

	if (CMMVersion::IsWow64())
	{
		m_bDisabledReDirection = Wow64DisableWow64FsRedirection(&m_pOldValue);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
#endif