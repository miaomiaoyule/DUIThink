#ifndef DUITHINK_API
	#ifdef DUITHINKLIB
		#define MMHELPLIB
		#define DUITHINK_API
	#else
		#ifdef DUITHINKSDK
			#define DUITHINK_API __declspec(dllexport)
		#else
			#define DUITHINK_API __declspec(dllimport)
		#endif
	#endif
#endif

#define DUITHINK_COMDAT __declspec(selectany)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

//////////////////////////////////////////////////////////////////////////
#include <comdef.h>
#include <gdiplus.h>
#include <ShlDisp.h>

using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Imm32.lib")

//////////////////////////////////////////////////////////////////////////
#include "../MMHelper/MMHelperHead.h"
#include "../ThirdDepend/tinyxml2/tinyxml2.h"
#include "../ThirdDepend/webp/src/webp/decode.h"
#include "../ThirdDepend/webp/src/webp/demux.h"
#include "../ThirdDepend/webp/src/webp/encode.h"
#include "../ThirdDepend/webp/src/webp/mux.h"
#include <olectl.h>
using namespace tinyxml2;

#if defined(DuiPlatform_SDL)
#else
	#define GDIPVER 0x0110
#endif

#if _MSC_VER < 1900
#ifdef _DEBUG
	#if defined _DLL
		#ifdef _WIN64
		#pragma comment(lib, "../lib/tinyxml2.201364D.lib")
		#pragma comment(lib, "../lib/MMHelper.201364D.lib")
		#else
		#pragma comment(lib, "../lib/tinyxml2.2013D.lib")
		#pragma comment(lib, "../lib/MMHelper.2013D.lib")
		#endif
	#else
		#ifdef _WIN64
		#pragma comment(lib, "../lib/tinyxml2.201364_MT_D.lib")
		#pragma comment(lib, "../lib/MMHelper.201364_MT_D.lib")
		#else
		#pragma comment(lib, "../lib/tinyxml2.2013_MT_D.lib")
		#pragma comment(lib, "../lib/MMHelper.2013_MT_D.lib")
		#endif
	#endif	
#else
	#ifdef _DLL
		#ifdef _WIN64
		#pragma comment(lib, "../lib/tinyxml2.201364.lib")
		#pragma comment(lib, "../lib/MMHelper.201364.lib")
		#else
		#pragma comment(lib, "../lib/tinyxml2.2013.lib")
		#pragma comment(lib, "../lib/MMHelper.2013.lib")
		#endif
	#else
		#ifdef _WIN64	
		#pragma comment(lib, "../lib/tinyxml2.201364_MT.lib")
		#pragma comment(lib, "../lib/MMHelper.201364_MT.lib")
		#else	
		#pragma comment(lib, "../lib/tinyxml2.2013_MT.lib")
		#pragma comment(lib, "../lib/MMHelper.2013_MT.lib")
		#endif
	#endif
#endif
#else
#if defined _DEBUG
	#ifdef _DLL
		#ifdef _WIN64
		#pragma comment(lib, "../lib/libwebp.201764D.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.201764D.lib")
		#pragma comment(lib, "../lib/libwebpdemux.201764D.lib")
		#pragma comment(lib, "../lib/tinyxml2.201764D.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.201764_Static_D.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.201764D.lib")
		#endif
		#else
		#pragma comment(lib, "../lib/libwebp.2017D.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.2017D.lib")
		#pragma comment(lib, "../lib/libwebpdemux.2017D.lib")
		#pragma comment(lib, "../lib/tinyxml2.2017D.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.2017_Static_D.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.2017D.lib")
		#endif
		#endif
	#else
		#ifdef _WIN64
		#pragma comment(lib, "../lib/libwebp.201764_MT_D.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.201764_MT_D.lib")
		#pragma comment(lib, "../lib/libwebpdemux.201764_MT_D.lib")
		#pragma comment(lib, "../lib/tinyxml2.201764_MT_D.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.201764_Static_MT_D.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.201764_MT_D.lib")
		#endif
		#else
		#pragma comment(lib, "../lib/libwebp.2017_MT_D.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.2017_MT_D.lib")
		#pragma comment(lib, "../lib/libwebpdemux.2017_MT_D.lib")
		#pragma comment(lib, "../lib/tinyxml2.2017_MT_D.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.2017_Static_MT_D.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.2017_MT_D.lib")
		#endif
		#endif
	#endif	
#else
	#ifdef _DLL
		#ifdef _WIN64
		#pragma comment(lib, "../lib/libwebp.201764.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.201764.lib")
		#pragma comment(lib, "../lib/libwebpdemux.201764.lib")
		#pragma comment(lib, "../lib/tinyxml2.201764.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.201764_Static.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.201764.lib")
		#endif
		#else
		#pragma comment(lib, "../lib/libwebp.2017.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.2017.lib")
		#pragma comment(lib, "../lib/libwebpdemux.2017.lib")
		#pragma comment(lib, "../lib/tinyxml2.2017.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.2017_Static.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.2017.lib")
		#endif
		#endif
	#else
		#ifdef _WIN64	
		#pragma comment(lib, "../lib/libwebp.201764_MT.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.201764_MT.lib")
		#pragma comment(lib, "../lib/libwebpdemux.201764_MT.lib")
		#pragma comment(lib, "../lib/tinyxml2.201764_MT.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.201764_Static_MT.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.201764_MT.lib")
		#endif
		#else	
		#pragma comment(lib, "../lib/libwebp.2017_MT.lib")
		#pragma comment(lib, "../lib/libwebpdecoder.2017_MT.lib")
		#pragma comment(lib, "../lib/libwebpdemux.2017_MT.lib")
		#pragma comment(lib, "../lib/tinyxml2.2017_MT.lib")
		#ifdef MMHELPLIB
		#pragma comment(lib, "../lib/MMHelper.2017_Static_MT.lib")
		#else
		#pragma comment(lib, "../lib/MMHelper.2017_MT.lib")
		#endif
		#endif
	#endif
#endif
#endif

//////////////////////////////////////////////////////////////////////////
#include "Define/DefineBase.h"
#include "Define/DefineControl.h"
#include "Define/DefineResource.h"
#include "Define/DefineAttribute.h"
#include "Define/DefineProp.h"
#include "Define/DefineMessage.h"
#include "Define/DefineWndManager.h"

#include "DUIUtils/stb_image.h"
#include "DUIUtils/XUnzip.h"
#include "DUIUtils/DUISize.h"
#include "DUIUtils/DUIPoint.h"
#include "DUIUtils/DUIRect.h"

#include "DUIExport/DefineBase.h"
#include "DUIExport/DefineMessage.h"
#include "DUIExport/DefineControl.h"
#include "DUIExport/DefineAttribute.h"
#include "DUIExport/DefineAnimation.h"
#include "DUIExport/DefineResource.h"
#include "DUIExport/IDuiInterface.h"
#include "DUIExport/DUIStruct.h"

#include "DUIResource/DUIResourceBase.h"
#include "DUIResource/DUIFontBase.h"
#include "DUIResource/DUIImageBase.h"
#include "DUIResource/DUIColorBase.h"

#include "DUIAttribute/DUIAttributeObject.h"
#include "DUIAttribute/DUIAttributeGroup.h"
#include "DUIAttribute/DUIAttributePosition.h"
#include "DUIAttribute/DUIAttributeBool.h"
#include "DUIAttribute/DUIAttributeCombox.h"
#include "DUIAttribute/DUIAttributeCursor.h"
#include "DUIAttribute/DUIAttributeDuiSelect.h"
#include "DUIAttribute/DUIAttributeTabSelect.h"
#include "DUIAttribute/DUIAttributeSize.h"
#include "DUIAttribute/DUIAttributeRect.h"
#include "DUIAttribute/DUIAttriImageSection.h"
#include "DUIAttribute/DUINumber/DUIAttributeFloat.h"
#include "DUIAttribute/DUINumber/DUIAttributeLong.h"
#include "DUIAttribute/DUINumber/DUIAttributeULong.h"
#include "DUIAttribute/DUINumber/DUIAttributeCtrlID.h"
#include "DUIAttribute/DUIText/DUIAttributeText.h"
#include "DUIAttribute/DUIText/DUIAttributeTextStyle.h"
#include "DUIAttribute/DUIText/DUIAttributeRichText.h"
#include "DUIAttribute/DUIAttributeColor.h"
#include "DUIAttribute/DUIAttributeColorSwitch.h"
#include "DUIAttribute/DUIAttributeHotKey.h"

#include "DUICore/DUIPropertyObject.h"
#include "DUICore/DUIRender.h"
#include "DUICore/DUINotifyPump.h"
#include "DUICore/DUIAnimation.h"
#include "DUICore/DUIPlatform/DUIWndBase.h"
#include "DUICore/DUIPlatform/DUIWndSDL.h"
#include "DUICore/DUIPlatform/DUIWndWin32.h"
#include "DUICore/DUIWnd.h"

#include "DUIControl/DUIControlBase.h"
#include "DUIControl/DUIStaticCtrl.h"
#include "DUIControl/DUIProgressCtrl.h"
#include "DUIControl/DUIProgressCircleCtrl.h"
#include "DUIControl/DUISliderCtrl.h"
#include "DUIControl/DUIScrollBarCtrl.h"
#include "DUIControl/DUIButtonCtrl.h"
#include "DUIControl/DUICheckBoxCtrl.h"
#include "DUIControl/DUIRadioBoxCtrl.h"
#include "DUIControl/DUIEditCtrl.h"
#include "DUIControl/DUIAnimateCtrl.h"
#include "DUIControl/DUIContainerCtrl.h"
#include "DUIControl/DUIVerticalLayoutCtrl.h"
#include "DUIControl/DUIHorizontalLayoutCtrl.h"
#include "DUIControl/DUIThinkEditCtrl.h"
#include "DUIControl/DUIRichEditCtrl.h"
#include "DUIControl/DUITabLayoutCtrl.h"
#include "DUIControl/DUIIPAddressCtrl.h"
#include "DUIControl/DUISpinCtrl.h"
#include "DUIControl/DUIListHeaderItemCtrl.h"
#include "DUIControl/DUIListHeaderCtrl.h"
#include "DUIControl/DUIListItemCtrl.h"
#include "DUIControl/DUIListViewCtrl.h"
#include "DUIControl/DUIComboxCtrl.h"
#include "DUIControl/DUITreeNodeCtrl.h"
#include "DUIControl/DUITreeViewCtrl.h"
#include "DUIControl/DUIMenuCtrl.h"
#include "DUIControl/DUIHotKeyCtrl.h"
#include "DUIControl/DUICalendarCtrl.h"
#include "DUIControl/DUIRotateMenuCtrl.h"
#include "DUIControl/DUIInstrumentCtrl.h"
#include "DUIControl/DUIClockCtrl.h"
#include "DUIControl/DUIGroupCtrl.h"
#include "DUIControl/DUIWaterCtrl.h"
#include "DUIControl/DUIKLineChartCtrl.h"
#include "DUIControl/DUIDateTimeCtrl.h"
#include "DUIControl/DUIWebBrowserCtrl.h"

#include "DUIUtils/Utils.h"

#include "DUIManager/DUIFactory.h"
#include "DUIManager/DUIGlobal.h"

#include "DUIXmlPack/DUIXmlPack.h"

extern DUITHINK_API tagDuiMenuCmd g_DuiMenuCmd;
extern DUITHINK_API tagDuiCalendarCmd g_DuiCalendarCmd;
extern DUITHINK_API CDUIMenuWnd * g_pDuiMenuWndRoot;
extern DUITHINK_API CDUICalendarWnd * g_pDuiCalendarWnd;
DUITHINK_API bool DuiIsWindowVisible(HWND hWnd);
DUITHINK_API bool DuiIsIconic(HWND hWnd);
DUITHINK_API bool DuiIsZoomed(HWND hWnd);
DUITHINK_API void DuiInvalidateRect(HWND hWnd, LPCRECT lpRect, bool bErase);
DUITHINK_API BOOL DuiScreenToClient(HWND hWnd, LPPOINT lpPoint);
DUITHINK_API BOOL DuiClientToScreen(HWND hWnd, LPPOINT lpPoint);
DUITHINK_API void DuiGetCursorPos(LPPOINT lpPoint);
DUITHINK_API void DuiSetCursorPos(int X, int Y);
DUITHINK_API void DuiGetCaretPos(LPPOINT lpPoint);
DUITHINK_API HWND DuiGetParent(HWND hWnd);
DUITHINK_API HWND DuiGetFocus();
DUITHINK_API void DuiGetWindowRect(HWND hWnd, LPRECT lpRect);
DUITHINK_API void DuiGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo);
DUITHINK_API void DuiSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
DUITHINK_API void DuiMoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);
DUITHINK_API void DuiMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
DUITHINK_API void DuiSetForegroundWindow(HWND hWnd);
DUITHINK_API void DuiSetFocus(HWND hWnd);
DUITHINK_API void DuiShowWindow(HWND hWnd, int nCmdShow);
DUITHINK_API bool DuiSendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DUITHINK_API HMONITOR DuiMonitorFromWindow(HWND hWnd, DWORD dwFlags);
DUITHINK_API void DuiUpdateWindow(HWND hWnd);
//////////////////////////////////////////////////////////////////////////