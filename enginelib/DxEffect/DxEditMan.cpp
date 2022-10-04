#include "stdafx.h"
#include "DxViewPort.h"
#include "SerialFile.h"

#include "DxEditMan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

//	Note : DxEditMan, 버전
//
const DWORD DxEditMan::VERSION = 0x00000001;

DxEditMan::DxEditMan (void)
{
}

DxEditMan::~DxEditMan (void)
{
}

HRESULT DxEditMan::ActiveMap ()
{
	return S_OK;
}

HRESULT DxEditMan::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxEditMan::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxEditMan::RestoreDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxEditMan::InvalidateDeviceObjects ()
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxEditMan::DeleteDeviceObjects ()
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxEditMan::FinalCleanup ()
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxEditMan::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxEditMan::Render ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	return S_OK;
}

void DxEditMan::LoadSet ( char *pFileName, LPDIRECT3DDEVICE8 pd3dDevice )
{
//	CDebugSet::ClearFile ( "LoadSet.log" );
//
//	BOOL	bResult = FALSE;
//	
//	CSerialFile	SFile;
//	BOOL bOpened = SFile.OpenFile ( FOT_READ, pFileName );
//	if ( !bOpened )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. error File Open..." );
//	}
//
//	char szFileType[FILETYPESIZE];
//	DWORD dwFileVer;
//	SFile.GetFileType ( szFileType, dwFileVer );
//
//	if ( dwFileVer > DxEditMan::VERSION )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. ERROR 맵 설정 파일(PSF) 이 현제 툴의 버전보다 상위의 버전입니다." );
//		return;
//	}
//
//	//	Note : Light
//	//
//	CDebugSet::ToFile ( "LoadSet.log", "-. [LightMan Loading] - Start" );
//	m_LightMan.Load ( SFile, dwFileVer, TRUE );
//	CDebugSet::ToFile ( "LoadSet.log", "-. [LightMan Loading] - End" );
//
//	//	Note : FrameMesh
//	//
//	CDebugSet::ToFile ( "LoadSet.log", "-. [FrameMesh Loading] - Start" );
//	if ( !m_FrameMesh.LoadSet ( SFile, pd3dDevice ) )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. 프레임 메쉬로드에 실패하였습니다." );
//		return;
//	}
//	CDebugSet::ToFile ( "LoadSet.log", "-. [FrameMesh Loading] - End" );
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		int StrLength = 0;
//		char* szNaviName = NULL;
//		
//		SFile >> StrLength;
//		szNaviName = new char [ StrLength ];
//		SFile.ReadBuffer ( szNaviName, StrLength );
//
//		DxFrame *pNaviFrame = NULL;
//        pNaviFrame = m_FrameMesh.FindFrame ( szNaviName );
//		if ( pNaviFrame )	SetNaviFrame ( pNaviFrame );
//		else
//		{
//			CDebugSet::ToFile ( "LoadSet.log", "   [%s]...Not Found Navigation Mesh..", szNaviName );
//		}
//
//		SAFE_DELETE_ARRAY ( szNaviName );
//	}
//
//	//	Note : ViewPort
//	//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [CameraAniMan Loading] - Start" );
//		m_CameraAniMan.LoadSet ( SFile, &m_FrameMesh );
//		CDebugSet::ToFile ( "LoadSet.log", "-. [CameraAniMan Loading] - End" );
//	}
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSoundMan Loading] - Start" );
//	
//		//	Note : Sound Log
//#ifdef	_DEBUG
//		CDebugSet::ToFile ( "UseSoundList.txt", "" );
//		CDebugSet::ToFile ( "UseSoundList.txt", "[-------------------------------------------------------------]" );
//		CDebugSet::ToFile ( "UseSoundList.txt", "[!.%s]", pFileName );	
//#endif	//	_DEBUG
//
//		DxSoundMan::Instance.LoadSet ( SFile );
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSoundMan Loading] - End" );
//	}
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [StaticSoundMan Loading] - Start" );
//		m_StaticSoundMan.LoadSet ( SFile );
//		CDebugSet::ToFile ( "LoadSet.log", "-. [StaticSoundMan Loading] - End" );
//	}
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDEFF Loading] - Start" );
//
//		bResult = TRUE;
//		while ( bResult )
//		{
//			PLANDEFF pLandEff = new DXLANDEFF;
//			pLandEff->Load ( SFile, pd3dDevice );
//			AddLandEff ( pLandEff );
//			
//			SFile >> bResult;
//		}
//	
//		BuildSingleEffTree ();
//
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDEFF Loading] - End" );
//	}
//
//	//	Note : 맵 ID 읽기.
//	//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		SFile >> m_MapID.dwID;
//	}
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		SFile.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );
//	}
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDSKINOBJ Loading] - Start" );
//
//		bResult = TRUE;
//		while ( bResult )
//		{
//			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ;
//			pLandSkinObj->Load ( SFile, pd3dDevice );
//			AddLandSkinObj ( pLandSkinObj );
//			
//			SFile >> bResult;
//		}
//	
//		BuildSkinObjTree ();
//
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDSKINOBJ Loading] - End" );
//	}
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		m_LandGateMan.Load ( SFile );
//	}
//
//	//	WeatherMan
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [WeatherMan Loading] - Start" );
//		m_WeatherMan.LoadSet ( SFile, pd3dDevice );
//		CDebugSet::ToFile ( "LoadSet.log", "-. [WeatherMan Loading] - End" );
//	}
//
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DxBgmSound Loading] - Start" );
//		m_BGMDATA.LoadSet ( SFile );
//		CDebugSet::ToFile ( "LoadSet.log", "-. [DxBgmSound Loading] - End" );
//	}
//
//	//	DxFogMan
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [FOG_PROPERTY Loading] - Start" );
//		m_FOG_PROPERTY.LoadSet ( SFile );
//		CDebugSet::ToFile ( "LoadSet.log", "-. [FOG_PROPERTY Loading] - End" );
//	}
//	else return;
//
//	//	DxFogMan
//	SFile >> bResult;
//	if ( bResult )
//	{
//		CDebugSet::ToFile ( "LoadSet.log", "-. [SKY_PROPERTY Loading] - Start" );
//		m_SKY_PROPERTY.LoadSet ( SFile );
//		CDebugSet::ToFile ( "LoadSet.log", "-. [SKY_PROPERTY Loading] - End" );
//	}
//	else return;
}

void DxEditMan::SaveSet ( char	*pFileName )
{
	//CSerialFile	SFile;

	//char szFileType[FILETYPESIZE] = "";
	//SFile.SetFileType ( szFileType, DxEditMan::VERSION );

	//BOOL bOpened = SFile.OpenFile ( FOT_WRITE, pFileName );
	//if ( !bOpened )
	//{
	//	MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
	//}

	////	Note : Light
	////
	//m_LightMan.Save ( SFile );

	////	Note : FrameMesh
	////
	//m_FrameMesh.SaveSet ( SFile );

	////	Note : 내비게이션 메시.
	////
	//if ( m_pNaviFrame )		SFile << BOOL ( TRUE );
	//else					SFile << BOOL ( FALSE );

	//if ( m_pNaviFrame )
	//{
	//	int	StrLength = strlen(m_pNaviFrame->szName)+1;
	//	SFile << StrLength;
	//	SFile.WriteBuffer ( m_pNaviFrame->szName, StrLength );
	//}

	////	Note : ViewPort
	////
	//SFile << BOOL ( TRUE );
	//m_CameraAniMan.SaveSet ( SFile );

	//SFile << BOOL ( TRUE );
	//DxSoundMan::Instance.SaveSet ( SFile );

	//SFile << BOOL ( TRUE );
	//m_StaticSoundMan.SaveSet ( SFile );

	//if ( m_pLandEffList )	SFile << BOOL ( TRUE );
	//else					SFile << BOOL ( FALSE );

	//PLANDEFF pCur = m_pLandEffList;
	//while ( pCur )
	//{
	//	pCur->Save ( SFile );

	//	pCur = pCur->m_pNext;
	//	if ( pCur )		SFile << BOOL ( TRUE );
	//	else			SFile << BOOL ( FALSE );
	//}

	////	Note : 맵 ID 저장.
	////
	//SFile << BOOL ( TRUE );
	//SFile << m_MapID.dwID;

	//SFile << BOOL ( TRUE );
	//SFile.WriteBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );


	////	Note : Land 에 삽입된 SkinObject
	////
	//if ( m_pLandSkinObjList )	SFile << BOOL ( TRUE );
	//else						SFile << BOOL ( FALSE );

	//PLANDSKINOBJ pLandSkinObj = m_pLandSkinObjList;
	//while ( pLandSkinObj )
	//{
	//	pLandSkinObj->Save ( SFile );
	//		
	//	pLandSkinObj = pLandSkinObj->m_pNext;
	//	if ( pLandSkinObj )		SFile << BOOL ( TRUE );
	//	else					SFile << BOOL ( FALSE );
	//}

	//SFile << BOOL ( TRUE );
	//m_LandGateMan.Save ( SFile );


	////	Note : Weather
	////
	//SFile << BOOL ( TRUE );
	//m_WeatherMan.SaveSet ( SFile );

	////	Note : BGM 파일 이름 [ 1개만 존재 ]
	//SFile << BOOL ( TRUE );
	//m_BGMDATA.SaveSet ( SFile );	

	////	Note : Fog
	//SFile << BOOL ( TRUE );
	//m_FOG_PROPERTY.SaveSet ( SFile );

	////	Note : Sky
	//SFile << BOOL ( TRUE );
	//m_SKY_PROPERTY.SaveSet ( SFile );

	////	Note : 다음에 추가될 데이터를 위해서 마킹 ------
	////
	//SFile << BOOL ( FALSE );
}

