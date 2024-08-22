#include "StdAfx.h"
#include "DUIWndManager.h"

//////////////////////////////////////////////////////////////////////////
#define GetPIDLFolder(pida)				(LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define GetPIDLItem(pida, i)			(LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])
#define Dui_ControlDrag					_T("DUIThinkControlDrag")

//////////////////////////////////////////////////////////////////////////
UINT CDUIWndManager::MapKeyState()
{
	UINT uState = 0;
	if (::GetKeyState(VK_CONTROL) < 0) uState |= MK_CONTROL;
	if (::GetKeyState(VK_RBUTTON) < 0) uState |= MK_RBUTTON;
	if (::GetKeyState(VK_LBUTTON) < 0) uState |= MK_LBUTTON;
	if (::GetKeyState(VK_SHIFT) < 0) uState |= MK_SHIFT;
	if (::GetKeyState(VK_MENU) < 0) uState |= MK_ALT;
	return uState;
}

////////////////////////////////////////////////////////////////////
DuiImplement_CreateWndManager(CDUIWndManager)
MMImplement_ClassName(CDUIWndManager)

CDUIWndManager::CDUIWndManager()
{
	CDUIGlobal::GetInstance()->AddWndManager(this);

	return;
}

CDUIWndManager::~CDUIWndManager(void)
{
	CDUIAnimationWnd::UnInit();
	CDUIGlobal::GetInstance()->RemoveWndManager(this);

	ReleaseRootCtrl();
	ReleasePaintScene();

	//wnd dc
	if (IsWindow(m_hWndAttach) && m_hDCPaint)
	{
		::ReleaseDC(m_hWndAttach, m_hDCPaint);
		m_hDCPaint = NULL;
	}

	m_hWndAttach = NULL;

	return;
}

bool CDUIWndManager::RegisterControlListen(IDUIInterface *pIControlListen)
{
	if (false == __super::RegisterControlListen(pIControlListen)) return false;

	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return true;

	return pRootCtrl->RegisterControlListen(pIControlListen);
}

bool CDUIWndManager::UnRegisterControlListen(IDUIInterface *pIControlListen)
{
	if (false == __super::UnRegisterControlListen(pIControlListen)) return false;

	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return false;

	return pRootCtrl->UnRegisterControlListen(pIControlListen);
}

bool CDUIWndManager::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (NULL == pAttributeObj) return true;

	__super::OnAttributeChange(pAttributeObj);

	if (m_hWndAttach && m_pRootCtrl)
	{
		AdjustWndSize();

		CDUIRect rcClient(0, 0, GetClientSize().cx, GetClientSize().cy);
		m_pRootCtrl->OnDuiSize(rcClient);

		Invalidate();
	}

	return true;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
void CDUIWndManager::OnWinDragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_DropData.vecDropFiles.clear();
	m_DropData.vecFileNameMap.clear();
	m_DropData.bHasGUID = false;
	for (auto &it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	m_DropData.vecPIDL.clear();

	// parse IDataObject
	CDUIPoint ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWndAttach, &ptMouse);
	m_ptMousePosLast = ptMouse;
	m_DropData.ptMouse = ptMouse;
	m_DropData.pIDataObject = pIDataObject;
	m_DropData.dwKeyState = dwKeyState;

	FORMATETC ftetc = { 0 };
	ftetc.lindex = -1;
	ftetc.dwAspect = DVASPECT_CONTENT;

	//CFSTR_PREFERREDDROPEFFECT
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(CFSTR_PREFERREDDROPEFFECT);
	ftetc.tymed = TYMED_HGLOBAL;

	//parse dropfiles
	ftetc.cfFormat = CF_HDROP;
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM medium = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop)
		{
			UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
			if (cFiles > 0)
			{
				for (int i = 0; i < cFiles; i++)
				{
					TCHAR szFileName[MAX_PATH] = { 0 };
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					m_DropData.vecDropFiles.push_back(szFileName);
				}
			}
		}

		GlobalUnlock(medium.hGlobal);
		ReleaseStgMedium(&medium);
	}

	//parse FileNameMap
	ftetc.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILENAMEMAP);
	ftetc.tymed = TYMED_HGLOBAL;
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop != NULL)
		{
			LPCWSTR s = reinterpret_cast<LPCWSTR>(hDrop);
			while (*s)
			{
				CMMString strFileName = CMMStrHelp::Format(_T("%ls"), s);
				m_DropData.vecFileNameMap.push_back(strFileName);
				s = 1 + wcschr(s, L'\0');
			}

			GlobalUnlock(medium.hGlobal);
			ReleaseStgMedium(&medium);

			//检测CFSTR_FILENAMEMAP与CF_HDROP文件数量保持一致,如不一致清空filenamemap
			if (m_DropData.vecFileNameMap.size() > 0)
			{
				if (m_DropData.vecFileNameMap.size() != m_DropData.vecDropFiles.size())
				{
					m_DropData.vecFileNameMap.clear();
				}
			}
		}
	}

	// parse pidl
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(CFSTR_SHELLIDLIST);
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM mediumPIDL = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &mediumPIDL))
	{
		LPIDA pida = (LPIDA)GlobalLock(mediumPIDL.hGlobal);
		PCIDLIST_ABSOLUTE pidl_parent = GetPIDLFolder(pida);
		for (int i = 0; i < pida->cidl; i++)
		{
			PCIDLIST_ABSOLUTE pcid = ILCombine(pidl_parent, GetPIDLItem(pida, i));
			m_DropData.vecPIDL.push_back(pcid);

			TCHAR szPath[MAX_PATH] = { 0 };
			SHGetPathFromIDList(pcid, szPath);
			if (lstrlen(szPath) == 0)
			{
				SHDESCRIPTIONID shdescid = { 0 };
				CComPtr<IShellFolder> psfParent = nullptr;
				PCUITEMID_CHILD lpidRelative = nullptr;
				HRESULT hr = SHBindToParent(pcid, IID_IShellFolder, (void**)&psfParent, &lpidRelative);
				if (hr != S_OK)
					continue;

				hr = SHGetDataFromIDList(psfParent, lpidRelative, SHGDFIL_DESCRIPTIONID, &shdescid, sizeof(SHDESCRIPTIONID));

				if (hr != S_OK)
					continue;

				LPOLESTR pszStr = NULL;
				hr = ::StringFromCLSID(shdescid.clsid, &pszStr);
				if (hr != S_OK)
					continue;

				CMMString strPath = pszStr;
				auto itor = std::find_if(m_DropData.vecDropFiles.begin(), m_DropData.vecDropFiles.end(), [&](CMMString& strTemp) {
					return strTemp.CompareNoCase(strPath) == 0;
				});
				if (!m_DropData.bHasGUID)
				{
					m_DropData.bHasGUID = true;
				}
				if (itor == m_DropData.vecDropFiles.end())
					m_DropData.vecDropFiles.push_back(strPath);
			}
		}

		GlobalUnlock(mediumPIDL.hGlobal);
		ReleaseStgMedium(&mediumPIDL);
	}

	// parse text
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(Dui_ControlDrag);
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM mediumText = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &mediumText))
	{
		PWSTR pzStr = (PWSTR)GlobalLock(mediumText.hGlobal);

		GlobalUnlock(mediumText.hGlobal);
		ReleaseStgMedium(&mediumText);
	}

	//notify
	m_pWinDragEnterCtrl = FindControlByDrop(ptMouse);
	if (m_pWinDragEnterCtrl)
	{
		m_pWinDragEnterCtrl->OnWinDragEnter(&m_DropData, &m_DropData.dwEffect);
	}

	return;
}

void CDUIWndManager::OnWinDragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (NULL == m_pRootCtrl) return;

	POINT ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWndAttach, &ptMouse);
	if (m_DropData.ptMouse == ptMouse
		&& m_DropData.dwKeyState == dwKeyState)
	{
		*pdwEffect = m_DropData.dwEffect;

		return;
	}

	m_DropData.dwKeyState = dwKeyState;
	m_DropData.ptMouse = ptMouse;
	m_ptMousePosLast = ptMouse;

	CDUIControlBase *pOverControl = FindControlByDrop(ptMouse);

	//leave
	if (pOverControl != m_pWinDragEnterCtrl)
	{
		if (m_pWinDragEnterCtrl
			&& m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
		{
			m_pWinDragEnterCtrl->OnWinDragLeave(&m_DropData);
		}

		m_pWinDragEnterCtrl = NULL;
	}
	//enter
	if (pOverControl != m_pWinDragEnterCtrl && pOverControl)
	{
		m_pWinDragEnterCtrl = pOverControl;
		pOverControl->OnWinDragEnter(&m_DropData, &m_DropData.dwEffect);
	}
	//over
	if (m_pWinDragEnterCtrl)
	{
		m_pWinDragEnterCtrl->OnWinDragOver(&m_DropData, m_DropData.dwEffect);
	}

	*pdwEffect = m_DropData.dwEffect;

	return;
}

void CDUIWndManager::OnWinDragLeave()
{
	if (NULL == m_pRootCtrl) return;

	m_DropData.ptMouse = m_ptMousePosLast;
	if (m_pWinDragEnterCtrl && m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
	{
		m_pWinDragEnterCtrl->OnWinDragLeave(&m_DropData);
	}
	for (auto& it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	m_pWinDragEnterCtrl = NULL;
	m_DropData = {};

	return;
}

void CDUIWndManager::OnWinDrop(IDataObject *pIDataObject, POINTL pt, DWORD *pdwEffect)
{
	if (NULL == m_pRootCtrl) return;

	POINT ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWndAttach, &ptMouse);
	m_ptMousePosLast = ptMouse;
	m_DropData.pIDataObject = pIDataObject;
	m_DropData.ptMouse = ptMouse;

	//从外部拖拽时查询数据对象是否支持异步操作
	if (m_DropData.pDragDropFrom != this)
	{
		//0x602 win7 
#if (_WIN32_WINNT >= 0x602)
		IDataObjectAsyncCapability *pac = NULL; 
#else
		IAsyncOperation *pac = NULL;
#endif

		BOOL bAsyncSupported = FALSE;
		/* Does the drop source support asynchronous copy */
		HRESULT hr = pIDataObject->QueryInterface(IID_PPV_ARGS(&pac));
		if (hr == S_OK && pac != NULL)
		{
			pac->GetAsyncMode(&bAsyncSupported);
			pac->Release();
			pac = NULL;
			m_DropData.bAsyncMode = (bAsyncSupported == TRUE);
		}
		else
		{
			m_DropData.bAsyncMode = false;
		}
	}

	//兼容从压缩包中直接拖拽文件，再parse下 
	m_DropData.vecDropFiles.clear();
	FORMATETC ftetc = { 0 };
	ftetc.lindex = -1;
	ftetc.dwAspect = DVASPECT_CONTENT;
	ftetc.cfFormat = CF_HDROP;
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM medium = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop)
		{
			UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
			if (cFiles > 0)
			{
				for (int i = 0; i < cFiles; i++)
				{
					TCHAR szFileName[MAX_PATH] = { 0 };
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					m_DropData.vecDropFiles.push_back(szFileName);
				}
			}
		}

		GlobalUnlock(medium.hGlobal);
		ReleaseStgMedium(&medium);
	}

	//notify
	if (m_pWinDragEnterCtrl && m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
	{
		m_pWinDragEnterCtrl->OnWinDrop(&m_DropData, pdwEffect);
	}

	m_pWinDragEnterCtrl = NULL;

	return;
}
#endif

bool CDUIWndManager::IsDesigned()
{
	return m_bDesigned;
}

void CDUIWndManager::SetDesigned(bool bDesigned)
{
	if (bDesigned == IsDesigned()) return;

	m_bDesigned = bDesigned;

	return;
}

LPVOID CDUIWndManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWndManager, Guid, dwQueryVer);
	QUERYINTERFACE(CDUIWndManager, Guid, dwQueryVer);

	return CDUIPropertyObject::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWndManager::GetDescribe() const
{
	return Dui_Wnd_Manager;
}

HWND CDUIWndManager::GetWndHandle()
{
	return m_hWndAttach;
}

HDC CDUIWndManager::GetWndDC()
{
	return m_hDCPaint;
}

void CDUIWndManager::SetCapture()
{
	::SetCapture(m_hWndAttach);
}

void CDUIWndManager::ReleaseCapture()
{
	::ReleaseCapture();
}

bool CDUIWndManager::IsCaptured()
{
	return ::GetCapture() == m_hWndAttach;
}

CDUIControlBase* CDUIWndManager::GetCaptureControl()
{
	return m_pCaptureCtrl;
}

CDUIControlBase* CDUIWndManager::GetFocusControl()
{
	return m_pFocusCtrl;
}

void CDUIWndManager::SetFocusControl(CDUIControlBase *pFocusCtrl)
{
	if (m_pFocusCtrl == pFocusCtrl) return;

	//kill focus
	if (m_pFocusCtrl)
	{
		CDUIControlBase *pControl = m_pFocusCtrl;
		m_pFocusCtrl = NULL;
		pControl->OnDuiKillFocus();
	}

	//set focus
	m_pFocusCtrl = pFocusCtrl;
	if (m_pFocusCtrl)
	{
		m_pFocusCtrl->OnDuiSetFocus();
	}

	//focus child wnd
	MMInterfaceHelper(CDUIEditCtrl, m_pFocusCtrl, pEditFocus);
	MMInterfaceHelper(CDUIHotKeyCtrl, m_pFocusCtrl, pHotKeyFocus);

	if (m_pFocusCtrl
		&& GetFocus() != m_hWndAttach
		&& NULL == pEditFocus
		&& NULL == pHotKeyFocus)
	{
		::SetFocus(m_hWndAttach);
	}

	return;
}

CDUIControlBase * CDUIWndManager::GetHoverCtrl()
{
	return m_pHoverCtrl;
}

void CDUIWndManager::AttachDialog(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));

	//release
	if (IsWindow(m_hWndAttach))
	{
		CMMDragDrop::UnRegister();

		if (m_hDCPaint)
		{
			::ReleaseDC(m_hWndAttach, m_hDCPaint);
			m_hDCPaint = NULL;
		}
	}

	// Remember the window context we came from
	m_hWndAttach = hWnd;
	m_hDCPaint = ::GetDC(hWnd);

	m_uTimerID = 0x1000;
	m_bMouseTracking = false;
	m_bRefreshToolTip = false;
	m_bRefreshViewNeeded = false;
	m_bPostedAppMsg = false;
	m_bFirstLayout = true;

	AdjustWndSize();

	return;
}

bool CDUIWndManager::AttachRootCtrl(CDUIContainerCtrl* pControl)
{
	if (NULL == pControl) return false;

	// Reset any previous attachment
	ReleaseRootCtrl();

	// Set the dialog root element
	m_pRootCtrl = pControl;

	//m_shadow.Create(this);
	m_mapControl.clear();

	// Initiate all control
	InitControls(pControl);

	//active tabpag
	for (auto GroupItem : m_mapRadioBoxGroup)
	{
		auto &vecRadioBox = GroupItem.second;

		for (int i = 0; i < vecRadioBox.size(); i++)
		{
			CDUIRadioBoxCtrl *pRadioBoxCtrl = vecRadioBox[i];
			if (NULL == pRadioBoxCtrl) continue;

			if (pRadioBoxCtrl->IsSelected())
			{
				pRadioBoxCtrl->PerformUnSelOther();
				pRadioBoxCtrl->PerformActiveBindTabPage();

				break;
			}
		}
	}

	CDUIGlobal::PerformNotifyChildAdd(this, m_pRootCtrl);

	NeedRefreshView();
	Invalidate();

	return true;
}

bool CDUIWndManager::ReleaseRootCtrl()
{
	CDUIContainerCtrl *pRootCtrl = DetachRootCtrl();

	MMSafeDelete(pRootCtrl);

	return true;
}

CDUIContainerCtrl * CDUIWndManager::GetRootCtrl()
{
	return m_pRootCtrl;
}

CDUIContainerCtrl * CDUIWndManager::DetachRootCtrl()
{
	CDUIGlobal::PerformNotifyChildRemove(this, m_pRootCtrl);

	OnDuiDelayDelete();

	CDUIContainerCtrl *pRootCtrl = m_pRootCtrl;
	if (pRootCtrl)
	{
		pRootCtrl->SetWndManager(NULL);
	}

	m_pRootCtrl = NULL;
	m_pCaptureCtrl = NULL;
	m_pFocusCtrl = NULL;
	m_pHoverCtrl = NULL;
	m_pEventCtrl = NULL;
	m_pWinDragCtrl = NULL;
	m_pWinDragEnterCtrl = NULL;

	m_ptMousePosLast = {};

	m_mapControl.clear();
	m_vecAsynNotify.clear();
	RemoveAllRadioBoxGroup();
	RemoveAllTimer();

	if (IsWindow(m_hWndTooltip)) DestroyWindow(m_hWndTooltip);

	m_hWndTooltip = NULL;

	return pRootCtrl;
}

bool CDUIWndManager::InitControls(CDUIControlBase *pControl)
{
	ASSERT(pControl);
	if (NULL == pControl) return false;
	pControl->SetWndManager(this);
	pControl->FindControl(__FindControlFromIDHash, this, DuiFind_All);

	return true;
}

void CDUIWndManager::InitControlIDHash(CDUIControlBase *pControl)
{
	__FindControlFromIDHash(pControl, this);

	return;
}

void CDUIWndManager::UnInitControlIDHash(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	m_mapControl.erase(pControl->GetCtrlID());

	return;
}

void CDUIWndManager::ReapControl(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;
	if (pControl == m_pCaptureCtrl) m_pCaptureCtrl = NULL;
	if (pControl == m_pFocusCtrl) m_pFocusCtrl = NULL;
	if (pControl == m_pHoverCtrl) m_pHoverCtrl = NULL;
	if (pControl == m_pEventCtrl) m_pEventCtrl = NULL;
	if (pControl == m_pWinDragCtrl) m_pWinDragCtrl = NULL;
	if (pControl == m_pWinDragEnterCtrl) m_pWinDragEnterCtrl = NULL;

	m_mapControl.erase(pControl->GetCtrlID());

	KillTimer(pControl);

	for (int i = m_vecAsynNotify.size() - 1; i >= 0; i--)
	{
		DuiNotify &Notify = m_vecAsynNotify[i];
		if (Notify.pNotifyCtrl == pControl
			|| Notify.DuiNotifyExtend.TreeView.pTreeNode == pControl
			|| Notify.DuiNotifyExtend.TreeView.pRootNode == pControl
			|| Notify.DuiNotifyExtend.TreeView.pRootView == pControl)
		{
			m_vecAsynNotify.erase(m_vecAsynNotify.begin() + i);
		}
	}

	return;
}

bool CDUIWndManager::VerifyControl(CDUIControlBase *pControl)
{
	if (NULL == pControl || NULL == m_pRootCtrl) return false;

	if (pControl == m_pRootCtrl) return true;

	return m_pRootCtrl->VerifyChild(pControl);
}

UINT_PTR CDUIWndManager::GetTag()
{
	return m_AttributeUserTag.GetValue();
}

void CDUIWndManager::SetTag(UINT_PTR pTag)
{
	if (pTag == m_AttributeUserTag.GetValue()) return;

	m_AttributeUserTag.SetValue(pTag);

	return;
}

bool CDUIWndManager::SetTimer(CDUIPropertyObject *pProp, UINT uTimerID, UINT uElapse)
{
	ASSERT(pProp != NULL);
	ASSERT(uElapse > 0);
	if (NULL == pProp || 0 == uElapse) return false;

	for (auto &TimerInfo : m_vecTimers)
	{
		if (TimerInfo.pProp == pProp
			&& TimerInfo.hWnd == m_hWndAttach
			&& TimerInfo.nLocalID == uTimerID)
		{
			if (TimerInfo.bKilled == true)
			{
				if (::SetTimer(m_hWndAttach, TimerInfo.uWinTimer, uElapse, NULL))
				{
					TimerInfo.bKilled = false;
					return true;
				}

				return false;
			}

			return false;
		}
	}

	m_uTimerID = (++m_uTimerID) % 0xF0; //0xf1-0xfe特殊用途
	if (false == ::SetTimer(m_hWndAttach, m_uTimerID, uElapse, NULL)) return false;

	DUITimerInfo TimerInfo;
	TimerInfo.hWnd = m_hWndAttach;
	TimerInfo.pProp = pProp;
	TimerInfo.pControl = MMInterfaceHelper(CDUIControlBase, pProp);
	TimerInfo.nLocalID = uTimerID;
	TimerInfo.uWinTimer = m_uTimerID;
	TimerInfo.bKilled = false;
	m_vecTimers.push_back(TimerInfo);

	return true;
}

bool CDUIWndManager::FindTimer(CDUIPropertyObject *pProp, UINT uTimerID)
{
	ASSERT(pProp != NULL);
	if (NULL == pProp) return false;

	auto FindIt = find_if(m_vecTimers.begin(), m_vecTimers.end(), [&](DUITimerInfo &TimerInfo)
	{
		return TimerInfo.pProp == pProp
			&& TimerInfo.hWnd == m_hWndAttach
			&& TimerInfo.nLocalID == uTimerID
			&& false == TimerInfo.bKilled;
	});

	return FindIt != m_vecTimers.end();
}

bool CDUIWndManager::KillTimer(CDUIPropertyObject *pProp, UINT uTimerID)
{
	ASSERT(pProp != NULL);
	if (NULL == pProp) return false;

	for (auto &TimerInfo : m_vecTimers)
	{
		if (TimerInfo.pProp == pProp
			&& TimerInfo.hWnd == m_hWndAttach
			&& TimerInfo.nLocalID == uTimerID)
		{
			if (TimerInfo.bKilled == false)
			{
				if (::IsWindow(m_hWndAttach)) ::KillTimer(TimerInfo.hWnd, TimerInfo.uWinTimer);

				TimerInfo.bKilled = true;

				return true;
			}
		}
	}

	return false;
}

bool CDUIWndManager::KillTimer(CDUIPropertyObject *pProp)
{
	ASSERT(pProp != NULL);
	if (NULL == pProp) return false;

	for (int i = m_vecTimers.size() - 1; i >= 0; i--)
	{
		DUITimerInfo TimerInfo = (m_vecTimers[i]);

		if (TimerInfo.pProp == pProp)
		{
			if (false == TimerInfo.bKilled) ::KillTimer(TimerInfo.hWnd, TimerInfo.uWinTimer);

			m_vecTimers.erase(m_vecTimers.begin() + i);
		}
	}

	return false;
}

bool CDUIWndManager::RemoveAllTimer()
{
	for (auto &TimerInfo : m_vecTimers)
	{
		if (TimerInfo.bKilled == false)
		{
			if (::IsWindow(m_hWndAttach)) ::KillTimer(m_hWndAttach, TimerInfo.uWinTimer);
		}
	}

	m_vecTimers.clear();

	return true;
}

bool CDUIWndManager::AddRadioBoxToGroup(CDUIRadioBoxCtrl *pControl)
{
	if (pControl == NULL) return false;

	auto &vecRadioBox = m_mapRadioBoxGroup[pControl->GetGroupName()];
	for (int i = 0; i < vecRadioBox.size(); i++)
	{
		if (vecRadioBox[i] == pControl)
		{
			return false;
		}
	}

	vecRadioBox.push_back(pControl);

	return true;
}

VecDuiRadioBoxCtrl CDUIWndManager::GetRadioBoxGroup(LPCTSTR lpszGroup)
{
	auto FindIt = m_mapRadioBoxGroup.find(lpszGroup);

	return FindIt != m_mapRadioBoxGroup.end() ? FindIt->second : VecDuiRadioBoxCtrl();
}

bool CDUIWndManager::RemoveRadioBoxFromGroup(CDUIRadioBoxCtrl *pControl)
{
	auto FindIt = m_mapRadioBoxGroup.find(pControl->GetGroupName());
	if (FindIt == m_mapRadioBoxGroup.end()) return false;

	auto &vecRadioBox = FindIt->second;

	for (int i = 0; i < vecRadioBox.size(); i++)
	{
		if (vecRadioBox[i] == pControl)
		{
			vecRadioBox.erase(vecRadioBox.begin() + i);

			return true;
		}
	}

	return false;
}

void CDUIWndManager::RemoveAllRadioBoxGroup()
{
	m_mapRadioBoxGroup.clear();

	return;
}

CDUIRadioBoxCtrl* CDUIWndManager::GetRadioBoxOfBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl, long lBindIndex)
{
	if (pTabCtrl == NULL || lBindIndex < 0) return NULL;

	for (auto GroupItem : m_mapRadioBoxGroup)
	{
		VecDuiRadioBoxCtrl &vecRadioBox = GroupItem.second;
		if (vecRadioBox.empty()) continue;

		CDUIRadioBoxCtrl *pRadioBoxCtrl = vecRadioBox[0];
		if (NULL == pRadioBoxCtrl) continue;

		if (pRadioBoxCtrl->GetBindTabCtrl() != pTabCtrl) continue;

		for (int i = 0; i < vecRadioBox.size(); i++)
		{
			pRadioBoxCtrl = vecRadioBox[i];
			if (NULL == pRadioBoxCtrl) continue;

			if (pRadioBoxCtrl->GetBindTabCtrlIndex() == lBindIndex) return pRadioBoxCtrl;
		}
	}

	return NULL;
}

HWND CDUIWndManager::GetToolTipWindow() const
{
	return m_hWndTooltip;
}

int CDUIWndManager::GetToolTipHoverTime() const
{
	return m_nToolTipHoverTime;
}

void CDUIWndManager::SetToolTipHoverTime(int nTime)
{
	m_nToolTipHoverTime = nTime;
}

void CDUIWndManager::Invalidate()
{
	if (NULL == m_hWndAttach) return;

	InvalidateRect(m_hWndAttach, NULL, TRUE);

	return;
}

bool CDUIWndManager::IsRefreshViewNeeded() const
{
	return m_bRefreshViewNeeded;
}

void CDUIWndManager::NeedRefreshView()
{
	m_bRefreshViewNeeded = true;

	return;
}

POINT CDUIWndManager::GetMousePosLast() const
{
	return m_ptMousePosLast;
}

POINT CDUIWndManager::GetMousePosDown() const
{
	return m_ptMousePosDown;
}

SIZE CDUIWndManager::GetClientSize() const
{
	RECT rcClient = {};
	::GetClientRect(m_hWndAttach, &rcClient);

	return CDUISize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

CDUIRect CDUIWndManager::GetWindowRect()
{
	CDUIRect rcWnd;
	::GetWindowRect(m_hWndAttach, &rcWnd);

	return rcWnd;
}

SIZE CDUIWndManager::GetWndInitSize()
{
	return GetDpiObj().Scale(m_AttributeWndInitSize.GetValue());
}

void CDUIWndManager::SetWndInitSize(int cx, int cy)
{
	if (GetWndInitSize() == CDUISize(cx, cy)) return;

	m_AttributeWndInitSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

SIZE CDUIWndManager::GetWndMinSize()
{
	return GetDpiObj().Scale(m_AttributeWndMinSize.GetValue());
}

void CDUIWndManager::SetWndMinSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	if (GetWndMinSize() == CDUISize(cx, cy)) return;

	m_AttributeWndMinSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

SIZE CDUIWndManager::GetWndMaxSize()
{
	return GetDpiObj().Scale(m_AttributeWndMaxSize.GetValue());
}

void CDUIWndManager::SetWndMaxSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	if (GetWndMaxSize() == CDUISize(cx, cy)) return;

	m_AttributeWndMaxSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

RECT CDUIWndManager::GetResizeTrack()
{
	return m_AttributeWndResizeTrack.GetValue();
}

void CDUIWndManager::SetResizeTrack(RECT &rcResizeTrack)
{
	if (GetResizeTrack() == rcResizeTrack) return;

	m_AttributeWndResizeTrack.SetValue(rcResizeTrack);

	return;
}

int CDUIWndManager::GetCaptionHeight()
{
	return GetDpiObj().Scale(m_AttributeWndCaptionHeight.GetValue());
}

void CDUIWndManager::SetCaptionHeight(int nHeight)
{
	if (nHeight == GetCaptionHeight()) return;

	m_AttributeWndCaptionHeight.SetValue(nHeight);

	return;
}

const CMMDpi & CDUIWndManager::GetDpiObj()
{
	return CDUIPropertyObject::GetDpiObj();
}

int CDUIWndManager::GetDpi()
{
	return GetDpiObj().GetDpi();
}

bool CDUIWndManager::SetDpi(int nDpi)
{
	return CDUIGlobal::GetInstance()->SetDpi(nDpi);
}

int CDUIWndManager::GetScale()
{
	return GetDpiObj().GetScale();
}

bool CDUIWndManager::SetScale(int nScale)
{
	if (nScale < 100) return false;

	SetDpi(MulDiv(nScale, 96, 100));

	return true;
}

CDUIWndManager * CDUIWndManager::GetWndManager()
{
	return this;
}

bool CDUIWndManager::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
{
	if (false == IsWindow(m_hWndAttach)) return false;
	if (false == ::CreateCaret(m_hWndAttach, hBmp, nWidth, nHeight)) return false;

	m_bCaretActive = false;
	m_rcCaret.right = m_rcCaret.left + nWidth;
	m_rcCaret.bottom = m_rcCaret.top + nHeight;

	return true;
}

bool CDUIWndManager::IsShowCaret()
{
	return m_bCaretActive;
}

void CDUIWndManager::ShowCaret(bool bShow)
{
	if (bShow == IsShowCaret()) return;

	m_bCaretActive = bShow;

	if (IsShowCaret())
	{
		::ShowCaret(GetWndHandle());

		Invalidate();

		return;
	}

	::HideCaret(GetWndHandle());

	Invalidate();

	return;
}

void CDUIWndManager::SetCaretPos(CDUIPoint pt)
{
	if (false == ::SetCaretPos(pt.x, pt.y)) return;

	m_rcCaret.Offset(pt.x - m_rcCaret.left, pt.y - m_rcCaret.top);

	return;
}

BYTE CDUIWndManager::GetWndAlpha()
{
	return m_AttributeWndAlpha.GetValue();
}

void CDUIWndManager::SetWndAlpha(BYTE cbWndAlpha)
{
	if (cbWndAlpha == GetWndAlpha()) return;

	cbWndAlpha = min(255, max(0, cbWndAlpha));
	m_AttributeWndAlpha.SetValue(cbWndAlpha);

	SetWndLayered(true);

	Invalidate();

	return;
}

bool CDUIWndManager::IsWndLayered()
{
	return m_AttributeWndLayered.GetValue();
}

void CDUIWndManager::SetWndLayered(bool bLayered)
{
	m_AttributeWndLayered.SetValue(bLayered);

	return;
}

bool CDUIWndManager::IsGdiplusRenderText()
{
	return m_AttributeGdiplusRenderText.GetValue();
}

void CDUIWndManager::SetGdiplusRenderText(bool bGdiplus)
{
	if (bGdiplus == IsGdiplusRenderText()) return;

	m_AttributeGdiplusRenderText.SetValue(bGdiplus);

	Invalidate();

	return;
}

Gdiplus::TextRenderingHint CDUIWndManager::GetGdiplusRenderTextType()
{
	return (Gdiplus::TextRenderingHint)m_AttributeGdiplusRenderType.GetCurSelItem();
}

void CDUIWndManager::SetGdiplusRenderTextType(Gdiplus::TextRenderingHint RenderType)
{
	if (RenderType == GetGdiplusRenderTextType()) return;

	m_AttributeGdiplusRenderType.SelectItem(RenderType);

	Invalidate();

	return;
}

bool CDUIWndManager::IsGdiplusRenderImage()
{
	return m_AttributeGdiplusRenderImage.GetValue();
}

void CDUIWndManager::SetGdiplusRenderImage(bool bGdiplus)
{
	if (bGdiplus == IsGdiplusRenderImage()) return;

	m_AttributeGdiplusRenderImage.SetValue(bGdiplus);

	Invalidate();

	return;
}

enDuiAnimateWndType CDUIWndManager::GetAnimateWndType()
{
	return (enDuiAnimateWndType)m_AttributeAnimationType.GetCurSelItem();
}

void CDUIWndManager::SetAnimateWndType(enDuiAnimateWndType AnimateWndType)
{
	if (AnimateWndType == GetAnimateWndType()) return;

	m_AttributeAnimationType.SelectItem(AnimateWndType);

	return;
}

int CDUIWndManager::GetAnimateWndFrame()
{
	return m_AttributeAnimationFrame.GetValue();
}

void CDUIWndManager::SetAnimateWndFrame(int nFrame)
{
	if (nFrame == GetAnimateWndFrame()) return;

	m_AttributeAnimationFrame.SetValue(nFrame);

	return;
}

int CDUIWndManager::GetAnimateWndElapse()
{
	return m_AttributeAnimationElapse.GetValue();
}

void CDUIWndManager::SetAnimateWndElapse(int nElapse)
{
	if (nElapse == GetAnimateWndElapse()) return;

	m_AttributeAnimationElapse.SetValue(nElapse);

	return;
}

bool CDUIWndManager::AddINotify(IDUINotify *pINotify)
{
	if (NULL == pINotify) return false;
	if (find(m_vecINotify.begin(), m_vecINotify.end(), pINotify) != m_vecINotify.end()) return true;

	m_vecINotify.push_back(pINotify);

	return true;
}

bool CDUIWndManager::RemoveINotify(IDUINotify *pINotify)
{
	m_vecINotify.erase(std::remove(m_vecINotify.begin(), m_vecINotify.end(), pINotify), m_vecINotify.end());

	return true;
}

void CDUIWndManager::SendNotify(DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl) return;

	CDUIWndManager *pWndManager = Notify.pNotifyCtrl->GetWndManager();
	if (NULL == pWndManager) return;

	Notify.uCtrlID = Notify.pNotifyCtrl->GetCtrlID();
	Notify.ptMouse = m_ptMousePosLast;
	Notify.wKeyState = MapKeyState();
	Notify.dwTimestamp = ::GetTickCount();

	int nCount = Notify.pNotifyCtrl->GetControlCallBackCount();
	for (int n = 0; n < nCount; n++)
	{
		if (false == pWndManager->VerifyControl(Notify.pNotifyCtrl)) return;

		IDUIControlCallBack *pICallBack = Notify.pNotifyCtrl->GetControlCallBack(n);
		if (NULL == pICallBack) continue;

		pICallBack->OnNotify(Notify.pNotifyCtrl, Notify);
	}
	for (int n = 0; n < m_vecINotify.size(); n++)
	{
		IDUINotify *pINotify = m_vecINotify[n];
		if (NULL == pINotify) continue;

		pINotify->OnNotify(Notify);
	}

	return;
}

void CDUIWndManager::SendNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (NULL == pControl) return;

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = pControl;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	SendNotify(Notify);

	return;
}

void CDUIWndManager::PostNotify(DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl || this != Notify.pNotifyCtrl->GetWndManager()) return;

	Notify.ptMouse = m_ptMousePosLast;
	Notify.wKeyState = MapKeyState();
	Notify.dwTimestamp = ::GetTickCount();
	m_vecAsynNotify.push_back(Notify);

	PostAppMsg();

	return;
}

void CDUIWndManager::PostNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam/* = 0*/, LPARAM lParam/* = 0*/)
{
	if (NULL == pControl) return;

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = pControl;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	PostNotify(Notify);

	return;
}

CDUIControlBase * CDUIWndManager::FindControl(POINT pt) const
{
	if (NULL == m_pRootCtrl) return NULL;

	return m_pRootCtrl->FindControl(__FindControlFromPoint, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWndManager::FindControl(UINT uCtrlID) const
{
	auto FindIt = m_mapControl.find(uCtrlID);
	return FindIt != m_mapControl.end() ? static_cast<CDUIControlBase*>(FindIt->second) : NULL;
}

CDUIControlBase * CDUIWndManager::FindControlByShortCut(TCHAR chChar) const
{
	if (NULL == m_pRootCtrl) return NULL;

	DuiFindShortCut FindShortCut = {};
	FindShortCut.chChar = toupper((int)chChar);

	UINT uFlag = DuiFind_Enabled | DuiFind_MeFirst | DuiFind_TopFirst | DuiFind_Visible;
	return m_pRootCtrl->FindControl(__FindControlFromShortcut, &FindShortCut, uFlag);
}

CDUIControlBase * CDUIWndManager::FindControlByDrop(POINT pt) const
{
	if (NULL == m_pRootCtrl) return NULL;

	return m_pRootCtrl->FindControl(__FindControlFromDrop, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWndManager::FindSubControlByPoint(CDUIContainerCtrl *pParent, POINT pt)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return NULL;

	return pParent->FindControl(__FindControlFromPoint, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWndManager::FindSubControlByID(CDUIContainerCtrl *pParent, UINT uCtrlID)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return NULL;

	return pParent->FindControl(__FindControlFromID, (LPVOID)uCtrlID, DuiFind_All);
}

VecDuiControlBase CDUIWndManager::FindSubControlsByClass(CDUIContainerCtrl *pParent, LPCTSTR pstrClass)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return {};

	m_vecFoundControls.clear();
	pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, DuiFind_All);

	return m_vecFoundControls;
}

bool CDUIWndManager::AddPreMessagePtr(IDUIPreMessage *pInterface)
{
	if (find(m_vecPreMessage.begin(), m_vecPreMessage.end(), pInterface) != m_vecPreMessage.end()) return true;

	m_vecPreMessage.push_back(pInterface);

	return true;
}

bool CDUIWndManager::RemovePreMessagePtr(IDUIPreMessage *pInterface)
{
	m_vecPreMessage.erase(std::remove(m_vecPreMessage.begin(), m_vecPreMessage.end(), pInterface), m_vecPreMessage.end());

	return true;
}

bool CDUIWndManager::BeginDragDrop(CDUIControlBase *pControl, WPARAM wParam, LPARAM lParam, int nFlag)
{
	if (NULL == pControl) return false;

	m_pWinDragCtrl = pControl;
	m_DropData.wParam = wParam;
	m_DropData.lParam = lParam;
	m_DropData.pDragDropFrom = this;

	CDUIRect rcCtrl = m_pWinDragCtrl->GetAbsoluteRect();

	//1、先获取DataObject
	CComPtr<IDataObject> pIDataObject = nullptr;
	CComPtr<IDropSource> pIDropSource = nullptr;
	SendNotify(m_pWinDragCtrl, DuiNotify_WinDragCreateObj, (WPARAM)&pIDataObject);
	if (NULL == pIDataObject)
	{
		CMMDropSource *pDropSource = new CMMDropSource;
		if (NULL == pDropSource) return false;

		pIDataObject = new CMMDataObject((CMMDropSource*)pDropSource);
		if (NULL == pIDataObject) return false;

		pIDropSource = pDropSource;
	}

	//bitmap
	HBITMAP hBitmapDrag = NULL;
	CDUIPoint ptOffset = {};
	SendNotify(m_pWinDragCtrl, DuiNotify_WinDragCreateBmp, (WPARAM)&hBitmapDrag, (LPARAM)&ptOffset);
	if (NULL == hBitmapDrag)
	{
		hBitmapDrag = CDUIRenderEngine::GenerateBitmap(this, m_pWinDragCtrl, rcCtrl);
		ptOffset = { rcCtrl.GetWidth() / 2, rcCtrl.GetHeight() / 2 };
	}

	CComPtr<IDragSourceHelper2> pIDragSourceHelper2 = nullptr;
	if (SUCCEEDED(GetDragDropHelper(IID_PPV_ARGS(&pIDragSourceHelper2))))
	{
		BITMAP bmap;
		GetObject(hBitmapDrag, sizeof(BITMAP), &bmap);

		SHDRAGIMAGE di = {};
		di.hbmpDragImage = hBitmapDrag;
		di.sizeDragImage.cx = bmap.bmWidth;
		di.sizeDragImage.cy = bmap.bmHeight;
		di.ptOffset.x = ptOffset.x;
		di.ptOffset.y = ptOffset.y;
		pIDragSourceHelper2->SetFlags(DSH_ALLOWDROPDESCRIPTIONTEXT);
		pIDragSourceHelper2->InitializeFromBitmap(&di, pIDataObject);
	}

	DWORD dwEffectResult;
	HRESULT hRes = ::SHDoDragDrop(NULL, pIDataObject, NULL, nFlag, &dwEffectResult);
	EndDragDrop();

	MMSafeDeleteObject(hBitmapDrag);

	return true;
}

void CDUIWndManager::EndDragDrop()
{
	SendNotify(m_pRootCtrl, DuiNotify_WinDragFinish);

	for (auto &it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	::ReleaseCapture();

	m_DropData = {};
	m_pWinDragCtrl = NULL;
	m_pCaptureCtrl = NULL;

	return;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
void CDUIWndManager::SetDropDescription(DROPIMAGETYPE dwDropType, PCWSTR pszMsg, PCWSTR pszInsert)
{
	SetDropTip(m_DropData.pIDataObject, dwDropType, pszMsg, pszInsert);
}
#endif

void CDUIWndManager::ClearDescription()
{
	SetDropTip(m_DropData.pIDataObject, DROPIMAGE_INVALID, L"", NULL);
}

const tagDuiDropData * CDUIWndManager::GetDropData()
{
	return &m_DropData;
}

HBITMAP CDUIWndManager::ProductBitmap(IN std::vector<CDUIControlBase*> vecCtrl, IN CDUIControlBase* pFromCtrl, OUT CDUIRect &rcBitmap, OUT CDUIPoint &ptFromOffset)
{
	for (auto pCtrl : vecCtrl)
	{
		if (NULL == pCtrl) continue;

		CDUIRect rcItem = pCtrl->GetAbsoluteRect();
		::UnionRect(&rcBitmap, &rcItem, &rcBitmap);
	}

	HDC hPaintDC = ::CreateCompatibleDC(GetWndDC());
	HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(GetWndDC(), rcBitmap.GetWidth(), rcBitmap.GetHeight());
	ASSERT(hPaintDC);
	ASSERT(hPaintBitmap);
	HBITMAP hPaintBitmapOld = (HBITMAP)::SelectObject(hPaintDC, hPaintBitmap);

	for (auto pCtrl : vecCtrl)
	{
		if (NULL == pCtrl) continue;

		HBITMAP hBitmap = CDUIRenderEngine::GenerateBitmap(this, pCtrl, pCtrl->GetAbsoluteRect());
		if (NULL == hBitmap) continue;

		CDUIRect rcPos = pCtrl->GetAbsoluteRect();
		rcPos.Offset(-rcBitmap.left, -rcBitmap.top);

		Bitmap *pBmp = CDUIRenderEngine::GetAlphaBitmap(hBitmap);
		CDUIRenderEngine::DrawImage(hPaintDC, pBmp, rcPos);
		DeleteObject(hBitmap);
		MMSafeDelete(pBmp);

		if (pCtrl == pFromCtrl)
		{
			ptFromOffset = { rcPos.left + rcPos.GetWidth() / 2, rcPos.top + rcPos.GetHeight() / 2 };
		}
	}

	SelectObject(hPaintDC, hPaintBitmapOld);
	DeleteDC(hPaintDC);

	return hPaintBitmap;
}

bool CDUIWndManager::OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes)
{
	bool bHandled = false;
	for (auto pPreMessage : m_vecPreMessage)
	{
		if (NULL == pPreMessage)
		{
			assert(false);
			break;
		}

		lRes = pPreMessage->OnPreWndMessage(uMsg, wParam, lParam, bHandled);

		if (bHandled)
		{
			return true;
		}
	}

	//message
	switch (uMsg)
	{
		case WM_SYSCHAR:
		{
			// Handle ALT-shortcut key-combinations
			CDUIControlBase *pControl = FindControlByShortCut(wParam);
			if (pControl)
			{
				pControl->SetFocus();
				pControl->Active();

				return true;
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return false;
}

bool CDUIWndManager::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes)
{
	//notify
	DispatchNotify();

	//delaydelete
	OnDuiDelayDelete();

	//message
	switch (uMsg)
	{
		case WM_DUIAPP:
		{
			m_bPostedAppMsg = false;

			return true;
		}
		case WM_LBUTTONDOWN:
		{
			lRes = OnDuiLButtonDown(wParam, lParam);

			break;
		}
		case WM_LBUTTONUP:
		{
			lRes = OnDuiLButtonUp(wParam, lParam);

			break;
		}
		case WM_LBUTTONDBLCLK:
		{
			lRes = OnDuiLButtonDlk(wParam, lParam);

			break;
		}
		case WM_RBUTTONDOWN:
		{
			lRes = OnDuiRButtonDown(wParam, lParam);

			break;
		}
		case WM_RBUTTONUP:
		{
			lRes = OnDuiRButtonUp(wParam, lParam);

			break;
		}
		case WM_RBUTTONDBLCLK:
		{
			lRes = OnDuiRButtonDlk(wParam, lParam);

			break;
		}
		case WM_MOUSEMOVE:
		{
			lRes = OnDuiMouseMove(wParam, lParam);

			break;
		}
		case WM_MOUSEHOVER:
		{
			lRes = OnDuiMouseHover(wParam, lParam);

			return true;
		}
		case WM_MOUSELEAVE:
		{
			lRes = OnDuiMouseLeave(wParam, lParam);

			break;
		}
		case WM_MOUSEWHEEL:
		{
			lRes = OnDuiMouseWheel(wParam, lParam);

			break;
		}
		case WM_KEYDOWN:
		{
			lRes = OnDuiKeyDown(wParam, lParam);

			break;
		}
		case WM_KEYUP:
		{
			lRes = OnDuiKeyUp(wParam, lParam);

			break;
		}
		case WM_SYSKEYDOWN:
		{
			lRes = OnDuiSysKeyDown(wParam, lParam);

			break;
		}
		case WM_SYSKEYUP:
		{
			lRes = OnDuiSysKeyUp(wParam, lParam);

			break;
		}
		case WM_CHAR:
		{
			lRes = OnDuiChar(wParam, lParam);

			break;
		}
		case WM_TIMER:
		{
			lRes = OnDuiTimer(wParam, lParam);

			break;
		}
		case WM_SIZE:
		{
			lRes = OnDuiSize();

			return true;
		}
		case WM_PAINT:
		{
			CDUIRect rcPaint;
			bool bUpdate = ::GetUpdateRect(m_hWndAttach, &rcPaint, FALSE);

#ifdef DUI_DESIGN
			bUpdate = true;
			if (rcPaint.Empty())
			{
				GetClientRect(m_hWndAttach, &rcPaint);
			}
#endif

			if (bUpdate)
			{
				PAINTSTRUCT ps = {};
				::BeginPaint(m_hWndAttach, &ps);

				lRes = OnDraw(rcPaint);

				::EndPaint(m_hWndAttach, &ps);
			}

			//design refresh
			if (g_pIDuiWndNotify)
			{
				g_pIDuiWndNotify->OnDuiWndPaint(this);
			}

			return true;
		}
		case WM_ERASEBKGND:
		{
			// We'll do the painting here...
			lRes = 1;

			return true;
		}
		case WM_CONTEXTMENU:
		{
			lRes = OnDuiContextMenu(wParam, lParam);

			break;
		}
		case WM_KILLFOCUS:
		{
			lRes = OnDuiKillFocus(wParam, lParam);

			break;
		}
		case WM_SETCURSOR:
		{
			lRes = OnDuiSetCursor(wParam, lParam);

			if (1 == lRes) return true;

			break;
		}
		case WM_DPICHANGED:
		{
			lRes = OnDuiDpiChanged(wParam, lParam);

			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR lpNMHDR = (LPNMHDR)lParam;
			if (lpNMHDR != NULL) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		case WM_COMMAND:
		{
			if (lParam == 0)
			{
				lRes = OnDuiCommand(wParam, lParam);

				break;
			}

			HWND hWndChild = (HWND)lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		case WM_IME_COMPOSITION:
		{
			lRes = OnDuiImeComPosition(wParam, lParam);
			
			if (lRes > 0) return true;

			break;
		}
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
			if (lParam == 0) break;
			HWND hWndChild = (HWND)lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
	}

	return false;
}

void CDUIWndManager::InitProperty()
{
	DuiCreateGroupAttribute(m_AttributeGroupWndInfo, _T("WndInfo"));
	DuiCreateAttribute(m_AttributeWndCaptionHeight, _T("WndCaptionHeight"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndInitSize, _T("WndInitSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndMinSize, _T("WndMinSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndMaxSize, _T("WndMaxSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndResizeTrack, _T("WndResizeTrack"), _T(""), m_AttributeGroupWndInfo);

	DuiCreateGroupAttribute(m_AttributeGroupLayered, _T("WndLayered"));
	DuiCreateAttribute(m_AttributeWndLayered, _T("WndLayered"), _T(""), m_AttributeGroupLayered);
	DuiCreateAttribute(m_AttributeWndAlpha, _T("WndAlpha"), _T(""), m_AttributeGroupLayered);

	DuiCreateGroupAttribute(m_AttributeGroupRenderText, _T("RenderText"));
	DuiCreateAttribute(m_AttributeGdiplusRenderText, _T("GdiplusRenderText"), _T(""), m_AttributeGroupRenderText);
	DuiCreateAttribute(m_AttributeGdiplusRenderType, _T("GdiplusRenderType"), _T(""), m_AttributeGroupRenderText);

	DuiCreateGroupAttribute(m_AttributeGroupRenderImage, _T("RenderImage"));
	DuiCreateAttribute(m_AttributeGdiplusRenderImage, _T("GdiplusRenderImage"), _T(""), m_AttributeGroupRenderImage);

	DuiCreateGroupAttribute(m_AttributeGroupAnimation, _T("Animation"));
	DuiCreateAttribute(m_AttributeAnimationType, _T("AnimationType"), _T("please set window layered style, or the animation is not perfect"), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeAnimationFrame, _T("AnimationFrame"), _T("Window Animation TotalFrame"), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeAnimationElapse, _T("AnimationElapse"), _T("Window Animation Elapse Of One Frame"), m_AttributeGroupAnimation);

	DuiCreateGroupAttribute(m_AttributeGroupCustom, _T("CustomData"));
	DuiCreateAttribute(m_AttributeUserTag, _T("UserTag"), _T(""), m_AttributeGroupCustom);

	//value
	if (false == m_AttributeGdiplusRenderType.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ TextRenderingHintSystemDefault, _T("HintSystemDefault") });
		AttriCombox.vecItem.push_back({ TextRenderingHintSingleBitPerPixelGridFit, _T("HintSingleBitPerPixelGridFit") });
		AttriCombox.vecItem.push_back({ TextRenderingHintSingleBitPerPixel, _T("HintSingleBitPerPixel") });
		AttriCombox.vecItem.push_back({ TextRenderingHintAntiAliasGridFit, _T("HintAntiAliasGridFit") });
		AttriCombox.vecItem.push_back({ TextRenderingHintAntiAlias, _T("HintAntiAlias") });
		AttriCombox.vecItem.push_back({ TextRenderingHintClearTypeGridFit, _T("HintClearTypeGridFit") });
		m_AttributeGdiplusRenderType.SetCombox(AttriCombox);
		m_AttributeGdiplusRenderType.SelectItem(TextRenderingHintSystemDefault);
	}
	if (false == m_AttributeAnimationType.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ AnimateWnd_None, _T("None") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Left, _T("Left") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftDiffuse, _T("LeftDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Right, _T("Right") });
		AttriCombox.vecItem.push_back({ AnimateWnd_RightDiffuse, _T("RightDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Top, _T("Top") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopDiffuse, _T("TopDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Bottom, _T("Bottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_BottomDiffuse, _T("BottomDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Size, _T("Size") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_SizeSpring, _T("SizeSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_SizeDiffuse, _T("SizeDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_CenterDiffuse, _T("CenterDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightDiffuse, _T("LeftRightDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightExpand, _T("LeftRightExpand") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_LeftRightExpandSpring, _T("LeftRightExpandSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightCombin, _T("LeftRightCombin") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomDiffuse, _T("TopBottomDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomExpand, _T("TopBottomExpand") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_TopBottomExpandSpring, _T("TopBottomExpandSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomCombin, _T("TopBottomCombin") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateLeftRight, _T("3DRotateLeftRight") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateTopBottom, _T("3DRotateTopBottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundLeft, _T("3DRoundLeft") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundRight, _T("3DRoundRight") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundTop, _T("3DRoundTop") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundBottom, _T("3DRoundBottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateReplaseLR, _T("3DRotateReplaseLR") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateReplaseTB, _T("3DRotateReplaseTB") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundReplaseLR, _T("3DRoundReplaseLR") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundReplaseTB, _T("3DRoundReplaseTB") });
		m_AttributeAnimationType.SetCombox(AttriCombox);
		m_AttributeAnimationType.SelectItem(AnimateWnd_None);
	}
	
	return;
}

void CDUIWndManager::InitComplete()
{
	__super::InitComplete();

	return;
}

bool CDUIWndManager::SaveAttribute(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode || NULL == pNode->GetDocument()) return false;

	if (false == __super::SaveAttribute(pNode)) return false;

	//控件
	if (m_pRootCtrl)
	{
		tinyxml2::XMLElement *pNodeCtrl = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(m_pRootCtrl->GetClass()));
		if (!pNodeCtrl) return true;

		m_pRootCtrl->SaveAttribute(pNodeCtrl);

		pNode->LinkEndChild(pNodeCtrl);
	}

	return true;
}

bool CDUIWndManager::SetTimer(UINT uTimerID, UINT nElapse)
{
	return SetTimer(this, uTimerID, nElapse);
}

bool CDUIWndManager::KillTimer(UINT uTimerID)
{
	return KillTimer(this, uTimerID);
}

bool CDUIWndManager::KillTimer()
{
	return KillTimer(this);
}

LRESULT CDUIWndManager::OnDraw(CDUIRect rcPaint)
{
	LRESULT lRes = 0;
	
	//layered
	DWORD dwStyle = ::GetWindowLong(m_hWndAttach, GWL_EXSTYLE);
	DWORD dwNewStyle = IsWndLayered() ? (dwStyle | WS_EX_LAYERED) : (dwStyle & ~WS_EX_LAYERED);
	if (dwStyle != dwNewStyle) ::SetWindowLong(m_hWndAttach, GWL_EXSTYLE, dwNewStyle);
	dwStyle = ::GetWindowLong(m_hWndAttach, GWL_EXSTYLE);

	//Should we paint?
	CDUIRect rcClient;
	::GetClientRect(m_hWndAttach, &rcClient);
	if (false == ::IntersectRect(&rcPaint, &rcPaint, &rcClient)) return lRes;
	if (NULL == m_pRootCtrl || rcPaint.Empty()) return lRes;

	//layout
	RefreshLayout();

	//animation wnd
	if (IsAnimatingWnd()) return lRes;

	//object
	if (NULL == m_hBmpBackground || NULL == m_hMemDcBackground)
	{
		//release
		ReleasePaintScene();

		//create
		(dwStyle & WS_EX_LAYERED) ? rcPaint = rcClient : rcPaint;
		m_hMemDcBackground = ::CreateCompatibleDC(m_hDCPaint);
		m_hBmpBackground = CDUIRenderEngine::CreateARGB32Bitmap(m_hDCPaint, rcClient.GetWidth(), rcClient.GetHeight(), &m_pBmpBackgroundBits);

		if (m_hMemDcBackground && m_hBmpBackground && m_pBmpBackgroundBits)
		{
			m_hBmpBackgroundOld = (HBITMAP)SelectObject(m_hMemDcBackground, m_hBmpBackground);
		}
	}
	if (NULL == m_hMemDcBackground || NULL == m_hBmpBackground || NULL == m_pBmpBackgroundBits)
	{
		Invalidate();

		return lRes;
	}

	//clear pixel
	CDUIRenderEngine::ClearPixel(m_pBmpBackgroundBits, rcClient.GetWidth(), rcPaint);

	//paint
	int iSaveDC = ::SaveDC(m_hMemDcBackground);
	m_pRootCtrl->OnDraw(m_hMemDcBackground, rcPaint);
	::RestoreDC(m_hMemDcBackground, iSaveDC);

	//update
	if (dwStyle & WS_EX_LAYERED)
	{
		RECT rcWnd = {};
		::GetWindowRect(m_hWndAttach, &rcWnd);
		POINT pt = { rcWnd.left, rcWnd.top };
		SIZE szWindow = { rcClient.GetWidth(), rcClient.GetHeight() };
		POINT ptSrc = { 0, 0 };
		BLENDFUNCTION Blend = { AC_SRC_OVER, 0, GetWndAlpha(), AC_SRC_ALPHA };
		::UpdateLayeredWindow(m_hWndAttach, m_hDCPaint, &pt, &szWindow, m_hMemDcBackground,
			&ptSrc, 0, &Blend, ULW_ALPHA);

		return lRes;
	}

	::BitBlt(m_hDCPaint, rcPaint.left, rcPaint.top, rcPaint.GetWidth(), rcPaint.GetHeight(),
		m_hMemDcBackground, rcPaint.left, rcPaint.top, SRCCOPY);

	return lRes;
}

LRESULT CDUIWndManager::OnDuiDelayDelete()
{
	while (false == m_queDelayDelete.empty())
	{
		auto pControl = m_queDelayDelete.front();
		m_queDelayDelete.pop_front();

		MMSafeDelete(pControl);
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiLButtonDown(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;
	m_ptMousePosDown = m_ptMousePosLast;

	::SetCapture(m_hWndAttach);

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonDown(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiLButtonDown, pt, DuiMsg, true);
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiLButtonUp(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	::ReleaseCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	m_pCaptureCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonUp(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiLButtonUp, pt, DuiMsg, true);
	}

	m_pCaptureCtrl = NULL;

	return 0;
}

LRESULT CDUIWndManager::OnDuiLButtonDlk(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	::SetCapture(m_hWndAttach);

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonDlk(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiLButtonDlk, pt, DuiMsg, true);
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiRButtonDown(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;
	m_ptMousePosDown = m_ptMousePosLast;

	::SetCapture(m_hWndAttach);

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonDown(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiRButtonDown, pt, DuiMsg, true);
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiRButtonUp(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	::ReleaseCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	m_pCaptureCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonUp(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiRButtonUp, pt, DuiMsg, true);
	}

	m_pCaptureCtrl = NULL;

	return 0;
}

LRESULT CDUIWndManager::OnDuiRButtonDlk(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//查找
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonDlk(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiRButtonDlk, pt, DuiMsg, true);
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiMouseMove(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	//capture
	if (m_pCaptureCtrl)
	{
		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		m_pCaptureCtrl->OnDuiMouseMove(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiMouseMove, pt, DuiMsg, false);

		if (NULL == m_pWinDragCtrl 
			&& (abs(m_ptMousePosLast.x - m_ptMousePosDown.x) >= 4 || abs(m_ptMousePosLast.y - m_ptMousePosDown.y) >= 4))
		{
			if (m_pCaptureCtrl->IsWinDragEnabled())
			{
				m_pCaptureCtrl->OnWinDragStart(pt, DuiMsg);

				//model
				Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnWinDragStart, pt, DuiMsg, false);
			}
			else if ((NULL == MMInterfaceHelper(CDUIThinkEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
				&& (NULL == MMInterfaceHelper(CDUIEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
				&& (NULL == MMInterfaceHelper(CDUIRichEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
				&& m_pCaptureCtrl->GetOwnerModelCtrl()
				&& m_pCaptureCtrl->GetOwnerModelCtrl()->IsWinDragEnabled())
			{
				m_pCaptureCtrl->GetOwnerModelCtrl()->OnWinDragStart(pt, DuiMsg);
			}
		}

		return 0;
	}

	//tooltip
	if (false == m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = {};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.hwndTrack = m_hWndAttach;
		tme.dwHoverTime = NULL == m_hWndTooltip ? m_nToolTipHoverTime : (DWORD)::SendMessage(m_hWndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
		BOOL bRes = _TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}

	CDUIControlBase *pControl = FindSubControlByPoint(m_pRootCtrl, pt);

	//hover
	if (pControl == m_pHoverCtrl && m_pHoverCtrl)
	{
		DuiMsg.pMsgCtrl = m_pHoverCtrl;
		DuiMsg.pMsgCtrl->OnDuiMouseMove(pt, DuiMsg);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseMove, pt, DuiMsg, false);

		return 0;
	}

	//leave
	if (m_pHoverCtrl)
	{
		DuiMessage msgLeave = {};
		msgLeave.wParam = DuiMsg.wParam;
		msgLeave.lParam = DuiMsg.lParam;
		msgLeave.pMsgCtrl = m_pHoverCtrl;
		m_pHoverCtrl->OnDuiMouseLeave(pt, msgLeave);
		Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseLeave, pt, msgLeave, false);

		if (m_hWndTooltip)
		{
			::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, false, (LPARAM)&m_ToolTip);
		}
	}

	//hover
	m_pHoverCtrl = pControl;
	m_bRefreshToolTip = true;

	if (m_pHoverCtrl)
	{
		//enter
		DuiMsg.pMsgCtrl = pControl;
		m_pHoverCtrl->OnDuiMouseEnter(pt, DuiMsg);
		Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseEnter, pt, DuiMsg, false);

		//move
		DuiMsg.pMsgCtrl = pControl;
		m_pHoverCtrl->OnDuiMouseMove(pt, DuiMsg);
		Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseMove, pt, DuiMsg, false);
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiMouseHover(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	CDUIPoint pt(lParam);
	m_bMouseTracking = false;

	if (NULL == m_pHoverCtrl) return lRes;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	DuiMsg.pMsgCtrl = m_pHoverCtrl;
	m_pHoverCtrl->OnDuiMouseHover(pt, DuiMsg);

	//track modify
	CMMString strToolTip = m_pHoverCtrl->GetToolTip();
	if (strToolTip.empty()) return lRes;
	if (false == m_bRefreshToolTip && m_strToolTip == strToolTip) return lRes;

	m_bRefreshToolTip = false;
	m_strToolTip = strToolTip;

	HINSTANCE hInst = CDUIGlobal::GetInstance()->GetInstanceHandle();
	::ZeroMemory(&m_ToolTip, sizeof(m_ToolTip));
	m_ToolTip.cbSize = sizeof(m_ToolTip);
	m_ToolTip.uFlags = TTF_IDISHWND;
	m_ToolTip.hwnd = m_hWndAttach;
	m_ToolTip.uId = (UINT_PTR)m_hWndAttach;
	m_ToolTip.hinst = hInst;
	m_ToolTip.lpszText = const_cast<LPTSTR>((LPCTSTR)strToolTip);
	m_ToolTip.rect = m_pHoverCtrl->GetAbsoluteRect();
	if (NULL == m_hWndTooltip)
	{
		m_hWndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndAttach, NULL, hInst, NULL);
		::SendMessage(m_hWndTooltip, TTM_ADDTOOL, 0, (LPARAM)&m_ToolTip);
	}
	::SendMessage(m_hWndTooltip, TTM_SETMAXTIPWIDTH, 0, m_pHoverCtrl->GetToolTipWidth());
	::SendMessage(m_hWndTooltip, TTM_SETTIPBKCOLOR, RGB(DUIARGBGetR(m_pHoverCtrl->GetToolTipBkColor()), DUIARGBGetG(m_pHoverCtrl->GetToolTipBkColor()), DUIARGBGetB(m_pHoverCtrl->GetToolTipBkColor())), 0);
	::SendMessage(m_hWndTooltip, TTM_SETTIPTEXTCOLOR, RGB(DUIARGBGetR(m_pHoverCtrl->GetToolTipTextColor()), DUIARGBGetG(m_pHoverCtrl->GetToolTipTextColor()), DUIARGBGetB(m_pHoverCtrl->GetToolTipTextColor())), 0);
	::SendMessage(m_hWndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTip);
	::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTip);

	return lRes;
}

LRESULT CDUIWndManager::OnDuiMouseLeave(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	DuiMessage msgLeave = {};
	msgLeave.wParam = wParam;
	msgLeave.lParam = MAKELPARAM(-1, -1);

	if (m_pCaptureCtrl)
	{
		msgLeave.pMsgCtrl = m_pCaptureCtrl;
		m_pCaptureCtrl->OnDuiMouseLeave(pt, msgLeave);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pCaptureCtrl, OnDuiMouseLeave, pt, msgLeave, false);
	}
	if (m_pHoverCtrl)
	{
		msgLeave.pMsgCtrl = m_pHoverCtrl;
		msgLeave.pMsgCtrl->OnDuiMouseLeave(pt, msgLeave);

		//model
		Dui_Dispatch_ModelMouseEvent(m_pHoverCtrl, OnDuiMouseLeave, pt, msgLeave, false);
	}

	m_pHoverCtrl = NULL;
	m_bRefreshToolTip = true;

	//提示窗体
	if (m_hWndTooltip) ::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, false, (LPARAM)&m_ToolTip);
	if (m_bMouseTracking && m_hWndAttach) ::SendMessage(m_hWndAttach, WM_MOUSEMOVE, 0, (LPARAM)-1);
	m_bMouseTracking = false;

	return 0;
}

LRESULT CDUIWndManager::OnDuiMouseWheel(WPARAM wParam, LPARAM lParam)
{
	//variant
	POINT pt = {};
	pt.x = (INT)((SHORT)(LOWORD(lParam)));
	pt.y = (INT)((SHORT)(HIWORD(lParam)));
	::ScreenToClient(m_hWndAttach, &pt);
	m_ptMousePosLast.x = pt.x;
	m_ptMousePosLast.y = pt.y;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;
	DuiMsg.wParam = wParam;

	//find
	CDUIControlBase *pControl = m_pCaptureCtrl;
	NULL == pControl ? pControl = FindControl(pt) : NULL;
	
	if (pControl)
	{
		m_pEventCtrl = pControl;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		bool bRes = DuiMsg.pMsgCtrl->OnDuiMouseWheel(pt, DuiMsg);

		if (false == bRes)
		{
			while (m_pEventCtrl = m_pEventCtrl->GetParent())
			{
				if (m_pEventCtrl
					&& m_pEventCtrl->IsEnabled()
					&& false == m_pEventCtrl->IsMouseThrough()
					&& ((m_pEventCtrl->GetHorizScrollBar() && m_pEventCtrl->GetHorizScrollBar()->IsVisible())
						|| (m_pEventCtrl->GetVertScrollBar() && m_pEventCtrl->GetVertScrollBar()->IsVisible())))
				{
					DuiMsg.pMsgCtrl = m_pEventCtrl;
					DuiMsg.pMsgCtrl->OnDuiMouseWheel(pt, DuiMsg);

					break;
				}
			}
		}
	}

	// Let's make sure that the scroll item below the cursor is the same as before...
	::SendMessage(m_hWndAttach, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(m_ptMousePosLast.x, m_ptMousePosLast.y));

	return 0;
}

LRESULT CDUIWndManager::OnDuiTimer(WPARAM wParam, LPARAM lParam)
{
	DuiMessage DuiMsg = {};

	for (auto &TimerInfo : m_vecTimers)
	{
		if (NULL == TimerInfo.pProp) continue;

		if (TimerInfo.hWnd == m_hWndAttach
			&& TimerInfo.uWinTimer == wParam
			&& TimerInfo.bKilled == false)
		{
			DuiMsg.pMsgCtrl = TimerInfo.pControl;
			DuiMsg.wParam = TimerInfo.nLocalID;
			DuiMsg.ptMouse = m_ptMousePosLast;
			if (DuiMsg.pMsgCtrl)
			{
				DuiMsg.pMsgCtrl->OnDuiTimer(TimerInfo.nLocalID, DuiMsg);
			}
			else
			{
				OnAnimationElapse(TimerInfo.nLocalID);
			}

			break;
		}
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiSize()
{
	ReleasePaintScene();

	if (m_pRootCtrl)
	{
		m_pRootCtrl->NeedRefreshView();
	}

	//design refresh
	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndSize(this);
	}

	return 0;
}

LRESULT CDUIWndManager::OnDuiKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
	DuiMsg.ptMouse = m_ptMousePosLast;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	if (DuiMsg.pMsgCtrl->IsEnabled())
	{
		DuiMsg.pMsgCtrl->OnDuiKeyDown(DuiMsg);
	}

	//model
	Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiKeyDown, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiKeyUp(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
	DuiMsg.ptMouse = m_ptMousePosLast;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	if (DuiMsg.pMsgCtrl->IsEnabled())
	{
		DuiMsg.pMsgCtrl->OnDuiKeyUp(DuiMsg);
	}

	//model
	Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiKeyUp, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiChar(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
	DuiMsg.ptMouse = m_ptMousePosLast;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	if (DuiMsg.pMsgCtrl->IsEnabled())
	{
		DuiMsg.pMsgCtrl->OnDuiChar(DuiMsg);
	}

	//model
	Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiChar, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
	DuiMsg.ptMouse = m_ptMousePosLast;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	if (DuiMsg.pMsgCtrl->IsEnabled())
	{
		DuiMsg.pMsgCtrl->OnDuiSysKeyDown(DuiMsg);
	}

	//model
	Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiSysKeyDown, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
	DuiMsg.ptMouse = m_ptMousePosLast;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	if (DuiMsg.pMsgCtrl->IsEnabled())
	{
		DuiMsg.pMsgCtrl->OnDuiSysKeyUp(DuiMsg);
	}

	//model
	Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiSysKeyUp, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiSysChar(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
	DuiMsg.ptMouse = m_ptMousePosLast;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	if (DuiMsg.pMsgCtrl->IsEnabled())
	{
		DuiMsg.pMsgCtrl->OnDuiSysChar(DuiMsg);
	}

	//model
	Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiSysChar, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiContextMenu(WPARAM wParam, LPARAM lParam)
{
	//pt
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	::ScreenToClient(m_hWndAttach, &pt);

	//menu ctrl
	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;
	while (m_pEventCtrl && false == m_pEventCtrl->IsContextMenu())
	{
		m_pEventCtrl = m_pEventCtrl->GetParent();
	}

	if (NULL == m_pEventCtrl) return 0;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.ptMouse = pt;
	DuiMsg.wKeyState = (WORD)DuiMsg.wParam;
	DuiMsg.dwTimestamp = ::GetTickCount();

	if (DuiMsg.pMsgCtrl->IsContextMenu())
	{
		DuiMsg.pMsgCtrl->OnDuiContextMenu(DuiMsg);
	}

	//model
	Dui_Dispatch_ModelKeyboardEvent(m_pEventCtrl, OnDuiContextMenu, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiKillFocus(WPARAM wParam, LPARAM lParam)
{
	HWND hWndFocus = GetFocus();
	if (m_hWndAttach == hWndFocus
		|| (m_hWndAttach == GetParent(hWndFocus) && (GetWindowLong(hWndFocus, GWL_STYLE) & WS_CHILD)) 
		|| (m_hWndAttach == ::GetWindowOwner(hWndFocus) && (GetWindowLong(hWndFocus, GWL_STYLE) & WS_CHILD))) return 0;

	SetFocusControl(NULL);

	return 0;
}

LRESULT CDUIWndManager::OnDuiSetCursor(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(lParam) != HTCLIENT) return 0;
	if (m_pCaptureCtrl) return 1;

	POINT pt = { 0 };
	::GetCursorPos(&pt);
	::ScreenToClient(m_hWndAttach, &pt);
	CDUIControlBase *pControl = FindControl(pt);
	if (NULL == pControl) return 0;
	if ((pControl->GetControlFlags() & DUIFLAG_SETCURSOR) == 0) return 0;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();
	DuiMsg.ptMouse = pt;
	pControl->OnDuiSetCursor(pt, DuiMsg);

	return 1;
}

LRESULT CDUIWndManager::OnDuiDpiChanged(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndManager::OnDuiCommand(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	::ScreenToClient(m_hWndAttach, &pt);

	m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pEventCtrl;
	DuiMsg.ptMouse = pt;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	DuiMsg.pMsgCtrl->OnDuiCommand(DuiMsg);

	//model
	Dui_Dispatch_ModelEvent(m_pEventCtrl, OnDuiCommand, DuiMsg);

	return 0;
}

LRESULT CDUIWndManager::OnDuiImeComPosition(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) return 0;

	POINT pt = {};
	GetCursorPos(&pt);
	::ScreenToClient(m_hWndAttach, &pt);

	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.pMsgCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;
	DuiMsg.ptMouse = pt;
	DuiMsg.wKeyState = MapKeyState();
	DuiMsg.dwTimestamp = ::GetTickCount();

	return DuiMsg.pMsgCtrl->OnDuiImeComPosition(DuiMsg);
}

void CDUIWndManager::OnDpiChanged(int nScalePre)
{
	nScalePre = max(100, nScalePre);

	//wndsize
	if (false == ::IsZoomed(GetWndHandle()))
	{
		CDUIRect rcWnd = GetWindowRect();
		rcWnd.right = rcWnd.left + (rcWnd.GetWidth()) * (GetScale() * 1.0f / nScalePre);
		rcWnd.bottom = rcWnd.top + (rcWnd.GetHeight()) * (GetScale() * 1.0f / nScalePre);
		SetWindowPos(GetWndHandle(), NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
	}

	//refresh
	if (GetRootCtrl())
	{
		GetRootCtrl()->OnDpiChanged(nScalePre);
		GetRootCtrl()->NeedRefreshView();
	}

	SendNotify(GetRootCtrl(), DuiNotify_DpiChanged);

	return;
}

void CDUIWndManager::OnResourceDelete(CDUIResourceBase *pResourceObj)
{
	if (NULL == m_pRootCtrl) return;

	m_pRootCtrl->OnResourceDelete(pResourceObj);

	Invalidate();

	return;
}

void CDUIWndManager::OnResourceSwitch(int nIndexRes)
{
	if (NULL == m_pRootCtrl) return;

	m_pRootCtrl->OnResourceSwitch(nIndexRes);

	Invalidate();

	return;
}

void CDUIWndManager::AdjustWndSize()
{
	CDUISize szWndInit = GetWndInitSize();
	if (szWndInit.cx > 0 && szWndInit.cy > 0)
	{
		CDUISize szWndMax = GetWndMaxSize();
		CDUISize szWndMin = GetWndMinSize();
		szWndInit.cx = szWndMax.cx > 0 ? min(szWndInit.cx, szWndMax.cx) : szWndInit.cx;
		szWndInit.cy = szWndMax.cy > 0 ? min(szWndInit.cy, szWndMax.cy) : szWndInit.cy;
		szWndInit.cx = max(szWndInit.cx, szWndMin.cx);
		szWndInit.cy = max(szWndInit.cy, szWndMin.cy);
		::SetWindowPos(m_hWndAttach, NULL, 0, 0, szWndInit.cx, szWndInit.cy, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	return;
}

void CDUIWndManager::AdjustImagesHSL()
{
	/*TImageInfo* data;
	for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++)
	{
		faw::String key = m_ResInfo.m_ImageHash.GetAt(i)->Key;
		if (!key.empty())
		{
			data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.find(key.str_view()));
			if (data && data->bUseHSL)
			{
				CDUIRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
	}*/

	Invalidate();

	return;
}

void CDUIWndManager::RefreshLayout()
{
	CDUIRect rcClient;
	::GetClientRect(m_hWndAttach, &rcClient);

	//layout
	bool bNeedLayoutMsg = false;
	if (false == IsRefreshViewNeeded()) return;
	if (rcClient.Empty() || ::IsIconic(m_hWndAttach)) return;

	m_bRefreshViewNeeded = false;

	if (m_pRootCtrl && m_pRootCtrl->IsRefreshViewNeeded())
	{
		m_pRootCtrl->OnDuiSize(rcClient);
		bNeedLayoutMsg = true;
	}
	else
	{
		m_vecFoundControls.clear();
		m_pRootCtrl->FindControl(__FindControlsFromUpdate, nullptr, DuiFind_Visible | DuiFind_MeFirst | DuiFind_UpdateTest);
		for (auto pControl : m_vecFoundControls)
		{
			pControl->OnDuiSize(pControl->GetModalParentRect());
		}
	}

	if (m_bFirstLayout)
	{
		SendNotify(m_pRootCtrl, DuiNotify_WndInited, 0, 0);
	}
	if (bNeedLayoutMsg)
	{
		SendNotify(m_pRootCtrl, DuiNotify_WndLayout, 0, 0);
	}
	if (m_bFirstLayout)
	{
		//animation wnd
#ifndef DUI_DESIGN
		if (AnimateWnd_None != GetAnimateWndType())
		{
			StartAnimationWnd();
		}
#endif
	}

	m_bFirstLayout = false;

	return;
}

void CDUIWndManager::DelayDelete(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	pControl->ReapControl();
	pControl->SetWndManager(NULL);
	pControl->SetParent(NULL);
	m_queDelayDelete.push_back(pControl);
	PostAppMsg();

	return;
}

void CDUIWndManager::DispatchNotify()
{
	while (m_vecAsynNotify.size() > 0)
	{
		auto Notify = m_vecAsynNotify.front();
		m_vecAsynNotify.erase(m_vecAsynNotify.begin());

		if (NULL == Notify.pNotifyCtrl) continue;

		for (int n = 0; n < Notify.pNotifyCtrl->GetControlCallBackCount(); n++)
		{
			IDUIControlCallBack *pICallBack = Notify.pNotifyCtrl->GetControlCallBack(n);
			if (NULL == pICallBack) continue;

			pICallBack->OnNotify(Notify.pNotifyCtrl, Notify);
		}
		for (int n = 0; n < m_vecINotify.size(); n++)
		{
			IDUINotify *pINotify = m_vecINotify[n];
			if (NULL == pINotify) continue;

			pINotify->OnNotify(Notify);
		}
	}

	return;
}

void CDUIWndManager::PostAppMsg()
{
	if (false == m_bPostedAppMsg)
	{
		::PostMessage(m_hWndAttach, WM_DUIAPP, 0, 0);

		m_bPostedAppMsg = true;
	}

	return;
}

void CDUIWndManager::ReleasePaintScene()
{
	if (m_hMemDcBackground)
	{
		SelectObject(m_hMemDcBackground, m_hBmpBackgroundOld);
	}

	MMSafeDeleteObject(m_hBmpBackground);
	MMSafeDeleteDC(m_hMemDcBackground);
	m_pBmpBackgroundBits = NULL;
	m_hBmpBackgroundOld = NULL;

	return;
}

VecDuiControlBase & CDUIWndManager::GetFoundControls()
{
	return m_vecFoundControls;
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlFromIDHash(CDUIControlBase *pThis, LPVOID pData)
{
	CDUIWndManager *pManager = static_cast<CDUIWndManager*>(pData);
	UINT uCtrlID = pThis->GetCtrlID();
	if (0 == uCtrlID) return NULL;
	// Add this control to the hash list
	pManager->m_mapControl[uCtrlID] = pThis;
	return NULL; // Attempt to add all controls
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlFromCount(CDUIControlBase* /*pThis*/, LPVOID pData)
{
	int* pnCount = static_cast<int*>(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlFromPoint(CDUIControlBase *pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->GetAbsoluteRect(), *pPoint) ? pThis : NULL;
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlFromShortcut(CDUIControlBase *pThis, LPVOID pData)
{
	if (false == pThis->IsVisible()) return NULL;

	DuiFindShortCut *pFindShortCut = static_cast<DuiFindShortCut*>(pData);
	if (pFindShortCut->chChar == toupper(pThis->GetShortcut())) return pThis;

	return NULL;
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlFromDrop(CDUIControlBase *pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->GetAbsoluteRect(), *pPoint) && pThis->IsWinDropEnabled() ? pThis : nullptr;
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlFromID(CDUIControlBase *pThis, LPVOID pData)
{
	UINT uFindID = *(static_cast<UINT*>(pData));
	UINT uCtrlID = pThis->GetCtrlID();

	return (uFindID == uCtrlID) ? pThis : NULL;
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlsFromClass(CDUIControlBase *pThis, LPVOID pData)
{
	LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
	LPCTSTR pType = pThis->GetClass();
	if (_tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0)
	{
		if (pThis->GetWndManager())
		{
			pThis->GetWndManager()->GetFoundControls().push_back(pThis);
		}
	}

	return NULL;
}

CDUIControlBase * CALLBACK CDUIWndManager::__FindControlsFromUpdate(CDUIControlBase *pThis, LPVOID pData)
{
	if (NULL == pThis) return NULL;

	if (pThis->IsRefreshViewNeeded() && pThis->GetWndManager())
	{
		pThis->GetWndManager()->GetFoundControls().push_back(pThis);

		return pThis;
	}

	return nullptr;
}
