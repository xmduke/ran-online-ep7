//=--------------------------------------------------------------------------=
//  (C) Copyright 1996-1999 Microsoft Corporation. All Rights Reserved.
//=--------------------------------------------------------------------------=
#ifndef __CUSTOMSITEH__
#define __CUSTOMSITEH__

#if _MFC_VER >= 0x0700
	#include <afxocc.h>
#else
	// NOTE: This line is a hardcoded reference to an MFC header file
	//  this path may need to be changed to refer to the location of VC5 install
	//  for successful compilation.
	#include <..\src\occimpl.h>
#endif

#include "idispimp.h"
#include <mshtmhst.h>

// 
// NOTE: 
// Some of the code in this file is MFC implementation specific.
// Changes in future versions of MFC implementation may require
// the code to be changed. Please check the readme of this
// sample for more information 
// 

class CCustomControlSite:public COleControlSite
{
public:
	CCustomControlSite(COleControlContainer *pCnt):COleControlSite(pCnt){}

protected:

	DECLARE_INTERFACE_MAP();
BEGIN_INTERFACE_PART(DocHostUIHandler, IDocHostUIHandler)
	STDMETHOD(ShowContextMenu)(/* [in] */ DWORD dwID,
            /* [in] */ POINT __RPC_FAR *ppt,
            /* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
            /* [in] */ IDispatch __RPC_FAR *pdispReserved);
	STDMETHOD(GetHostInfo)( 
            /* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
	STDMETHOD(ShowUI)( 
            /* [in] */ DWORD dwID,
            /* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
            /* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
            /* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
            /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc);
	STDMETHOD(HideUI)(void);
	STDMETHOD(UpdateUI)(void);
	STDMETHOD(EnableModeless)(/* [in] */ BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(/* [in] */ BOOL fEnable);
	STDMETHOD(OnFrameWindowActivate)(/* [in] */ BOOL fEnable);
	STDMETHOD(ResizeBorder)( 
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
            /* [in] */ BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)( 
            /* [in] */ LPMSG lpMsg,
            /* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
            /* [in] */ DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)( 
            /* [out] */ LPOLESTR __RPC_FAR *pchKey,
            /* [in] */ DWORD dw);
	STDMETHOD(GetDropTarget)(
            /* [in] */ IDropTarget __RPC_FAR *pDropTarget,
            /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
    STDMETHOD(GetExternal)( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
    STDMETHOD(TranslateUrl)( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ OLECHAR __RPC_FAR *pchURLIn,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
    STDMETHOD(FilterDataObject)( 
            /* [in] */ IDataObject __RPC_FAR *pDO,
            /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet);
END_INTERFACE_PART(DocHostUIHandler)
};

class CCustomOccManager :public COccManager
{
public:
	//CCustomOccManager();
	COleControlSite* CreateSite(COleControlContainer* pCtrlCont);
};

enum CustomControlSiteMessages
{
	kCCSN_NoMessage = 0,
	kCCSN_CreateSite,
	kCCSN_ShowContextMenu,
	kCCSN_MessageLimit
};

struct kCCSN_CreateSiteParams
{
	COleControlContainer	*pCtrlCont; //in param
	CCustomControlSite		*pSite;		//out param
};

struct kCCSN_ShowContextMenuParams
{
	DWORD		dwID;
	POINT		*pptPosition;
	IUnknown	*pCommandTarget;
	IDispatch	*pDispatchObjectHit;
	HRESULT		result;
};

BOOL CustomControlSiteSendToAncestors(HWND hWnd, WPARAM wParam, LPARAM lParam);

#endif
