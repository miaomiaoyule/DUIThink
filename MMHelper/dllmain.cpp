// dllmain.cpp : library load/unload entry
// Windows DLL  -> DllMain
// Linux/macOS/iOS/Android shared lib -> constructor/destructor (optional)
// Android JNI host may additionally export JNI_OnLoad in the app/JNI layer

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)

#if defined(DuiPlatform_SDL)
#ifndef APIENTRY
#ifdef _MSC_VER
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif
#ifndef DLL_PROCESS_ATTACH
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3
#define DLL_PROCESS_DETACH 0
#endif
#endif

BOOL APIENTRY DllMain(HMODULE /*hModule*/,
	DWORD ul_reason_for_call,
	LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#else

// ELF/Mach-O shared library: no DllMain.
// Optional load/unload hooks (GCC/Clang). Prefer explicit Init/UnInit in app code.
#if defined(__GNUC__) || defined(__clang__)
__attribute__((constructor)) static void MMHelper_LibraryOnLoad(void)
{
}

__attribute__((destructor)) static void MMHelper_LibraryOnUnload(void)
{
}
#endif

#endif
