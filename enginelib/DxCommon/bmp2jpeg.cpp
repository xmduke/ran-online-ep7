#include "stdafx.h"

#include "ijl.h"
#include <string>
#include "bmp2jpeg.h"

#pragma comment(lib, "../=Library/ijl15.lib") // lib 파일을 등록해야 합니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

BOOL BitmapToJPG ( LPCTSTR lpszPathName, BOOL bBitmapDelete )
{
    CBitmap csBitmap;
    BITMAP  stBitmap;
    bool bRes = true; // 리턴값

	// 비트맵 로드
	csBitmap.m_hObject = LoadImage(NULL, lpszPathName, IMAGE_BITMAP, NULL, NULL,
									LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	csBitmap.GetBitmap(&stBitmap); // 비트맵 정보를 얻는다.
	
	IJLERR jerr;
	DWORD dib_pad_bytes;
	
	// Allocate the IJL JPEG_CORE_PROPERTIES structure.
	JPEG_CORE_PROPERTIES jcprops;
	BYTE * imageData; // 이미지 픽셀 정보

	while(true)
	{
		// Initialize the IntelR JPEG Library.
		jerr = ijlInit(&jcprops);

		if(IJL_OK != jerr)
		{
			TRACE0("JCP 초기화에 실패했습니다.\n");
			bRes = false;
			break;
		}

		int nDIBChannels = 3;
		if ( stBitmap.bmBitsPixel != 24 && stBitmap.bmBitsPixel != 32 )
		{
			TRACE0("24비트, 32비트 비트맵이 아닙니다.");
            bRes = false;
			break;
		}

		if ( stBitmap.bmBitsPixel == 32 )

		dib_pad_bytes = IJL_DIB_PAD_BYTES(stBitmap.bmWidth, 3);
	
		// Set up information to write from the pixel buffer.
		jcprops.DIBWidth    = stBitmap.bmWidth;
		jcprops.DIBHeight   = stBitmap.bmHeight;
		jcprops.DIBChannels = 3;
		jcprops.DIBPadBytes = dib_pad_bytes;
		jcprops.DIBColor    = IJL_BGR;
		
		int imageSize = (stBitmap.bmWidth*stBitmap.bmBitsPixel/8+dib_pad_bytes) * stBitmap.bmHeight;

		imageData = new BYTE[imageSize]; // 이미지 사이즈 만큼 데어터를 초기화
        if ( imageData==NULL )
        {
            TRACE( "이미지를 위한 메모리 할당에 실패했습니다.\n" );
            AfxThrowUserException();
            bRes = false;
			break;
        }

		DWORD dwRead = csBitmap.GetBitmapBits ( imageSize , imageData); // 비트맵 이지미 정보를 얻는다.
		if ( dwRead==0 )
		{
            TRACE( "GetBitmapBits() dib 읽기에 실패하였습니다.\n" );
			AfxThrowUserException();
            bRes = false;
			break;
		}

		if ( stBitmap.bmBitsPixel==32 )
		{
			for ( int i=0; i<imageSize/4; ++i )
			{
				DWORD dwDATA = * (DWORD*) ( imageData + i*4 );

				*(imageData+i*3+0) = BYTE(dwDATA>>0&0xFF);
				*(imageData+i*3+1) = BYTE(dwDATA>>8&0xFF);
				*(imageData+i*3+2) = BYTE(dwDATA>>16&0xFF);
			}
		}

		jcprops.DIBBytes        = imageData;
				
		string JPGPathName(lpszPathName);
		string::size_type idx = JPGPathName.find_last_of(".");
		JPGPathName.replace(idx+1, string::npos, "jpg");

		jcprops.JPGFile         = const_cast<LPTSTR>(JPGPathName.c_str());
	    		
		// Specify JPEG file creation parameters.
		jcprops.JPGWidth        = stBitmap.bmWidth;
		jcprops.JPGHeight       = stBitmap.bmHeight;
		
		// Note: the following are default values and thus
		// do not need to be set.
		jcprops.JPGChannels     = 3;
		jcprops.JPGColor        = IJL_YCBCR;
		jcprops.JPGSubsampling  = IJL_411; // 4:1:1 subsampling.
		jcprops.jquality        = 90; // 이미지의 화질, 보통은 75, 최고 100
		
		// Write the actual JPEG image from the pixel buffer.
		jerr = ijlWrite(&jcprops,IJL_JFILE_WRITEWHOLEIMAGE);
		
		if(IJL_OK != jerr)
		{
			TRACE0("이미지를 파일에 저장하는데 실패했습니다.\n");
			bRes = false;
			break;
		}
		
		break; // while 루프를 빠져나옵니다.
	}

	// Clean up the IntelR JPEG Library.
	ijlFree(&jcprops);

    if( bBitmapDelete == TRUE )		// 만약 플래그가 true이면
        DeleteFile(lpszPathName);	// 비트맵 파일을 삭제합니다.

	if ( imageData )
		delete [] imageData; // 동적으로 할당된 메모리 해제

	return bRes;
}
