#include "StdAfx.h"
#include "DUIThinkEditCtrl.h"

//////////////////////////////////////////////////////////////////////////
#define ID_MENU_UNDO					101
#define ID_MENU_CUT						102
#define ID_MENU_COPY					103
#define ID_MENU_PASTE					104
#define ID_MENU_CLEAR					105
#define ID_MENU_SELECTALL				106
#define ID_MENU_REDO					107

#define Max_ThinkEditHistory			(50)

static VecDuiRichTextItem g_vecDuiRichTextPreEdit;

//////////////////////////////////////////////////////////////////////////
//std::set<WORD> CDUIThinkEditCtrl::m_setEmoji;

void AddEmojiStore(WORD wCode)
{
	//CDUIThinkEditCtrl::m_setEmoji.insert(wCode);

	return;
}

void AddEmojiStore(WORD wStart, WORD wFinish)
{
	for (WORD wCode = wStart; wCode <= wFinish; wCode++)
	{
		AddEmojiStore(wCode);
	}

	return;
}

void InitEmojiStore()
{
	AddEmojiStore(0x0023);
	AddEmojiStore(0x002A);
	AddEmojiStore(0x0030, 0x0039);
	AddEmojiStore(0x00A9);
	AddEmojiStore(0x00AE);
	AddEmojiStore(0x203C);
	AddEmojiStore(0x2049);
	AddEmojiStore(0x2122);
	AddEmojiStore(0x2139);
	AddEmojiStore(0x2194, 0x2199);
	AddEmojiStore(0x21A9, 0x21AA);
	AddEmojiStore(0x231A, 0x231B);
	AddEmojiStore(0x2328);
	AddEmojiStore(0x23CF);
	AddEmojiStore(0x23E9, 0x23F3);
	AddEmojiStore(0x23F8, 0x23FA);
	AddEmojiStore(0x24C2);
	AddEmojiStore(0x25AA, 0x25AB);
	AddEmojiStore(0x25B6);
	AddEmojiStore(0x25C0);
	AddEmojiStore(0x25FB, 0x25FE);
	AddEmojiStore(0x2600, 0x2604);
	AddEmojiStore(0x260E);
	AddEmojiStore(0x2611);
	AddEmojiStore(0x2614, 0x2615);
	AddEmojiStore(0x2618);
	AddEmojiStore(0x261D);
	AddEmojiStore(0x2620);
	AddEmojiStore(0x2622, 0x2623);
	AddEmojiStore(0x2626);
	AddEmojiStore(0x262A);
	AddEmojiStore(0x262E, 0x262F);
	AddEmojiStore(0x2638, 0x263A);
	AddEmojiStore(0x2640);
	AddEmojiStore(0x2642);
	AddEmojiStore(0x2648, 0x2653);
	AddEmojiStore(0x265F, 0x2660);
	AddEmojiStore(0x2663);
	AddEmojiStore(0x2665, 0x2666);
	AddEmojiStore(0x2668);
	AddEmojiStore(0x267B);
	AddEmojiStore(0x267E, 0x267F);
	AddEmojiStore(0x2692, 0x2697);
	AddEmojiStore(0x2699);
	AddEmojiStore(0x269B, 0x269C);
	AddEmojiStore(0x26A0, 0x26A1);
	AddEmojiStore(0x26AA, 0x26AB);
	AddEmojiStore(0x26B0, 0x26B1);
	AddEmojiStore(0x26BD, 0x26BE);
	AddEmojiStore(0x26C4, 0x26C5);
	AddEmojiStore(0x26C8);
	AddEmojiStore(0x26CE);
	AddEmojiStore(0x26CF);
	AddEmojiStore(0x26D1);
	AddEmojiStore(0x26D3, 0x26D4);
	AddEmojiStore(0x26E9, 0x26EA);
	AddEmojiStore(0x26F0, 0x26F5);
	AddEmojiStore(0x26F7, 0x26FA);
	AddEmojiStore(0x26FD);
	AddEmojiStore(0x2702);
	AddEmojiStore(0x2705);
	AddEmojiStore(0x2708, 0x2709);
	AddEmojiStore(0x270A, 0x270B);
	AddEmojiStore(0x270C, 0x270D);
	
	return;
}

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIThinkEditCtrl)
MMImplement_ClassName(CDUIThinkEditCtrl)

CDUIThinkEditCtrl::CDUIThinkEditCtrl()
{
	return;
}

CDUIThinkEditCtrl::~CDUIThinkEditCtrl()
{
	if (m_pWndManager)
	{
		m_pWndManager->RemovePreMessagePtr(this);
	}

	MMSafeDelete(m_pBmpShadowText);

	return;
}

LRESULT CDUIThinkEditCtrl::OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (NULL == m_pWndManager) return 0;

	if (WM_IME_COMPOSITION == uMsg
		|| WM_KEYDOWN == uMsg)
	{
		if (IsFocused())
		{
			HIMC hImc = ::ImmGetContext(m_pWndManager->GetWndHandle());
			if (hImc)
			{
				//pos
				POINT ptCaret;
				::GetCaretPos(&ptCaret);
				COMPOSITIONFORM Composition;
				Composition.dwStyle = CFS_POINT;
				Composition.ptCurrentPos.x = ptCaret.x;
				Composition.ptCurrentPos.y = ptCaret.y;
				ImmSetCompositionWindow(hImc, &Composition);

				//font
				do
				{
					CDUIAttributeTextStyle *pTextStyle = GetAttributeTextStyleActive();
					if (NULL == pTextStyle) break;

					CDUIFontBase *pFontBase = pTextStyle->GetFontBaseCur();
					if (NULL == pFontBase) break;

					HFONT hFont = pFontBase->GetHandle();
					if (NULL == hFont) break;

					LOGFONT LogFont = {};
					GetObject(hFont, sizeof(LogFont), &LogFont);
					ImmSetCompositionFont(hImc, &LogFont);

				} while (false);
				
				ImmReleaseContext(m_pWndManager->GetWndHandle(), hImc);
			}

			return 0;
		}
	}

	return 0;
}

bool CDUIThinkEditCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj))
	{
		if (DuiAttribute_TextStyle == pAttributeObj->GetAttributeType())
		{
			NeedRefreshView();

			return true;
		}
		if (pAttributeObj == &m_AttributeText)
		{
			SetText(GetText());

			return true;
		}
		if (pAttributeObj == &m_AttributeRichText)
		{
			SetRichTextItem(__super::GetRichTextItem());

			return true;
		}

		return true;
	}

	return false;
}

LPVOID CDUIThinkEditCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIThinkEditCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIThinkEditCtrl::GetDescribe() const
{
	return _T("ThinkEditCtrl");
}

void CDUIThinkEditCtrl::Invalidate()
{
	__super::Invalidate();

	if (m_pBindCtrl)
	{
		m_pBindCtrl->Invalidate();
	}

	return;
}

void CDUIThinkEditCtrl::RefreshView()
{
	__super::RefreshView();

	if (NULL == m_pWndManager) return;

	//measure
	tagDuiTextStyle TextStyle = GetTextStyleActive();
	CDUIRect rcMeasure;
	PerformMeasureString(m_vecRichTextItem, TextStyle, m_mapLineVecRichTextDraw, rcMeasure);

	//range pos
	CDUIRect rcRange = GetTextRange();
	CDUISize szScrollPos = GetScrollPos();
	m_szScrollPos = {};
	m_szScrollRange.cx = max(rcMeasure.GetWidth() - rcRange.GetWidth(), 0);
	m_szScrollRange.cy = max(rcMeasure.GetHeight() - rcRange.GetHeight(), 0);
	szScrollPos.cx = min(szScrollPos.cx, m_szScrollRange.cx);
	szScrollPos.cy = min(szScrollPos.cy, m_szScrollRange.cy);
	SetScrollPos(szScrollPos);

	//caret
	PerformAdjustCaret();

	//animate
	KillTimer(Dui_TimerAnimate_ID);
	for (auto &LineVecRichTextDraw : m_mapLineVecRichTextDraw)
	{
		for (auto &RichTextDrawItem : LineVecRichTextDraw.second)
		{
			if (RichTextItem_Image != RichTextDrawItem.ItemType) continue;

			CDUIImageBase *pImageBase = CDUIGlobal::GetInstance()->GetImageResource(RichTextDrawItem.strImageResName);
			if (NULL == pImageBase || NULL == pImageBase->GetAnimateImage()) continue;

			auto AnimateImageInfo = pImageBase->GetAnimateImageInfo();
			if (RichTextDrawItem.nAnimateFrameCur < 0 || RichTextDrawItem.nAnimateFrameCur >= AnimateImageInfo.nFrameCount) continue;

			RichTextDrawItem.nAnimateElapseCur = ((long*)AnimateImageInfo.vecGifPropertyItem[0].value)[RichTextDrawItem.nAnimateFrameCur] * 10;

			SetTimer(Dui_TimerAnimate_ID, Dui_TimerThinkEditAnimate_Elapse);
		}
	}

	MMSafeDelete(m_pBmpShadowText);

	return;
}

bool CDUIThinkEditCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	//caret timer
	if (m_pWndManager 
		&& false == m_pWndManager->FindTimer(this, Dui_TimerCaret_ID)
		&& IsFocused())
	{
		SetTimer(Dui_TimerCaret_ID, Dui_TimerCaret_Elapse);
	}

	//clip
	CDUIRect rcRange = GetTextRange();
	CDUIRect rcPaint;
	IntersectRect(&rcPaint, &m_rcPaint, &rcRange);
	CDUIRenderClip Clip;
	CDUIRenderClip::GenerateClip(hDC, rcPaint, Clip);

	//caret
	PaintCaret(hDC);

	return true;
}

CDUISize CDUIThinkEditCtrl::MeasureString(LPCTSTR lpszText)
{
	if (NULL == m_pWndManager) return {};

	tagDuiTextStyle TextStyle = GetTextStyleNormal();
	CDUIRect rcMeasure;
	if (false == MMInvalidString(lpszText))
	{
		PerformMeasureString(lpszText, TextStyle, MapLineVecDuiRichTextDraw(), rcMeasure);
	}
	else
	{
		PerformMeasureString(m_vecRichTextItem, TextStyle, MapLineVecDuiRichTextDraw(), rcMeasure);
	}

	return { rcMeasure.GetWidth(), rcMeasure.GetHeight() };
}

CDUISize CDUIThinkEditCtrl::MeasureString(VecDuiRichTextBase vecRichTextBase)
{
	if (NULL == m_pWndManager) return {};

	VecDuiRichTextItem vecRichTextItem;
	for (auto &RichTextBase : vecRichTextBase)
	{
		//text
		if (RichTextItem_Text == RichTextBase.ItemType)
		{
			for (int n = 0; n < RichTextBase.strText.GetLength(); n++)
			{
				vecRichTextItem.push_back(tagDuiRichTextItem());
				vecRichTextItem.back().strText = RichTextBase.strText[n];
			}
		}
		//image
		else if (RichTextItem_Image == RichTextBase.ItemType && false == RichTextBase.strImageResName.IsEmpty())
		{
			vecRichTextItem.push_back(RichTextBase);
		}
	}

	//measure
	tagDuiTextStyle TextStyle = GetTextStyleNormal();
	CDUIRect rcMeasure;
	PerformMeasureString(vecRichTextItem, TextStyle, MapLineVecDuiRichTextDraw(), rcMeasure);

	return { rcMeasure.GetWidth(), rcMeasure.GetHeight() };
}

bool CDUIThinkEditCtrl::SetText(LPCTSTR lpszText)
{
	auto bRes = __super::SetText(lpszText);

#ifndef DUI_DESIGN
	if (false == bRes) return false;
#endif

	tagDuiTextStyle TextStyle = GetTextStyle();
	VecDuiRichTextItem vecRichTextItem;
	CMMString strText = __super::GetText();
	for (int n = 0; n < strText.GetLength(); n++)
	{
		int nEmoji = CDUIGlobal::IsEmoji(strText[n]);
		CMMString strTextSub = CMMString(strText.GetBuffer() + n, max(1, nEmoji));
		nEmoji > 0 ? n += (nEmoji - 1) : 0;

		vecRichTextItem.push_back(tagDuiRichTextItem());
		vecRichTextItem.back().strText = strTextSub;
		vecRichTextItem.back().vecFontResSwitch = TextStyle.vecFontResSwitch;
		vecRichTextItem.back().vecColorResSwitch = TextStyle.vecColorResSwitch;
	}

	SetRichTextItem(vecRichTextItem);

	NeedRefreshView();

	return true;
}

VecDuiRichTextItem CDUIThinkEditCtrl::GetRichTextItem()
{
	return m_vecRichTextItem;
}

bool CDUIThinkEditCtrl::SetRichTextItem(const VecDuiRichTextItem &vecRichTextItem)
{
	if (false == __super::SetRichTextItem(vecRichTextItem)) return false;

	tagDuiTextStyle TextStyle = GetTextStyle();
	VecDuiRichTextItem vecRichTextItemAdjust;
	CMMString strText;

	for (auto RichTextItem : vecRichTextItem)
	{
		//text
		if (RichTextItem_Text == RichTextItem.ItemType)
		{
			if (RichTextItem.strText.IsEmpty()) continue;

			if (false == IsRichEdit() || RichTextItem.vecFontResSwitch.empty())
			{
				RichTextItem.vecFontResSwitch = TextStyle.vecFontResSwitch;
			}
			if (false == IsRichEdit() || RichTextItem.vecColorResSwitch.empty())
			{
				RichTextItem.vecColorResSwitch = TextStyle.vecColorResSwitch;
			}
			for (int n = 0; n < RichTextItem.strText.GetLength(); n++)
			{
				int nEmoji = CDUIGlobal::IsEmoji(RichTextItem.strText[n]);
				CMMString strTextSub = CMMString(RichTextItem.strText.GetBuffer() + n, max(1, nEmoji));
				nEmoji > 0 ? n += (nEmoji - 1) : 0;

				vecRichTextItemAdjust.push_back(tagDuiRichTextItem());
				vecRichTextItemAdjust.back().strText = strTextSub;
				vecRichTextItemAdjust.back().vecFontResSwitch = RichTextItem.vecFontResSwitch;
				vecRichTextItemAdjust.back().vecColorResSwitch = RichTextItem.vecColorResSwitch;
				strText += strTextSub;
			}
		}
		//image
		else if (RichTextItem_Image == RichTextItem.ItemType && false == RichTextItem.strImageResName.IsEmpty())
		{
			vecRichTextItemAdjust.push_back(RichTextItem);
		}
	}

	m_vecRichTextItem = vecRichTextItemAdjust;

	return __super::SetText(strText);
}

enDuiEditTextType CDUIThinkEditCtrl::GetEditTextType()
{
	return (enDuiEditTextType)m_AttributeEditTextType.GetCurSelItem();
}

void CDUIThinkEditCtrl::SetEditTextType(enDuiEditTextType TextType)
{
	if (TextType == GetEditTextType()) return;

	m_AttributeEditTextType.SelectItem(TextType);

	return;
}

bool CDUIThinkEditCtrl::IsNumberOnly()
{
	return EditText_Number == GetEditTextType()
		|| EditText_NumberInt == GetEditTextType()
		|| EditText_NumberDouble == GetEditTextType();
}

bool CDUIThinkEditCtrl::IsRichEdit()
{
	return m_AttributeRichEdit.GetValue();
}

void CDUIThinkEditCtrl::SetRichEdit(bool bRichEdit)
{
	if (bRichEdit == IsRichEdit()) return;

	m_AttributeRichEdit.SetValue(bRichEdit);

	return;
}

bool CDUIThinkEditCtrl::IsReadOnly()
{
	return m_AttributeReadOnly.GetValue();
}

void CDUIThinkEditCtrl::SetReadOnly(bool bReadOnly)
{
	if (bReadOnly == IsReadOnly()) return;

	m_AttributeReadOnly.SetValue(bReadOnly);

	return;
}

bool CDUIThinkEditCtrl::IsPasswordMode()
{
	return m_AttributePasswordMode.GetValue();
}

void CDUIThinkEditCtrl::SetPasswordMode(bool bPasswordMode)
{
	if (bPasswordMode == IsPasswordMode()) return;

	m_AttributePasswordMode.SetValue(bPasswordMode);

	Invalidate();

	return;
}

bool CDUIThinkEditCtrl::IsAutoSelAll()
{
	return m_AttributeAutoSelAll.GetValue();
}

void CDUIThinkEditCtrl::SetAutoSelAll(bool bAutoSelAll)
{
	if (bAutoSelAll == IsAutoSelAll()) return;

	m_AttributeAutoSelAll.SetValue(bAutoSelAll);

	return;
}

bool CDUIThinkEditCtrl::IsWantReturn()
{
	return m_AttributeWantReturn.GetValue();
}

void CDUIThinkEditCtrl::SetWantReturn(bool bWantReturn)
{
	if (bWantReturn == IsWantReturn()) return;

	m_AttributeWantReturn.SetValue(bWantReturn);

	return;
}

UINT CDUIThinkEditCtrl::GetMaxChar()
{
	return m_AttributeMaxChar.GetValue();
}

void CDUIThinkEditCtrl::SetMaxChar(UINT uMax)
{
	if (uMax == GetMaxChar()) return;

	m_AttributeMaxChar.SetValue(uMax);

	return;
}

int CDUIThinkEditCtrl::GetNumberMinLimit()
{
	return m_AttributeNumberMinLimit.GetValue();
}

void CDUIThinkEditCtrl::SetNumberMinLimit(int nMinNum)
{
	if (nMinNum == GetNumberMinLimit()) return;

	m_AttributeNumberMinLimit.SetValue(nMinNum);

	return;
}

int CDUIThinkEditCtrl::GetNumberMaxLimit()
{
	return m_AttributeNumberMaxLimit.GetValue();
}

void CDUIThinkEditCtrl::SetNumberMaxLimit(int nMaxNum)
{
	if (nMaxNum == GetNumberMaxLimit()) return;

	m_AttributeNumberMaxLimit.SetValue(nMaxNum);

	return;
}

TCHAR CDUIThinkEditCtrl::GetPasswordChar()
{
	return *m_AttributePasswordChar.GetValue();
}

void CDUIThinkEditCtrl::SetPasswordChar(TCHAR chPasswordChar)
{
	if (chPasswordChar == GetPasswordChar()) return;

	m_AttributePasswordChar.SetValue(CMMString(chPasswordChar));

	Invalidate();

	return;
}

CMMString CDUIThinkEditCtrl::GetTipText()
{
	return m_AttributeTipText.GetValue();
}

void CDUIThinkEditCtrl::SetTipText(LPCTSTR lpszTipText)
{
	if (0 == lstrcmp(lpszTipText, m_AttributeTipText.GetValue())) return;

	m_AttributeTipText.SetValue(lpszTipText);

	Invalidate();

	return;
}

tagDuiTextStyle CDUIThinkEditCtrl::GetTextStyleTipTextNormal()
{
	return m_AttributeTextStyleTipTextNormal.GetTextStyle();
}

ARGB CDUIThinkEditCtrl::GetTextColorTipTextNormal()
{
	return m_AttributeTextStyleTipTextNormal.GetTextColor();
}

void CDUIThinkEditCtrl::SetTextStyleTipTextNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleTipTextNormal.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

CDUIRect CDUIThinkEditCtrl::GetLineRange(int nLine)
{
	return CDUIRenderHelp::GetLineRange(m_mapLineVecRichTextDraw, nLine);
}

CDUIRect CDUIThinkEditCtrl::GetCaretPos()
{
	//adjust
	m_nCaretRow = min(m_nCaretRow, (int)m_mapLineVecRichTextDraw.size() - 1);
	if (m_nCaretRow < 0 || m_nCaretRow >= m_mapLineVecRichTextDraw.size())
	{
		m_nCaretRow = -1;
		m_nCaretColumn = -1;
	}
	else if (m_nCaretColumn < 0 || m_nCaretColumn >= m_mapLineVecRichTextDraw[m_nCaretRow].size())
	{
		m_nCaretColumn = -1;
	}

	m_nCaretSelFromRow = min(m_nCaretSelFromRow, (int)m_mapLineVecRichTextDraw.size() - 1);
	if (m_nCaretSelFromRow < 0 || m_nCaretSelFromRow >= m_mapLineVecRichTextDraw.size())
	{
		m_nCaretSelFromRow = -1;
		m_nCaretSelFromColumn = -1;
	}
	else if (m_nCaretSelFromColumn < 0 || m_nCaretSelFromColumn >= m_mapLineVecRichTextDraw[m_nCaretSelFromRow].size())
	{
		m_nCaretSelFromColumn = -1;
	}

	//pre measure
	tagDuiTextStyle TextStyle = GetTextStyleActive();
	CDUIRect rcMeasure;
	PerformMeasureString(_T("啊"), TextStyle, MapLineVecDuiRichTextDraw(), rcMeasure);

	//none
	if (m_nCaretRow < 0 || m_nCaretRow >= m_mapLineVecRichTextDraw.size())
	{
		CDUIRect rcRange = GetTextRange();
		rcRange.right = rcRange.left + 1;
		rcRange.bottom = rcRange.top + rcMeasure.GetHeight();

		return rcRange;
	}

	//caret pos
	VecDuiRichTextDraw LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_nCaretRow];
	CDUIRect rcCaret = LineVecRichTextDraw[0].rcDraw;
	rcCaret.right = rcCaret.left + 1;
	rcCaret.top = rcCaret.bottom - rcMeasure.GetHeight();
	if (m_nCaretColumn < 0)
	{
		CDUIRect rcText = LineVecRichTextDraw[0].rcDraw;
		rcCaret.Offset(rcText.left - rcCaret.left, rcText.bottom - rcCaret.GetHeight() - rcCaret.top);
	}
	else
	{
		CDUIRect rcText = LineVecRichTextDraw[m_nCaretColumn].rcDraw;
		rcCaret.Offset(rcText.right - rcCaret.left, rcText.bottom - rcCaret.GetHeight() - rcCaret.top);
	}

	//scroll
	CDUIRect rcRange = GetTextRange();
	CDUISize szScrollPos = GetScrollPos();
	if (rcCaret.left < rcRange.left && m_szScrollRange.cx > 0)
	{
		szScrollPos.cx -= (rcRange.left - rcCaret.left);
		rcCaret.Offset(rcRange.left - rcCaret.left, 0);
	}
	if (rcCaret.left > rcRange.right && m_szScrollRange.cx > 0)
	{
		szScrollPos.cx += (rcCaret.left - rcRange.right);
		rcCaret.Offset(-(rcCaret.left - rcRange.right), 0);
	}
	if (rcCaret.top < rcRange.top && m_szScrollRange.cy > 0)
	{
		szScrollPos.cy -= (rcRange.top - rcCaret.top);
		rcCaret.Offset(0, rcRange.top - rcCaret.top);
	}
	if (rcCaret.bottom > rcRange.bottom && m_szScrollRange.cy > 0)
	{
		szScrollPos.cy += (rcCaret.bottom - rcRange.bottom);
		rcCaret.Offset(0, -(rcCaret.bottom - rcRange.bottom));
	}

	SetScrollPos(szScrollPos);

	return rcCaret;
}

void CDUIThinkEditCtrl::SetCaretPos(int nRow, int nColumn)
{
	m_nCaretRow = nRow;
	m_nCaretColumn = nColumn;
	m_nCaretSelFromRow = nRow;
	m_nCaretSelFromColumn = nColumn;

	PerformAdjustCaret();

	return;
}

TCHAR CDUIThinkEditCtrl::GetCaretChar(int nRow, int nColumn)
{
	if (nRow < 0 || nRow >= m_mapLineVecRichTextDraw.size()) return 0;

	auto LineVecRichTextDraw = m_mapLineVecRichTextDraw[nRow];
	if (nColumn < 0 || nColumn >= LineVecRichTextDraw.size()) return 0;

	return LineVecRichTextDraw[nColumn].strText[0];
}

TCHAR CDUIThinkEditCtrl::GetCaretCharCur()
{
	return GetCaretChar(m_nCaretRow, m_nCaretColumn);
}

TCHAR CDUIThinkEditCtrl::GetCaretCharNext()
{
	TCHAR chChar = 0;

	int nRow = m_nCaretRow;

	do
	{
		if (nRow < 0 || nRow >= m_mapLineVecRichTextDraw.size()) break;

		auto LineVecRichTextDraw = m_mapLineVecRichTextDraw[nRow];
		if (nRow == m_nCaretRow && m_nCaretColumn >= (int)LineVecRichTextDraw.size() - 1)
		{
			nRow++;
			continue;
		}

		int nColumn = nRow == m_nCaretRow ? m_nCaretColumn + 1 : 0;
		chChar = LineVecRichTextDraw[nColumn].strText[0];

		break;

	} while (true);


	return chChar;
}

CDUISize CDUIThinkEditCtrl::GetScrollPos()
{
	return m_szScrollPos;
}

void CDUIThinkEditCtrl::SetScrollPos(CDUISize szPos)
{
	CDUISize szScrollPosOld = GetScrollPos();

	m_szScrollPos = szPos;
	m_szScrollPos.cx = max(m_szScrollPos.cx, 0);
	m_szScrollPos.cx = min(m_szScrollPos.cx, m_szScrollRange.cx);
	m_szScrollPos.cy = max(m_szScrollPos.cy, 0);
	m_szScrollPos.cy = min(m_szScrollPos.cy, m_szScrollRange.cy);
	if (szScrollPosOld == m_szScrollPos) return;

	//scroll
	for (int nLine = 0; nLine < m_mapLineVecRichTextDraw.size(); nLine++)
	{
		auto &LineVecRichTextDraw = m_mapLineVecRichTextDraw[nLine];
		for (int n = 0; n < LineVecRichTextDraw.size(); n++)
		{
			tagDuiRichTextDraw &RichTextDrawItem = LineVecRichTextDraw[n];
			RichTextDrawItem.rcDraw.Offset(-(m_szScrollPos.cx - szScrollPosOld.cx), -(m_szScrollPos.cy - szScrollPosOld.cy));
		}
	}

	Invalidate();

	return;
}

CDUISize CDUIThinkEditCtrl::GetScrollRange()
{
	return m_szScrollRange;
}

CMMString CDUIThinkEditCtrl::GetSelectString()
{
	if (NULL == m_pWndManager || m_mapLineVecRichTextDraw.empty()) return _T("");

	//from target
	int nRowFrom = 0;
	int	nRowTarget = 0;
	int	nColumnFrom = 0;
	int nColumnTarget = 0;
	PerformCalcSelect(nRowFrom, nColumnFrom, nRowTarget, nColumnTarget);

	if (nRowFrom < 0 || nRowFrom >= m_mapLineVecRichTextDraw.size()) return _T("");

	//string
	CMMString strText;
	for (int nRow = nRowFrom; nRow <= nRowTarget; nRow++)
	{
		VecDuiRichTextDraw LineVecRichTextDraw = m_mapLineVecRichTextDraw[nRow];

		int nColumnPre = 0;
		int nColumnNext = (int)LineVecRichTextDraw.size();

		if (nRow == nRowFrom)
		{
			nColumnPre = nColumnFrom + 1;
		}
		if (nRow == nRowTarget)
		{
			nColumnNext = nColumnTarget + 1;
		}

		nColumnPre = min(nColumnPre, (int)LineVecRichTextDraw.size());
		nColumnNext = min(nColumnNext, (int)LineVecRichTextDraw.size());
		nColumnNext = max(nColumnNext, nColumnPre);

		for (int nColumn = nColumnPre; nColumn < nColumnNext && nColumn < LineVecRichTextDraw.size(); nColumn++)
		{
			strText += LineVecRichTextDraw[nColumn].strText;
		}
	}

	return strText;
}

CDUIRect CDUIThinkEditCtrl::GetSelectRange()
{
	if (NULL == m_pWndManager || m_mapLineVecRichTextDraw.empty()) return {};

	//from target
	int nRowFrom = 0;
	int	nRowTarget = 0;
	int	nColumnFrom = 0;
	int nColumnTarget = 0;
	PerformCalcSelect(nRowFrom, nColumnFrom, nRowTarget, nColumnTarget);

	if (nRowFrom < 0 || nRowFrom >= m_mapLineVecRichTextDraw.size()) return {};

	//range
	CDUIRect rcRange;
	for (int nRow = nRowFrom; nRow <= nRowTarget; nRow++)
	{
		VecDuiRichTextDraw LineVecRichTextDraw = m_mapLineVecRichTextDraw[nRow];

		int nColumnPre = 0;
		int nColumnNext = (int)LineVecRichTextDraw.size();

		if (nRow == nRowFrom)
		{
			nColumnPre = nColumnFrom + 1;
		}
		if (nRow == nRowTarget)
		{
			nColumnNext = nColumnTarget + 1;
		}

		nColumnPre = min(nColumnPre, (int)LineVecRichTextDraw.size());
		nColumnNext = min(nColumnNext, (int)LineVecRichTextDraw.size());
		nColumnNext = max(nColumnNext, nColumnPre);

		for (int nColumn = nColumnPre; nColumn < nColumnNext && nColumn < LineVecRichTextDraw.size(); nColumn++)
		{
			if (rcRange.IsEmpty()) rcRange = LineVecRichTextDraw[nColumn].rcDraw;

			rcRange.left = min(rcRange.left, LineVecRichTextDraw[nColumn].rcDraw.left);
			rcRange.top = min(rcRange.top, LineVecRichTextDraw[nColumn].rcDraw.top);
			rcRange.right = max(rcRange.right, LineVecRichTextDraw[nColumn].rcDraw.right);
			rcRange.bottom = max(rcRange.bottom, LineVecRichTextDraw[nColumn].rcDraw.bottom);
		}
	}

	return rcRange;
}

void CDUIThinkEditCtrl::SetSel(long nStartChar, long nEndChar)
{
	if (m_mapLineVecRichTextDraw.size() <= 0) return;
	if (-1 == nStartChar && -1 == nEndChar) return SetSelAll();

	nStartChar = max(0, nStartChar);
	nEndChar = max(0, nEndChar);
	if (nStartChar > nEndChar) return;

	m_nCaretRow = -1;
	m_nCaretColumn = -1;
	m_nCaretSelFromRow = -1;
	m_nCaretSelFromColumn = -1;

	for (int nLine = 0; nLine < m_mapLineVecRichTextDraw.size(); nLine++)
	{
		auto LineVecRichTextDraw = m_mapLineVecRichTextDraw[nLine];

		if (LineVecRichTextDraw.size() > nStartChar
			&& -1 == m_nCaretRow)
		{
			m_nCaretRow = nLine;
			m_nCaretColumn = 0 == nStartChar ? -1 : nStartChar - 1;
		}
		if (LineVecRichTextDraw.size() > nEndChar
			&& -1 == m_nCaretSelFromRow)
		{
			m_nCaretSelFromRow = nLine;
			m_nCaretSelFromColumn = 0 == nEndChar ? -1 : nEndChar - 1;

			break;
		}

		nStartChar -= LineVecRichTextDraw.size();
		nEndChar -= LineVecRichTextDraw.size();
	}

	Invalidate();

	return;
}

void CDUIThinkEditCtrl::SetSelAll()
{
	if (m_mapLineVecRichTextDraw.size() <= 0) return;

	m_nCaretRow = 0;
	m_nCaretColumn = -1;
	m_nCaretSelFromRow = m_mapLineVecRichTextDraw.size() - 1;
	m_nCaretSelFromColumn = m_mapLineVecRichTextDraw[m_nCaretSelFromRow].size() - 1;

	PerformAdjustCaret();

	Invalidate();

	return;
}

void CDUIThinkEditCtrl::SetReplaceSel(LPCTSTR lpszReplace, CMMString strImageResName, bool bHistory)
{
	//clear
	PerformClearSelect();

	//history
	tagDuiThinkEditHistory History = {};
	History.bAdd = true;
	History.ItemType = strImageResName.IsEmpty() ? RichTextItem_Text : RichTextItem_Image;
	History.nCaretRowPre = m_nCaretRow;
	History.nCaretColumnPre = m_nCaretColumn;
	History.strText = lpszReplace;
	History.strImageResName = strImageResName;

	//cur line
	tagDuiTextStyle TextStyle = GetTextStyle();
	do
	{
		if (m_nCaretRow >= 0 && m_nCaretRow < m_mapLineVecRichTextDraw.size())
		{
			auto &LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_nCaretRow];
			if (m_nCaretColumn < 0 || m_nCaretColumn >= LineVecRichTextDraw.size())
			{
				LineVecRichTextDraw.insert(LineVecRichTextDraw.begin(), tagDuiRichTextDraw());
				LineVecRichTextDraw.front().ItemType = History.ItemType;
				LineVecRichTextDraw.front().strText = lpszReplace;
				LineVecRichTextDraw.front().strImageResName = strImageResName;
				LineVecRichTextDraw.front().vecFontResSwitch = TextStyle.vecFontResSwitch;
				LineVecRichTextDraw.front().vecColorResSwitch = TextStyle.vecColorResSwitch;

				break;
			}

			tagDuiRichTextDraw RichTextDrawItem;
			RichTextDrawItem.ItemType = History.ItemType;
			RichTextDrawItem.strText = lpszReplace;
			RichTextDrawItem.strImageResName = strImageResName;
			RichTextDrawItem.vecFontResSwitch = TextStyle.vecFontResSwitch;
			RichTextDrawItem.vecColorResSwitch = TextStyle.vecColorResSwitch;
			LineVecRichTextDraw.insert(LineVecRichTextDraw.begin() + m_nCaretColumn + 1, RichTextDrawItem);

			break;
		}

		tagDuiRichTextDraw RichTextDrawItem;
		RichTextDrawItem.ItemType = History.ItemType;
		RichTextDrawItem.strText = lpszReplace;
		RichTextDrawItem.strImageResName = strImageResName;
		RichTextDrawItem.vecFontResSwitch = TextStyle.vecFontResSwitch;
		RichTextDrawItem.vecColorResSwitch = TextStyle.vecColorResSwitch;
		m_mapLineVecRichTextDraw[0].push_back(RichTextDrawItem);

	} while (false);

	//text
	VecDuiRichTextItem vecRichTextItem;
	for (auto LineVecRichTextDraw : m_mapLineVecRichTextDraw)
	{
		for (auto RichTextDrawItem : LineVecRichTextDraw.second)
		{
			vecRichTextItem.push_back(RichTextDrawItem);
		}
	}

	SetRichTextItem(vecRichTextItem);

	//adjust
	RefreshView();

	//caret
	if (RichTextItem_Text == History.ItemType)
	{
		int nLen = lstrlen(lpszReplace);
		for (int n = 0; n < nLen; n++)
		{
			PerformMoveCaretHoriz(false, false, false);

			int nEmoji = CDUIGlobal::IsEmoji(lpszReplace[n]);
			nEmoji > 0 ? n += nEmoji - 1 : 0;	
		}
	}
	else if (RichTextItem_Image == History.ItemType)
	{
		PerformMoveCaretHoriz(false, false, false);
	}

	//history
	History.nCaretRowNext = m_nCaretRow;
	History.nCaretColumnNext = m_nCaretColumn;

	if (bHistory)
	{
		PerformAddHistory(History);
	}

	Invalidate();

	return;
}

bool CDUIThinkEditCtrl::CanUndo()
{
	return 0 <= m_nIndexHistory && m_nIndexHistory < m_queHistory.size();
}

bool CDUIThinkEditCtrl::CanRedo()
{
	return m_nIndexHistory < (int)m_queHistory.size() - 1;
}

void CDUIThinkEditCtrl::Undo()
{
	if (false == CanUndo()) return;

	auto History = m_queHistory[m_nIndexHistory--];

	PerformActiveHistory(History, true);

	return;
}

void CDUIThinkEditCtrl::Redo()
{
	if (false == CanRedo()) return;

	auto History = m_queHistory[++m_nIndexHistory];

	PerformActiveHistory(History, false);

	return;
}

void CDUIThinkEditCtrl::Clear()
{
	PerformClearSelect();

	return;
}

void CDUIThinkEditCtrl::Copy()
{
	CMMService::SetClipboardString(NULL, GetSelectString());

	return;
}

void CDUIThinkEditCtrl::Cut()
{
	Copy();

	PerformClearSelect();

	return;
}

void CDUIThinkEditCtrl::Paste()
{
	auto vecFile = CMMService::GetClipboardFile();

	//text
	if (vecFile.empty())
	{
		SetReplaceSel(CMMService::GetClipboardString());

		return;
	}

	//image
	for (auto strFile : vecFile)
	{
		CDUIImageBase *pImageBase = CDUIGlobal::GetInstance()->GetImageResourceByFile(strFile);
		if (pImageBase)
		{
			SetReplaceSel(_T(""), pImageBase->GetResourceName());

			continue;
		}

		pImageBase = new CDUIImageBase(CMMService::ProductGUID(), strFile);
		if (false == CDUIGlobal::GetInstance()->AddResource(pImageBase))
		{
			MMSafeDelete(pImageBase);

			continue;
		}

		SetReplaceSel(_T(""), pImageBase->GetResourceName());
	}

	return;
}

bool CDUIThinkEditCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDown(pt, Msg)) return false;
	if (m_bMouseDownFocus) return true;

	PerformHitText(pt, true);

	return true;
}

bool CDUIThinkEditCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonUp(pt, Msg)) return false;

	m_bMouseDownFocus = false;

	return true;
}

bool CDUIThinkEditCtrl::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiLButtonDlk(pt, Msg)) return false;

	SetSelAll();

	return true;
}

bool CDUIThinkEditCtrl::OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonDown(pt, Msg)) return false;
	if (m_bMouseDownFocus) return true;

	//hit select
	CDUIRect rcSelRange = GetSelectRange();
	if (false == rcSelRange.PtInRect(pt))
	{
		PerformHitText(pt, true);
	}

	return true;
}

bool CDUIThinkEditCtrl::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiRButtonUp(pt, Msg)) return false;

	m_bMouseDownFocus = false;

	return true;
}

bool CDUIThinkEditCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == __super::OnDuiMouseMove(pt, Msg)) return false;

	if (MK_LBUTTON & Msg.wParam)
	{
		PerformHitText(pt, false);

		Invalidate();
	}

	return true;
}

bool CDUIThinkEditCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (GetScrollRange().cy <= 0) return __super::OnDuiMouseWheel(pt, Msg);

	switch ((int)(short)HIWORD(Msg.wParam))
	{
		case WHEEL_DELTA:
		{
			CDUISize szScrollPos = GetScrollPos();
			szScrollPos.cy -= GetLineRange(0).GetHeight();
			SetScrollPos(szScrollPos);

			break;
		}
		case -WHEEL_DELTA:
		{
			CDUISize szScrollPos = GetScrollPos();
			szScrollPos.cy += GetLineRange(0).GetHeight();
			SetScrollPos(szScrollPos);

			break;
		}
	}

	__super::OnDuiMouseWheel(pt, Msg);

	return true;
}

bool CDUIThinkEditCtrl::OnDuiSetFocus()
{
	if (NULL == m_pWndManager) return false;
	if (false == __super::OnDuiSetFocus()) return false;

	//pre edit
	g_vecDuiRichTextPreEdit = GetRichTextItem();

	//active
	HWND hWndForground = GetForegroundWindow();
	DWORD dwProIDForground = ::GetWindowThreadProcessId(hWndForground, NULL);
	DWORD dwProIDCur = ::GetCurrentThreadId();

	::AttachThreadInput(dwProIDCur, dwProIDForground, TRUE);
	::SetActiveWindow(m_pWndManager->GetWndHandle());
	::BringWindowToTop(m_pWndManager->GetWndHandle());
	::SetForegroundWindow(m_pWndManager->GetWndHandle());
	::AttachThreadInput(dwProIDCur, dwProIDForground, FALSE);

	//edit
	RefreshView();

	if (IsAutoSelAll() && false == m_mapLineVecRichTextDraw.empty())
	{
		SetSelAll();

		m_bMouseDownFocus = (CDUIWndManager::MapKeyState() & MK_LBUTTON)
			|| (CDUIWndManager::MapKeyState() & MK_RBUTTON);
	}

	return true;
}

bool CDUIThinkEditCtrl::OnDuiKillFocus()
{
	if (false == __super::OnDuiKillFocus()) return false;
	if (NULL == m_pWndManager) return false;

	KillTimer(Dui_TimerCaret_ID);
	m_pWndManager->ShowCaret(false);
	m_bShowCaret = false;
	m_szScrollPos = {};

	do
	{
		//number
		if (EditText_NumberDouble == GetEditTextType())
		{
			double lfNum = _tcstod(GetText(), NULL);
			CMMString strText = CMMStrHelp::Format(_T("%0.6lf"), lfNum);
			SetText(strText);

			break;
		}
		if (EditText_NumberInt == GetEditTextType())
		{
			int nNum = _tcstol(GetText(), NULL, 10);
			CMMString strText = CMMStrHelp::Format(_T("%d"), nNum);
			SetText(strText);

			break;
		}
	} while (false);

	//notify
	if (g_vecDuiRichTextPreEdit != GetRichTextItem()
		&& false == IsReadOnly())
	{
		m_pWndManager->SendNotify(this, DuiNotify_Edited);
	}

	return true;
}

void CDUIThinkEditCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (NULL == m_pWndManager) return;

	m_pWndManager->AddPreMessagePtr(this);

	return;
}

void CDUIThinkEditCtrl::OnDuiWndManagerDetach()
{
	__super::OnDuiWndManagerDetach();

	if (m_pWndManager)
	{
		m_pWndManager->RemovePreMessagePtr(this);
	}

	return;
}

void CDUIThinkEditCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	if (Dui_TimerAnimate_ID == uTimerID)
	{
		for (auto &LineVecRichTextDraw : m_mapLineVecRichTextDraw)
		{
			for (auto &RichTextDrawItem : LineVecRichTextDraw.second)
			{
				if (RichTextItem_Image != RichTextDrawItem.ItemType) continue;

				CDUIImageBase *pImageBase = CDUIGlobal::GetInstance()->GetImageResource(RichTextDrawItem.strImageResName);
				if (NULL == pImageBase || NULL == pImageBase->GetAnimateImage()) continue;

				auto AnimateImageInfo = pImageBase->GetAnimateImageInfo();
				if (AnimateImageInfo.nFrameCount <= 0) continue;

				RichTextDrawItem.nAnimateElapseCur -= Dui_TimerThinkEditAnimate_Elapse;
				if (RichTextDrawItem.nAnimateElapseCur <= 0)
				{
					RichTextDrawItem.nAnimateFrameCur++;
					RichTextDrawItem.nAnimateFrameCur %= AnimateImageInfo.nFrameCount;
					RichTextDrawItem.nAnimateElapseCur = ((long*)AnimateImageInfo.vecGifPropertyItem[0].value)[RichTextDrawItem.nAnimateFrameCur] * 10;
				}
			}
		}

		Invalidate();

		return;
	}
	if (Dui_TimerCaret_ID == uTimerID)
	{
		m_bShowCaret = !m_bShowCaret;

		Invalidate();

		return;
	}

	return __super::OnDuiTimer(uTimerID, Msg);
}

LRESULT CDUIThinkEditCtrl::OnDuiKeyDown(const DuiMessage &Msg)
{
	__super::OnDuiKeyDown(Msg);

	switch (Msg.chKey)
	{
		case VK_LEFT:
		{
			PerformMoveCaretHoriz(true, CDUIWndManager::MapKeyState() & MK_SHIFT);

			break;
		}
		case VK_RIGHT:
		{
			PerformMoveCaretHoriz(false, CDUIWndManager::MapKeyState() & MK_SHIFT);

			break;
		}
		case VK_UP:
		{
			PerformMoveCaretVert(true);

			break;
		}
		case VK_DOWN:
		{
			PerformMoveCaretVert(false);

			break;
		}
		case VK_BACK:
		{
			if (m_nCaretRow == m_nCaretSelFromRow
				&& m_nCaretColumn == m_nCaretSelFromColumn
				&& false == IsReadOnly())
			{
				int nRowPre = m_nCaretRow;
				int nColumnPre = m_nCaretColumn;

				PerformMoveCaretHoriz(true);

				m_nCaretSelFromRow = nRowPre;
				m_nCaretSelFromColumn = nColumnPre;
			}

			PerformClearSelect();

			break;
		}
		case 'A':
		case 'a':
		{
			if (CDUIWndManager::MapKeyState() & MK_CONTROL)
			{
				SetSelAll();
			}

			break;
		}
		case 'Z':
		case 'z':
		{
			if (CDUIWndManager::MapKeyState() & MK_CONTROL)
			{
				Undo();
			}

			break;
		}
		case 'Y':
		case 'y':
		{
			if (CDUIWndManager::MapKeyState() & MK_CONTROL)
			{
				Redo();
			}

			break;
		}
		case 'C':
		case 'c':
		{
			if (CDUIWndManager::MapKeyState() & MK_CONTROL)
			{
				Copy();
			}

			break;
		}
		case 'V':
		case 'v':
		{
			if (CDUIWndManager::MapKeyState() & MK_CONTROL)
			{
				Paste();
			}

			break;
		}
		case VK_INSERT:
		{
			auto vecFile = CMMService::GetClipboardFile();
			if (vecFile.size() > 0)
			{
				Paste();
			}

			break;
		}
		case VK_RETURN:
		{
			tagDuiTextStyle TextStyle = GetTextStyleActive();
			if (false == TextStyle.IsWordBreak() || false == IsWantReturn())
			{
				KillFocus();
			}

			break;
		}
	}

	Invalidate();

	return 0;
}

LRESULT CDUIThinkEditCtrl::OnDuiChar(const DuiMessage &Msg)
{
	__super::OnDuiChar(Msg);

	//emtpy
	if ((m_nCaretRow < 0 && false == m_mapLineVecRichTextDraw.empty())
		|| m_nCaretRow >= (int)m_mapLineVecRichTextDraw.size()
		|| VK_BACK == Msg.chKey
		|| (MK_CONTROL & Msg.wKeyState)
		|| IsReadOnly()) return 0;

	//number
	if (IsNumberOnly()
		&& _T('.') != Msg.wParam
		&& _T('-') != Msg.wParam
		&& _T('%') != Msg.wParam
		&& _T('\r') != Msg.wParam
		&& (Msg.wParam < _T('0') || Msg.wParam > _T('9'))) return 0;

	//maxchar
	if (m_vecRichTextItem.size() >= GetMaxChar()) return 0;

	static TCHAR chEmoji = 0;
	CMMString strReplace = CMMString((TCHAR)Msg.wParam);
	if (0 != chEmoji)
	{
		strReplace = chEmoji + strReplace;

		chEmoji = 0;
	}
	else if (CDUIGlobal::IsEmoji(Msg.wParam) > 1)
	{
		chEmoji = (TCHAR)Msg.wParam;

		return 0;
	}

	//add
	SetReplaceSel(strReplace);

	//number limit
	if (IsNumberOnly())
	{
		int nNumSrc = _ttol(GetText());
		int nNumAdjust = min(max(nNumSrc, GetNumberMinLimit()), GetNumberMaxLimit());
		if (nNumSrc != nNumAdjust)
		{
			SetText(CMMStrHelp::Format(_T("%d"), nNumAdjust));
		}
	}

	return 0;
}

LRESULT CDUIThinkEditCtrl::OnDuiContextMenu(const DuiMessage &Msg)
{
	if (NULL == m_pWndManager) return 0;

	__super::OnDuiContextMenu(Msg);

	//menu
	HMENU hPopMenu = CreatePopupMenu();
	AppendMenu(hPopMenu, 0, ID_MENU_UNDO, _T("撤销(&U)"));
	AppendMenu(hPopMenu, 0, ID_MENU_REDO, _T("重做(&R)"));
	AppendMenu(hPopMenu, MF_SEPARATOR, 0, _T(""));
	AppendMenu(hPopMenu, 0, ID_MENU_CUT, _T("剪切(&X)"));
	AppendMenu(hPopMenu, 0, ID_MENU_COPY, _T("复制(&C)"));
	AppendMenu(hPopMenu, 0, ID_MENU_PASTE, _T("粘帖(&V)"));
	AppendMenu(hPopMenu, 0, ID_MENU_CLEAR, _T("清空(&L)"));
	AppendMenu(hPopMenu, MF_SEPARATOR, 0, _T(""));
	AppendMenu(hPopMenu, 0, ID_MENU_SELECTALL, _T("全选(&A)"));

	//enable
	UINT uUndo = (CanUndo() ? 0 : MF_GRAYED);
	::EnableMenuItem(hPopMenu, ID_MENU_UNDO, MF_BYCOMMAND | uUndo);
	UINT uRedo = (CanRedo() ? 0 : MF_GRAYED);
	EnableMenuItem(hPopMenu, ID_MENU_REDO, MF_BYCOMMAND | uRedo);
	UINT uSel = (GetSelectString().IsEmpty()) ? MF_GRAYED : 0;
	UINT uReadonly = IsReadOnly() ? MF_GRAYED : 0;
	EnableMenuItem(hPopMenu, ID_MENU_CUT, MF_BYCOMMAND | uSel | uReadonly);
	EnableMenuItem(hPopMenu, ID_MENU_COPY, MF_BYCOMMAND | uSel);
	EnableMenuItem(hPopMenu, ID_MENU_CLEAR, MF_BYCOMMAND | uSel | uReadonly);
	EnableMenuItem(hPopMenu, ID_MENU_PASTE, MF_BYCOMMAND | uReadonly);

	CDUIPoint ptScreen = Msg.ptMouse;
	::ClientToScreen(m_pWndManager->GetWndHandle(), &ptScreen);
	TrackPopupMenu(hPopMenu, TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, 0, m_pWndManager->GetWndHandle(), NULL);
	DestroyMenu(hPopMenu);

	return 0;
}

LRESULT CDUIThinkEditCtrl::OnDuiCommand(const DuiMessage &Msg)
{
	__super::OnDuiCommand(Msg);

	UINT uCmd = (UINT)Msg.wParam;
	switch (uCmd)
	{
		case ID_MENU_UNDO:
		{
			Undo();

			break;
		}
		case ID_MENU_REDO:
		{
			Redo();

			break;
		}
		case ID_MENU_CUT:
		{
			Cut();

			break;
		}
		case ID_MENU_COPY:
		{
			Copy();

			break;
		}
		case ID_MENU_PASTE:
		{
			Paste();

			break;
		}
		case ID_MENU_CLEAR:
		{
			Clear();

			break;
		}
		case ID_MENU_SELECTALL:
		{
			SetSelAll();

			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

LRESULT CDUIThinkEditCtrl::OnDuiImeComPosition(const DuiMessage &Msg)
{
	if (NULL == m_pWndManager) return 0;

	__super::OnDuiImeComPosition(Msg);

	//emoji
	CMMString strBuff;
	HIMC hImc = ::ImmGetContext(m_pWndManager->GetWndHandle());
	if (hImc)
	{
		int nLen = ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, NULL);
		strBuff = CMMString(_T('\0'), nLen + 1);
		ImmGetCompositionString(hImc, GCS_COMPSTR, strBuff.GetBuffer(), strBuff.GetLength());
		
		for (int n = 0; n < strBuff.GetLength(); n++)
		{
			if (CDUIGlobal::IsEmoji(strBuff[n]) > 0)
			{
				SetReplaceSel(strBuff);

				ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, NULL);
			}
		}

		ImmReleaseContext(m_pWndManager->GetWndHandle(), hImc);
	}

	return 0;
}

void CDUIThinkEditCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupEdit, _T("Edit"));
	DuiCreateAttribute(m_AttributeColorSelect, _T("ColorSelect"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeImageSelect, _T("ImageSelect"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeEditTextType, _T("EditTextType"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeRichEdit, _T("RichEdit"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeReadOnly, _T("ReadOnly"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributePasswordMode, _T("PasswordMode"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeAutoSelAll, _T("AutoSelAll"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeWantReturn, _T("WantReturn"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeMaxChar, _T("MaxChar"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeNumberMinLimit, _T("NumberMinLimit"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeNumberMaxLimit, _T("NumberMaxLimit"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributePasswordChar, _T("PasswordChar"), _T(""), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeTipText, _T("TipText"), _T("Tip Info For Empty Edit"), m_AttributeGroupEdit);
	DuiCreateAttribute(m_AttributeTextStyleTipTextNormal, _T("TextStyleTipTextNormal"), _T(""), m_AttributeGroupEdit);

	return;
}

void CDUIThinkEditCtrl::InitAttriValue()
{
	__super::InitAttriValue();

	DuiInitAttriValue(m_AttributeContextMenu, true);
	DuiInitAttriColorRes(m_AttributeColorSelect, Name_ColorSelBk);
	DuiInitAttriValue(m_AttributeMaxChar, 255);
	DuiInitAttriValue(m_AttributeNumberMinLimit, 0x80000000);
	DuiInitAttriValue(m_AttributeNumberMaxLimit, 0x7fffffff);
	DuiInitAttriValue(m_AttributePasswordChar, _T("*"));

	if (false == m_AttributeEditTextType.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ EditText_None, _T("None") });
		AttriCombox.vecItem.push_back({ EditText_Number, _T("Number") });
		AttriCombox.vecItem.push_back({ EditText_NumberInt, _T("NumberInt") });
		AttriCombox.vecItem.push_back({ EditText_NumberDouble, _T("NumberDouble") });
		m_AttributeEditTextType.SetCombox(AttriCombox);
		m_AttributeEditTextType.SelectItem(EditText_None);
	}
	if (false == m_AttributeCursor.IsModified())
	{
		m_AttributeCursor.SelectItem(Cursor_IBeam);
	}

	return;
}

void CDUIThinkEditCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeContextMenu, false);

	//richtext
	tagDuiTextStyle TextStyle = GetTextStyle();
	CMMString strText = GetText();
	m_vecRichTextItem.clear();
	for (int n = 0; n < strText.GetLength(); n++)
	{
		m_vecRichTextItem.push_back(tagDuiRichTextItem());
		m_vecRichTextItem.back().strText = strText[n];
		m_vecRichTextItem.back().vecFontResSwitch = TextStyle.vecFontResSwitch;
		m_vecRichTextItem.back().vecColorResSwitch = TextStyle.vecColorResSwitch;
	}

	return;
}

void CDUIThinkEditCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pWndManager) return;

	//normal
	CDUIRect rcRange = GetTextRange();
	CMMString strTipText = GetTipText();

	//clip
	CDUIRect rcPaint;
	IntersectRect(&rcPaint, &m_rcPaint, &rcRange);
	CDUIRenderClip Clip;
	CDUIRenderClip::GenerateClip(hDC, rcPaint, Clip);

	//textstyle
	CDUIAttributeTextStyle *pAttribute = NULL;
	if (m_vecRichTextItem.empty())
	{
		pAttribute = &m_AttributeTextStyleTipTextNormal;
	}
	else if (false == IsEnabled())
	{
		pAttribute = &m_AttributeTextStyleDisabled;
	}
	else if (m_nControlStatus & ControlStatus_Pushed)
	{
		pAttribute = &m_AttributeTextStylePushed;
	}
	else if (m_nControlStatus & ControlStatus_Hot)
	{
		pAttribute = &m_AttributeTextStyleHot;
	}
	else if (IsFocused())
	{
		pAttribute = &m_AttributeTextStyleFocus;
	}
	else
	{
		pAttribute = &m_AttributeTextStyleNormal;
	}

	NULL == pAttribute || pAttribute->IsEmpty() ? pAttribute = &m_AttributeTextStyleNormal : pAttribute;
	NULL == pAttribute || pAttribute->IsEmpty() ? pAttribute = &m_AttributeTextStyle : pAttribute;
	if (NULL == pAttribute) return;

	tagDuiTextStyle TextStyle = pAttribute->GetTextStyle();
	tagDuiRichText RichTextInfo;
	RichTextInfo.dwTextStyle = TextStyle.dwTextStyle;

	//tiptext
	if (m_vecRichTextItem.empty() && false == strTipText.IsEmpty())
	{
		RichTextInfo.vecRichTextItem.clear();
		for (int n = 0; n < strTipText.GetLength(); n++)
		{
			RichTextInfo.vecRichTextItem.push_back(tagDuiRichTextItem());
			RichTextInfo.vecRichTextItem.back().vecFontResSwitch = TextStyle.vecFontResSwitch;
			RichTextInfo.vecRichTextItem.back().vecColorResSwitch = TextStyle.vecColorResSwitch;
			RichTextInfo.vecRichTextItem.back().strText = strTipText[n];
		}

		CDUIRenderEngine::DrawRichText(hDC, rcRange, RichTextInfo, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), GetRichTextLineSpace());

		return;
	}

	//paintsel
	PaintSelectBk(hDC);

	//normal
	if (false == IsFocused())
	{
		for (auto &LineVecRichTextDraw : m_mapLineVecRichTextDraw)
		{
			for (auto &RichTextDrawItem : LineVecRichTextDraw.second)
			{
				RichTextInfo.vecRichTextItem.push_back(RichTextDrawItem);

				if (false == IsRichEdit())
				{
					RichTextInfo.vecRichTextItem.back().vecFontResSwitch = TextStyle.vecFontResSwitch;
					RichTextInfo.vecRichTextItem.back().vecColorResSwitch = TextStyle.vecColorResSwitch;
				}
			}
		}
		if (IsPasswordMode())
		{
			for (auto &RichTextItem : RichTextInfo.vecRichTextItem)
			{
				RichTextItem.strText = CMMString(GetPasswordChar(), RichTextItem.strText.GetLength());
			}
		}

		//shadow bmp
		if (IsShadowText())
		{
			if (NULL == m_pBmpShadowText 
				|| m_pBmpShadowText->GetWidth() != rcRange.GetWidth() 
				|| m_pBmpShadowText->GetHeight() != rcRange.GetHeight())
			{
				MMSafeDelete(m_pBmpShadowText);

				CDUIRect rcDraw(0, 0, rcRange.GetWidth(), rcRange.GetHeight());
				CDUIMemDC MemDC(hDC, rcDraw, false);
				CDUIRenderEngine::DrawRichText(MemDC, rcDraw, RichTextInfo, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), GetRichTextLineSpace(), IsShadowText());
				CDUIRenderEngine::RestorePixelAlpha(MemDC.GetMemBmpBits(), rcDraw.GetWidth(), rcDraw);
				m_pBmpShadowText = CDUIRenderEngine::GetAlphaBitmap(MemDC.GetMemBitmap());
			}
			if (m_pBmpShadowText)
			{
				CDUIRenderEngine::DrawImage(hDC, m_pBmpShadowText, rcRange);
			}

			return;
		}

		//normal
		CDUIRenderEngine::DrawRichText(hDC, rcRange, RichTextInfo, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), GetRichTextLineSpace(), IsShadowText());

		return;
	}

	//focus
	CDUIRenderEngine::DrawRichText(hDC, rcRange, m_mapLineVecRichTextDraw, TextStyle.dwTextStyle, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType());

	return;
}

void CDUIThinkEditCtrl::PaintSelectBk(HDC hDC)
{
	if (false == IsFocused()) return;

	int nRowFrom = 0;
	int	nRowTarget = 0;
	int	nColumnFrom = 0;
	int nColumnTarget = 0;
	PerformCalcSelect(nRowFrom, nColumnFrom, nRowTarget, nColumnTarget);

	if (nRowFrom < 0 || nRowFrom >= m_mapLineVecRichTextDraw.size()) return;

	for (int nRow = nRowFrom; nRow <= nRowTarget; nRow++)
	{
		VecDuiRichTextDraw LineVecRichTextDraw = m_mapLineVecRichTextDraw[nRow];
		if (LineVecRichTextDraw.empty()) continue;

		CDUIRect rcRow = GetLineRange(nRow);
		if (nRow == nRowFrom)
		{
			if (nColumnFrom >= 0 && nColumnFrom < (int)LineVecRichTextDraw.size())
			{
				rcRow.left = LineVecRichTextDraw[nColumnFrom].rcDraw.right;
			}
			if (nColumnFrom < 0)
			{
				rcRow.left = LineVecRichTextDraw[0].rcDraw.left;
			}
		}
		if (nRow == nRowTarget)
		{
			if (nColumnTarget >= 0 && nColumnTarget < (int)LineVecRichTextDraw.size())
			{
				rcRow.right = LineVecRichTextDraw[nColumnTarget].rcDraw.right;
			}
			if (nColumnTarget < 0)
			{
				rcRow.right = LineVecRichTextDraw[0].rcDraw.left;
			}
		}

		if (rcRow.IsEmpty()) continue;

		m_AttributeColorSelect.FillRect(hDC, rcRow, IsColorHSL());
		m_AttributeImageSelect.Draw(hDC, rcRow, m_rcPaint);
	}

	return;
}

void CDUIThinkEditCtrl::PaintCaret(HDC hDC)
{
	CDUIAttributeTextStyle *pAttribute = GetAttributeTextStyleActive();
	if (NULL == pAttribute) return;

	CDUIColorBase *pColorBase = pAttribute->GetColorBaseCur();
	CDUIRect rcCaret = GetCaretPos();
	CDUIRenderEngine::FillRect(hDC, rcCaret, m_bShowCaret ? (pColorBase ? pColorBase->GetColor() : 0xff000000) : 0x00000000);

	return;
}

CDUIControlBase * CDUIThinkEditCtrl::GetBindCtrl()
{
	return m_pBindCtrl;
}

void CDUIThinkEditCtrl::SetBindCtrl(CDUIControlBase *pBindCtrl)
{
	m_pBindCtrl = pBindCtrl;

	return;
}

bool CDUIThinkEditCtrl::HasWordWrap(int nLine)
{
	if (nLine < 0 || nLine >= m_mapLineVecRichTextDraw.size()) return false;

	auto LineVecRichTextDraw = m_mapLineVecRichTextDraw[nLine];
	if (LineVecRichTextDraw.empty()) return false;

	return _T("\n") == LineVecRichTextDraw[0].strText || _T("\r") == LineVecRichTextDraw[0].strText;
}

CDUIRect CDUIThinkEditCtrl::GetTextPosition()
{
	if (m_mapLineVecRichTextDraw.size() <= 0) return {};

	CDUIRect rcPos;

	//left top right
	auto LineVecRichTextDraw = m_mapLineVecRichTextDraw[0];
	rcPos = LineVecRichTextDraw[0].rcDraw;
	rcPos.right = LineVecRichTextDraw.back().rcDraw.right;

	//right bottom
	LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_mapLineVecRichTextDraw.size() - 1];
	rcPos.right = max(rcPos.right, LineVecRichTextDraw.back().rcDraw.right);
	rcPos.bottom = max(rcPos.bottom, LineVecRichTextDraw.back().rcDraw.bottom);

	return rcPos;
}

CDUIRect CDUIThinkEditCtrl::GetTextPositionVisible()
{
	CDUIRect rcPos;
	CDUIRect rcTextPos = GetTextPosition();
	CDUIRect rcTextRange = GetTextRange();

	IntersectRect(&rcPos, &rcTextPos, &rcTextRange);

	return rcPos;
}

void CDUIThinkEditCtrl::PerformMeasureString(IN LPCTSTR lpszText, IN tagDuiTextStyle TextStyle, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, OUT CDUIRect &rcMeasure)
{
	VecDuiRichTextItem vecRichTextItem;

	CMMString strText = lpszText;
	for (int n = 0; n < strText.GetLength(); n++)
	{
		vecRichTextItem.push_back(tagDuiRichTextItem());
		vecRichTextItem.back().strText = IsPasswordMode() ? GetPasswordChar() : strText[n];
	}

	PerformMeasureString(vecRichTextItem, TextStyle, mapLineVecRichTextDraw, rcMeasure);

	return;
}

void CDUIThinkEditCtrl::PerformMeasureString(IN VecDuiRichTextItem vecRichTextItem, IN tagDuiTextStyle TextStyle, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, OUT CDUIRect &rcMeasure)
{
	//range
	CDUIRect rcRange = GetTextRange();

	tagDuiRichText RichTextInfo = {};
	RichTextInfo.dwTextStyle = TextStyle.dwTextStyle;

	for (auto RichTextItem : vecRichTextItem)
	{
		RichTextInfo.vecRichTextItem.push_back(tagDuiRichTextItem());
		RichTextInfo.vecRichTextItem.back() = RichTextItem;
		IsPasswordMode() ? RichTextInfo.vecRichTextItem.back().strText = GetPasswordChar() : _T("");
		if (false == IsRichEdit() || RichTextInfo.vecRichTextItem.back().vecFontResSwitch.empty())
		{
			RichTextInfo.vecRichTextItem.back().vecFontResSwitch = TextStyle.vecFontResSwitch;
		}
		if (false == IsRichEdit() || RichTextInfo.vecRichTextItem.back().vecColorResSwitch.empty())
		{
			RichTextInfo.vecRichTextItem.back().vecColorResSwitch = TextStyle.vecColorResSwitch;
		}
	}

	RichTextInfo.dwTextStyle |= DT_CALCRECT;
	CDUIRect rcItem = rcRange;
	RichTextInfo.IsWordBreak() ? rcItem.bottom += 9999 : rcItem.right += 9999;
	rcMeasure = CDUIRect(rcItem.left, rcItem.top, rcItem.left, rcItem.top);
	CDUIRenderHelp::MeasureRichText(m_pWndManager->GetWndDC(), RichTextInfo, mapLineVecRichTextDraw, rcMeasure, rcItem, m_pWndManager->IsGdiplusRenderText(), m_pWndManager->GetGdiplusRenderTextType(), GetRichTextLineSpace());
	CDUIRenderHelp::AdjustRichText(RichTextInfo, rcRange, mapLineVecRichTextDraw, rcMeasure);

	return;
}

void CDUIThinkEditCtrl::PerformHitText(CDUIPoint pt, bool bFromMouseDown)
{
	if (NULL == m_pWndManager) return;

	CDUIRect rcPos = GetTextPositionVisible();
	bool bHitBottom = pt.y > rcPos.bottom;
	bool bHitTop = pt.y < rcPos.top;

	//scroll
	if (false == rcPos.PtInRect(pt))
	{
		CDUISize szScrollPos = GetScrollPos();
		if (pt.x < rcPos.left) szScrollPos.cx -= rcPos.left - pt.x;
		if (pt.x > rcPos.right) szScrollPos.cx += pt.x - rcPos.right;
		if (bHitTop) szScrollPos.cy -= rcPos.top - pt.y;
		if (bHitBottom) szScrollPos.cy += pt.y - rcPos.bottom;

		SetScrollPos(szScrollPos);
	}

	//row
	int nLine = bHitBottom ? (int)m_mapLineVecRichTextDraw.size() - 1 : 0;
	do
	{
		m_nCaretRow = -1;

		CDUIRect rcLine = GetLineRange(nLine);
		if ((bHitBottom && rcLine.top < rcPos.bottom)
			|| (bHitTop && rcLine.bottom > rcPos.top)
			|| (false == bHitBottom && false == bHitTop && rcLine.top <= pt.y && pt.y <= rcLine.bottom))
		{
			m_nCaretRow = nLine;

			break;
		}

		bHitBottom ? nLine-- : nLine++;

	} while (nLine >= 0 && nLine < m_mapLineVecRichTextDraw.size());

	//column
	do
	{
		m_nCaretColumn = -1;

		//over left
		if (pt.x < rcPos.left || m_nCaretRow < 0 || m_nCaretRow >= m_mapLineVecRichTextDraw.size())
		{
			m_nCaretColumn = -1;

			break;
		}

		//over right
		auto LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_nCaretRow];
		if (pt.x > rcPos.right && m_mapLineVecRichTextDraw.size() > 1)
		{
			m_nCaretColumn = (int)LineVecRichTextDraw.size() - 1;

			break;
		}

		//this row
		for (int n = 0; n < LineVecRichTextDraw.size(); n++)
		{
			bool bFinalyItem = n == (int)LineVecRichTextDraw.size() - 1;
			auto RichTextDrawItem = LineVecRichTextDraw[n];
			if (false == bFinalyItem
				&& (pt.x < RichTextDrawItem.rcDraw.left || pt.x > RichTextDrawItem.rcDraw.right)) continue;

			if (pt.x >= RichTextDrawItem.rcDraw.left + RichTextDrawItem.rcDraw.GetWidth() / 2)
			{
				m_nCaretColumn = n;
			}
			else
			{
				m_nCaretColumn = n - 1;
			}

			break;
		}

	} while (false);

	//caret pos
	CDUIRect rcCaret = GetCaretPos();

	m_pWndManager->CreateCaret(NULL, rcCaret.GetWidth(), rcCaret.GetHeight());
	m_pWndManager->SetCaretPos({ rcCaret.left, rcCaret.top });
	m_bShowCaret = true;

	if (bFromMouseDown)
	{
		m_nCaretSelFromRow = m_nCaretRow;
		m_nCaretSelFromColumn = m_nCaretColumn;
	}

	return;
}

void CDUIThinkEditCtrl::PerformMoveCaretHoriz(bool bLeft, bool bSelect, bool bAdjustWordWrap)
{
	if (NULL == m_pWndManager) return;

	do
	{
		//none
		if (m_nCaretRow < 0 && m_mapLineVecRichTextDraw.size() > 0)
		{
			m_nCaretRow = 0;
			m_nCaretColumn = 0;

			break;
		}

		if (m_nCaretRow < 0 || m_nCaretRow >= m_mapLineVecRichTextDraw.size()) break;

		//cur column
		VecDuiRichTextDraw LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_nCaretRow];
		if (m_nCaretColumn >= (int)LineVecRichTextDraw.size()) return;

		//left
		if (bLeft && (m_nCaretRow > 0 || m_nCaretColumn >= 0)) m_nCaretColumn--;

		//right
		if (false == bLeft && (m_nCaretRow < (int)m_mapLineVecRichTextDraw.size() - 1 || m_nCaretColumn < (int)LineVecRichTextDraw.size() - 1)) m_nCaretColumn++;

		//left border column
		if (m_nCaretColumn < 0)
		{
			if (m_nCaretRow > 0)
			{
				m_nCaretRow--;
				LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_nCaretRow];
				m_nCaretColumn = (int)LineVecRichTextDraw.size() - 1;
			}
		}

		//right finaly column
		if (m_nCaretColumn >= (int)LineVecRichTextDraw.size())
		{
			if (m_nCaretRow < (int)m_mapLineVecRichTextDraw.size() - 1)
			{
				m_nCaretRow++;
				LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_nCaretRow];
				m_nCaretColumn = 0;
			}
		}

	} while (false);

	//verify
	if (m_nCaretRow < 0 || m_nCaretRow >= m_mapLineVecRichTextDraw.size()) return;

	//hit
	if (false == bSelect)
	{
		m_nCaretSelFromRow = m_nCaretRow;
		m_nCaretSelFromColumn = m_nCaretColumn;
	}

	//caret
	PerformAdjustCaret();

	//word wrap
	if ((_T('\n') == GetCaretCharCur() || _T('\r') == GetCaretCharCur())
		&& bAdjustWordWrap
		&& ((bLeft && m_nCaretRow > 0) || (false == bLeft && 0 != GetCaretChar(m_nCaretRow, m_nCaretColumn + 1))))
	{
		PerformMoveCaretHoriz(bLeft, bSelect, false);

		m_bShowCaret = true;
		Invalidate();
		UpdateWindow(m_pWndManager->GetWndHandle());

		return;
	}

	m_bShowCaret = true;
	Invalidate();
	UpdateWindow(m_pWndManager->GetWndHandle());

	return;
}

void CDUIThinkEditCtrl::PerformMoveCaretVert(bool bUp)
{
	if (NULL == m_pWndManager) return;

	int nCaretRowPre = m_nCaretRow;

	//has wordwrap
	bool bWordWrapPre = HasWordWrap(nCaretRowPre);

	do
	{
		//none
		if (m_nCaretRow < 0 && m_mapLineVecRichTextDraw.size() > 0)
		{
			m_nCaretRow = 0;
			m_nCaretColumn = 0;

			break;
		}

		if (m_nCaretRow < 0 || m_nCaretRow >= m_mapLineVecRichTextDraw.size()) break;

		//up
		if (bUp && m_nCaretRow > 0) m_nCaretRow--;

		//down
		if (false == bUp && m_nCaretRow < (int)m_mapLineVecRichTextDraw.size() - 1) m_nCaretRow++;

		//cur line
		VecDuiRichTextDraw LineVecRichTextDraw = m_mapLineVecRichTextDraw[m_nCaretRow];

		//over column
		if (m_nCaretColumn >= (int)LineVecRichTextDraw.size())
		{
			m_nCaretColumn = (int)LineVecRichTextDraw.size() - 1;
		}

	} while (false);

	//verify
	if (m_nCaretRow < 0 || m_nCaretRow >= m_mapLineVecRichTextDraw.size()) return;

	//shift
	bool bSelect = 0 != (CDUIWndManager::MapKeyState() & MK_SHIFT);
	if (false == bSelect)
	{
		m_nCaretSelFromRow = m_nCaretRow;
		m_nCaretSelFromColumn = m_nCaretColumn;
	}

	//caret
	PerformAdjustCaret();

	//word wrap
	if (nCaretRowPre != m_nCaretRow
		&& bWordWrapPre != HasWordWrap(m_nCaretRow)
		&& (0 <= m_nCaretRow && m_nCaretRow < m_mapLineVecRichTextDraw.size()))
	{
		if (bWordWrapPre
			&& m_nCaretColumn > 0
			&& m_nCaretColumn < m_mapLineVecRichTextDraw[m_nCaretRow].size()
			&& _T("\n") != m_mapLineVecRichTextDraw[m_nCaretRow][m_nCaretColumn - 1].strText
			&& _T("\r") != m_mapLineVecRichTextDraw[m_nCaretRow][m_nCaretColumn - 1].strText)
		{
			PerformMoveCaretHoriz(true, bSelect, false);
		}
		if (false == bWordWrapPre
			&& m_nCaretColumn < (int)m_mapLineVecRichTextDraw[m_nCaretRow].size() - 1)
		{
			PerformMoveCaretHoriz(false, bSelect, false);
		}
	}

	m_bShowCaret = true;
	Invalidate();
	UpdateWindow(m_pWndManager->GetWndHandle());

	return;
}

void CDUIThinkEditCtrl::PerformClearSelect(bool bHistory)
{
	if (NULL == m_pWndManager || IsReadOnly()) return;

	//from target
	int nRowFrom = 0;
	int	nRowTarget = 0;
	int	nColumnFrom = 0;
	int nColumnTarget = 0;
	PerformCalcSelect(nRowFrom, nColumnFrom, nRowTarget, nColumnTarget);

	if (nRowFrom < 0 || nRowFrom >= m_mapLineVecRichTextDraw.size()) return;

	//history
	if (bHistory)
	{
		tagDuiThinkEditHistory History;
		History.bAdd = false;
		History.nCaretRowPre = nRowFrom;
		History.nCaretColumnPre = nColumnFrom;
		History.nCaretRowNext = nRowTarget;
		History.nCaretColumnNext = nColumnTarget;
		History.strText = GetSelectString();

		PerformAddHistory(History);
	}

	//remove
	for (int nRow = nRowFrom; nRow <= nRowTarget; nRow++)
	{
		VecDuiRichTextDraw &LineVecRichTextDraw = m_mapLineVecRichTextDraw[nRow];

		int nColumnPre = 0;
		int nColumnNext = (int)LineVecRichTextDraw.size();

		if (nRow == nRowFrom)
		{
			nColumnPre = nColumnFrom + 1;
		}
		if (nRow == nRowTarget)
		{
			nColumnNext = nColumnTarget + 1;
		}

		nColumnPre = min(nColumnPre, (int)LineVecRichTextDraw.size());
		nColumnNext = min(nColumnNext, (int)LineVecRichTextDraw.size());
		nColumnNext = max(nColumnNext, nColumnPre);

		LineVecRichTextDraw.erase(LineVecRichTextDraw.begin() + nColumnPre, LineVecRichTextDraw.begin() + nColumnNext);
	}

	//remove line
	auto mapLineVecRichTextDraw = m_mapLineVecRichTextDraw;
	for (int nLine = 0; nLine < mapLineVecRichTextDraw.size(); nLine++)
	{
		auto LineVecRichTextDraw = mapLineVecRichTextDraw[nLine];
		if (LineVecRichTextDraw.empty())
		{
			m_mapLineVecRichTextDraw.erase(nLine);
		}
	}

	//text
	VecDuiRichTextItem vecRichTextItem;
	for (auto LineVecRichTextDraw : m_mapLineVecRichTextDraw)
	{
		for (auto RichTextDrawItem : LineVecRichTextDraw.second)
		{
			vecRichTextItem.push_back(RichTextDrawItem);
		}
	}

	SetRichTextItem(vecRichTextItem);

	//caret
	m_nCaretRow = nRowFrom;
	m_nCaretSelFromRow = nRowFrom;
	m_nCaretColumn = nColumnFrom;
	m_nCaretSelFromColumn = nColumnFrom;

	//refresh
	RefreshView();

	//caret
	PerformAdjustCaret();

	return;
}

void CDUIThinkEditCtrl::PerformAdjustCaret()
{
	if (NULL == m_pWndManager) return;

	CDUIRect rcCaret = GetCaretPos();

	if (IsFocused())
	{
		m_pWndManager->SetCaretPos({ rcCaret.left, rcCaret.top });
	}

	return;
}

void CDUIThinkEditCtrl::PerformCalcSelect(OUT int &nRowFrom, OUT int &nColumnFrom, OUT int &nRowTarget, OUT int &nColumnTarget)
{
	nRowFrom = m_nCaretSelFromRow;
	nColumnFrom = m_nCaretSelFromColumn;
	nRowTarget = m_nCaretRow;
	nColumnTarget = m_nCaretColumn;

	if (m_nCaretRow == m_nCaretSelFromRow && m_nCaretColumn == m_nCaretSelFromColumn) return;

	nRowFrom = min(m_nCaretRow, m_nCaretSelFromRow);
	nRowTarget = max(m_nCaretRow, m_nCaretSelFromRow);
	nColumnFrom = nRowFrom == m_nCaretRow ? m_nCaretColumn : m_nCaretSelFromColumn;
	nColumnTarget = nRowFrom == m_nCaretRow ? m_nCaretSelFromColumn : m_nCaretColumn;
	nColumnFrom = nRowFrom == nRowTarget ? min(m_nCaretColumn, m_nCaretSelFromColumn) : nColumnFrom;
	nColumnTarget = nRowFrom == nRowTarget ? max(m_nCaretColumn, m_nCaretSelFromColumn) : nColumnTarget;

	return;
}

void CDUIThinkEditCtrl::PerformAddHistory(tagDuiThinkEditHistory History)
{
	if (History.strText.IsEmpty()) return;

	//index
	if (m_nIndexHistory < 0 || m_nIndexHistory >= m_queHistory.size())
	{
		m_nIndexHistory = -1;
		m_queHistory.clear();
	}
	else
	{
		m_queHistory.erase(m_queHistory.begin() + m_nIndexHistory + 1, m_queHistory.end());
	}

	//add
	if (false == History.strText.IsEmpty())
	{
		m_queHistory.push_back(History);
		m_nIndexHistory++;
	}
	if (m_queHistory.size() > Max_ThinkEditHistory)
	{
		m_queHistory.pop_front();
		m_nIndexHistory--;
	}

	return;
}

void CDUIThinkEditCtrl::PerformActiveHistory(tagDuiThinkEditHistory History, bool bUndo)
{
	//add
	if ((History.bAdd && bUndo)
		|| (false == History.bAdd && false == bUndo))
	{
		SetCaretPos(History.nCaretRowNext, History.nCaretColumnNext);

		for (int n = 0; n < History.strText.GetLength(); n++)
		{
			PerformMoveCaretHoriz(true, true, false);
		}

		PerformClearSelect(false);

		return;
	}

	//remove
	SetCaretPos(History.nCaretRowPre, History.nCaretColumnPre);

	SetReplaceSel(History.strText, History.strImageResName, false);

	return;
}