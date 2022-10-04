#ifndef _CListEx_H_
#define _CListEx_H_

#include <windows.h>

//-----------------------------------------------
//
//	동적 할당된 데이타 관리와 삭제.
//	데이타 동적할당은 없음.
//	할당된 데이타 관리.
//
//	리스트  탐색 함수 원형.
//
//	BOOL (*FindFunc)( void*, LPTYPE )
//	
//	void* -- 탐색시의 비교값.
//	LPTYPE -- 탐색될 노드의 주소.
//
//-----------------------------------------------
//
//	CListEX::FindNode ( void *pValue, LPTYPE &pData ) 함수 추가
//	이 함수는 CListEX::pCurrent 의 값이 바뀌지 않는다.
//
//	Last Update - 2000/07/12 -jdh.
//

template<class TYPE>
class CListEx
{
protected:
	class NODE
	{
	public:
		NODE *Before, *Next;
		TYPE *pData;

	public:
		NODE() { pData=NULL;}
		~NODE() { if ( pData!=NULL) delete pData; }
	};
	typedef TYPE* LPTYPE;
	
	static BOOL DefaultFindFunc ( void* value, LPTYPE pData);
	static BOOL DefaultAddFunc( void* value, LPTYPE pData,CListEx<TYPE>* pList);

	BOOL (*FindFunc) ( void*, LPTYPE );
	BOOL (*AddFunc) (void*, LPTYPE,CListEx<TYPE>*);

	NODE Head, Tail;	// 시작, 끝
	NODE *pCurrent;		// 탐색 위치.
	int	 Amount;
	// 노드 생성, 파괴.
	NODE* NewNode () { return new NODE;}
	void DelNode ( NODE *pNode );

public:
	// 생성자, 파괴자, 초기화
	CListEx ();
	CListEx ( void* Func );
	~CListEx ();
	void SetFindFunc( void* Func );
	
	//2002.05.10추가
	void SetAddFunc( void* Func);

	// 상태검사.
	BOOL IsEmpty ();
	BOOL IsEnd ();

	// 데이타 삽입
	void AddHead ( LPTYPE pData );
	void AddTail ( LPTYPE pData );
	void AddCurrent ( LPTYPE pData );
	
	// 데이타 삭제
	BOOL DelHead ();
	BOOL DelTail ();
	BOOL DelCurrent ();
	void RemoveAll ();

	// 데이타 값확인.
	BOOL GetHead ( LPTYPE &pData );
	BOOL GetTail ( LPTYPE &pData );
	BOOL GetCurrent ( LPTYPE &pData );
	LPTYPE GetCurrent();
	// 데이타 탐색.
	void SetHead ();
	void GoNext ();
	void GoPrev();
	BOOL FindNode ( void *pValue );
	BOOL FindNode ( void *pValue, LPTYPE &pData );
	
	//2002.06.07추가
	int GetAmount(){return Amount;}
};

//-------------------- 노드 생성, 파괴.
template<class TYPE>
inline void CListEx<TYPE>::DelNode ( NODE *pNode )
{
	if ( pCurrent==pNode )
	{
		if ( pCurrent->Next!=&Tail ) pCurrent = pCurrent->Next;
		else pCurrent = pCurrent->Before;
	}

	if ( pNode ) { delete pNode; pNode = NULL; }
}

//-------------------- 생성자, 파괴자, 초기화
template<class TYPE>
CListEx<TYPE>::CListEx ()
{
	Head.Before = &Tail;
	Head.Next = &Tail;

	Tail.Before = &Head;
	Tail.Next = &Head;
	
	pCurrent = Head.Next;

	FindFunc = DefaultFindFunc;
	//
	AddFunc = DefaultAddFunc;
	Amount=0;
}

template<class TYPE>
CListEx<TYPE>::CListEx ( void* Func )
{
	Head.Before = &Tail;
	Head.Next = &Tail;

	Tail.Before = &Head;
	Tail.Next = &Head;
	
	pCurrent = Head.Next;

	FindFunc = ( BOOL (__cdecl*)( void*, LPTYPE ) ) Func;
	//
	AddFunc = DefaultAddFunc;
	Amount=0;
}

template<class TYPE>
CListEx<TYPE>::~CListEx ()
{
	RemoveAll ();
}

template<class TYPE >
void CListEx<TYPE>::SetFindFunc( void* Func )
{
	FindFunc = ( BOOL (__cdecl*)( void*, LPTYPE ) ) Func;
}

template<class TYPE>
void CListEx<TYPE>::SetAddFunc(void* Func)
{
	AddFunc= ( BOOL (__cdecl*)( void*, LPTYPE ,CListEx<TYPE>*) ) Func;

}

//-------------------- 상태검사.
template<class TYPE>
inline BOOL CListEx<TYPE>::IsEmpty ()
{
	if ( Head.Next == &Tail ) return TRUE;
	return FALSE;
}

template<class TYPE>
inline BOOL CListEx<TYPE>::IsEnd ()
{
	if ( pCurrent == &Tail || pCurrent == &Head ) return TRUE;
	return FALSE;
}

template<class TYPE>
inline BOOL CListEx<TYPE>::DefaultFindFunc ( void* value, LPTYPE pData)
{
//	LPTYPE pValue = (TYPE*) value;
//	if ( (*pValue)==(*pData) ) return TRUE;
	return FALSE;
}
template<class TYPE>
inline BOOL CListEx<TYPE>::DefaultAddFunc ( void* value, LPTYPE pData,CListEx<TYPE>* pList)
{
//	LPTYPE pValue = (TYPE*) value;
//	if ( (*pValue)==(*pData) ) return TRUE;
	return FALSE;
}
//-------------------- 데이타 삽입.
template<class TYPE>
void CListEx<TYPE>::AddHead ( LPTYPE pData )
{
	NODE *pBackNode = Head.Next;
	
	NODE *pNewNode = NewNode ();	// 새 노드 생성.
	pNewNode->pData = pData;		// 값 입력.
	
	pNewNode->Next = pBackNode;		// 연결.
	pNewNode->Before = &Head;

	Head.Next = pNewNode;
	pBackNode->Before = pNewNode;

	//
	//
	Amount++;
}

template<class TYPE>
void CListEx<TYPE>::AddTail ( LPTYPE pData )
{
	NODE *pBackNode = Tail.Before;

	NODE *pNewNode = NewNode ();	// 새 노드 생성.
	pNewNode->pData = pData;			// 값 입력.

	pNewNode->Next = &Tail;			// 연결.
	pNewNode->Before = pBackNode;

	Tail.Before = pNewNode;
	pBackNode->Next = pNewNode;
	//
	//
	Amount++;
}

template<class TYPE>
void CListEx<TYPE>::AddCurrent ( LPTYPE pData )
{
	GASSERT ( pCurrent!=&Tail );

	NODE *pNextNode = pCurrent->Next;

	NODE *pNewNode = NewNode ();	// 새 노드 생성.
	pNewNode->pData = pData;			// 값 입력.
	
	pNewNode->Next = pNextNode;		// 연결.
	pNewNode->Before = pCurrent;

	pNextNode->Before = pNewNode;
	pCurrent->Next = pNewNode;

	pCurrent = pNewNode;
	//
	//
	Amount++;
}

//-------------------- 데이타 삭재.
template<class TYPE>
BOOL CListEx<TYPE>::DelHead ()
{
	if ( IsEmpty () ) return FALSE;

	NODE *pDelNode = Head.Next;
	
	Head.Next = pDelNode->Next;		// 노드 연결 변경.
	pDelNode->Next->Before = &Head;

	DelNode ( pDelNode );			// 노드 삭제.
	//
	//
	Amount--;
	return TRUE;
}

template<class TYPE>
BOOL CListEx<TYPE>::DelTail ()
{
	if ( IsEmpty () ) return FALSE;

	NODE *pDelNode = Tail.Before;
	
	Tail.Before = pDelNode->Before;	// 노드 연결 변경.
	pDelNode->Before->Next = &Tail;

	DelNode ( pDelNode );			// 노드 삭제.
	//
	//
	Amount--;
	return TRUE;
}

template<class TYPE>
BOOL CListEx<TYPE>::DelCurrent ()
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return FALSE;

	NODE *pBeforeNode = pCurrent->Before;
	NODE *pNextNode = pCurrent->Next;

	pBeforeNode->Next = pNextNode;
 	pNextNode->Before = pBeforeNode;

	DelNode ( pCurrent );			// 노드 삭제.

	pCurrent = pNextNode;
	//
	//
	Amount--;
	return TRUE;
}

template<class TYPE>
void CListEx<TYPE>::RemoveAll ()
{
	for( SetHead(); !IsEmpty() ; )
	{
		DelCurrent();
	}
	//
	//
	Amount=0;
}

//-------------------- 데이타 값확인.
template<class TYPE>
BOOL CListEx<TYPE>::GetHead ( LPTYPE &pData)
{
	if ( IsEmpty () ) return FALSE;

	pData = Head.Next->pData;		// 값 출력.

	return TRUE;
}

template<class TYPE>
BOOL CListEx<TYPE>::GetTail ( LPTYPE &pData )
{
	if ( IsEmpty () ) return FALSE;

	pData = Tail.Before->pData;		// 값 출력.

	return TRUE;
}

template<class TYPE>
BOOL CListEx<TYPE>::GetCurrent ( LPTYPE &pData )
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return FALSE;

	pData = pCurrent->pData;		// 값 출력.

	return TRUE;
}
template<class TYPE>
TYPE* CListEx<TYPE>::GetCurrent ()
{
	if ( pCurrent == &Head || pCurrent == &Tail ) return FALSE;
	return pCurrent->pData;
}

//-------------------- 데이타 탐색
template<class TYPE>
inline void CListEx<TYPE>::SetHead ()
{
	pCurrent = Head.Next;
}

template<class TYPE>
inline void CListEx<TYPE>::GoNext ()
{
	GASSERT ( pCurrent != &Tail && pCurrent != &Head );

	pCurrent = pCurrent->Next;
}

template<class TYPE>
inline void CListEx<TYPE>::GoPrev ()
{
	GASSERT ( pCurrent != &Tail && pCurrent != &Head );

	pCurrent = pCurrent->Before;
}

template<class TYPE>
BOOL CListEx<TYPE>::FindNode ( void *pValue )
{
	if ( FindFunc == NULL ) return FALSE;

	BOOL IsFind = FALSE;
	LPTYPE pFindData=NULL;
	for( SetHead(); !IsEnd(); GoNext() )
	{
		GetCurrent ( pFindData );
		if( FindFunc ( pValue, pFindData) )
		{
			IsFind = TRUE;
			break;
		}
	}
	
	return (IsFind);
}

template<class TYPE>
BOOL CListEx<TYPE>::FindNode ( void *pValue, LPTYPE &pData )
{
	if ( FindFunc == NULL ) return FALSE;

	// 탐색전의 Current 값 백업
	NODE *pBackCurrent = pCurrent;

	BOOL IsFind = FALSE;
	LPTYPE pFindData=NULL;
	for( SetHead(); !IsEnd(); GoNext() )
	{
		GetCurrent ( pFindData );
		if( FindFunc ( pValue, pFindData) )
		{
			pData = pFindData;
			IsFind = TRUE;
			break;
		}
	}
	
	// 커런트 값 복원
	pCurrent = pBackCurrent;

	return (IsFind);
}
#endif //  _CListEx_H_