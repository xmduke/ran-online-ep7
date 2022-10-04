/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Feb 13 17:35:49 2004
 */
/* Compiler settings for C:\Temp\S2ACOM\S2ACOM.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#include "stdafx.h"


#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IS2Air = {0x4F91871D,0x9B96,0x45C2,{0xB6,0x70,0x2D,0x5D,0x9C,0xE5,0xFF,0x44}};


const IID LIBID_S2ACOMLib = {0xB4E8FA68,0xCEAE,0x448F,{0xAE,0x14,0x29,0x92,0xF5,0xA7,0x70,0x05}};


const CLSID CLSID_S2Air = {0x31C80217,0x22B5,0x4BA9,{0x8A,0x3C,0x0B,0x4F,0xC2,0x4A,0x44,0x1F}};


#ifdef __cplusplus
}
#endif

