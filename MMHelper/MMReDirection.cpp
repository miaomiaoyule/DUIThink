#include "stdafx.h"
#include "MMReDirection.h"

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