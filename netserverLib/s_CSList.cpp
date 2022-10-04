///////////////////////////////////////////////////////////////////////////////
// s_CSList.cpp
//
// class CSList
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CSList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SERVER_UTIL;

CSList::CSList()
{
	size = 0;
	pHead = NULL;
	pTail = NULL;
}

CSList::~CSList()
{
	RemoveAll();
}

// Head/Tail Access

// Returns the head element of the list (cannot be empty). 
SListElmt* CSList::GetHead()
{
	return pHead;
}

// Returns the tail element of the list (cannot be empty). 
SListElmt* CSList::GetTail()
{
	return pTail;
}

// Operations

// Removes the element from the head of the list. 
void CSList::RemoveHead()
{
	SListElmt* pTemp;
	
	if (pHead == NULL)
	{
		return;
	}
	// data 는 사용자가 직접 지워야 한다.
	pTemp = pHead->next;
	delete pHead;
	pHead = pTemp;
	size--;
}

// Removes the element from the tail of the list.  
void CSList::RemoveTail()
{
}

// Adds an element (or all the elements in another list) to the head of the list (makes a new head). 
void CSList::AddHead(void *data)
{
	SListElmt* pNewElement;	
	
	pNewElement = new SListElmt;
	pNewElement->data = data;
	
	// 비어있다면...
	if (size == 0)
	{
		pHead = pNewElement;	
		pHead->next = NULL;	
		pTail = pNewElement;
		pTail->next = NULL;
	}
	else
	{	
		pNewElement->next = pHead;
		pHead = pNewElement; 
	}

	size++;
}

// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
void CSList::AddTail(void *data)
{
	SListElmt* pNewElement;	
	
	pNewElement = new SListElmt;
	pNewElement->data = data;
	
	// 비어있다면...
	if (size == 0)
	{
		pHead = pNewElement;	
		pHead->next = NULL;	
		pTail = pNewElement;
		pTail->next = NULL;
	}
	else
	{	
		pNewElement->next = NULL;
		pTail->next = pNewElement;
		pTail = pNewElement;	
	}
	size++;
}

// Removes all the elements from this list. 
void CSList::RemoveAll()
{	
	int i;
	int nSize;	

	SListElmt* pTemp;
	nSize = size;

	for (i=0; i<nSize; i++)
	{
		pTemp = pHead->next;
		delete pHead;
		pHead = pTemp;		
	}
	
	pHead = NULL;
	pTail = NULL;
	size = 0;
}

// Iteration

// Returns the position of the head element of the list. 
SListElmt* CSList::GetHeadPosition()
{
	return pHead;
}

// Returns the position of the tail element of the list. 
SListElmt* CSList::GetTailPosition()
{
	return pTail;
}

// Gets the next element for iterating. 
SListElmt* CSList::GetNext(SListElmt* pElement)
{
	return pElement->next;
}

// Gets the previous element for iterating. 
//CSList::GetPrev()
//{
//}

// Retrieval/Modification

// Gets the element at a given position. 
//CSList::GetAt()
//{
//}

// Sets the element at a given position. 
//CSList::SetAt()
//{
//}

// Removes an element from this list, specified by position. 
//void CSList::RemoveAt(SListElmt* pElement)
//{
//}

// Insertion

// Inserts a new element before a given position. 
//CSList::InsertBefore()
//{
//}

// Inserts a new element after a given position. 
//CSList::InsertAfter()
//{
//}

// Searching

// Gets the position of an element specified by pointer value. 
//CSList::Find()
//{
//}

// Gets the position of an element specified by a zero-based index. 
//CSList::FindIndex()
//{
//}

// Status

// Returns the number of elements in this list. 
int CSList::GetCount()
{
	return size;
}

// Tests for the empty list condition (no elements). 
bool CSList::IsEmpty()
{
	if (size == 0)
		return true;
	else
		return false;
}