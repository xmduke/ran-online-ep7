////////////////////////////////////////////////////////////////////////////////
// 반드시 Unicode 를 지원해야 한다.
////////////////////////////////////////////////////////////////////////////////
#ifndef _MINLIB_H_
#define _MINLIB_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

////////////////////////////////////////////////////////////////////////////////
/// Data type define
#ifndef BYTE
typedef unsigned char	BYTE;
#endif 

#ifndef WORD
typedef unsigned short	WORD;
#endif

#ifndef DWORD
typedef unsigned long	DWORD;
#endif

#ifndef ULONG
typedef unsigned long	ULONG;
#endif

#ifndef UINT
typedef unsigned int	UINT;
#endif

#ifndef BOOL
typedef int	BOOL;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifdef _UNICODE
typedef wchar_t	tchar;
#else
typedef char tchar;
#endif

#ifdef _UNICODE
typedef std::string tstring;
#else
typedef std::wstring tstring;
#endif

////////////////////////////////////////////////////////////////////////////////
/// Macro define
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	{ if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

/// Combine low & high to form a 32 bit quantity.
#ifndef MAKEULONG
#define MAKEULONG(low, high)	((ULONG)(((USHORT)(low)) | ((ULONG)((USHORT)(high))) << 16))
#endif

#ifndef MAKEWORD
#define MAKEWORD(low, high)	((WORD)(((BYTE)((DWORD_PTR)(low) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(high) & 0xff))) << 8))
#endif

#ifndef MAKELONG
#define MAKELONG(low, high)	((LONG)(((WORD)((DWORD_PTR)(low) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(high) & 0xffff))) << 16))
#endif

#ifndef LOWORD
#define LOWORD(l)	((WORD)((DWORD_PTR)(l) & 0xffff))
#endif

#ifndef HIWORD
#define HIWORD(l)	((WORD)((DWORD_PTR)(l) >> 16))
#endif

#ifndef LOBYTE
#define LOBYTE(w)	((BYTE)((DWORD_PTR)(w) & 0xff))
#endif

#ifndef HIBYTE
#define HIBYTE(w)	((BYTE)((DWORD_PTR)(w) >> 8))
#endif

namespace minLib
{
	/// 라이브러리를 초기화 한다.
	void initLib();
	void shutDownLib();
};

#endif // _MINLIB_H_
