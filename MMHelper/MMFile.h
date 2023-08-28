#ifndef __MM_FILE_H__
#define __MM_FILE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
//file type
enum enMMFileType
{
	FileType_None,
	FileType_Dir,
	FileType_Doc,
	FileType_Image,
	FileType_Movie,
	FileType_Zip,
	FileType_Exe,
	FileType_Lnk,
	FileType_File,
};

//file encode
enum enMMFileEncode
{
	FileEncode_Null = -1,
	FileEncode_Ansi = 0,
	FileEncode_Unicode,
	FileEncode_Unicode_big_endian,
	FileEncode_UTF8_Bom
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMFile
{
public:
	//************************************
	// Description:	combin folder + filename
	// Parameter: 	LPCTSTR lpszPath ����C:\Text or C:\Text\
	// Parameter: 	LPCTSTR lpszFileName ����Text.txt
	//************************************
	static CMMString CombinFile(LPCTSTR lpszPath, LPCTSTR lpszFileName);

	//************************************
	// Description:	parse filetype from file or ext
	// Parameter: 	LPCTSTR lpszFile ����C:\Text.txt or Text.txt or txt or C:\Text
	//************************************
	static enMMFileType ParseFileType(LPCTSTR lpszFile);

	//************************************
	// Description:	��һ������·���ַ����������ļ�������չ��
	// Parameter: 	LPCTSTR lpszFileName ����C:\Text.txt �� Text.txt
	// Parameter: 	CMMString & strName ����Text
	// Parameter: 	CMMString & strExt ����txt
	//************************************
	static bool ParseFileName(LPCTSTR lpszFileName, CMMString &strName, CMMString &strExt);

	//************************************
	// Description:	��һ������·���ַ���������·�����ļ���
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	// Parameter: 	CMMString & strPath ����C:\
	// Parameter: 	CMMString & strName ����Text.txt
	//************************************
	static bool ParseFilePathName(LPCTSTR lpszFileFull, CMMString &strPath, CMMString &strName);

	//************************************
	// Description:	
	// Parameter: 	FILE * pFile
	//************************************
	static enMMFileEncode GetFileEncode(FILE *pFile);

	//************************************
	// Description:	
	// Parameter: 	const char * pStr �����ļ������ж��ĵ���ʽ������Ӧ�� >= 2�����򷵻�[FileEncode_Null]
	//************************************
	static enMMFileEncode GetFileEncode(const char *pStr);

	//************************************
	// Description:	�����ļ���PCID
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static PCIDLIST_ABSOLUTE GetPCIDLFromPath(LPCTSTR lpszFileFull);

	//************************************
	// Description:	��ȡ�ļ�����
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	// Parameter: 	std::vector<BYTE> vecData
	//************************************
	static bool GetFileData(IN LPCTSTR lpszFileFull, OUT std::vector<BYTE> &vecData);

	//************************************
	// Description:	��ȡ�ļ�����
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	// Parameter: 	CMMString &strData
	//************************************
	static bool GetFileData(IN LPCTSTR lpszFileFull, OUT CMMString &strData);

	//************************************
	// Description:	��ȡ�ļ�tip
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static CMMString GetFileTip(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return local file modifytime
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static SYSTEMTIME GetFileModifyTime(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return local file createtime
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static SYSTEMTIME GetFileCreateTime(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return local file last access time
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static SYSTEMTIME GetFileLastAccessTime(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return file size
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static uint64_t GetFileSize(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return file version
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static bool GetFileVersion(IN LPCTSTR lpszFileFull, OUT DWORD &dwVersionInfo);

	//************************************
	// Description:	return file version
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	//************************************
	static CMMString GetFileVersion(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	get folder of dir
	// Parameter: 	LPCTSTR lpszDirFull ����C:\windows
	//************************************
	static std::vector<CMMString> GetFolderOfDir(IN LPCTSTR lpszDirFull);

	//************************************
	// Description:	get file(uninclude folder) of dir
	// Parameter: 	LPCTSTR lpszDirFull ����C:\windows
	//************************************
	static std::vector<CMMString> GetFileOfDir(IN LPCTSTR lpszDirFull);

	//************************************
	// Description:	get files of dir
	// Parameter: 	LPCTSTR lpszDirFull ����C:\windows
	//************************************
	static std::vector<CMMString> GetFilesOfDir(IN LPCTSTR lpszDirFull);

	//************************************
	// Description:	return file version
	// Parameter: 	LPCTSTR lpszFileFull ����C:\Text.txt 
	// Parameter: 	CMMString &strData 
	//************************************
	static bool WriteFileData(IN LPCTSTR lpszFileFull, IN CMMString &strData, bool bClearOld = false);

	//************************************
	// Description:	�ݹ����·���µĺ����ض����ֵ��ļ�
	// Parameter: 	CMMString strPath ����C:\Program Files (x86)
	// Parameter: 	CMMString strHasName ����qq
	//************************************
	static bool RemoveFileByHasName(CMMString strPath, CMMString strHasName);

	//************************************
	// Description:	open folder and select file 
	// Parameter: 	CMMString strFileFull ����C:\user\desktop\test.txt
	//************************************
	static bool OpenFolderAndSelectFile(CMMString strFileFull);

	//************************************
	// Description:	copy or move file or folder to dest 
	// Parameter: 	CMMString strSrc ����C:\user\desktop\test.txt C:\user\desktop\test
	// Parameter: 	CMMString strDest ����D:\user\desktop\test.txt D:\user\desktop\test
	//************************************
	static bool OperatorFileOrFolder(CMMString strSrc, CMMString strDest, int nOperator = FO_COPY, bool bUI = false, bool bMultiFile = false);
};

//////////////////////////////////////////////////////////////////////////

#endif
