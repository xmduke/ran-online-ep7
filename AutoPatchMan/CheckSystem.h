#pragma	once

namespace	NS_CHECK_SYSTEM
{	
	//	설명
	//		시스템의 DirectX의 버전을 가져온다.
	//		버전 정보를 체크한 적이 있다면,
	//		파일에서 버전을 로드한다.
	//		GetDXVersion ()의 부하가 생각보다 큼.
	//
	//	그럼 DirectX의 버전이 갱신되었다면...?
	DWORD	GetDirectXVersion ();
};