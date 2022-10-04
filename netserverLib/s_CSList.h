///////////////////////////////////////////////////////////////////////////////
// s_CSList.h
//
// class CSList
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CSLIST_H
#define S_CSLSIT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace SERVER_UTIL
{
	typedef struct SListElmt_ {
		void *data;
		SListElmt_ *next;
	}SListElmt;

	class CSList
	{
	public:
		// Constructs an empty ordered list. 
		CSList();
		CSList::~CSList();

	private:
		int size;
		SListElmt* pHead;
		SListElmt* pTail;

		// Head/Tail Access

	public:
		// Returns the head element of the list (cannot be empty). 
		SListElmt* GetHead(); 
		// Returns the tail element of the list (cannot be empty). 
		SListElmt* GetTail(); 

		// Operations

		// Removes the element from the head of the list. 
		void RemoveHead(); 
		// Removes the element from the tail of the list.  
		void RemoveTail();
		// Adds an element (or all the elements in another list) to the head of the list (makes a new head). 
		void AddHead(void *data);
		// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
		void AddTail(void *data); 
		// Removes all the elements from this list. 
		void RemoveAll();


		// Iteration

		// Returns the position of the head element of the list. 
		SListElmt* GetHeadPosition();
		// Returns the position of the tail element of the list. 
		SListElmt* GetTailPosition();
		// Gets the next element for iterating. 
		SListElmt* GetNext(SListElmt* pElement);
		// Gets the previous element for iterating. 
	//	GetPrev();

		// Retrieval/Modification

		// Gets the element at a given position. 
	//	GetAt(); 
		// Sets the element at a given position. 
	//	SetAt(); 
		// Removes an element from this list, specified by position. 
		void RemoveAt(SListElmt* pElement);

		// Insertion

		// Inserts a new element before a given position. 
	//	InsertBefore();	
		// Inserts a new element after a given position. 
	//	InsertAfter();

		// Searching

		// Gets the position of an element specified by pointer value. 
	//	Find();
		// Gets the position of an element specified by a zero-based index. 
	//	FindIndex();

		// Status

		// Returns the number of elements in this list. 
		int GetCount();
		// Tests for the empty list condition (no elements). 
		bool IsEmpty();
	};
}

#endif // S_CSLSIT_H