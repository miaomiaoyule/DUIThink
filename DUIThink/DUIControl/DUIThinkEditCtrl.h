#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIThinkEditCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIThinkEditCtrl = { 0x83439E85,0x4226,0x4E6D,0xB2,0x9A,0xD4,0x83,0xBB,0x75,0x51,0xB1 };
class DUITHINK_API CDUIThinkEditCtrl 
	: public CDUIButtonCtrl
	, public IDuiPreMessage
{
	friend class CDUIComboxCtrl;

	DuiDeclare_CreateControl(CDUIThinkEditCtrl)
	MMDeclare_ClassName(CDUIThinkEditCtrl)

public:
	CDUIThinkEditCtrl();
	~CDUIThinkEditCtrl();

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupEdit;
	CDUIAttributeColorSwitch			m_AttributeColorSelect = Name_ColorSelBk;
	CDUIAttriImageSection				m_AttributeImageSelect;
	CDUIAttributeCombox					m_AttributeEditTextType;
	CDUIAttributeBool					m_AttributeRichEdit;
	CDUIAttributeBool					m_AttributeReadOnly;
	CDUIAttributeBool					m_AttributePasswordMode;
	CDUIAttributeBool					m_AttributeAutoSelAll;
	CDUIAttributeBool					m_AttributeWantReturn;
	CDUIAttributeLong					m_AttributeMaxChar = 255;
	CDUIAttributeLong					m_AttributeNumberMinLimit = 0x80000000;
	CDUIAttributeLong					m_AttributeNumberMaxLimit = 0x7fffffff;
	CDUIAttributeText					m_AttributePasswordChar = _T("*");
	CDUIAttributeText					m_AttributeTipText;
	CDUIAttributeTextStyle				m_AttributeTextStyleTipTextNormal;

	//variant
protected:
	CDUIControlBase *					m_pBindCtrl = NULL;
	VecDuiRichTextItem					m_vecRichTextItem;
	MapLineVecDuiRichTextDraw			m_mapLineVecRichTextDraw;
	CMMString							m_strInput;
	CDUISize							m_szScrollRange;
	CDUISize							m_szScrollPos;
	bool								m_bMouseDownFocus = false;
	bool								m_bShowCaret = false;
	int									m_nCaretRow = -1;
	int									m_nCaretColumn = -1;
	int									m_nCaretRowFrom = -1;
	int									m_nCaretColumnFrom = -1;
	int									m_nIndexHistory = -1;
	QueDuiThinkEditHistory				m_queHistory;
	Gdiplus::Bitmap *					m_pBmpText = NULL;

	//override
protected:
	LRESULT OnPreWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method 
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void Invalidate() override;
	void RefreshView() override;
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//layout
	CDUISize MeasureString(LPCTSTR lpszText = _T("")) override;
	virtual CDUISize MeasureString(VecDuiRichTextBase vecRichTextBase);

	//text
	bool SetText(LPCTSTR lpszText) override;
	VecDuiRichTextItem GetRichTextItem() override;
	bool SetRichTextItem(const VecDuiRichTextItem &vecRichTextItem) override;

	//attribute
	virtual enDuiEditTextType GetEditTextType();
	virtual void SetEditTextType(enDuiEditTextType TextType);
	virtual bool IsNumberOnly();
	virtual bool IsRichEdit();
	virtual void SetRichEdit(bool bRichEdit);
	virtual bool IsReadOnly();
	virtual void SetReadOnly(bool bReadOnly);
	virtual bool IsPasswordMode();
	virtual void SetPasswordMode(bool bPasswordMode);
	virtual bool IsAutoSelAll();
	virtual void SetAutoSelAll(bool bAutoSelAll);
	virtual bool IsWantReturn();
	virtual void SetWantReturn(bool bWantReturn);
	virtual UINT GetMaxChar();
	virtual void SetMaxChar(UINT uMax);
	virtual int GetNumberMinLimit();
	virtual void SetNumberMinLimit(int nMinNum);
	virtual int GetNumberMaxLimit();
	virtual void SetNumberMaxLimit(int nMaxNum);
	virtual TCHAR GetPasswordChar();
	virtual void SetPasswordChar(TCHAR chPasswordChar);
	virtual CMMString GetTipText();
	virtual void SetTipText(LPCTSTR lpszTipText);
	virtual tagDuiTextStyle GetTextStyleTipTextNormal();
	virtual ARGB GetTextColorTipTextNormal();
	virtual void SetTextStyleTipTextNormal(const tagDuiTextStyle &TextStyle);

	//edit
	virtual CDUIRect GetLineRange(int nLine);
	virtual CDUIRect GetCaretPos();
	virtual void SetCaretPos(int nRow, int nColumn);
	virtual TCHAR GetCaretChar(int nRow, int nColumn);
	virtual TCHAR GetCaretCharCur();
	virtual TCHAR GetCaretCharNext();
	virtual CDUISize GetScrollPos();
	virtual void SetScrollPos(CDUISize szPos);
	virtual CDUISize GetScrollRange();
	virtual CMMString GetSelectString();
	virtual CDUIRect GetSelectRange();
	virtual CDUISize GetSel();
	virtual void SetSel(long nStartChar, long nEndChar);
	virtual void SetSelAll();
	virtual void SetReplaceSel(LPCTSTR lpszText, LPCTSTR lpszImageResName = _T(""), bool bHistory = true);
	virtual bool CanUndo();
	virtual bool CanRedo();
	virtual void Undo();
	virtual void Redo();
	virtual void Clear();
	virtual void Copy();
	virtual void Cut();
	virtual void Paste();

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiSetFocus() override;
	bool OnDuiKillFocus() override;

	void OnDuiWndManagerAttach() override;
	void OnDuiWndManagerDetach() override;
	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;

	LRESULT OnDuiKeyDown(const DuiMessage &Msg) override;
	LRESULT OnDuiChar(const DuiMessage &Msg) override;
	LRESULT OnDuiContextMenu(const DuiMessage &Msg) override;
	LRESULT OnDuiCommand(const DuiMessage &Msg) override;
	LRESULT OnDuiImeComPosition(const DuiMessage &Msg) override;

	//property 
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:
	void PaintText(HDC hDC) override;
	virtual void PaintSelectBk(HDC hDC);
	virtual void PaintCaret(HDC hDC);
	virtual void PaintExtractBmpText(HDC hDC, tagDuiRichText RichTextInfo, CDUIRect rcRange, tagDuiTextStyle TextStyle);

	//help
protected:
	virtual CDUIControlBase * GetBindCtrl();
	virtual void SetBindCtrl(CDUIControlBase *pBindCtrl);
	virtual bool HasWordWrap(int nLine);
	virtual CDUIRect GetTextPosition();
	virtual CDUIRect GetTextPositionVisible();
	virtual void PerformMeasureString(IN LPCTSTR lpszText, IN tagDuiTextStyle TextStyle, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, OUT CDUIRect &rcMeasure);
	virtual void PerformMeasureString(IN VecDuiRichTextItem vecRichTextItem, IN tagDuiTextStyle TextStyle, OUT MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, OUT CDUIRect &rcMeasure);
	virtual void PerformHitText(CDUIPoint pt, bool bFromMouseDown);
	virtual void PerformMoveCaretHoriz(bool bLeft, bool bSelect = true, bool bAdjustWordWrap = true);
	virtual void PerformMoveCaretVert(bool bUp);
	virtual void PerformClearSelect(bool bHistory = true);
	virtual void PerformAdjustCaret();
	virtual void PerformAdjustRichText();
	virtual void PerformAdjustScrollPos(CDUIRect rcMeasure);
	virtual void PerformCalcSelect(OUT int &nRowFrom, OUT int &nColumnFrom, OUT int &nRowTarget, OUT int &nColumnTarget);
	virtual void PerformAddHistory(tagDuiThinkEditHistory History);
	virtual void PerformActiveHistory(tagDuiThinkEditHistory History, bool bUndo);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()