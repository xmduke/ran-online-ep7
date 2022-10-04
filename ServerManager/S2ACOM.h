/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Feb 13 17:35:49 2004
 */
/* Compiler settings for C:\Temp\S2ACOM\S2ACOM.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __S2ACOM_h__
#define __S2ACOM_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IS2Air_FWD_DEFINED__
#define __IS2Air_FWD_DEFINED__
typedef interface IS2Air IS2Air;
#endif 	/* __IS2Air_FWD_DEFINED__ */


#ifndef __S2Air_FWD_DEFINED__
#define __S2Air_FWD_DEFINED__

#ifdef __cplusplus
typedef class S2Air S2Air;
#else
typedef struct S2Air S2Air;
#endif /* __cplusplus */

#endif 	/* __S2Air_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IS2Air_INTERFACE_DEFINED__
#define __IS2Air_INTERFACE_DEFINED__

/* interface IS2Air */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IS2Air;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4F91871D-9B96-45C2-B670-2D5D9CE5FF44")
    IS2Air : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartCon( 
            BSTR ServerIP,
            BSTR ServerPort,
            BSTR UserID,
            BSTR Passwd,
            int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendMsg( 
            BSTR MobileNum,
            BSTR Message,
            int __RPC_FAR *pVal,
            BSTR __RPC_FAR *pBVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendMsg_Order( 
            BSTR MobileNum,
            BSTR Message,
            BSTR Send_Time,
            int __RPC_FAR *pVal,
            BSTR __RPC_FAR *pBVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryMsg( 
            BSTR MobileNum,
            BSTR MessageID,
            int __RPC_FAR *pVal,
            BSTR __RPC_FAR *pBVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndCon( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IS2AirVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IS2Air __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IS2Air __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IS2Air __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IS2Air __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IS2Air __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IS2Air __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IS2Air __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartCon )( 
            IS2Air __RPC_FAR * This,
            BSTR ServerIP,
            BSTR ServerPort,
            BSTR UserID,
            BSTR Passwd,
            int __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendMsg )( 
            IS2Air __RPC_FAR * This,
            BSTR MobileNum,
            BSTR Message,
            int __RPC_FAR *pVal,
            BSTR __RPC_FAR *pBVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendMsg_Order )( 
            IS2Air __RPC_FAR * This,
            BSTR MobileNum,
            BSTR Message,
            BSTR Send_Time,
            int __RPC_FAR *pVal,
            BSTR __RPC_FAR *pBVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryMsg )( 
            IS2Air __RPC_FAR * This,
            BSTR MobileNum,
            BSTR MessageID,
            int __RPC_FAR *pVal,
            BSTR __RPC_FAR *pBVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndCon )( 
            IS2Air __RPC_FAR * This);
        
        END_INTERFACE
    } IS2AirVtbl;

    interface IS2Air
    {
        CONST_VTBL struct IS2AirVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IS2Air_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IS2Air_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IS2Air_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IS2Air_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IS2Air_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IS2Air_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IS2Air_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IS2Air_StartCon(This,ServerIP,ServerPort,UserID,Passwd,pVal)	\
    (This)->lpVtbl -> StartCon(This,ServerIP,ServerPort,UserID,Passwd,pVal)

#define IS2Air_SendMsg(This,MobileNum,Message,pVal,pBVal)	\
    (This)->lpVtbl -> SendMsg(This,MobileNum,Message,pVal,pBVal)

#define IS2Air_SendMsg_Order(This,MobileNum,Message,Send_Time,pVal,pBVal)	\
    (This)->lpVtbl -> SendMsg_Order(This,MobileNum,Message,Send_Time,pVal,pBVal)

#define IS2Air_QueryMsg(This,MobileNum,MessageID,pVal,pBVal)	\
    (This)->lpVtbl -> QueryMsg(This,MobileNum,MessageID,pVal,pBVal)

#define IS2Air_EndCon(This)	\
    (This)->lpVtbl -> EndCon(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IS2Air_StartCon_Proxy( 
    IS2Air __RPC_FAR * This,
    BSTR ServerIP,
    BSTR ServerPort,
    BSTR UserID,
    BSTR Passwd,
    int __RPC_FAR *pVal);


void __RPC_STUB IS2Air_StartCon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IS2Air_SendMsg_Proxy( 
    IS2Air __RPC_FAR * This,
    BSTR MobileNum,
    BSTR Message,
    int __RPC_FAR *pVal,
    BSTR __RPC_FAR *pBVal);


void __RPC_STUB IS2Air_SendMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IS2Air_SendMsg_Order_Proxy( 
    IS2Air __RPC_FAR * This,
    BSTR MobileNum,
    BSTR Message,
    BSTR Send_Time,
    int __RPC_FAR *pVal,
    BSTR __RPC_FAR *pBVal);


void __RPC_STUB IS2Air_SendMsg_Order_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IS2Air_QueryMsg_Proxy( 
    IS2Air __RPC_FAR * This,
    BSTR MobileNum,
    BSTR MessageID,
    int __RPC_FAR *pVal,
    BSTR __RPC_FAR *pBVal);


void __RPC_STUB IS2Air_QueryMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IS2Air_EndCon_Proxy( 
    IS2Air __RPC_FAR * This);


void __RPC_STUB IS2Air_EndCon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IS2Air_INTERFACE_DEFINED__ */



#ifndef __S2ACOMLib_LIBRARY_DEFINED__
#define __S2ACOMLib_LIBRARY_DEFINED__

/* library S2ACOMLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_S2ACOMLib;

EXTERN_C const CLSID CLSID_S2Air;

#ifdef __cplusplus

class DECLSPEC_UUID("31C80217-22B5-4BA9-8A3C-0B4FC24A441F")
S2Air;
#endif
#endif /* __S2ACOMLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
