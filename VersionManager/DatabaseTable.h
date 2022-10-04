#ifndef DATABASE_TABLE_H_
#define DATABASE_TABLE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct FolderList
{
	int nIdx;
	CString strName;
};

struct FileList
{
	int nIdx;
	int nVer;
    int nFtp;
	CString strFile;
	CString strDate;
	CString strDesc;
#ifdef CHINAPARAM
	CString strMD5;
#endif
};

struct CompressFile
{
	CString strSrc; // Full Path
	CString strTarget; // Full TargetPath
	CString strFileName; // File Name
    CString strPurePath;
	CString strMD5;
	int		nDir;
};

struct FullFileList
{
    int nIdx;
    CString strFolder;
    CString strFileName;
#ifdef CHINAPARAM
	CString strMD5;
#endif
    int     nVersion;	
};

enum emFileList
{	
	DATE_SIZE     = 21,
	MD5_SIZE      = 33, // MD5값 32자리 + 종료문자 1 = 33
	FILENAME_SIZE = 64,
	SUBPATH_SIZE  = 128,
	DESC_SIZE     = 256,
};

struct RanFileList
{ 
    char szFileName[FILENAME_SIZE];
    char szSubPath[SUBPATH_SIZE];
	int nVersion;
#ifdef CHINAPARAM
	char szMD5[MD5_SIZE];
#endif    
};

#endif // DATABASE_TABLE_H_
