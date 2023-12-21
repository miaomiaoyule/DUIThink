#ifndef __MM_DEFINEGLOBAL_H__
#define __MM_DEFINEGLOBAL_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	#define Name_AppPreviewExe			_T("DUIPreviewD.exe")
	#define Name_AppUpdateExe			_T("MMUpdate.exe")
#else
	#define Name_AppPreviewExe			_T("DUIPreview.exe")
	#define Name_AppUpdateExe			_T("MMUpdate.exe")
#endif

#define Name_AppDUIThink				_T("DUIThink")
#define Name_ImageVScrollBk				_T("{88D041B7-27A9-4819-9986-BF3A631B2A36}")
#define Name_ImageVScrollThumb			_T("{A07E93B2-3D41-41DC-A03A-5E56339BAD2C}")
#define Name_ImageHScrollBk				_T("{689FF69E-A8B1-453C-A1FE-58D234E97A06}")
#define Name_ImageHScrollThumb			_T("{D1C016EE-350C-4335-8573-D9156841952A}")
#define Name_ImageCheckExpand			_T("{F1127AB2-0950-430F-A490-E90A40C394CD}")
#define Name_ImageCheckExpandSelect		_T("{0A9C16DA-415E-40DB-BB10-24E1F6855595}")
#define Name_ImageCheckExpandUnSelect	_T("{30BF89B6-4079-40A4-9EB2-D24288D12D7C}")
#define Name_ImageSliderBk				_T("{AE3A1229-31BA-480A-9B1B-F98A92D797AD}")
#define Name_ImageSliderThumb			_T("{F8B7DCBC-923C-4E60-A2FA-1032B9BBFDEE}")
#define Name_ImageBtnMin				_T("{54D213E5-B49F-4A54-A5AC-7B3EE84ABE6C}")
#define Name_ImageBtnMax				_T("{11F140C1-5089-416A-AFC8-FAF0BC31DCFC}")
#define Name_ImageBtnRestore			_T("{E3B96193-7CF7-4D67-BB8A-99961F2A6DAA}")
#define Name_ImageBtnClose				_T("{66EB709C-5356-4559-894B-CE733B57AFB3}")
#define Name_ImageBtnBk					_T("{328E1955-BA65-4F32-9FD0-A6183785595B}")
#define Name_ImageBtnArrowUp			_T("{27B6E60D-8BF0-4080-8E69-8C32E4BA3634}")
#define Name_ImageBtnArrowDown			_T("{19B40436-9EB4-4F5C-8A3B-37C5A1266C4A}")
#define Name_ImageBtnLeft				_T("{3ED746FD-6AA5-4D94-B5C0-0804750E786A}")
#define Name_ImageBtnRight				_T("{0EC922B7-6ECE-48F9-B65E-0317A4CF2BA6}")
#define Name_ImageLogo					_T("{1BB8AA78-A0FE-48B6-8807-2890C92B994E}")
#define Name_ImageCheckBoxUnSelect		_T("{6327FE22-6CD6-4104-B0C5-75CA625F71C9}")
#define Name_ImageCheckBoxSelect		_T("{95A0A73A-4E45-4D47-A9AB-5FC7E0F3A641}")
#define Name_ImageRadioBoxUnSelect		_T("{3901AB19-D453-4EE1-A2B2-0EE5701B9A9A}")
#define Name_ImageRadioBoxSelect		_T("{982B9EAD-50CE-49C7-9332-896DC826A32E}")
#define Name_ImageProgressBk			_T("{3FD27CF4-C264-46DE-A8B4-E8D4865BE862}")
#define Name_ImageProgressFore			_T("{3F41F77E-0B95-4DF8-84BF-78295A3A69DF}")
#define Name_ImageComboxBk				_T("{6115C34A-A7D3-4C3F-8222-3BBBAE1ECE38}")
#define Name_ImageCardMaster1			_T("{26A1CF3B-F758-458F-B25C-22E16D3D6439}")
#define Name_ImageCardMaster2			_T("{DA2C36A7-1758-4716-8BCD-BDB208AAC65C}")
#define Name_ImageCardMaster3			_T("{E8FC09D1-2B03-4308-88E2-8542A6E9A23F}")
#define Name_ImageCardMaster4			_T("{B94A4083-6E90-424E-B35C-CB03DB4EE881}")
#define Name_ImageCardMaster5			_T("{3FA79189-52BE-413F-B120-A2A5CDAB1B56}")
#define Name_ImageClockDial				_T("{55B3D83B-F683-4D24-932E-B6B8593E8A21}")
#define Name_ImageClockHour				_T("{C3D2A82D-D4C7-4ECF-8825-89CE3D514115}")
#define Name_ImageClockMinute			_T("{3C3FD836-F084-48C8-B756-1A6A51F356EE}")
#define Name_ImageClockSecond			_T("{61E627E2-3B62-4654-BC49-5496DC2B810C}")
#define Name_ImageClockCenterDot		_T("{C6CBA3A4-0E50-4AC8-BDC3-847E1FEDE1A1}")
#define Name_ImageGif					_T("{FA94D6A6-40B8-41CB-83FA-B886716BE3C3}")
#define Name_ImageSequenceFrame			_T("{3482C09B-88D0-4A30-B854-9DBC23B83587}")
#define Name_Imagebtn_normal			_T("{5D038E0C-9B44-4CCA-AAF3-9B14175FEF27}")
#define Name_Imagebtn_highlight			_T("{EB778ACD-B963-4648-AF85-FE14195C1770}")
#define Name_Imagebtn_down				_T("{C59429A4-D032-4517-B0A5-54D1647C3370}")
#define Name_ImageExpandToolsArrow		_T("{792618B7-4B73-47BE-ABBE-6C21591B5D9B}")
#define Name_Imageformedit				_T("{B5379AF9-FE2E-459A-8CC2-9EC5BF12FA14}")
#define Name_Imagegray					_T("{D54E16B0-2812-4B19-B9C6-441D844BA704}")
#define Name_ImageIconColorRes			_T("{83BEF7C1-D440-406D-A5A4-77E1E8C4037F}")
#define Name_ImageIconControl			_T("{A30FFCCA-CFEA-42DF-86B7-E1EFBE0AE219}")
#define Name_ImageIconControlNotify		_T("{66FF3484-BE2C-41EA-A27A-565778027E00}")
#define Name_ImageIconFontRes			_T("{650168FA-7169-4037-8667-291EE9F19118}")
#define Name_ImageIconImageRes			_T("{55646323-DF1C-4EF6-AC93-786667C950D7}")
#define Name_ImageIconProject			_T("{B6B52D37-64B0-41EA-B111-53F174BF4DC2}")
#define Name_ImageIconProperty			_T("{3090CABA-9F7A-4957-A9DD-FA7813A6917A}")
#define Name_ImageIconToolbox			_T("{EAEB909F-534F-41CB-8F5E-0946B6002CC0}")
#define Name_ImagelineH					_T("{A03CB0C8-889D-4A66-9494-F56A941DCB22}")
#define Name_ImagelineV					_T("{77AAB5A7-1532-4B9B-ACD0-BE8D03C5F182}")
#define Name_ImageRadioTabSelected		_T("{8DD32057-468D-463C-8924-8176E523453F}")
#define Name_ImageRadioTabUnSelected	_T("{DEA66482-38D5-48D8-86C3-2CE1460ECE78}")
#define Name_Imagetoolbar				_T("{5C53E3DE-0C5F-4C40-946E-A8C4E1C3101D}")
#define Name_Imagetoolbar_hot			_T("{7CE29BDE-A3D3-48D0-8713-538071001FFF}")
#define Name_Imagetoolbar_pushed		_T("{A2FECBAC-B787-48C9-9894-A548FFFFAEE8}")
#define Name_Imagetoolbar_disabled		_T("{CCE21DB5-F9E7-4FAE-954E-CC0D4C82E60D}")

//////////////////////////////////////////////////////////////////////////
#define Len_Account						(32)
#define Len_NickName					(32)
#define Len_WeChatCode					(MAX_PATH)
#define Len_CtrlClass					(50)
#define Len_CtrlIcon					(50)
#define Len_CtrlDllName					(50)
#define Len_CtrlAuthorName				(50)
#define Len_CtrlAuthorQuickMark			(50)

//////////////////////////////////////////////////////////////////////////
#define Key_Dui_PreviewProject			_T("PreviewProject")
#define Key_Dui_PreviewDuiName			_T("PreviewDuiName")
#define Key_Dui_PreviewParentWnd		_T("PreviewParentWnd")
#define Key_Dui_PreviewDpiScale			_T("PreviewDpiScale")
#define Key_Dui_PreviewFrameWnd			_T("PreviewFrameWnd")
#define Key_Dui_LaunchFromVS			_T("LaunchFromVS")
		
//////////////////////////////////////////////////////////////////////////
static TCHAR g_szPublicImage[][MAX_PATH][MAX_PATH]
{
	{ Name_ImageVScrollBk, _T("SkinDesign/VScrollBk.png") },
	{ Name_ImageVScrollThumb, _T("SkinDesign/VScrollThumb.png") },
	{ Name_ImageHScrollBk, _T("SkinDesign/HScrollBk.png") },
	{ Name_ImageHScrollThumb, _T("SkinDesign/HScrollThumb.png") },
	{ Name_ImageCheckExpand, _T("SkinDesign/CheckExpand.png") },
	{ Name_ImageCheckExpandSelect, _T("SkinDesign/CheckExpandSelect.png") },
	{ Name_ImageCheckExpandUnSelect, _T("SkinDesign/CheckExpandUnSelect.png") },
	{ Name_ImageSliderBk, _T("SkinDesign/SliderBk.bmp") },
	{ Name_ImageSliderThumb, _T("SkinDesign/SliderThumb.png") },
	{ Name_ImageBtnMin, _T("SkinDesign/BtnMin.png") },
	{ Name_ImageBtnMax, _T("SkinDesign/BtnMax.png") },
	{ Name_ImageBtnRestore, _T("SkinDesign/BtnRestore.png") },
	{ Name_ImageBtnClose, _T("SkinDesign/BtnClose.png") },
	{ Name_ImageBtnBk, _T("SkinDesign/BtnBk.png") },
	{ Name_ImageBtnArrowUp, _T("SkinDesign/BtnArrowUp.png") },
	{ Name_ImageBtnArrowDown, _T("SkinDesign/BtnArrowDown.png") },
	{ Name_ImageBtnLeft, _T("SkinDesign/BtnLeft.png") },
	{ Name_ImageBtnRight, _T("SkinDesign/BtnRight.png") },
	{ Name_ImageLogo, _T("SkinDesign/ImageLogo.png") },
	{ Name_ImageCheckBoxUnSelect, _T("SkinDesign/CheckBoxUnSelect.png") },
	{ Name_ImageCheckBoxSelect, _T("SkinDesign/CheckBoxSelect.png") },
	{ Name_ImageRadioBoxUnSelect, _T("SkinDesign/RadioBoxUnSelect.png") },
	{ Name_ImageRadioBoxSelect, _T("SkinDesign/RadioBoxSelect.png") },
	{ Name_ImageProgressBk, _T("SkinDesign/ProgressBk.png") },
	{ Name_ImageProgressFore, _T("SkinDesign/ProgressFore.png") },
	{ Name_ImageComboxBk, _T("SkinDesign/Combox.png") },
	{ Name_ImageCardMaster1, _T("SkinDesign/CardMaster1.jpg") },
	{ Name_ImageCardMaster2, _T("SkinDesign/CardMaster2.png") },
	{ Name_ImageCardMaster3, _T("SkinDesign/CardMaster3.png") },
	{ Name_ImageCardMaster4, _T("SkinDesign/CardMaster4.png") },
	{ Name_ImageCardMaster5, _T("SkinDesign/CardMaster5.png") },
	{ Name_ImageClockDial, _T("SkinDesign/ClockDial.png") },
	{ Name_ImageClockHour, _T("SkinDesign/ClockHour.png") },
	{ Name_ImageClockMinute, _T("SkinDesign/ClockMinute.png") },
	{ Name_ImageClockSecond, _T("SkinDesign/ClockSecond.png") },
	{ Name_ImageClockCenterDot, _T("SkinDesign/ClockCenterDot.png") },
	{ Name_ImageGif, _T("SkinDesign/Face.gif") },
	{ Name_ImageSequenceFrame, _T("SkinDesign/SequenceFrame.jpeg") },
	{ Name_Imagebtn_normal, _T("SkinDesign/btn_normal.png") },
	{ Name_Imagebtn_highlight, _T("SkinDesign/btn_highlight.png") },
	{ Name_Imagebtn_down, _T("SkinDesign/btn_down.png") },
	{ Name_ImageExpandToolsArrow, _T("SkinDesign/ExpandToolsArrow.svg") },
	{ Name_Imageformedit, _T("SkinDesign/formedit.bmp") },
	{ Name_Imagegray, _T("SkinDesign/gray.png") },
	{ Name_ImageIconColorRes, _T("SkinDesign/IconColorRes.png") },
	{ Name_ImageIconControl, _T("SkinDesign/IconControl.png") },
	{ Name_ImageIconControlNotify, _T("SkinDesign/IconControlNotify.png") },
	{ Name_ImageIconFontRes, _T("SkinDesign/IconFontRes.png") },
	{ Name_ImageIconImageRes, _T("SkinDesign/IconImageRes.png") },
	{ Name_ImageIconProject, _T("SkinDesign/IconProject.png") },
	{ Name_ImageIconProperty, _T("SkinDesign/IconProperty.png") },
	{ Name_ImageIconToolbox, _T("SkinDesign/IconToolbox.png") },
	{ Name_ImagelineH, _T("SkinDesign/lineH.png") },
	{ Name_ImagelineV, _T("SkinDesign/lineV.png") },
	{ Name_ImageRadioTabSelected, _T("SkinDesign/RadioTabSelected.png") },
	{ Name_ImageRadioTabUnSelected, _T("SkinDesign/RadioTabUnSelected.png") },
	{ Name_Imagetoolbar, _T("SkinDesign/toolbar.png") },
	{ Name_Imagetoolbar_hot, _T("SkinDesign/toolbar_hot.png") },
	{ Name_Imagetoolbar_pushed, _T("SkinDesign/toolbar_pushed.png") },
	{ Name_Imagetoolbar_disabled, _T("SkinDesign/toolbar_disabled.png") },
};

//////////////////////////////////////////////////////////////////////////

#endif