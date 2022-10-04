//=--------------------------------------------------------------------------=
//  (C) Copyright 1996-1999 Microsoft Corporation. All Rights Reserved.
//=--------------------------------------------------------------------------=

//
// NOTE: 
// Some of the code in this file is MFC implementation specific.
// Changes in future versions of MFC implementation may require
// the code to be changed. Please check the readme of this
// sample for more information 
// 

// NOTE2: This file also contains additional code written by Jim Xochellis

#include "stdafx.h"
//#undef AFX_DATA
//#define AFX_DATA AFX_DATA_IMPORT
#include "AutoPatchMan.h"
//#undef AFX_DATA
//#define AFX_DATA AFX_DATA_EXPORT

#include "custsite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_INTERFACE_MAP(CCustomControlSite, COleControlSite)
	INTERFACE_PART(CCustomControlSite, IID_IDocHostUIHandler, DocHostUIHandler)
END_INTERFACE_MAP()


ULONG FAR EXPORT  CCustomControlSite::XDocHostUIHandler::AddRef()
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return pThis->ExternalAddRef();
}


ULONG FAR EXPORT  CCustomControlSite::XDocHostUIHandler::Release()
{                            
    METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return pThis->ExternalRelease();
}

HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::QueryInterface(REFIID riid, void **ppvObj)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}

// * CImpIDocHostUIHandler::GetHostInfo
// *
// * Purpose: Called at initialization
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::GetHostInfo( DOCHOSTUIINFO* pInfo )
{

	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	pInfo->dwFlags = DOCHOSTUIFLAG_DIALOG;//DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO;
    pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

    return S_OK;
}

// * CImpIDocHostUIHandler::ShowUI
// *
// * Purpose: Called when MSHTML.DLL shows its UI
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::ShowUI(
				DWORD dwID, 
				IOleInPlaceActiveObject * /*pActiveObject*/,
				IOleCommandTarget * pCommandTarget,
				IOleInPlaceFrame * /*pFrame*/,
				IOleInPlaceUIWindow * /*pDoc*/)
{

	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	// We've already got our own UI in place so just return S_OK
    return S_OK;
}

// * CImpIDocHostUIHandler::HideUI
// *
// * Purpose: Called when MSHTML.DLL hides its UI
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::HideUI(void)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return S_OK;
}

// * CImpIDocHostUIHandler::UpdateUI
// *
// * Purpose: Called when MSHTML.DLL updates its UI
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::UpdateUI(void)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	// MFC is pretty good about updating it's UI in it's Idle loop so I don't do anything here
	return S_OK;
}

// * CImpIDocHostUIHandler::EnableModeless
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::EnableModeless
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::EnableModeless(BOOL /*fEnable*/)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::OnDocWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnDocWindowActivate
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::OnDocWindowActivate(BOOL /*fActivate*/)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::OnFrameWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnFrameWindowActivate
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::OnFrameWindowActivate(BOOL /*fActivate*/)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::ResizeBorder
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::ResizeBorder
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::ResizeBorder(
				LPCRECT /*prcBorder*/, 
				IOleInPlaceUIWindow* /*pUIWindow*/,
				BOOL /*fRameWindow*/)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::ShowContextMenu
// *
// * Purpose: Called when MSHTML.DLL would normally display its context menu
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::ShowContextMenu(
				DWORD dwID, 
				POINT* pptPosition,
				IUnknown* pCommandTarget,
				IDispatch* pDispatchObjectHit)
{
HRESULT result = S_FALSE;

	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)

// Additional code written by Jim Xochellis starts

	if (pCommandTarget != NULL)
	{
	IOleWindow *oleWnd	= NULL;
	HRESULT hr = 0;

	    hr = pCommandTarget->QueryInterface(IID_IOleWindow, (void**)&oleWnd);

		if ( (hr == S_OK) && (oleWnd != NULL) )
		{
		HWND hwnd = NULL;

			hr = oleWnd->GetWindow(&hwnd);

			if ( (hr == S_OK) && (hwnd != NULL) )
			{
				kCCSN_ShowContextMenuParams params =
						{dwID, pptPosition, pCommandTarget, pDispatchObjectHit, S_FALSE};

				if ( CustomControlSiteSendToAncestors(hwnd, kCCSN_ShowContextMenu, (LPARAM)&params) )
					result = params.result;
			}
		}
	}

// Additional code written by Jim Xochellis ends

    return result;
}

// * CImpIDocHostUIHandler::TranslateAccelerator
// *
// * Purpose: Called from MSHTML.DLL's TranslateAccelerator routines
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg,
            /* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
            /* [in] */ DWORD nCmdID)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return S_FALSE;
}

// * CImpIDocHostUIHandler::GetOptionKeyPath
// *
// * Purpose: Called by MSHTML.DLL to find where the host wishes to store 
// *	its options in the registry
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::GetOptionKeyPath(BSTR* pbstrKey, DWORD)
{

	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return E_NOTIMPL;
}

STDMETHODIMP CCustomControlSite::XDocHostUIHandler::GetDropTarget( 
            /* [in] */ IDropTarget __RPC_FAR *pDropTarget,
            /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return E_NOTIMPL;
}


STDMETHODIMP CCustomControlSite::XDocHostUIHandler::GetExternal( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
#ifdef USE_MFC7_HTMLVIEW_FEATURES
	return E_NOTIMPL;
#else
	// return the IDispatch we have for extending the object Model
	IDispatch* pDisp = (IDispatch*)theApp.m_pDispOM;
	pDisp->AddRef();
	*ppDispatch = pDisp;
    return S_OK;
#endif
}
        
STDMETHODIMP CCustomControlSite::XDocHostUIHandler::TranslateUrl( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ OLECHAR __RPC_FAR *pchURLIn,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return E_NOTIMPL;
}
        
STDMETHODIMP CCustomControlSite::XDocHostUIHandler::FilterDataObject( 
            /* [in] */ IDataObject __RPC_FAR *pDO,
            /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
    return E_NOTIMPL;
}

//--------------------------------------------------------------------------
// Additional code written by Jim Xochellis

COleControlSite* CCustomOccManager::CreateSite(COleControlContainer* pCtrlCont)
{
COleControlSite *result = NULL;

	if (pCtrlCont != NULL)
	{
	kCCSN_CreateSiteParams params = {pCtrlCont, NULL};

		//Ask the ancestor windows of the WebBrowser control if they wan't
		//to customise it with a custom control site.
		if ( CustomControlSiteSendToAncestors(pCtrlCont->m_pWnd->m_hWnd,
				kCCSN_CreateSite, (LPARAM)&params) )
		{
			result = params.pSite;
		}
		
		//Fallback to the default control site that MFC provides
		if (result == NULL)
			result = COccManager::CreateSite(pCtrlCont);
	}
	
	return result;
}

//Returns TRUE if message has been handled and FALSE otherwise
BOOL CustomControlSiteSendToAncestors(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
BOOL hasBeenHandled = FALSE;

	if (hWnd != NULL)
	{
	LRESULT msgResult = 0;

		const UINT WM_CUSTOM_CONTROLSITE_MSG  = RegisterWindowMessage(_T("LauncherMsg"));

		do 
		{
			msgResult = ::SendMessage(hWnd, WM_CUSTOM_CONTROLSITE_MSG, wParam, lParam);

			if (msgResult != 0)
			{
				hasBeenHandled = TRUE;
				hWnd = NULL;
			}
			else
				hWnd = ::GetParent(hWnd);
		}
		while (hWnd != NULL);
	}

	return hasBeenHandled;
}
