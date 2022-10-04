// GameSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoPatchMan.h"
#include "GameSetDlg.h"

#include "../EngineLib/DxCommon/DxFontMan.h"
#include "DebugSet.h"
#include "../EngineSoundLib/DxSound/DxSoundMan.h"

#include "RanParam.h"
#include "EtcFunction.h"

#include "GlobalVariable.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static	const int	INTERPOLIATION = -25;

//std::string szCharShadow[5] = 
//{
//	"그림자 없음",	//	1단계
//	"그림자 저급",		//
//	"그림자 중급",
//	"그림자 고급1",
//	"그림자 고급2"
//};
//
//std::string szCharDetail[3] = 
//{
//	"저급",
//	"중급",
//	"고급",
//};
//
//std::string sz3DAlgorithm[3] =
//{	
//	"기본",
//	"하드웨어가속 HALF",
//	"하드웨어가속 FULL"
//};
//
//std::string szFogRange[3] = 
//{
//	"가까이",
//	"보통",
//	"멀리"
//};

BOOL CGameSetDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( GetDlgItem ( IDC_SLIDER_BGM )->m_hWnd )
	{
		if ( GetDlgItem ( IDC_SLIDER_BGM )->m_hWnd == pMsg->hwnd )
		{	
			if ( pMsg->message == WM_LBUTTONUP ||
				 pMsg->message == WM_KEYUP ||
				 pMsg->message == WM_MOUSEWHEEL )
			{	
				m_BgmVolume = ((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_BGM ))->GetPos ();
				m_BgmVolume = (INTERPOLIATION<=m_BgmVolume)?0:m_BgmVolume;
				
			}
		}
	}

	if ( GetDlgItem ( IDC_SLIDER_SFX )->m_hWnd )
	{
		if ( GetDlgItem ( IDC_SLIDER_SFX )->m_hWnd == pMsg->hwnd )
		{	
			if ( pMsg->message == WM_LBUTTONUP ||
				 pMsg->message == WM_KEYUP ||
				 pMsg->message == WM_MOUSEWHEEL )
			{
				m_SfxVolume = ((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_SFX ))->GetPos ();
				m_SfxVolume = (INTERPOLIATION<=m_SfxVolume)?0:m_SfxVolume;
			}
		}
	}

	if ( GetDlgItem ( IDC_SLIDER_MAP )->m_hWnd )
	{
		if ( GetDlgItem ( IDC_SLIDER_MAP )->m_hWnd == pMsg->hwnd )
		{	
			if ( pMsg->message == WM_LBUTTONUP ||
				 pMsg->message == WM_KEYUP ||
				 pMsg->message == WM_MOUSEWHEEL )
			{				
				m_MapVolume = ((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_MAP ))->GetPos ();
				m_MapVolume = (INTERPOLIATION<=m_MapVolume)?0:m_MapVolume;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CGameSetDlg::InitCtrls ()
{
	//	해상도 결정
	D3DDeviceInfo* pDeviceInfo = m_D3DApp.GetCurDeviceInfo ();
	DWORD dwNumModes = pDeviceInfo->dwNumModes;
	D3DModeInfo* pModeInfo = pDeviceInfo->modes;
    
	CString str;
	for ( DWORD i = 0; i < dwNumModes; i++ )
	{
		D3DModeInfo &sModeInfo = pDeviceInfo->modes[i];

		DWORD	bpp = 16;
		D3DFORMAT &d3dformat = sModeInfo.Format;
		if ((d3dformat == D3DFMT_R8G8B8)	||
			(d3dformat == D3DFMT_A8R8G8B8)	||
			(d3dformat == D3DFMT_X8R8G8B8) )
		{
			bpp = 32;
		}

		str.Format ( "%4d * %4d * %d", sModeInfo.Width, sModeInfo.Height, bpp );
		((CComboBox*)GetDlgItem ( IDC_COMBO_RESOLUTION ))->AddString ( str );
	}

	SetWin_Combo_Init ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption, 4 );
	SetWin_Combo_Init ( this, IDC_COMBO_FOGRANGE, szFogRange, 3 );
	SetWin_Combo_Init ( this, IDC_COMBO_SHADOWCHAR, szCharShadow, 5 );
	SetWin_Combo_Init ( this, IDC_COMBO_SKINDETAIL, szCharDetail, 3 );
	SetWin_Combo_Init ( this, IDC_COMBO_3DALGORITHM, sz3DAlgorithm, 3 );

	((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_BGM ))->SetRange ( -2000, 0 + INTERPOLIATION );
	((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_SFX ))->SetRange ( -2000, 0 + INTERPOLIATION );
	((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_MAP ))->SetRange ( -2000, 0 + INTERPOLIATION );


#ifdef CH_PARAM
	{
		SetDlgItemText( IDC_RADIO1, m_strAreaName[0] );
		SetDlgItemText( IDC_RADIO2, m_strAreaName[1] );
		SetDlgItemText( IDC_RADIO3, m_strAreaName[2] );
//		SetDlgItemText( IDC_RADIO4, m_strAreaName[3] );
//		SetDlgItemText( IDC_RADIO5, m_strAreaName[4] );
//		SetDlgItemText( IDC_RADIO6, m_strAreaName[5] ); // 중국지역 추가
	}
#endif

	return TRUE;
}

BOOL CGameSetDlg::UpdateCtrls ()
{
	CString str;

	str.Format ( "%4d * %4d * %d",
		RANPARAM::dwScrWidth, RANPARAM::dwScrHeight, (RANPARAM::emScrFormat)?32:16 );

	bool bSEL = SetWin_Combo_SelMfc ( this, IDC_COMBO_RESOLUTION, str );
	if ( !bSEL )
	{
		//	해상도 결정
		D3DDeviceInfo* pDeviceInfo = m_D3DApp.GetCurDeviceInfo ();
		D3DModeInfo &sModeInfo = pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];

		DWORD	bpp = 16;
		D3DFORMAT &d3dformat = sModeInfo.Format;
		if ((d3dformat == D3DFMT_R8G8B8)	||
			(d3dformat == D3DFMT_A8R8G8B8)	||
			(d3dformat == D3DFMT_X8R8G8B8) )
		{
			bpp = 32;
		}

		str.Format ( "%4d * %4d * %d",
			sModeInfo.Width, sModeInfo.Height, bpp );

		bSEL = SetWin_Combo_SelMfc ( this, IDC_COMBO_RESOLUTION, str );
		if ( !bSEL )
		{
			SetWin_Combo_Sel ( this, IDC_COMBO_RESOLUTION, 0 );
		}
	}
	
	SetWin_Check ( this, IDC_CHECK_WINDOWMODE, RANPARAM::bScrWindowed );

	//	의도된것임
	OnCbnSelchangeComboResolution ();

	m_BgmVolume = RANPARAM::nSndMusic;
	m_SfxVolume = RANPARAM::nSndSfx;
	m_MapVolume = RANPARAM::nSndMap;
	
	((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_BGM ))->SetPos ( m_BgmVolume );
	((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_SFX ))->SetPos ( m_SfxVolume );
	((CSliderCtrl*)GetDlgItem ( IDC_SLIDER_MAP ))->SetPos ( m_MapVolume );	
	
	
	SetWin_Check ( this, IDC_CHECK_3DSOUND, RANPARAM::b3D_SOUND );
	SetWin_Check ( this, IDC_CHECK_GAMECURSOR, RANPARAM::bGameCursor );

	SetWin_Combo_Sel ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption[RANPARAM::dwVideoLevel] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[RANPARAM::dwFogRange] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[RANPARAM::dwShadowChar] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[RANPARAM::dwSkinDetail] );
	SetWin_Combo_Sel ( this, IDC_COMBO_3DALGORITHM, sz3DAlgorithm[RANPARAM::dw3DAlgorithm] );

	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, RANPARAM::bShadowLand );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT, RANPARAM::bRealReflect );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, RANPARAM::bRefract );	
	SetWin_Check ( this, IDC_CHECK_GLOW, RANPARAM::bGlow );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, RANPARAM::bBuff );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, RANPARAM::bPost );	
	SetWin_Check ( this, IDC_CHECK_FRAME_LIMIT, RANPARAM::bFrameLimit );	

	SetWin_Check ( this, IDC_CHECK_MAP, !RANPARAM::bSndMuteMap );	
	SetWin_Check ( this, IDC_CHECK_BGM, !RANPARAM::bSndMuteMusic );	
	SetWin_Check ( this, IDC_CHECK_SFX, !RANPARAM::bSndMuteSfx );	

	SetWin_Check ( this, IDC_CHECK_PASSIVE, RANPARAM::bUsePassiveDN );

	SetWin_Check ( this, IDC_CHECK_TEXTURE_FONT, RANPARAM::strFontType==COMMENT::FONTSYS[0].c_str() );

	return TRUE;
}

void CGameSetDlg::InitDlgText ()
{
	SetWindowText( ID2LAUNCHERTEXT("IDD_GAMESETDLG"));
	
	SetDlgItemText( IDC_DISPLAY_STATIC, ID2LAUNCHERTEXT("IDC_DISPLAY_STATIC") );
	SetDlgItemText( IDC_CHECK_WINDOWMODE, ID2LAUNCHERTEXT("IDC_CHECK_WINDOWMODE") );
	
	SetDlgItemText( IDC_DOWNLOAD_STATIC, ID2LAUNCHERTEXT("IDC_DOWNLOAD_STATIC") );		
	SetDlgItemText( IDC_CHECK_PASSIVE, ID2LAUNCHERTEXT("IDC_CHECK_PASSIVE") );
	SetDlgItemText( IDC_DOWNAREA_STATIC, ID2LAUNCHERTEXT("IDC_DOWNAREA_STATIC") );

	SetDlgItemText( IDOK, ID2LAUNCHERTEXT("GAMESET_ID_OK") );
	SetDlgItemText( IDCANCEL, ID2LAUNCHERTEXT("GAMESET_ID_CANCEL") );

	SetDlgItemText( IDC_GRAPHIC_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",0) );
	SetDlgItemText( IDC_DEFAULT_OPTION_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",1) );
	SetDlgItemText( IDC_FOGRANGE_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",2) );
	SetDlgItemText( IDC_SHADOWCHAR_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",3) );
	SetDlgItemText( IDC_SKINDETAIL_STATIC, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",4) );
	SetDlgItemText( IDC_CHECK_SHADOWLAND, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",5) );
	SetDlgItemText( IDC_CHECK_REALREFLECT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",6) );
	SetDlgItemText( IDC_CHECK_REFRACT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",7) );
	SetDlgItemText( IDC_CHECK_GLOW, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",8) );
	SetDlgItemText( IDC_CHECK_BUFFEFFECT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",9) );
	SetDlgItemText( IDC_CHECK_POSTPROCESSING, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",10) );
	SetDlgItemText( IDC_CHECK_GAMECURSOR, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",11) );
	SetDlgItemText( IDC_CHECK_TEXTURE_FONT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",12) );
	SetDlgItemText( IDC_CHECK_FRAME_LIMIT, ID2LAUNCHERTEXT("IDC_GRAPHIC_OPTION",13) );
	
	SetDlgItemText( IDC_SOUND_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",0) );
	SetDlgItemText( IDC_CHECK_3DSOUND, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",1) );
	SetDlgItemText( IDC_BUTTON_COMPATIBLE, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",2) );
	SetDlgItemText( IDC_BGM_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",3) );
	SetDlgItemText( IDC_MAP_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",4) );
	SetDlgItemText( IDC_SFX_STATIC, ID2LAUNCHERTEXT("IDC_SOUND_OPTION",5) );
}

BOOL CGameSetDlg::InverseUpdateCtrls ()
{
	RANPARAM::dwScrWidth = m_dwScrWidth;
	RANPARAM::dwScrHeight = m_dwScrHeight;
	RANPARAM::emScrFormat = m_emScrFormat;
	RANPARAM::uScrRefreshHz = m_uScrRefreshHz;
	RANPARAM::bScrWindowed = GetWin_Check ( this, IDC_CHECK_WINDOWMODE );

	RANPARAM::bGameCursor = GetWin_Check ( this, IDC_CHECK_GAMECURSOR );

	RANPARAM::nSndMap = m_MapVolume;
	RANPARAM::nSndSfx = m_SfxVolume;
	RANPARAM::nSndMusic = m_BgmVolume;


	RANPARAM::dwVideoLevel = ((CComboBox*)GetDlgItem ( IDC_COMBO_DEFAULTOPTION ))->GetCurSel ();
	RANPARAM::dw3DAlgorithm = ((CComboBox*)GetDlgItem ( IDC_COMBO_3DALGORITHM ))->GetCurSel ();
	RANPARAM::dwShadowChar	= ((CComboBox*)GetDlgItem ( IDC_COMBO_SHADOWCHAR ))->GetCurSel ();
	RANPARAM::bShadowLand	= ((CButton*)GetDlgItem ( IDC_CHECK_SHADOWLAND ))->GetCheck ();
	RANPARAM::bRealReflect	= ((CButton*)GetDlgItem ( IDC_CHECK_REALREFLECT ))->GetCheck ();
	RANPARAM::bRefract	= ((CButton*)GetDlgItem ( IDC_CHECK_REFRACT ))->GetCheck ();
	RANPARAM::bGlow	= ((CButton*)GetDlgItem ( IDC_CHECK_GLOW ))->GetCheck ();
	RANPARAM::bBuff	= ((CButton*)GetDlgItem ( IDC_CHECK_BUFFEFFECT ))->GetCheck ();
	RANPARAM::bPost	= ((CButton*)GetDlgItem ( IDC_CHECK_POSTPROCESSING ))->GetCheck ();
	RANPARAM::bFrameLimit	= ((CButton*)GetDlgItem ( IDC_CHECK_FRAME_LIMIT ))->GetCheck ();

	RANPARAM::dwSkinDetail	= ((CComboBox*)GetDlgItem ( IDC_COMBO_SKINDETAIL ))->GetCurSel ();
	RANPARAM::dwFogRange	= ((CComboBox*)GetDlgItem ( IDC_COMBO_FOGRANGE ))->GetCurSel ();

	RANPARAM::bSndMuteMap	= !((CButton*)GetDlgItem ( IDC_CHECK_MAP ))->GetCheck ();
	RANPARAM::bSndMuteMusic	= !((CButton*)GetDlgItem ( IDC_CHECK_BGM ))->GetCheck ();
	RANPARAM::bSndMuteSfx	= !((CButton*)GetDlgItem ( IDC_CHECK_SFX ))->GetCheck ();

	RANPARAM::b3D_SOUND = ((CButton*)GetDlgItem ( IDC_CHECK_3DSOUND ))->GetCheck ();
	RANPARAM::bUsePassiveDN = ((CButton*)GetDlgItem ( IDC_CHECK_PASSIVE ))->GetCheck ();

	BOOL bTEX_FONT = GetWin_Check ( this, IDC_CHECK_TEXTURE_FONT );
	RANPARAM::strFontType = bTEX_FONT ? COMMENT::FONTSYS[0].c_str() : COMMENT::FONTSYS[1].c_str();

#ifdef CH_PARAM
	{
		UpdateData();

		RANPARAM::nChinaRegion = m_nDownloadArea;

		StringCchCopy( RANPARAM::LoginAddress, STRING_NUM_128, RANPARAM::China_Region[m_nDownloadArea].LoginAddress.GetString() );

		for( int i=0 ; ; ++i )
		{
			if( RANPARAM::China_Region[m_nDownloadArea].HttpAddressTable[i].IsEmpty() ) 
				break;

			StringCchCopy( RANPARAM::HttpAddressTable[i], STRING_NUM_128, RANPARAM::China_Region[m_nDownloadArea].HttpAddressTable[i].GetString() );
		}

		RANPARAM::strNewsURL = RANPARAM::China_Region[m_nDownloadArea].strNewsURL;
	}
#endif

	return TRUE;
}