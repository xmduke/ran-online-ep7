////////////////////////////////////////////////////////////////////////////////
//	이름	:	CSoundSourceManagerDoc
//	목적	:	실질적인 데이타(wav, 코멘트...) 관리를 함
//	부가설명:
//	설계기능:	Search, Backup, 강제 수정, 예전 이름과 비교후 파일 중복 체크
//				Category 이용.
//	작성자	:	성기엽
//	eMail	:	kysung@mincoms.co.kr
//	WWW		:	http://www.ran-online.co.kr
//	
//	로그
//	$6.	2002-11-28T18:57	[기본 디렉토리.. Sounds로 설정]
//	$5.	2002-10-31T09:30	[SoundSourceMan 클래스로 Doc에 박힌 구조 뜯어내기]
//	$4.	2002-10-24T17:36	[데이타 Locking 구현, 구간 삭제 구현]
//	$3.	2002-10-24T11:03	[SSoundSource구조체상에,
//							코드이름으로 바뀌기 전의 파일 이름 저장 필드삽입]
//							[Lock 필드 삽입 ( 수정할수 없도록 만드는 필드 )]
//							[음원 테스트 모듈 삽입]
//							[파일 이름 포맷 지정]
//	$2.	2002-10-23T16:43	[버전 관리 추가]
//	$1.	2002-10-22T21:07	[최초 작성, 세이브 로드]
//							[오류 복구 시스템]
//							[Primary Key 생성 모듈 작성]
//
////////////////////////////////////////////////////////////////////////////////

// SoundSourceManagerDoc.h : CSoundSourceManagerDoc 클래스의 인터페이스
//
#pragma once

class CSoundSourceManagerDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CSoundSourceManagerDoc();
	DECLARE_DYNCREATE(CSoundSourceManagerDoc)

//	<--	오류 복구 시스템	-->	//
private:
	BOOL	ExtraSaveFile ();
	BOOL	ExtraLoadFile ();
	BOOL	ProgramNormalExit();

//	<--	세이브/로드	-->	//
public:
	BOOL	DefaultSaveFile ();
	BOOL	DefaultLoadFile ();

// 재정의
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현
public:
	virtual ~CSoundSourceManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()

};