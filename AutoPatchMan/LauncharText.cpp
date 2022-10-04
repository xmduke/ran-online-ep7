#include "StdAfx.h"
#include "LauncharText.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLauncherTextMan& CLauncherTextMan::GetInstance()
{
	static CLauncherTextMan Instance;
	return Instance;
}

CLauncherTextMan::CLauncherTextMan ()
{
	memset( m_szPath, 0, sizeof( m_szPath ) );
}

CLauncherTextMan::~CLauncherTextMan ()
{
    Clear ();
}

void CLauncherTextMan::Clear ()
{
	
	NS_TEXT_LOADER::ClearText ( m_LauncherText.mapFlags );
}

BOOL CLauncherTextMan::LoadText ( const char* szFileName,BOOL bXML )
{
	CString strPath(m_szPath);
	strPath += szFileName;

	
	if( !NS_TEXT_LOADER::LoadText( strPath.GetString(), SLAUNCHERTEXT::progVersion, m_LauncherText.mapFlags, bXML ) )
	{
		DefaultSetText();
	}

	return TRUE;
}

BOOL CLauncherTextMan::DefaultSetText()
{

	const int nIDSize = 125;
	const int nStrSize = 36;


	UINT szID[nIDSize];
	std::string strID[nStrSize];

	szID[0] = ABOUT_BOX_ID_OK;
	szID[1] = DOWNAREA_ID_OK;
	szID[2] = GAMESET_ID_CANCEL;
	szID[3] = GAMESET_ID_OK;
	szID[4] = IDC_AREA_STATIC;
	szID[5] = IDC_ATI_STATIC;
	szID[6] = IDC_BUTTON_EXIT;
	szID[7] = IDC_BUTTON_OPTION;
	szID[8] = IDC_BUTTON_PATCH;
	szID[9] = IDC_BUTTON_RETRY;
	szID[10] = IDC_BUTTON_START;
	szID[11] = IDC_CHECK_NOCHECK;
	szID[12] = IDC_CHECK_PASSIVE;
	szID[13] = IDC_CHECK_USE_HTTP;
	szID[14] = IDC_CHECK_WINDOWMODE;
	szID[15] = IDC_DISPLAY_STATIC;
	szID[16] = IDC_DOWNAREA_STATIC;
	szID[17] = IDC_DOWNLOAD_STATIC;
	szID[18] = IDC_NVIDIA_STATIC;
	szID[19] = IDC_READYTOSTART;
	szID[20] = IDC_STATIC_ATI_LINK;
	szID[21] = IDC_STATIC_NVIDIA_LINK;
	szID[22] = IDC_TITLE_STATIC;
	szID[23] = WARNING_ID_OK;
	szID[24] = IDD_GAMESETDLG;
	szID[25] = IDD_ABOUT_BOX;
	szID[26] = IDD_DIALOG_DOWNLOAD_AREA;
	szID[27] = IDD_DIALOG_WARNING_WHQL;
	
	szID[28] = IDC_VIDEO_STATIC;
	szID[29] = IDC_VER_STATIC;
	
	szID[30] = IDS_WHQL_CVD;
	szID[31] = IDS_WHQL_UCVD;
	szID[32] = IDS_ABOUTBOX;

	szID[33] = IDC_GRAPHIC_OPTION0;
	szID[34] = IDC_GRAPHIC_OPTION1;
	szID[35] = IDC_GRAPHIC_OPTION2;
	szID[36] = IDC_GRAPHIC_OPTION3;
	szID[37] = IDC_GRAPHIC_OPTION4;
	szID[38] = IDC_GRAPHIC_OPTION5;
	szID[39] = IDC_GRAPHIC_OPTION6;
	szID[40] = IDC_GRAPHIC_OPTION7;
	szID[41] = IDC_GRAPHIC_OPTION8;
	szID[42] = IDC_GRAPHIC_OPTION9;
	szID[43] = IDC_GRAPHIC_OPTION10;
	szID[44] = IDC_GRAPHIC_OPTION11;
	szID[45] = IDC_GRAPHIC_OPTION12;
	
	szID[46] = IDC_SOUND_OPTION0;
	szID[47] = IDC_SOUND_OPTION1;
	szID[48] = IDC_SOUND_OPTION2;
	szID[49] = IDC_SOUND_OPTION3;
	szID[50] = IDC_SOUND_OPTION4;
	szID[51] = IDC_SOUND_OPTION5;
	
	szID[52] = IDS_MESSAGE0;
	szID[53] = IDS_MESSAGE1;
	szID[54] = IDS_MESSAGE2;
	szID[55] = IDS_MESSAGE3;
	szID[56] = IDS_MESSAGE4;
	szID[57] = IDS_MESSAGE5;
	szID[58] = IDS_MESSAGE6;
	szID[59] = IDS_MESSAGE7;
	szID[60] = IDS_MESSAGE8;
	szID[61] = IDS_MESSAGE9;
	szID[62] = IDS_MESSAGE10;
	szID[63] = IDS_MESSAGE11;
	szID[64] = IDS_MESSAGE12;
	szID[65] = IDS_MESSAGE13;
	szID[66] = IDS_MESSAGE14;
	szID[67] = IDS_MESSAGE15;
	szID[68] = IDS_MESSAGE16;
	szID[69] = IDS_MESSAGE17;
	szID[70] = IDS_MESSAGE18;
	szID[71] = IDS_MESSAGE19;
	szID[72] = IDS_MESSAGE20;
	szID[73] = IDS_MESSAGE21;
	szID[74] = IDS_MESSAGE22;
	szID[75] = IDS_MESSAGE23;
	szID[76] = IDS_MESSAGE24;
	szID[77] = IDS_MESSAGE25;
	szID[78] = IDS_MESSAGE26;
	szID[79] = IDS_MESSAGE27;
	szID[80] = IDS_MESSAGE28;
	szID[81] = IDS_MESSAGE29;
	szID[82] = IDS_MESSAGE30;
	szID[83] = IDS_MESSAGE31;
	szID[84] = IDS_MESSAGE32;
	szID[85] = IDS_MESSAGE33;
	szID[86] = IDS_MESSAGE34;
	szID[87] = IDS_MESSAGE35;
	szID[88] = IDS_MESSAGE36;
	szID[89] = IDS_MESSAGE37;
	szID[90] = IDS_MESSAGE38;
	szID[91] = IDS_MESSAGE39;
	szID[92] = IDS_MESSAGE40;
	szID[93] = IDS_MESSAGE41;
	szID[94] = IDS_MESSAGE42;
	szID[95] = IDS_MESSAGE43;
	szID[96] = IDS_MESSAGE44;
	szID[97] = IDS_MESSAGE45;
	szID[98] = IDS_MESSAGE46;
	szID[99] = IDS_MESSAGE47;
	szID[100] = IDS_MESSAGE48;
	szID[101] = IDS_MESSAGE49;
	szID[102] = IDS_MESSAGE50;
	szID[103] = IDS_MESSAGE51;
	szID[104] = IDS_MESSAGE52;
	szID[105] = IDS_MESSAGE53;
	szID[106] = IDS_MESSAGE54;
	szID[107] = IDS_MESSAGE55;
	szID[108] = IDS_MESSAGE56;
	szID[109] = IDS_MESSAGE57;
	szID[110] = IDS_MESSAGE58;
	szID[111] = IDS_MESSAGE59;
	szID[112] = IDS_MESSAGE60;
	szID[113] = IDS_MESSAGE61;
	szID[114] = IDS_MESSAGE62;
	szID[115] = IDS_MESSAGE63;
	szID[116] = IDS_MESSAGE64;
	szID[117] = IDS_MESSAGE65;
	szID[118] = IDS_MESSAGE66;
	szID[119] = IDS_MESSAGE67;
	szID[120] = IDS_MESSAGE68;
	szID[121] = IDS_MESSAGE69;
	szID[122] = IDS_MESSAGE70;
	szID[123] = IDS_MESSAGE71;
	szID[124] = IDS_MESSAGE72;
	szID[125] = IDS_MESSAGE73;

	strID[0] = "ABOUT_BOX_ID_OK";
	strID[1] = "DOWNAREA_ID_OK";
	strID[2] = "GAMESET_ID_CANCEL";
	strID[3] = "GAMESET_ID_OK";
	strID[4] = "IDC_AREA_STATIC";
	strID[5] = "IDC_ATI_STATIC";
	strID[6] = "IDC_BUTTON_EXIT";
	strID[7] = "IDC_BUTTON_OPTION";
	strID[8] = "IDC_BUTTON_PATCH";
	strID[9] = "IDC_BUTTON_RETRY";
	strID[10] = "IDC_BUTTON_START";
	strID[11] = "IDC_CHECK_NOCHECK";
	strID[12] = "IDC_CHECK_PASSIVE";
	strID[13] = "IDC_CHECK_USE_HTTP";
	strID[14] = "IDC_CHECK_WINDOWMODE";
	strID[15] = "IDC_DISPLAY_STATIC";
	strID[16] = "IDC_DOWNAREA_STATIC";
	strID[17] = "IDC_DOWNLOAD_STATIC";
	strID[18] = "IDC_NVIDIA_STATIC";
	strID[19] = "IDC_READYTOSTART";
	strID[20] = "IDC_STATIC_ATI_LINK";
	strID[21] = "IDC_STATIC_NVIDIA_LINK";
	strID[22] = "IDC_TITLE_STATIC";
	strID[23] = "WARNING_ID_OK";
	strID[24] = "IDD_GAMESETDLG";	
	strID[25] = "IDD_ABOUT_BOX";
	strID[26] = "IDD_DIALOG_DOWNLOAD_AREA";
	strID[27] = "IDD_DIALOG_WARNING_WHQL";	
	strID[28] = "IDC_VIDEO_STATIC";
	strID[29] = "IDC_VER_STATIC";
	strID[30] = "IDS_WHQL_CVD";
	strID[31] = "IDS_WHQL_UCVD";
	strID[32] = "IDS_ABOUTBOX";
	
	strID[33] = "IDC_GRAPHIC_OPTION";
	strID[34] = "IDC_SOUND_OPTION";
	strID[35] = "IDS_MESSAGE";

	CString strBuff;
	CStringArray* strArray;
	int i;
	
	for ( i = 0; i< 33; ++i)
	{
		strArray = new CStringArray;
		strBuff.LoadString(szID[i]);
		strArray->Add(strBuff);
		m_LauncherText.mapFlags[strID[i]] = strArray;
	}

	// 弊贰侨 可记
	strArray = new CStringArray;
	for ( i = 0; i< 13; ++i) 
	{		
		strBuff.LoadString(szID[i+33]);		
		strArray->Add(strBuff);
	}
	m_LauncherText.mapFlags[strID[33]] = strArray;

	
	// 荤款靛 可记
	strArray = new CStringArray;
	for ( i = 0; i< 6; ++i) 
	{		
		strBuff.LoadString(szID[i+46]);		
		strArray->Add(strBuff);
	}
	m_LauncherText.mapFlags[strID[34]] = strArray;


	// IDS_MESSAGE
	strArray = new CStringArray;
	for ( i = 0; i< 74; ++i) 
	{		
		strBuff.LoadString(szID[i+52]);		
		strArray->Add(strBuff);
	}
	m_LauncherText.mapFlags[strID[35]] = strArray;

	return TRUE;
}


const CString & CLauncherTextMan::GetLauncherText ( std::string strKey, int Index )
{
	return GetText ( strKey, Index);
}

const CString &	CLauncherTextMan::GetText ( std::string strKey, int Index)
{
	return NS_TEXT_LOADER::GetText ( strKey, Index, m_LauncherText.mapFlags );
}

int	CLauncherTextMan::GetNumber ( std::string strKey, int Index)
{
	return NS_TEXT_LOADER::GetNumber ( strKey, Index, m_LauncherText.mapFlags );
}

const char * ID2LAUNCHERTEXT( const char* szKeyword, int nIndex )
{
	return CLauncherTextMan::GetInstance().GetText( szKeyword, nIndex);
}