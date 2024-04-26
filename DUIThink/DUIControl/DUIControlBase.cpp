#include "StdAfx.h"
#include "DUIControlBase.h"

//////////////////////////////////////////////////////////////////////////
#define Water_StoneSize					(10)
#define Water_StoneDepth				(50)

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIControlBase)
MMImplement_ClassName(CDUIControlBase)

CDUIControlBase::CDUIControlBase(void)
{
	return;
}

CDUIControlBase::~CDUIControlBase(void)
{
	//callback
	for (int n = 0; n < m_vecIControlCallBack.size(); n++)
	{
		IDUIControlCallBack *pICallBack = m_vecIControlCallBack[n];
		if (NULL == pICallBack) continue;

		pICallBack->OnRelease(this);
	}

	MMSafeDelete(m_pBmpCustomBack);

	ReapControl();

	m_pParent = NULL;
	m_pWndManager = NULL;

	return;
}

bool CDUIControlBase::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (NULL == pAttributeObj) return true;

	__super::OnAttributeChange(pAttributeObj);

	//名称改变
	if (pAttributeObj == &m_AttributeObjectID)
	{
		SetCtrlID(GetCtrlID());

		return true;
	}

	NeedParentRefreshView();

	return false;
}

void CDUIControlBase::OnResourceDelete(CDUIResourceBase *pResourceObj)
{
	return CDUIGlobal::PerformResourceDelete(this, pResourceObj);
}

void CDUIControlBase::OnResourceSwitch(int nIndexRes)
{
	return CDUIGlobal::PerformResourceSwitch(this, nIndexRes);
}

LPVOID CDUIControlBase::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIControlBase, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIControlBase::GetDescribe() const
{
	return Dui_Ctrl_Control;
}

UINT CDUIControlBase::GetControlFlags()
{
	return DUIFLAG_SETCURSOR;
}

CDUIControlBase * CDUIControlBase::Clone(bool bIncludeChild, bool bRefreshCtrlID)
{
	CDUIControlBase *pControlClone = CDUIFactory::GetInstance()->CreateControlObj((LPCSTR)CT2CA(GetClass()));
	if (NULL == pControlClone) return NULL;

	pControlClone->InitProperty();

	if (m_vecAttributeGroup.size() != pControlClone->m_vecAttributeGroup.size())
	{
		assert(false);
		MMSafeDelete(pControlClone);

		return NULL;
	}
	for (int n = 0; n < m_vecAttributeGroup.size(); n++)
	{
		CDUIAttributeGroup *pAttriGroup = m_vecAttributeGroup[n];
		if (NULL == pAttriGroup) continue;

		*pControlClone->m_vecAttributeGroup[n] = *pAttriGroup;
	}
	if (NULL == MMInterfaceHelper(CDUIContainerCtrl, pControlClone))
	{
		pControlClone->InitComplete();
	}

	//ctrl id
	if (bRefreshCtrlID
		&& NULL == MMInterfaceHelper(CDUIListItemCtrl, this)
		&& NULL == GetOwnerModelCtrl()
		&& GetCtrlID() > Dui_CtrlIDInner_Finish 
		&& GetCtrlID() == pControlClone->GetCtrlID())
	{
		pControlClone->SetCtrlID(CDUIGlobal::GetInstance()->GenerateCtrlID());
	}

	return pControlClone;
}

int CDUIControlBase::GetScale()
{
	return m_pWndManager ? m_pWndManager->GetScale() : 100;
}

int CDUIControlBase::GetControlCallBackCount()
{
	return m_vecIControlCallBack.size();
}

IDUIControlCallBack * CDUIControlBase::GetControlCallBack(int nIndex)
{
	return 0 <= nIndex && nIndex < m_vecIControlCallBack.size() ? m_vecIControlCallBack[nIndex] : NULL;
}

void CDUIControlBase::RegisterControlCallBack(IDUIControlCallBack *pCallBack)
{
	if (find(m_vecIControlCallBack.begin(), m_vecIControlCallBack.end(), pCallBack) != m_vecIControlCallBack.end()) return;

	m_vecIControlCallBack.push_back(pCallBack);

	return;
}

void CDUIControlBase::UnRegisterControlCallBack(IDUIControlCallBack *pCallBack)
{
	auto FindIt = find(m_vecIControlCallBack.begin(), m_vecIControlCallBack.end(), pCallBack);
	if (FindIt == m_vecIControlCallBack.end()) return;

	m_vecIControlCallBack.erase(FindIt);

	return;
}

UINT CDUIControlBase::InitCtrlID()
{
	if (GetCtrlID() <= 0)
	{
		SetCtrlID(CDUIGlobal::GetInstance()->GenerateCtrlID());
	}

	return GetCtrlID();
}

UINT CDUIControlBase::GetCtrlID()
{
	return m_AttributeObjectID.GetValue();
}

bool CDUIControlBase::SetCtrlID(UINT uID)
{
	if (0 == uID) return false;

#ifndef DUI_DESIGN
	if (uID == GetCtrlID()) return true;
#endif

	//value
	UINT uIDOld = GetCtrlID();
	m_AttributeObjectID.SetValue(uID);

	//modify id map
	if (false == CDUIGlobal::GetInstance()->ModifyCtrlID(uIDOld, uID, this))
	{
		return false;
	}

	//info
	if (m_pWndManager)
	{
		m_pWndManager->UnInitControlIDHash(this);
	}

	if (m_pWndManager)
	{
		m_pWndManager->InitControlIDHash(this);
		m_pWndManager->SendNotify(this, DuiNotify_CtrlIDChanged);
	}

	return true;
}

void CDUIControlBase::RefreshCtrlID()
{
	if (0 < GetCtrlID() && GetCtrlID() < Dui_CtrlIDInner_Finish) return;

	SetCtrlID(CDUIGlobal::GetInstance()->GenerateCtrlID());

	return;
}

HWND CDUIControlBase::GetWndHandle()
{
	return m_pWndManager ? m_pWndManager->GetWndHandle() : NULL;
}

bool CDUIControlBase::SetWndManager(CDUIWndManager *pWndManager)
{
	if (m_pWndManager == pWndManager) return false;

	//uninit
	OnDuiWndManagerDetach();

	//init
	m_pWndManager = static_cast<CDUIWndManager*>(pWndManager);
	m_nControlStatus = ControlStatus_Normal;

	OnDuiWndManagerAttach();

	return true;
}

CDUIWndManager * CDUIControlBase::GetWndManager()
{
	return m_pWndManager;
}

void CDUIControlBase::SetParent(CDUIContainerCtrl *pParent)
{
	m_pParent = pParent;

	return;
}

CDUIContainerCtrl * CDUIControlBase::GetParent() const
{
	return m_pParent;
}

CDUIControlBase * CDUIControlBase::GetPrevSiblingCtrl()
{
	if (NULL == m_pParent) return NULL;

	return m_pParent->GetChildAt(m_pParent->GetChildIndex(this) - 1);
}

CDUIControlBase * CDUIControlBase::GetNextSiblingCtrl()
{
	if (NULL == m_pParent) return NULL;

	return m_pParent->GetChildAt(m_pParent->GetChildIndex(this) + 1);
}

void CDUIControlBase::SetVisible(bool bVisible)
{
	if (bVisible == m_AttributeVisible.GetValue()) return;

	m_AttributeVisible.SetValue(bVisible);

	if (false == IsVisible() && m_pWndManager && m_pWndManager->GetFocusControl() == this)
	{
		m_pWndManager->SetFocusControl(NULL);
	}

	NeedParentRefreshView();

	//notify
	CDUIGlobal::PerformNotifyVisibleChange(this);

	//notify
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_VisibleChanged);
	}

	//callback
	for (int n = 0; n < m_vecIControlCallBack.size(); n++)
	{
		IDUIControlCallBack *pICallBack = m_vecIControlCallBack[n];
		if (NULL == pICallBack) continue;

		pICallBack->OnVisibleChanged(this);
	}

	return;
}

bool CDUIControlBase::IsVisible()
{
	return m_AttributeVisible.GetValue() && m_bInternVisible;
}

void CDUIControlBase::SetEnabled(bool bEnabled)
{
	if (IsEnabled() == bEnabled) return;

	m_AttributeEnable.SetValue(bEnabled);

	Invalidate();

	return;
}

bool CDUIControlBase::IsEnabled()
{
	return m_AttributeEnable.GetValue();
}

bool CDUIControlBase::IsMouseThrough()
{
	return m_AttributeMouseThrough.GetValue();
}

void CDUIControlBase::SetMouseThrough(bool bThrough)
{
	if (bThrough == IsMouseThrough()) return;

	m_AttributeMouseThrough.SetValue(bThrough);

	return;
}

bool CDUIControlBase::IsClickTransmit()
{
	return m_AttributeClickTransmit.GetValue();
}

void CDUIControlBase::SetClickTransmit(bool bTransmit)
{
	if (bTransmit == IsClickTransmit()) return;

	m_AttributeClickTransmit.SetValue(bTransmit);

	return;
}

bool CDUIControlBase::IsCaptured()
{
	return m_pWndManager && m_pWndManager->GetCaptureControl() == this;
}

bool CDUIControlBase::IsFocused()
{
	return m_pWndManager && m_pWndManager->GetFocusControl() == this;
}

void CDUIControlBase::SetFocus()
{
	if (m_pWndManager)
	{
		m_pWndManager->SetFocusControl(this);
	}

	return;
}

void CDUIControlBase::KillFocus()
{
	if (NULL == m_pWndManager || false == IsFocused()) return;

	m_pWndManager->SetFocusControl(NULL);

	return;
}

bool CDUIControlBase::Active()
{
	if (false == IsVisible()) return false;
	if (false == IsEnabled()) return false;

	if (m_pWndManager && false == GetActiveUrl().empty())
	{
		::ShellExecute(m_pWndManager->GetWndHandle(), NULL, GetActiveUrl(), NULL, NULL, SW_SHOW);
	}

	return true;
}

enDuiCursor CDUIControlBase::GetCursor()
{
	return (enDuiCursor)m_AttributeCursor.GetCurSelItem();
}

void CDUIControlBase::SetCursor(enDuiCursor Cursor)
{
	m_AttributeCursor.SelectItem(Cursor);

	Invalidate();

	return;
}

TCHAR CDUIControlBase::GetShortcut() const
{
	return m_chShortcut;
}

void CDUIControlBase::SetShortcut(TCHAR ch)
{
	m_chShortcut = ch;

	return;
}

CMMString CDUIControlBase::GetActiveUrl()
{
	return m_AttributeActiveUrl.GetValue();
}

void CDUIControlBase::SetActiveUrl(LPCTSTR lpszUrl)
{
	m_AttributeActiveUrl.SetValue(lpszUrl);

	return;
}

bool CDUIControlBase::IsContextMenu()
{
	return m_AttributeContextMenu.GetValue();
}

void CDUIControlBase::SetContextMenu(bool bContextMenu)
{
	if (bContextMenu == IsContextMenu()) return;

	m_AttributeContextMenu.SetValue(bContextMenu);

	return;
}

void CDUIControlBase::SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl)
{
	m_pOwnerModelCtrl = pOwnerModelCtrl;

	return;
}

CDUIControlBase * CDUIControlBase::GetOwnerModelCtrl()
{
	return m_pOwnerModelCtrl;
}

CDUIRect CDUIControlBase::GetModalParentRect()
{
	return m_rcModalParent;
}

void CDUIControlBase::MovePosition(SIZE szOffset)
{
	m_rcModalParent.Offset(szOffset);

	m_rcAbsolute = m_AttributePosition.GetAbsoluteRect(m_rcModalParent);

	return;
}

bool CDUIControlBase::SetAbsoluteRect(CDUIRect &rcAbsolute)
{
	if (rcAbsolute == GetAbsoluteRect()) return false;

	m_AttributePosition.SetPositionFromAbsolute(rcAbsolute);

	NeedParentRefreshView();

	return true;
}

CDUIRect CDUIControlBase::GetAbsoluteRect()
{
	return m_rcAbsolute;
}

bool CDUIControlBase::IsFloat()
{
	return m_AttributePosition.IsFloat();
}

void CDUIControlBase::SetFloat(bool bFloat)
{
	if (bFloat == IsFloat()) return;

	m_AttributePosition.SetFloat(bFloat);

	NeedParentRefreshView();

	return;
}

int CDUIControlBase::GetWidth()
{
	return m_rcAbsolute.GetWidth();
}

int CDUIControlBase::GetFixedWidth()
{
	return m_AttributePosition.GetFixedWidth();
}

void CDUIControlBase::SetFixedWidth(int nWidth)
{
	if (nWidth < 0 || nWidth == DuiDpiScaleBackCtrl(GetFixedWidth())) return;

	nWidth = max(nWidth, DuiDpiScaleBackCtrl(GetMinWidth()));
	nWidth = min(nWidth, DuiDpiScaleBackCtrl(GetMaxWidth()));
	m_AttributePosition.SetFixedWidth(nWidth);

	NeedParentRefreshView();

	return;
}

int CDUIControlBase::GetHeight()
{
	return m_rcAbsolute.GetHeight();
}

int CDUIControlBase::GetFixedHeight()
{
	return m_AttributePosition.GetFixedHeight();
}

void CDUIControlBase::SetFixedHeight(int nHeight)
{
	if (nHeight < 0 || nHeight == DuiDpiScaleBackCtrl(GetFixedHeight())) return;

	nHeight = max(nHeight, DuiDpiScaleBackCtrl(GetMinHeight()));
	nHeight = min(nHeight, DuiDpiScaleBackCtrl(GetMaxHeight()));
	m_AttributePosition.SetFixedHeight(nHeight);

	NeedParentRefreshView();

	return;
}

enDuiHorizAlignType CDUIControlBase::GetHorizAlignType()
{
	return m_AttributePosition.GetHorizAlignType();
}

void CDUIControlBase::SetHorizAlignType(enDuiHorizAlignType AlignType)
{
	if (AlignType == GetHorizAlignType()) return;

	m_AttributePosition.SetHorizAlignType(AlignType);

	NeedParentRefreshView();

	return;
}

enDuiVertAlignType CDUIControlBase::GetVertAlignType()
{
	return m_AttributePosition.GetVertAlignType();
}

void CDUIControlBase::SetVertAlignType(enDuiVertAlignType AlignType)
{
	if (AlignType == GetVertAlignType()) return;

	m_AttributePosition.SetVertAlignType(AlignType);

	NeedParentRefreshView();

	return;
}

CDUIRect CDUIControlBase::GetPadding()
{
	tagDuiPosition Position = m_AttributePosition.GetPosition();

	CDUIRect rcPadding;
	rcPadding.left = IsDpiPadding() ? DuiDpiScaleCtrl(Position.HorizPosition.nLeftAlignValue) : Position.HorizPosition.nLeftAlignValue;
	rcPadding.top = IsDpiPadding() ? DuiDpiScaleCtrl(Position.VertPosition.nTopAlignValue) : Position.VertPosition.nTopAlignValue;
	rcPadding.right = IsDpiPadding() ? DuiDpiScaleCtrl(Position.HorizPosition.nRightAlignValue) : Position.HorizPosition.nRightAlignValue;
	rcPadding.bottom = IsDpiPadding() ? DuiDpiScaleCtrl(Position.VertPosition.nBottomAlignValue) : Position.VertPosition.nBottomAlignValue;

	return rcPadding;
}

void CDUIControlBase::SetPadding(long lLeft, long lTop, long lRight, long lBottom)
{
	CDUIRect rcPadding = GetPadding();
	if (lLeft == rcPadding.left
		&& lTop == rcPadding.top
		&& lRight == rcPadding.right
		&& lBottom == rcPadding.bottom) return;

	m_AttributePosition.SetLeftAlignValue(lLeft);
	m_AttributePosition.SetTopAlignValue(lTop);
	m_AttributePosition.SetRightAlignValue(lRight);
	m_AttributePosition.SetBottomAlignValue(lBottom);

	NeedParentRefreshView();

	return;
}

void CDUIControlBase::SetPadding(CDUIRect rcPadding)
{
	return SetPadding(rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom);
}

void CDUIControlBase::SetPaddingL(long lLeft)
{
	if (lLeft == m_AttributePosition.GetLeftAlignValue()) return;

	m_AttributePosition.SetLeftAlignValue(lLeft);

	NeedParentRefreshView();

	return;
}

void CDUIControlBase::SetPaddingT(long lTop)
{
	if (lTop == m_AttributePosition.GetTopAlignValue()) return;

	m_AttributePosition.SetTopAlignValue(lTop);

	NeedParentRefreshView();

	return;
}

void CDUIControlBase::SetPaddingR(long lRight)
{
	if (lRight == m_AttributePosition.GetRightAlignValue()) return;

	m_AttributePosition.SetRightAlignValue(lRight);

	NeedParentRefreshView();

	return;
}

void CDUIControlBase::SetPaddingB(long lBottom)
{
	if (lBottom == m_AttributePosition.GetBottomAlignValue()) return;

	m_AttributePosition.SetBottomAlignValue(lBottom);

	NeedParentRefreshView();

	return;
}

bool CDUIControlBase::IsPaddingHorizCenter()
{
	return HorizAlign_Center == m_AttributePosition.GetHorizAlignType();
}

void CDUIControlBase::SetPaddingHorizCenter(int nXOffset)
{
	if ((IsPaddingHorizCenter() && nXOffset == GetPadding().left)) return;

	m_AttributePosition.SetHorizAlignType(HorizAlign_Center);
	m_AttributePosition.SetLeftAlignValue(nXOffset);

	NeedParentRefreshView();

	return;
}

bool CDUIControlBase::IsPaddingVertCenter()
{
	return VertAlign_Middle == m_AttributePosition.GetVertAlignType();
}

void CDUIControlBase::SetPaddingVertCenter(int nYOffset)
{
	if ((IsPaddingVertCenter() && nYOffset == GetPadding().top)) return;

	m_AttributePosition.SetVertAlignType(VertAlign_Middle);
	m_AttributePosition.SetTopAlignValue(nYOffset);

	NeedParentRefreshView();

	return;
}

bool CDUIControlBase::IsDpiPadding()
{
	return m_AttributeDpiPadding.GetValue();
}

void CDUIControlBase::SetDpiPadding(bool bDpiPadding)
{
	if (bDpiPadding == IsDpiPadding()) return;

	m_AttributeDpiPadding.SetValue(bDpiPadding);

	NeedParentRefreshView();

	return;
}

long CDUIControlBase::GetMinWidth()
{
	return DuiDpiScaleCtrl(m_AttributeMinSize.GetValue().cx);
}

void CDUIControlBase::SetMinWidth(long lWidth)
{
	if (lWidth < 0 || lWidth == GetMinWidth()) return;

	m_AttributeMinSize.SetValue(lWidth, m_AttributeMinSize.GetValue().cy);

	NeedParentRefreshView();

	return;
}

long CDUIControlBase::GetMaxWidth()
{
	return DuiDpiScaleCtrl(m_AttributeMaxSize.GetValue().cx);
}

void CDUIControlBase::SetMaxWidth(long lWidth)
{
	if (lWidth < 0 || lWidth == GetMaxWidth()) return;

	m_AttributeMaxSize.SetValue(lWidth, m_AttributeMaxSize.GetValue().cy);

	NeedParentRefreshView();

	return;
}

long CDUIControlBase::GetMinHeight()
{
	return DuiDpiScaleCtrl(m_AttributeMinSize.GetValue().cy);
}

void CDUIControlBase::SetMinHeight(long lHeight)
{
	if (lHeight < 0 || lHeight == GetMinHeight()) return;

	m_AttributeMinSize.SetValue(m_AttributeMinSize.GetValue().cx, lHeight);

	NeedParentRefreshView();

	return;
}

long CDUIControlBase::GetMaxHeight()
{
	return DuiDpiScaleCtrl(m_AttributeMaxSize.GetValue().cy);
}

void CDUIControlBase::SetMaxHeight(long lHeight)
{
	if (lHeight < 0 || lHeight == GetMaxHeight()) return;

	m_AttributeMaxSize.SetValue(m_AttributeMaxSize.GetValue().cx, lHeight);

	NeedParentRefreshView();

	return;
}

bool CDUIControlBase::SetTimer(UINT uTimerID, UINT nElapse)
{
	if (NULL == m_pWndManager) return false;

	return m_pWndManager->SetTimer(this, uTimerID, nElapse);
}

bool CDUIControlBase::KillTimer(UINT uTimerID)
{
	if (NULL == m_pWndManager) return false;

	return m_pWndManager->KillTimer(this, uTimerID);
}

bool CDUIControlBase::KillTimer()
{
	if (NULL == m_pWndManager) return false;

	return m_pWndManager->KillTimer(this);
}

ARGB CDUIControlBase::GetBkColor()
{
	return m_AttributeColorBk.GetColorValue();
}

void CDUIControlBase::SetBkColor(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorBk.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

bool CDUIControlBase::IsColorHSL()
{
	return m_AttributeIsColorHSL.GetValue();
}

void CDUIControlBase::SetColorHSL(bool bColorHSL)
{
	if (bColorHSL == IsColorHSL()) return;

	m_AttributeIsColorHSL.SetValue(bColorHSL);

	Invalidate();

	return;
}

DWORD CDUIControlBase::GetAdjustColor(DWORD dwColor)
{
	if (false == IsColorHSL()) return dwColor;

	short H, S, L;
	CDUIGlobal::GetInstance()->GetHSL(&H, &S, &L);
	return CDUIRenderEngine::AdjustColor(dwColor, H, S, L);
}

tagDuiImageSection CDUIControlBase::GetBkImageSection()
{
	return m_AttributeImageBack.GetImageSection();
}

void CDUIControlBase::SetBkImageSection(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageBack.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIControlBase::SetBkImage(CMMString strImageRes)
{
	SetBkImage(std::vector<CMMString>{ strImageRes });

	return;
}

void CDUIControlBase::SetBkImage(std::vector<CMMString> vecImageRes)
{
	tagDuiImageSection ImageSection;
	ImageSection.vecImageResSwitch = vecImageRes;
	SetBkImageSection(ImageSection);

	return;
}

tagDuiImageSection CDUIControlBase::GetForeImageSection()
{
	return m_AttributeImageFore.GetImageSection();
}

void CDUIControlBase::SetForeImageSection(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageFore.SetImageSection(ImageSection);

	Invalidate();

	return;
}

SIZE CDUIControlBase::GetRoundSize()
{
	return m_AttributeRoundSize.GetValue();
}

void CDUIControlBase::SetRoundSize(SIZE cxyRound)
{
	if (cxyRound == GetRoundSize()) return;

	m_AttributeRoundSize.SetValue(cxyRound);

	Invalidate();

	return;
}

Gdiplus::Bitmap * CDUIControlBase::GetCustomBmpBack() const
{
	return m_pBmpCustomBack;
}

void CDUIControlBase::SetCustomBmpBack(Gdiplus::Bitmap *pBmp, int nAlign)
{
	MMSafeDelete(m_pBmpCustomBack);

	m_pBmpCustomBack = pBmp;
	m_nCustomBackAlign = nAlign;

	Invalidate();

	return;
}

void CDUIControlBase::SetCustomBmpBack(HBITMAP hBimap, int nAlign)
{
	MMSafeDelete(m_pBmpCustomBack);

	m_pBmpCustomBack = CDUIRenderEngine::GetAlphaBitmap(hBimap);
	m_nCustomBackAlign = nAlign;

	MMSafeDeleteObject(hBimap);

	Invalidate();

	return;
}

enDuiLineStyle CDUIControlBase::GetBorderStyle()
{
	return (enDuiLineStyle)m_AttributeBorderStyle.GetCurSelItem();
}

void CDUIControlBase::SetBorderStyle(enDuiLineStyle LineStyle)
{
	if (LineStyle == GetBorderStyle()) return;

	m_AttributeBorderStyle.SelectItem(LineStyle);

	Invalidate();

	return;
}

ARGB CDUIControlBase::GetBorderColor()
{
	return m_AttributeColorBorder.GetColorValue();
}

void CDUIControlBase::SetBorderColor(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorBorder.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

ARGB CDUIControlBase::GetFocusBorderColor()
{
	return m_AttributeColorBorderFocus.GetColorValue();
}

void CDUIControlBase::SetFocusBorderColor(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorBorderFocus.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

CDUIRect CDUIControlBase::GetBorderLine()
{
	return m_AttributeBorderLine.GetValue();
}

void CDUIControlBase::SetBorderLine(RECT rcBorder)
{
	if (rcBorder == GetBorderLine()) return;

	m_AttributeBorderLine.SetValue(rcBorder);

	Invalidate();

	return;
}

CMMString CDUIControlBase::GetToolTip()
{
	return m_AttributeToolTip.GetValue();
}

void CDUIControlBase::SetToolTip(LPCTSTR pstrText)
{
	if (MMInvalidString(pstrText) || _tcscmp(GetToolTip(), pstrText) == 0) return;

	CMMString strTemp(pstrText);
	strTemp.Replace(_T("<n>"), _T("\r\n"));
	m_AttributeToolTip.SetValue(strTemp);

	return;
}

int CDUIControlBase::GetToolTipWidth()
{
	return m_AttributeToolTipWidth.GetValue();
}

void CDUIControlBase::SetToolTipWidth(int nWidth)
{
	if (nWidth == GetToolTipWidth()) return;

	m_AttributeToolTipWidth.SetValue(nWidth);

	return;
}

ARGB CDUIControlBase::GetToolTipBkColor()
{
	return m_AttributeToolTipBkColor.GetColorValue();
}

void CDUIControlBase::SetToolTipBkColor(ARGB dwColor)
{
	if (dwColor == GetToolTipBkColor()) return;

	m_AttributeToolTipBkColor.SetColor(dwColor);

	return;
}

ARGB CDUIControlBase::GetToolTipTextColor()
{
	return m_AttributeToolTipTextColor.GetColorValue();
}

void CDUIControlBase::SetToolTipTextColor(ARGB dwColor)
{
	if (dwColor == GetToolTipTextColor()) return;

	m_AttributeToolTipTextColor.SetColor(dwColor);

	return;
}

CMMString CDUIControlBase::GetUserData()
{
	return m_AttributeUserData.GetValue();
}

void CDUIControlBase::SetUserData(LPCTSTR pstrText)
{
	if (0 == _tcscmp(GetUserData(), pstrText)) return;

	m_AttributeUserData.SetValue(pstrText);

	return;
}

UINT_PTR CDUIControlBase::GetTag()
{
	return m_AttributeUserTag.GetValue();
}

void CDUIControlBase::SetTag(UINT_PTR pTag)
{
	if (pTag == GetTag()) return;

	m_AttributeUserTag.SetValue(pTag);

	return;
}

void CDUIControlBase::Invalidate()
{
	if (false == IsVisible()) return;

	CDUIRect rcInvalidate = GetAbsoluteRect();

	if (m_pWndManager && m_pWndManager->GetWndHandle())
	{
		::InvalidateRect(m_pWndManager->GetWndHandle(), &rcInvalidate, TRUE);
	}

	return;
}

void CDUIControlBase::NeedRefreshView()
{
	if (false == IsVisible()) return;

	m_bRefreshViewNeeded = true;

	Invalidate();

	if (m_pWndManager)
	{
		m_pWndManager->NeedRefreshView();
	}

	return;
}

void CDUIControlBase::NeedParentRefreshView()
{
	if (m_pParent)
	{
		m_pParent->NeedRefreshView();

		return;
	}

	NeedRefreshView();

	return;
}

bool CDUIControlBase::IsRefreshViewNeeded() const
{
	return m_bRefreshViewNeeded;
}

void CDUIControlBase::RefreshView()
{
	Invalidate();

	return;
}

bool CDUIControlBase::OnDraw(HDC hDC, const RECT &rcPaint, bool bGenerateBmp)
{
	if (false == ::IntersectRect(&m_rcPaint, &rcPaint, &m_rcAbsolute)) return false;

	CDUISize szBorderRound = GetRoundSize();
	CDUIRenderClip Clip;
	CDUIRenderClip::GenerateClip(hDC, m_rcPaint, Clip);

	DoPaint(hDC, bGenerateBmp);

	//callback
	if (false == bGenerateBmp)
	{
		for (int n = 0; n < m_vecIControlCallBack.size(); n++)
		{
			IDUIControlCallBack *pICallBack = m_vecIControlCallBack[n];
			if (NULL == pICallBack) continue;

			pICallBack->OnPaint(this, hDC);
		}
	}

	return true;
}

bool CDUIControlBase::DoPaint(HDC hDC, bool bGenerateBmp)
{
	if (false == IsVisible()) return false;
	
	PaintBkColor(hDC);
	PaintBkImage(hDC);
	PaintStatusColor(hDC);
	PaintStatusImage(hDC);
	PaintForeImage(hDC);
	PaintText(hDC);
	PaintBorder(hDC);

	return true;
}

bool CDUIControlBase::IsWinDragEnabled()
{
	return m_AttributeWinDragEnable.GetValue();
}

void CDUIControlBase::SetWinDragEnabled(bool bDragEnabled)
{
	if (bDragEnabled == IsWinDragEnabled()) return;

	m_AttributeWinDragEnable.SetValue(bDragEnabled);

	if (IsWinDragEnabled() && m_pWndManager)
	{
		m_pWndManager->Register(m_pWndManager->GetWndHandle());
	}

	return;
}

bool CDUIControlBase::IsWinDropEnabled()
{
	return m_AttributeWinDropEnable.GetValue();
}

void CDUIControlBase::SetWinDropEnabled(bool bDropEnabled)
{
	if (bDropEnabled == IsWinDropEnabled()) return;

	m_AttributeWinDropEnable.SetValue(bDropEnabled);

	if (IsWinDropEnabled() && m_pWndManager)
	{
		m_pWndManager->Register(m_pWndManager->GetWndHandle());
	}

	return;
}

bool CDUIControlBase::IsShadowText()
{
	return false;
}

bool CDUIControlBase::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus |= ControlStatus_Pushed;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_LButtonDown, Msg.wParam, Msg.lParam);
	}
	if (m_pWndManager && false == GetActiveUrl().empty())
	{
		::ShellExecute(m_pWndManager->GetWndHandle(), NULL, GetActiveUrl(), NULL, NULL, SW_SHOW);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus &= ~ControlStatus_Pushed;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_LButtonUp, Msg.wParam, Msg.lParam);
	}
	if (GetParent()
		&& GetParent()->IsAnimateDrag()
		&& m_pParent->IsAnimateDraging())
	{
		m_pParent->OnDragMoveEnd(this);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus |= ControlStatus_Pushed;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_LDbClick, Msg.wParam, Msg.lParam);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus |= ControlStatus_Pushed;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_RButtonDown, Msg.wParam, Msg.lParam);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus &= ~ControlStatus_Pushed;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_RButtonUp, Msg.wParam, Msg.lParam);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus |= ControlStatus_Pushed;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_RDbClick, Msg.wParam, Msg.lParam);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (GetCursor())
	{
		::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(GetCursor())));
	}
	else
	{
		::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
	}

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_SetCursor, Msg.wParam, Msg.lParam);
	}

	return true;
}

bool CDUIControlBase::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus |= ControlStatus_Hot;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_MouseEnter, Msg.wParam, Msg.lParam);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiMouseHover(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_MouseHover, Msg.wParam, Msg.lParam);
	}

	return true;
}

bool CDUIControlBase::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	int nControlStatusOld = m_nControlStatus;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_MouseMove, Msg.wParam, Msg.lParam);
	}
	if (m_pWndManager
		&& (m_nControlStatus & ControlStatus_Pushed)
		&& GetParent()
		&& GetParent()->IsAnimateDrag()
		&& NULL == m_pOwnerModelCtrl
		&& false == IsWinDragEnabled())
	{
		if (false == m_pParent->IsAnimateDraging())
		{
			m_pParent->OnDragMoveBegin(this);
		}

		m_pParent->OnDragMoving(this);

		return true;
	}
	if (m_pOwnerModelCtrl)
	{
		if (m_pOwnerModelCtrl->GetParent() && m_pOwnerModelCtrl->GetParent()->IsAnimateDraging())
		{
			return true;
		}
	}
	if (nControlStatusOld != m_nControlStatus)
	{
		Invalidate();
	}

	return true;
}

void CDUIControlBase::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_nControlStatus &= ~ControlStatus_Hot;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_MouseLeave, Msg.wParam, Msg.lParam);
	}

	return;
}

bool CDUIControlBase::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_MouseWheel, Msg.wParam, Msg.lParam);
	}

	return false;
}

bool CDUIControlBase::OnDuiSetFocus()
{
	if (false == IsEnabled()) return false;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_SetFocus);
	}

	Invalidate();

	return true;
}

bool CDUIControlBase::OnDuiKillFocus()
{
	if (false == IsEnabled()) return false;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_KillFocus);
	}

	Invalidate();

	return true;
}

void CDUIControlBase::OnDuiWndManagerAttach()
{
	if (NULL == m_pWndManager) return;

	m_pWndManager->SendNotify(this, DuiNotify_WndManagerAttach);

	if (IsWinDragEnabled() || IsWinDropEnabled())
	{
		m_pWndManager->Register(m_pWndManager->GetWndHandle());
	}

	return;
}

void CDUIControlBase::OnDuiWndManagerDetach()
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_WndManagerDetach);
	}

	return;
}

void CDUIControlBase::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_Timer, Msg.wParam, Msg.lParam);
	}

	return;
}

void CDUIControlBase::OnDuiSize(CDUIRect rcParentAbs)
{
	m_bRefreshViewNeeded = false;
	m_rcModalParent = rcParentAbs;
	m_rcAbsolute = m_AttributePosition.GetAbsoluteRect(m_rcModalParent);

	RefreshView();

	//callback
	for (int n = 0; n < m_vecIControlCallBack.size(); n++)
	{
		IDUIControlCallBack *pICallBack = m_vecIControlCallBack[n];
		if (NULL == pICallBack) continue;

		pICallBack->OnSize(this);
	}

	return;
}

LRESULT CDUIControlBase::OnDuiKeyDown(const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_KeyDown, Msg.wParam, Msg.lParam);
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiKeyUp(const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_KeyUp, Msg.wParam, Msg.lParam);
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiChar(const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_Char, Msg.wParam, Msg.lParam);
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiSysKeyDown(const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_SysKeyDown, Msg.wParam, Msg.lParam);
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiSysKeyUp(const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_SysKeyUp, Msg.wParam, Msg.lParam);
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiSysChar(const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_SysChar, Msg.wParam, Msg.lParam);
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiContextMenu(const DuiMessage &Msg)
{
	if (IsContextMenu())
	{
		if (m_pWndManager)
		{
			m_pWndManager->SendNotify(this, DuiNotify_Menu, Msg.wParam, Msg.lParam);
		}
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiCommand(const DuiMessage &Msg)
{
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_Command, Msg.wParam, Msg.lParam);
	}

	return 0;
}

LRESULT CDUIControlBase::OnDuiImeComPosition(const DuiMessage &Msg)
{
	return 0;
}

bool CDUIControlBase::OnWinDragStart(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == IsWinDragEnabled()) return false;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_WinDragStart, Msg.wParam, Msg.lParam);
	}

	return true;
}

bool CDUIControlBase::OnWinDragEnter(const tagDuiDropData *pDropData, DWORD *pdwEffect)
{
	if (false == IsWinDropEnabled()) return false;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_WinDragEnter, (WPARAM)pDropData, (LPARAM)pdwEffect);
	}

	return true;
}

bool CDUIControlBase::OnWinDragOver(const tagDuiDropData *pDropData, DWORD &dwEffect)
{
	if (false == IsWinDropEnabled()) return false;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_WinDragOver, (WPARAM)pDropData, (LPARAM)&dwEffect);
	}

	return true;
}

bool CDUIControlBase::OnWinDragLeave(const tagDuiDropData *pDropData)
{
	if (false == IsWinDropEnabled()) return false;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_WinDragLeave, (WPARAM)pDropData);
	}

	return true;
}

bool CDUIControlBase::OnWinDrop(const tagDuiDropData *pDropData, DWORD *pdwEffect)
{
	if (false == IsWinDropEnabled()) return false;

	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_WinDrop, (WPARAM)pDropData, LPARAM(pdwEffect));
	}

	return true;
}

void CDUIControlBase::InitProperty()
{
	DuiCreateGroupAttribute(m_AttributeGroupBase, _T("ControlBase"));
	DuiCreateAttribute(m_AttributeObjectID, Key_Dui_PropCtrlID, _T(""), m_AttributeGroupBase);
	DuiCreateAttribute(m_AttributeEnable, _T("Enable"), _T(""), m_AttributeGroupBase);
	DuiCreateAttribute(m_AttributeVisible, _T("Visible"), _T(""), m_AttributeGroupBase);
	DuiCreateAttribute(m_AttributeUserData, _T("UserData"), _T(""), m_AttributeGroupBase);
	DuiCreateAttribute(m_AttributeUserTag, _T("UserTag"), _T(""), m_AttributeGroupBase);
	DuiCreateAttribute(m_AttributeWinDragEnable, _T("WinDragEnable"), _T(""), m_AttributeGroupBase);
	DuiCreateAttribute(m_AttributeWinDropEnable, _T("WinDropEnable"), _T(""), m_AttributeGroupBase);

	DuiCreateGroupAttribute(m_AttributeGroupPosition, Key_Dui_PropPosition);
	DuiCreateAttribute(m_AttributePosition, Key_Dui_PropPosition, _T(""), m_AttributeGroupPosition);
	DuiCreateAttribute(m_AttributeMinSize, _T("MinSize"), _T(""), m_AttributeGroupPosition);
	DuiCreateAttribute(m_AttributeMaxSize, _T("MaxSize"), _T(""), m_AttributeGroupPosition);
	DuiCreateAttribute(m_AttributeDpiPadding, _T("DpiPadding"), _T(""), m_AttributeGroupPosition);

	//图像相关
	DuiCreateGroupAttribute(m_AttributeGroupBk, _T("Back"));
	DuiCreateAttribute(m_AttributeColorBk, _T("ColorBk"), _T(""), m_AttributeGroupBk);
	DuiCreateAttribute(m_AttributeIsColorHSL, _T("IsColorHSL"), _T(""), m_AttributeGroupBk);
	DuiCreateAttribute(m_AttributeImageBack, _T("ImageBack"), _T(""), m_AttributeGroupBk);
	DuiCreateAttribute(m_AttributeImageFore, _T("ImageFore"), _T(""), m_AttributeGroupBk);
	DuiCreateAttribute(m_AttributeRoundSize, _T("RoundSize"), _T(""), m_AttributeGroupBk);

	//边框相关
	DuiCreateGroupAttribute(m_AttributeGroupBorder, _T("Border"));
	DuiCreateAttribute(m_AttributeBorderStyle, _T("BorderStyle"), _T(""), m_AttributeGroupBorder);
	DuiCreateAttribute(m_AttributeColorBorder, _T("ColorBorder"), _T(""), m_AttributeGroupBorder);
	DuiCreateAttribute(m_AttributeColorBorderFocus, _T("ColorFocusBorder"), _T(""), m_AttributeGroupBorder);
	DuiCreateAttribute(m_AttributeBorderLine, DuiCompatibleAttriName(_T("BorderRect"), _T("BorderLine")), _T(""), m_AttributeGroupBorder);
	DuiModifyAttriName(m_AttributeBorderLine, _T("BorderLine"));

	DuiCreateGroupAttribute(m_AttributeGroupMouse, _T("Mouse"));
	DuiCreateAttribute(m_AttributeMouseThrough, _T("MouseThrough"), _T(""), m_AttributeGroupMouse);
	DuiCreateAttribute(m_AttributeClickTransmit, _T("ClickTransmit"), _T("notify listitem"), m_AttributeGroupMouse);
	DuiCreateAttribute(m_AttributeCursor, _T("ControlCursor"), _T(""), m_AttributeGroupMouse);
	DuiCreateAttribute(m_AttributeContextMenu, _T("ContextMenu"), _T(""), m_AttributeGroupMouse);
	DuiCreateAttribute(m_AttributeActiveUrl, _T("ActiveUrl"), _T(""), m_AttributeGroupMouse);

	//控件提示
	DuiCreateGroupAttribute(m_AttributeGroupToolTip, _T("ToolTip"));
	DuiCreateAttribute(m_AttributeToolTip, _T("ToolTip"), _T(""), m_AttributeGroupToolTip);
	DuiCreateAttribute(m_AttributeToolTipWidth, _T("ToolTipWidth"), _T(""), m_AttributeGroupToolTip);
	DuiCreateAttribute(m_AttributeToolTipBkColor, _T("ToolTipBkColor"), _T(""), m_AttributeGroupToolTip);
	DuiCreateAttribute(m_AttributeToolTipTextColor, _T("ToolTipTextColor"), _T(""), m_AttributeGroupToolTip);

	return;
}

void CDUIControlBase::InitAttriValue()
{
	__super::InitAttriValue();

	//baseinfo
	DuiInitAttriValue(m_AttributeEnable, true);
	DuiInitAttriValue(m_AttributeVisible, true);

	DuiInitAttriSizeValue(m_AttributeMinSize, 0, 0);
	DuiInitAttriSizeValue(m_AttributeMaxSize, 999999, 999999);
	DuiInitAttriValue(m_AttributeDpiPadding, true);

	DuiInitAttriValue(m_AttributeIsColorHSL, false);

	DuiInitAttriValue(m_AttributeMouseThrough, false);
	DuiInitAttriValue(m_AttributeClickTransmit, true);
	DuiInitAttriValue(m_AttributeContextMenu, false);

	DuiInitAttriValue(m_AttributeToolTip, _T(""));
	DuiInitAttriValue(m_AttributeToolTipWidth, 500);
	DuiInitAttriColor(m_AttributeToolTipBkColor, DUIARGB(255, 253, 198, 104));
	DuiInitAttriColor(m_AttributeToolTipTextColor, 0);

	DuiInitAttriValue(m_AttributeUserData, _T(""));
	DuiInitAttriValue(m_AttributeUserTag, 0);

	if (false == m_AttributeBorderStyle.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ LineStyle_Null, _T("Null") });
		AttriCombox.vecItem.push_back({ LineStyle_Solid, _T("Solid") });
		AttriCombox.vecItem.push_back({ LineStyle_Dash, _T("Dash") });
		AttriCombox.vecItem.push_back({ LineStyle_Dot, _T("Dot") });
		AttriCombox.vecItem.push_back({ LineStyle_DashDot, _T("DashDot") });
		AttriCombox.vecItem.push_back({ LineStyle_DashDotDot, _T("DashDotDot") });
		m_AttributeBorderStyle.SetCombox(AttriCombox);
		m_AttributeBorderStyle.SelectItem(LineStyle_Solid);
	}

	//光标
	if (false == m_AttributeCursor.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_Arrow, _T("Arrow") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_IBeam, _T("IBeam") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_Wait, _T("Wait") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_Cross, _T("Cross") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_UpArrow, _T("UpArrow") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_SizeNWSE, _T("SizeNWSE") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_SizeNESW, _T("SizeNESW") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_SizeWE, _T("SizeWE") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_SizeNS, _T("SizeNS") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_SizeALL, _T("SizeALL") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_No, _T("No") });
		AttriCombox.vecItem.push_back({ enDuiCursor::Cursor_Hand, _T("Hand") });
		m_AttributeCursor.SetCombox(AttriCombox);
		m_AttributeCursor.SelectItem(Cursor_Arrow);
	}

	return;
}

void CDUIControlBase::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeToolTipBkColor, false);
	DuiInitAttriVisible(m_AttributeToolTipTextColor, false);

	SetInternVisible(m_pParent ? m_pParent->IsVisible() : true);

	return;
}

void CDUIControlBase::PaintBkColor(HDC hDC)
{
	CDUISize szBorderRound = GetRoundSize();
	if (szBorderRound.cx > 0 && szBorderRound.cy > 0)
	{
		CDUIRect rcBorder = GetBorderLine();
		int nSize = max(rcBorder.left, rcBorder.top);
		nSize = max(nSize, rcBorder.right);
		nSize = max(nSize, rcBorder.bottom);

		m_AttributeColorBk.FillRoundRect(hDC, GetBorderRect(), nSize, szBorderRound.cx, szBorderRound.cy, IsColorHSL());
	
		return;
	}
	
	m_AttributeColorBk.FillRect(hDC, m_rcAbsolute, IsColorHSL());

	return;
}

void CDUIControlBase::PaintBkImage(HDC hDC)
{
	m_AttributeImageBack.Draw(hDC, m_rcAbsolute, m_rcPaint);

	if (m_pBmpCustomBack)
	{
		int nDestWidth = 0, nDestHeight = 0;
		int nImageWidth = m_pBmpCustomBack->GetWidth(), nImageHeight = m_pBmpCustomBack->GetHeight();
		CDUIRect rcDest;

		if (0 == nImageWidth || 0 == nImageHeight) return;

		//等比
		if (CustomBack_ScaleSuitable == (CustomBack_ScaleSuitable & m_nCustomBackAlign))
		{
			double lfScaleBmp = (double)nImageWidth / (double)nImageHeight;
			if (nImageWidth >= nImageHeight)
			{
				nDestWidth = min(GetWidth(), nImageWidth);
				nDestHeight = nDestWidth / lfScaleBmp;
			}
			else
			{
				nDestHeight = min(GetHeight(), nImageHeight);
				nDestWidth = nDestHeight * lfScaleBmp;
			}

			nDestWidth = max(nDestWidth, 1);
			nDestHeight = max(nDestHeight, 1);

			//center
			if (CustomBack_Center == (CustomBack_Center & m_nCustomBackAlign))
			{
				rcDest.left = GetAbsoluteRect().left + GetWidth() / 2 - nDestWidth / 2;
				rcDest.right = rcDest.left + nDestWidth;
			}
			//right
			else if (CustomBack_Right == (CustomBack_Right & m_nCustomBackAlign))
			{
				rcDest.right = GetAbsoluteRect().right;
				rcDest.left = rcDest.right - nDestWidth;
			}
			//left
			else
			{
				rcDest.left = GetAbsoluteRect().left;
				rcDest.right = rcDest.left + nDestWidth;
			}

			//vcenter
			if (CustomBack_VCenter == (CustomBack_VCenter & m_nCustomBackAlign))
			{
				rcDest.top = GetAbsoluteRect().top + GetHeight() / 2 - nDestHeight / 2;
				rcDest.bottom = rcDest.top + nDestHeight;
			}
			//bottom
			else if (CustomBack_Bottom == (CustomBack_Bottom & m_nCustomBackAlign))
			{
				rcDest.bottom = GetAbsoluteRect().bottom;
				rcDest.top = rcDest.bottom - nDestHeight;
			}
			//top
			else
			{
				rcDest.top = GetAbsoluteRect().top;
				rcDest.bottom = rcDest.top + nDestHeight;
			}
		}
		//dengbi
		else if (CustomBack_Scale == (CustomBack_Scale & m_nCustomBackAlign))
		{
			rcDest = GetAbsoluteRect();
			rcDest.left = rcDest.left + rcDest.GetWidth() / 2 - nImageWidth / 2;
			rcDest.top = rcDest.top + rcDest.GetHeight() / 2 - nImageHeight / 2;
			rcDest.right = rcDest.left + nImageWidth;
			rcDest.bottom = rcDest.top + nImageHeight;
		}
		//拉伸
		else
		{
			rcDest = GetAbsoluteRect();
		}

		CDUIRenderEngine::DrawImage(hDC, m_pBmpCustomBack, rcDest);
	}

	return;
}

void CDUIControlBase::PaintForeImage(HDC hDC)
{
	m_AttributeImageFore.Draw(hDC, m_rcAbsolute, m_rcPaint);

	return;
}

void CDUIControlBase::PaintStatusColor(HDC hDC)
{
	return;
}

void CDUIControlBase::PaintStatusImage(HDC hDC)
{
	return;
}

void CDUIControlBase::PaintText(HDC hDC)
{
	return;
}

void CDUIControlBase::PaintBorder(HDC hDC)
{
	if (enDuiLineStyle::LineStyle_Null == GetBorderStyle()) return;

	CDUIAttributeColorSwitch *pAttribute = IsFocused() ? &m_AttributeColorBorderFocus : &m_AttributeColorBorder;
	NULL == pAttribute || pAttribute->empty() ? pAttribute = &m_AttributeColorBorder : NULL;
	if (NULL == pAttribute) return;

	CDUIRect rcBorderLine = GetBorderLine();
	CDUISize szBorderRound = GetRoundSize();
	CDUISize szBreakTop = GetBorderBreakTop();
	int nSize = max(rcBorderLine.left, rcBorderLine.top);
	nSize = max(nSize, rcBorderLine.right);
	nSize = max(nSize, rcBorderLine.bottom);
	if (nSize > 0 && szBorderRound.cx > 0 && szBorderRound.cy > 0)
	{
		pAttribute->DrawRoundRect(hDC, GetBorderRect(), nSize, szBorderRound.cx, szBorderRound.cy, IsColorHSL(), szBreakTop);

		return;
	}
	if (szBreakTop.cx > 0 || szBreakTop.cy > 0)
	{
		pAttribute->DrawRect(hDC, GetBorderRect(), nSize, szBreakTop);

		return;
	}
	if (rcBorderLine.left > 0)
	{
		CDUIRect rcBorderDraw = m_rcAbsolute;
		rcBorderDraw.right = rcBorderDraw.left;
		pAttribute->DrawLine(hDC, rcBorderDraw, rcBorderLine.left, GetBorderStyle(), IsColorHSL());
	}
	if (rcBorderLine.top > 0)
	{
		CDUIRect rcBorderDraw = m_rcAbsolute;
		rcBorderDraw.bottom = rcBorderDraw.top;
		pAttribute->DrawLine(hDC, rcBorderDraw, rcBorderLine.top, GetBorderStyle(), IsColorHSL());
	}
	if (rcBorderLine.right > 0)
	{
		CDUIRect rcBorderDraw = m_rcAbsolute;
		rcBorderDraw.left = rcBorderDraw.right - rcBorderLine.right;
		rcBorderDraw.right = rcBorderDraw.left;
		pAttribute->DrawLine(hDC, rcBorderDraw, rcBorderLine.right, GetBorderStyle(), IsColorHSL());
	}
	if (rcBorderLine.bottom > 0)
	{
		CDUIRect rcBorderDraw = m_rcAbsolute;
		rcBorderDraw.top = rcBorderDraw.bottom - rcBorderLine.bottom;
		rcBorderDraw.bottom = rcBorderDraw.top;
		pAttribute->DrawLine(hDC, rcBorderDraw, rcBorderLine.bottom, GetBorderStyle(), IsColorHSL());
	}

	return;
}

void CDUIControlBase::SetInternVisible(bool bVisible)
{
	if (bVisible == m_bInternVisible) return;

	m_bInternVisible = bVisible;

	if (false == IsVisible() && m_pWndManager && m_pWndManager->GetFocusControl() == this)
	{
		m_pWndManager->SetFocusControl(NULL);
	}

	//notify
	CDUIGlobal::PerformNotifyVisibleChange(this);

	//notify
	if (m_pWndManager)
	{
		m_pWndManager->SendNotify(this, DuiNotify_VisibleChanged);
	}

	//callback
	for (int n = 0; n < m_vecIControlCallBack.size(); n++)
	{
		IDUIControlCallBack *pICallBack = m_vecIControlCallBack[n];
		if (NULL == pICallBack) continue;

		pICallBack->OnVisibleChanged(this);
	}

	return;
}

CDUIControlBase * CDUIControlBase::FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags)
{
	if ((uFlags & DuiFind_Visible) && false == IsVisible()) return NULL;
	if ((uFlags & DuiFind_Enabled) && false == IsEnabled()) return NULL;
	if ((uFlags & DuiFind_HitTest) && (false == IsEnabled() || IsMouseThrough() || false == ::PtInRect(&m_rcAbsolute, *static_cast<LPPOINT>(pData)))) return NULL;

	return Proc(this, pData);
}

void CDUIControlBase::ReapControl()
{
	if (m_pWndManager)
	{
		m_pWndManager->ReapControl(this);
	}

	return;
}

CDUIAttributeColorSwitch * CDUIControlBase::GetAttributeStatusColor()
{
	return NULL;
}

CDUIAttriImageSection * CDUIControlBase::GetAttributeStatusImage()
{
	return NULL;
}

CDUIRect CDUIControlBase::GetBorderRect()
{
	return m_rcAbsolute;
}

CDUISize CDUIControlBase::GetBorderBreakTop()
{
	return {};
}

//////////////////////////////////////////////////////////////////////////
bool operator == (IDUIInterface *pLeft, const CDUIControlBase &pControl)
{
	return MMInterfaceHelper(CDUIControlBase, pLeft) == &pControl;
}

bool operator != (IDUIInterface *pLeft, const CDUIControlBase &pControl)
{
	return false == operator == (pLeft, pControl);
}

