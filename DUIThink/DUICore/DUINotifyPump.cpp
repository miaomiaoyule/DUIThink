#include "stdafx.h"
#include "DUINotifyPump.h"

//////////////////////////////////////////////////////////////////////////
DuiBaseBegin_Message_Map(CDUINotifyPump)
DuiEnd_Message_Map()

static const Dui_MsgMapEntry * DuiFindMessageEntry(const Dui_MsgMapEntry *lpEntry, const DuiNotify &Notify)
{
	const Dui_MsgMapEntry *pNotifyEntry = NULL;
	while (lpEntry->nSig != DuiSign_end)
	{
		if (lpEntry->Notify == Notify.NotifyType)
		{
			if (0 != lpEntry->uCtrlID && lpEntry->uCtrlID == Notify.uCtrlID)
			{
				return lpEntry;
			}
			else if (0 == lpEntry->uCtrlID)
			{
				pNotifyEntry = lpEntry;
			}
		}

		lpEntry++;
	}

	return pNotifyEntry;
}

void CDUINotifyPump::OnNotify(const DuiNotify &Notify)
{
	const Dui_MsgMapEntry *lpEntry = NULL;
	const Dui_MsgMap *pMessageMap = NULL;

	for (pMessageMap = GetDuiMessageMap(); pMessageMap != NULL; pMessageMap = (*pMessageMap->pfnGetBaseMap)())
	{
		ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
		if ((lpEntry = DuiFindMessageEntry(pMessageMap->lpEntries, Notify)) != NULL)
		{
			goto Dispatch;
		}
	}

	return;

Dispatch:
	union DuiMessageMapFunctions mmf;
	mmf.pfn = lpEntry->pfn;

	bool bRes = false;
	switch (lpEntry->nSig)
	{
		case DuiSign_l_w_l:
		{
			(this->*mmf.pfn_Notify_l_w_l)(Notify.wParam, Notify.lParam);
			bRes = true;
			break;
		}
		case DuiSign_v_n:
		{
			(this->*mmf.pfn_Notify_v_n)(Notify);
			bRes = true;
			break;
		}
		default:
		{
			ASSERT(false);
			break;
		}
	}

	return;
}
