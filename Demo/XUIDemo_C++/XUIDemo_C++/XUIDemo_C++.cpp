// DUIThinkDemo_C++.cpp : 定义应用程序的入口点。
//

#include "StdAfx.h"
#include "DUIThinkDemo_C++.h"
#if defined(__APPLE__)
#include <SDL3/SDL_main.h>
#endif
#if defined(__ANDROID__)
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_system.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>
#endif

#if defined(__ANDROID__)
namespace
{
	void MakeDirRecursive(const std::string &strPath)
	{
		for (size_t i = 1; i < strPath.size(); ++i)
		{
			if ('/' == strPath[i])
			{
				SDL_CreateDirectory(strPath.substr(0, i).c_str());
			}
		}
		SDL_CreateDirectory(strPath.c_str());
	}

	SDL_EnumerationResult SDLCALL RemoveTreeEntry(void * /*userdata*/, const char *dirname, const char *fname)
	{
		if (NULL == dirname || NULL == fname || 0 == fname[0])
		{
			return SDL_ENUM_CONTINUE;
		}

		std::string strPath = dirname;
		if (false == strPath.empty() && '/' != strPath.back())
		{
			strPath += '/';
		}
		strPath += fname;

		SDL_PathInfo info = {};
		if (SDL_GetPathInfo(strPath.c_str(), &info) && SDL_PATHTYPE_DIRECTORY == info.type)
		{
			SDL_EnumerateDirectory(strPath.c_str(), RemoveTreeEntry, NULL);
		}
		SDL_RemovePath(strPath.c_str());
		return SDL_ENUM_CONTINUE;
	}

	void RemoveTree(const std::string &strPath)
	{
		SDL_PathInfo info = {};
		if (false == SDL_GetPathInfo(strPath.c_str(), &info))
		{
			return;
		}
		if (SDL_PATHTYPE_DIRECTORY == info.type)
		{
			SDL_EnumerateDirectory(strPath.c_str(), RemoveTreeEntry, NULL);
		}
		SDL_RemovePath(strPath.c_str());
	}

	bool CopyAssetFile(AAssetManager *pMgr, const std::string &strAsset, const std::string &strDest)
	{
		AAsset *pAsset = AAssetManager_open(pMgr, strAsset.c_str(), AASSET_MODE_BUFFER);
		if (NULL == pAsset)
		{
			return false;
		}

		const size_t nSize = (size_t)AAsset_getLength(pAsset);
		const void *pData = AAsset_getBuffer(pAsset);
		if (nSize > 0 && NULL == pData)
		{
			SDL_Log("AAsset_getBuffer failed: %s", strAsset.c_str());
			AAsset_close(pAsset);
			return false;
		}

		const size_t nSlash = strDest.rfind('/');
		if (std::string::npos != nSlash)
		{
			MakeDirRecursive(strDest.substr(0, nSlash));
		}

		SDL_IOStream *pOut = SDL_IOFromFile(strDest.c_str(), "wb");
		if (NULL == pOut)
		{
			SDL_Log("write failed: %s (%s)", strDest.c_str(), SDL_GetError());
			AAsset_close(pAsset);
			return false;
		}

		const bool bOk = (0 == nSize) || (nSize == SDL_WriteIO(pOut, pData, nSize));
		SDL_CloseIO(pOut);
		AAsset_close(pAsset);
		return bOk;
	}

	bool CopyAssetDir(JNIEnv *pEnv, jobject jAssets, jmethodID midList, AAssetManager *pMgr,
		const std::string &strAssetDir, const std::string &strDestDir)
	{
		jstring jPath = pEnv->NewStringUTF(strAssetDir.c_str());
		jobjectArray jNames = (jobjectArray)pEnv->CallObjectMethod(jAssets, midList, jPath);
		pEnv->DeleteLocalRef(jPath);
		if (pEnv->ExceptionCheck())
		{
			pEnv->ExceptionDescribe();
			pEnv->ExceptionClear();
			return false;
		}
		if (NULL == jNames)
		{
			SDL_Log("AssetManager.list failed: %s", strAssetDir.c_str());
			return false;
		}

		const jsize nCount = pEnv->GetArrayLength(jNames);
		if (0 == nCount)
		{
			pEnv->DeleteLocalRef(jNames);
			SDL_Log("asset dir empty: %s", strAssetDir.c_str());
			return false;
		}

		MakeDirRecursive(strDestDir);

		bool bOk = true;
		for (jsize i = 0; i < nCount && bOk; ++i)
		{
			jstring jName = (jstring)pEnv->GetObjectArrayElement(jNames, i);
			const char *pszName = pEnv->GetStringUTFChars(jName, NULL);
			const std::string strName = pszName ? pszName : "";
			pEnv->ReleaseStringUTFChars(jName, pszName);
			pEnv->DeleteLocalRef(jName);

			const std::string strChildAsset = strAssetDir + "/" + strName;
			const std::string strChildDest = strDestDir + "/" + strName;
			if (false == CopyAssetFile(pMgr, strChildAsset, strChildDest))
			{
				bOk = CopyAssetDir(pEnv, jAssets, midList, pMgr, strChildAsset, strChildDest);
			}
		}

		pEnv->DeleteLocalRef(jNames);
		return bOk;
	}

	bool CopyAndroidDuiProj(CMMString &strProjFile)
	{
		const char *pszInternal = SDL_GetAndroidInternalStoragePath();
		if (NULL == pszInternal || 0 == pszInternal[0])
		{
			SDL_Log("SDL_GetAndroidInternalStoragePath failed");
			return false;
		}

		JNIEnv *pEnv = (JNIEnv *)SDL_GetAndroidJNIEnv();
		jobject jActivity = (jobject)SDL_GetAndroidActivity();
		if (NULL == pEnv || NULL == jActivity)
		{
			SDL_Log("Android JNI unavailable: %s", SDL_GetError());
			if (jActivity) pEnv->DeleteLocalRef(jActivity);
			return false;
		}

		jclass clsActivity = pEnv->GetObjectClass(jActivity);
		jmethodID midGetAssets = pEnv->GetMethodID(clsActivity, "getAssets", "()Landroid/content/res/AssetManager;");
		jobject jAssets = pEnv->CallObjectMethod(jActivity, midGetAssets);
		pEnv->DeleteLocalRef(clsActivity);
		pEnv->DeleteLocalRef(jActivity);
		if (NULL == jAssets)
		{
			SDL_Log("getAssets failed");
			return false;
		}

		jclass clsAssets = pEnv->GetObjectClass(jAssets);
		jmethodID midList = pEnv->GetMethodID(clsAssets, "list", "(Ljava/lang/String;)[Ljava/lang/String;");
		pEnv->DeleteLocalRef(clsAssets);
		AAssetManager *pMgr = AAssetManager_fromJava(pEnv, jAssets);
		if (NULL == midList || NULL == pMgr)
		{
			SDL_Log("AAssetManager list/fromJava failed");
			pEnv->DeleteLocalRef(jAssets);
			return false;
		}

		const std::string strDestRoot = std::string(pszInternal) + "/DUIThinkDemo_C++.DuiProj";
		RemoveTree(strDestRoot);

		const bool bOk = CopyAssetDir(pEnv, jAssets, midList, pMgr, "DUIThinkDemo_C++.DuiProj", strDestRoot);
		pEnv->DeleteLocalRef(jAssets);
		if (false == bOk)
		{
			SDL_Log("copy DuiProj from APK assets failed");
			return false;
		}

		const std::string strProjPath = strDestRoot + "/DUIThinkDemo_C++.DuiProj";
		SDL_PathInfo info = {};
		if (false == SDL_GetPathInfo(strProjPath.c_str(), &info)
			|| SDL_PATHTYPE_FILE != info.type
			|| 0 == info.size)
		{
			SDL_Log("copied project file missing or empty: %s", strProjPath.c_str());
			return false;
		}

		SDL_Log("copied DuiProj %llu bytes -> %s", (unsigned long long)info.size, strProjPath.c_str());
		strProjFile = CA2CT(strProjPath);
		return true;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(DuiPlatform_SDL) && !defined(_WIN32)
int main(int argc, char *argv[])
{
	HINSTANCE hInstance = NULL;
	CMMString strCmdLine;
	for (int i = 1; i < argc; ++i)
	{
		if (i > 1) strCmdLine += _T(" ");
		strCmdLine += CA2CT(argv[i]);
	}

	LPCTSTR lpCmdLine = strCmdLine;
#else
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#endif
	CDUIGlobal::GetInstance()->Init(hInstance);
	//CDUIGlobal::GetInstance()->SetDpi(CMMDpi::GetDpiOfMainMonitor());

#if defined(__ANDROID__)
	CMMString strProjFile;
	if (false == CopyAndroidDuiProj(strProjFile))
	{
		CDUIGlobal::GetInstance()->UnInit();
		return 1;
	}

	CDUIGlobal::GetInstance()->LoadProjectFromFile(strProjFile);
#else
	CDUIGlobal::GetInstance()->LoadProjectFromFile(_T("../DUIThinkDemo_C++.DuiProj/DUIThinkDemo_C++.DuiProj"));
#endif

	CMMDpi::SetProcessDPIAwareness(enMMPROCESS_DPI_AWARENESS::MMPROCESS_PER_MONITOR_DPI_AWARE);

	//cmd line
	CMMCommandLine CmdLine(lpCmdLine);
	CMMString strValue;

	do
	{
		//dpi dlg
		CmdLine.Get(Name_DpiDlgKey, strValue);
		if (false == strValue.empty())
		{
			//have
			HWND hWndDpiDlg = FindWindow(_T("CDlgDpi"), NULL);
			if (hWndDpiDlg) break;

			//create
			CMMString strDuiName = strValue;

			CmdLine.Get(Name_ParentWndKey, strValue);
			HWND hWndParent = (HWND)(_tcstoul(strValue, NULL, 10));

			CDlgDpi DlgDpi(strDuiName, hWndParent);
			DlgDpi.Create(hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
			DlgDpi.DoBlock();

			break;
		}

		//normal demo
		CDlgDemo DlgDemo;
		DlgDemo.Create(NULL, _T("DUIThinkDemo"), DUI_WNDSTYLE_FRAME, DUI_WNDSTYLE_EX_FRAME);
		DlgDemo.DoModal();

	} while (false);

	CDUIGlobal::GetInstance()->UnInit();

	return 0;
}
