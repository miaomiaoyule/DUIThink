#ifndef __DUIRICHEDITCTRL_H__
#define __DUIRICHEDITCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUITextHost;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIRichEditCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIRichEditCtrl = { 0x5F2106BF,0xD35E,0x47A9,0xBF,0xC3,0x5A,0x54,0x29,0x7D,0x77,0xD6 };
class DUITHINK_API CDUIRichEditCtrl
	: public CDUIContainerCtrl
	, public IDUIPreMessage
{
	friend class CDUITextHost;

	DuiDeclare_CreateControl(CDUIRichEditCtrl)
	MMDeclare_ClassName(CDUIContainerCtrl)

public:
	CDUIRichEditCtrl(void);
	~CDUIRichEditCtrl(void);

	//attribute
protected:
	//edit
	CDUIAttributeGroup					m_AttributeGroupRichEdit;
	CDUIAttributeBool					m_AttributeRichText;
	CDUIAttributeBool					m_AttributeReadOnly;
	CDUIAttributeBool					m_AttributeNumberOnly;
	CDUIAttributeBool					m_AttributePasswordMode;
	CDUIAttributeText					m_AttributePasswordChar;
	CDUIAttributeBool					m_AttributeAllowBeep;
	CDUIAttributeBool					m_AttributeSaveSelection;
	CDUIAttributeBool					m_AttributeTransparent;
	CDUIAttributeBool					m_AttributeAutoSelAll;
	CDUIAttributeLong					m_AttributeMaxChar;
	CDUIAttributeLong					m_AttributeNumberMinLimit;
	CDUIAttributeLong					m_AttributeNumberMaxLimit;
	CDUIAttributeText					m_AttributeTipText;
	CDUIAttributeText					m_AttributeText;
	CDUIAttributeTextStyle				m_AttributeTextStyleTipTextNormal;
	CDUIAttributeTextStyle				m_AttributeTextStyleTipTextHot;
	CDUIAttributeTextStyle				m_AttributeTextStyleNormal;
	CDUIAttributeTextStyle				m_AttributeTextStyleHot;
	CDUIAttributeTextStyle				m_AttributeTextStyleFocus;
	CDUIAttributeTextStyle				m_AttributeTextStyleDisabled;
	CDUIAttriImageSection				m_AttributeImageNormal;
	CDUIAttriImageSection				m_AttributeImageHot;
	CDUIAttriImageSection				m_AttributeImageFocus;
	CDUIAttriImageSection				m_AttributeImageDisabled;
	CDUIAttributeRect					m_AttributeTextPadding;

	//variant
protected:
	CDUITextHost *						m_pTextHost = NULL;

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
	void RefreshView() override;

	//attribute
	void SetEnabled(bool bEnable = true) override;
	void SetScrollPos(SIZE szPos, bool bMsg = true) override;
	void LineUp() override;
	void LineDown() override;
	void PageUp() override;
	void PageDown() override;
	void HomeUp() override;
	void EndDown() override;
	void LineLeft() override;
	void LineRight() override;
	void PageLeft() override;
	void PageRight() override;
	void HomeLeft() override;
	void EndRight() override;

	//rich
	virtual bool IsRichText();
	virtual void SetRichText(bool bRichText = true);
	virtual bool IsReadOnly();
	virtual void SetReadOnly(bool bReadOnly = true);
	virtual bool IsNumberOnly();
	virtual void SetNumberOnly(bool bNumberOnly);
	virtual bool IsPasswordMode();
	virtual void SetPasswordMode(bool bPasswordMode);
	virtual bool IsAllowBeep();
	virtual void SetAllowBeep(bool bAllowBeep);
	virtual bool IsSaveSelection();
	virtual void SetSaveSelection(bool bSaveSelection);
	virtual bool IsTransparent();
	virtual void SetTransparent(bool bTransparent);
	virtual bool IsAutoSelAll();
	virtual void SetAutoSelAll(bool bAutoSelAll);
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
	virtual CMMString GetText();
	virtual CMMString GetText(long nStartChar, long nEndChar) const;
	virtual bool SetText(LPCTSTR lpszText);
	virtual HFONT GetFont();
	virtual ARGB GetTextColor();
	virtual RECT GetTextPadding();
	virtual void SetTextPadding(RECT rcPadding);

	virtual ARGB GetTextColorNormal();
	virtual ARGB GetTextColorHot();
	virtual ARGB GetTextColorFocus();
	virtual ARGB GetTextColorDisabled();
	virtual ARGB GetTextColorTipNormal();
	virtual ARGB GetTextColorTipHot();
	virtual HFONT GetFontNormal();
	virtual HFONT GetFontHot();
	virtual HFONT GetFontFocus();
	virtual HFONT GetFontDisabled();
	virtual HFONT GetFontTipNormal();
	virtual HFONT GetFontTipHot();
	virtual tagDuiTextStyle GetTextStyleNormal();
	virtual tagDuiTextStyle GetTextStyleHot();
	virtual tagDuiTextStyle GetTextStyleFocus();
	virtual tagDuiTextStyle GetTextStyleDisabled();
	virtual tagDuiTextStyle GetTextStyleTipNormal();
	virtual tagDuiTextStyle GetTextStyleTipHot();
	virtual void SetTextStyleNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleHot(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleFocus(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleDisabled(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleTipNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleTipHot(const tagDuiTextStyle &TextStyle);

	virtual tagDuiImageSection GetImageSectionNormal();
	virtual tagDuiImageSection GetImageSectionHot();
	virtual tagDuiImageSection GetImageSectionFocus();
	virtual tagDuiImageSection GetImageSectionDisabled();
	virtual void SetImageSectionNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionFocus(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionDisabled(const tagDuiImageSection &ImageSection);

	virtual long GetStyle();
	virtual void SetFont(LPCTSTR lpszFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	virtual long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
	virtual bool GetModify() const;
	virtual void SetModify(bool bModified = true) const;
	virtual void GetSel(CHARRANGE &cr) const;
	virtual void GetSel(long &nStartChar, long &nEndChar) const;
	virtual CMMString GetSelText() const;
	virtual int SetSel(CHARRANGE &cr);
	virtual int SetSel(long nStartChar, long nEndChar);
	virtual void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo);
	virtual void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
	virtual int SetSelAll();
	virtual int SetSelNone();
	virtual WORD GetSelectionType() const;
	virtual bool GetZoom(int &nNum, int &nDen) const;
	virtual bool SetZoom(int nNum, int nDen);
	virtual bool SetZoomOff();
	virtual bool GetAutoUrlDetect() const;
	virtual bool SetAutoUrlDetect(bool bAutoDetect = true);
	virtual DWORD GetEventMask() const;
	virtual DWORD SetEventMask(DWORD dwEventMask);
	virtual void HideSelection(bool bHide = true, bool bChangeStyle = false);
	virtual void ScrollCaret();
	virtual int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
	virtual int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
	virtual DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
	virtual bool SetDefaultCharFormat(CHARFORMAT2 &cf);
	virtual DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
	virtual bool SetSelectionCharFormat(CHARFORMAT2 &cf);
	virtual bool SetWordCharFormat(CHARFORMAT2 &cf);
	virtual DWORD GetParaFormat(PARAFORMAT2 &pf) const;
	virtual bool SetParaFormat(PARAFORMAT2 &pf);
	virtual bool CanUndo();
	virtual bool CanRedo();
	virtual bool CanPaste();
	virtual bool Redo();
	virtual bool Undo();
	virtual void Clear();
	virtual void Copy();
	virtual void Cut();
	virtual void Paste();
	virtual int GetLineCount() const;
	virtual CMMString GetLine(int nIndex, int nMaxLength) const;
	virtual int LineIndex(int nLine = -1) const;
	virtual int LineLength(int nLine = -1) const;
	virtual bool LineScroll(int nLines, int nChars = 0);
	virtual CDUIPoint GetCharPos(long lChar) const;
	virtual long LineFromChar(long nIndex) const;
	virtual CDUIPoint PosFromChar(UINT nChar) const;
	virtual int CharFromPos(CDUIPoint pt) const;
	virtual void EmptyUndoBuffer();
	virtual UINT SetUndoLimit(UINT nLimit);
	virtual long StreamIn(int nFormat, EDITSTREAM &es);
	virtual long StreamOut(int nFormat, EDITSTREAM &es);
	virtual bool SetDropAcceptFile(bool bAccept);

	virtual IDropTarget * GetTxDropTarget();
	virtual IRichEditOle * GetIRichEditOle();
	virtual bool SetOLECallback(IRichEditOleCallback *pCallback);
	virtual bool InsertNotGIFImage(LPCTSTR szFilePath, DWORD dwUserData);
	virtual bool InsertGIFImage(LPCTSTR szFilePath, DWORD dwUserData);

	virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const;

	//message
protected:
	bool OnDuiSetCursor(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiSetFocus() override;
	bool OnDuiKillFocus() override;
	void OnDuiWndManagerAttach() override;
	void OnDuiWndManagerDetach() override;
	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;
	LRESULT OnDuiContextMenu(const DuiMessage &Msg) override;
	LRESULT OnDuiCommand(const DuiMessage &Msg) override;

	virtual void OnTxNotify(DWORD iNotify, void *pv);

	//property
protected:
	void InitProperty() override;
	void InitAttriValue() override;
	void InitComplete() override;

	//paint
protected:
	void PaintStatusImage(HDC hDC) override;
	void PaintText(HDC hDC) override;

	//help
protected:
	void ConstructTextStyle();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif