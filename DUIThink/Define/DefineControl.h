#ifndef __DUIDEFINECONTROL_H__
#define __DUIDEFINECONTROL_H__

#pragma once					

//////////////////////////////////////////////////////////////////////////
#define DuiInitAttriVisible(Attri, Visible) { Attri.SetVisible(Visible); }
#define DuiModifyAttriDescribe(Attri, Describe) { Attri.SetAttributeDescr(Describe); }
#define DuiModifyAttriName(Attri, Name) { Attri.SetAttributeName(Name); }
#define DuiGetAttriGroupIndex(Attri) (CDUIGlobal::GetAttributeGroupIndex(&Attri))
#define DuiSetAttriGroupIndex(Attri, nIndex) (CDUIGlobal::SetAttributeGroupIndex(&Attri, nIndex))

//////////////////////////////////////////////////////////////////////////
//describe
#define Dui_Prop_Object								(_T("PropertyObj"))
#define Dui_Wnd_Manager								(_T("WndManager"))

#define Dui_Ctrl_Control							(_T("ControlCtrl"))
#define Dui_Ctrl_Static								(_T("StaticCtrl"))
#define Dui_Ctrl_Edit								(_T("EditCtrl"))
#define Dui_Ctrl_RichEdit							(_T("RichEditCtrl"))
#define Dui_Ctrl_Button								(_T("ButtonCtrl"))
#define Dui_Ctrl_CheckBox							(_T("CheckBoxCtrl"))
#define Dui_Ctrl_RadioBox							(_T("RadioBoxCtrl"))
#define Dui_Ctrl_Progress							(_T("ProgressCtrl"))
#define Dui_Ctrl_Slider								(_T("SliderCtrl"))
#define Dui_Ctrl_ScrollBar							(_T("ScrollBarCtrl"))
#define Dui_Ctrl_Combox								(_T("ComboxCtrl"))
#define Dui_Ctrl_Animate							(_T("AnimateCtrl"))
#define Dui_Ctrl_MenuItem							(_T("MenuItemCtrl"))
#define Dui_Ctrl_Menu								(_T("MenuCtrl"))
#define Dui_Ctrl_HotKey								(_T("HotKeyCtrl"))

#define Dui_Ctrl_Container							(_T("ContainerCtrl"))
#define Dui_Ctrl_HorizontalLayout					(_T("HorizontalLayoutCtrl"))
#define Dui_Ctrl_VerticalLayout						(_T("VerticalLayoutCtrl"))
#define Dui_Ctrl_ListHeaderItem						(_T("ListHeaderItemCtrl"))
#define Dui_Ctrl_ListHeader							(_T("ListHeaderCtrl"))
#define Dui_Ctrl_ListItem							(_T("ListItemCtrl"))
#define Dui_Ctrl_ListView							(_T("ListViewCtrl"))
#define Dui_Ctrl_IPAddress							(_T("IPAddressCtrl"))
#define Dui_Ctrl_Spin								(_T("SpinCtrl"))
#define Dui_Ctrl_Calendar							(_T("CalendarCtrl"))
#define Dui_Ctrl_RotateMenuItem						(_T("RotateMenuItemCtrl"))
#define Dui_Ctrl_RotateMenu							(_T("RotateMenuCtrl"))
#define Dui_Ctrl_Instrument							(_T("InstrumentCtrl"))
#define Dui_Ctrl_Clock								(_T("ClockCtrl"))
#define Dui_Ctrl_DateTime							(_T("DateTimeCtrl"))
#define Dui_Ctrl_TreeView							(_T("TreeViewCtrl"))
#define Dui_Ctrl_TreeNode							(_T("TreeNodeCtrl"))
#define Dui_Ctrl_TabLayout							(_T("TabLayoutCtrl"))
#define Dui_Ctrl_WkeBrowser							(_T("WkeBrowserCtrl"))

//////////////////////////////////////////////////////////////////////////

#endif

