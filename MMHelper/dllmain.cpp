// dllmain.cpp : library load/unload entry
// Windows DLL  -> DllMain
// Linux/macOS/iOS/Android shared lib -> constructor/destructor (optional)
// Android JNI host may additionally export JNI_OnLoad in the app/JNI layer

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)

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
