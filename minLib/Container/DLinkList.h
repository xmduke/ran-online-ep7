#ifndef _DLINKLIST_H_
#define _DLINKLIST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace minLib 
{

template<typename TYPE>
struct NODE
{
	TYPE	Data;		
	NODE*	pPrev;
	NODE*	pNext;

	NODE()
	{ 
		Reset();
	};

	void Reset()
	{			
		pPrev		= NULL;
		pNext		= NULL;
	};
};

template<class TYPE>
class DLinkList
{		
	
public:
	DLinkList();
	~DLinkList();

protected:
	NODE<TYPE>*	m_pHead;						// Head position
	NODE<TYPE>*	m_pTail;						// Tail position
	int			m_nSize;						// Size of list
	
public:
	NODE<TYPE>*	GetHead(void);					// Return head element
	NODE<TYPE>*	GetTail(void);					// Return tail element

	void		RemoveHead(void);				// Delete head element
	void		RemoveTail(void);				// Delete tail element

	NODE<TYPE>*	GetNext(NODE<TYPE>* pData);		// Return next element
	NODE<TYPE>*	GetPrev(NODE<TYPE>* pData);		// Return previous element
	
	void		AddHead(void);					// Add new element to head
	void		AddTail(void);					// Add new element to tail
	
	void		AddHead(NODE<TYPE>* pData);		// Add element to head
	void		AddTail(NODE<TYPE>* pData);		// Add element to tail
	
	NODE<TYPE>*	Get(void);						// Return free element
	void		Release(NODE<TYPE>* pData);		// Release element

	void		Remove(NODE<TYPE>* pData);		// Delete element	
			
	int			GetSize(void);					// Return size of list

	bool		IsEmpty(void);					// Check, is list empty?
	bool		IsHead(NODE<TYPE>* pData);
	bool		IsTail(NODE<TYPE>* pData);
	bool		IsEnd(NODE<TYPE>* pData);
};

template<typename TYPE>
DLinkList<TYPE>::DLinkList()
{
	m_pHead = NULL;		
	m_pTail = m_pHead;
	m_nSize = 0;

}

template<typename TYPE>
DLinkList<TYPE>::~DLinkList()
{
    int nSize = m_nSize;
	int i;
	for (i=0; i<nSize; i++)
		RemoveHead();
}

template<typename TYPE>
void DLinkList<TYPE>::AddHead(void)
{
	NODE<TYPE>* pTemp = NULL;
	pTemp = new NODE<TYPE>;
			
	AddHead(pTemp);
}

template<typename TYPE>
void DLinkList<TYPE>::AddHead(NODE<TYPE>* pData)
{
	if (pData == NULL) return;

	// 비어있다면...	
	if (m_nSize == 0)
	{
		m_pHead = pData;
		m_pHead->pNext = NULL;	
		m_pHead->pPrev = NULL;
		m_pTail = m_pHead;
	}
	else
	{	
		pData->pPrev = NULL;
		pData->pNext = m_pHead;
		m_pHead->pPrev = pData;
		m_pHead = pData;
	}
	m_nSize++;
}	

template<typename TYPE>
void DLinkList<TYPE>::RemoveHead(void)
{
	NODE<TYPE>* pTemp = NULL;
	
	if (m_pHead == NULL) return;
		
	pTemp = m_pHead->pNext;
	delete m_pHead;
	if (pTemp != NULL)
	{
		m_pHead = pTemp;
		m_pHead->pPrev = NULL;			
	}
	m_nSize--;
}

template<typename TYPE>
NODE<TYPE>* DLinkList<TYPE>::GetHead()
{			
	return m_pHead;	
}	

template<typename TYPE>
void DLinkList<TYPE>::AddTail(void)
{
	NODE<TYPE>* pTemp = NULL;
	pTemp = new NODE<TYPE>;		
	AddTail(pTemp);
}

template<typename TYPE>
void DLinkList<TYPE>::AddTail(NODE<TYPE>* pData)
{
	if (pData == NULL) return;

	// 비어있다면...	
	if (m_nSize == 0)
	{
		m_pHead = pData;			
		m_pHead->pNext = NULL;	
		m_pHead->pPrev = NULL;
		m_pTail = m_pHead;
	}
	else
	{	
		pData->pNext = NULL;
		pData->pPrev = m_pTail;
		m_pTail->pNext = pData;
		m_pTail = pData;
	}
	m_nSize++;
}

template<typename TYPE>
void DLinkList<TYPE>::RemoveTail(void)
{
	NODE<TYPE>* pTemp = NULL;
	
	if (m_pTail == NULL) return;
		
	pTemp = m_pTail->pPrev;
	delete m_pTail;
	if (pTemp != NULL)
	{			
		m_pTail = pTemp;
		m_pTail->pNext = NULL;			
	}
	m_nSize--;
}

template<typename TYPE>
NODE<TYPE>* DLinkList<TYPE>::GetTail()
{
	return m_pTail;
}

template<typename TYPE>
NODE<TYPE>* DLinkList<TYPE>::GetNext(NODE<TYPE>* pData)
{
	return pData->pNext;
}

template<typename TYPE>
NODE<TYPE>* DLinkList<TYPE>::GetPrev(NODE<TYPE>* pData)
{
	return pData->pPrev;
}

template<typename TYPE>
NODE<TYPE>* DLinkList<TYPE>::Get(void)
{
	NODE<TYPE>* pTemp = NULL;
	pTemp = GetHead();
	if (pTemp == NULL)
	{
		AddHead();
		pTemp = GetHead();
	}
	Release(pTemp);
	return pTemp;
}

template<typename TYPE>
void DLinkList<TYPE>::Release(NODE<TYPE>* pData)
{		
	if (pData == NULL) return;

	if (pData == m_pHead)
	{
		m_pHead = pData->pNext;
		// Check Head and Tail
		if (m_pHead == NULL)
			m_pTail = NULL;
		else
			pData->pNext->pPrev = NULL;
	}
	else
	{
		pData->pPrev->pNext = pData->pNext;

		// Check tail
		if (pData->pNext == NULL)
			m_pTail = pData->pPrev;
		else
			pData->pNext->pPrev = pData->pPrev;
	}
	m_nSize--;
}

template<typename TYPE>
void DLinkList<TYPE>::Remove(NODE<TYPE>* pData)
{
	if (pData == NULL) return;

	if (pData == m_pHead)
		RemoveHead();
	else if (pData == m_pTail )
		RemoveTail();
	else
	{
		pData->pPrev->pNext = pData->pNext;
		pData->pNext->pPrev = pData->pPrev;

		delete pData;
		m_nSize--;
	}
}

template<typename TYPE>
int DLinkList<TYPE>::GetSize(void)
{ 
	return m_nSize; 
}


template<typename TYPE>
bool DLinkList<TYPE>::IsEmpty(void)
{
	if (m_nSize == 0) return true;
	else				return false;
}

template<typename TYPE>
bool DLinkList<TYPE>::IsHead(NODE<TYPE>* pData)
{
	if (pData == m_pHead) return true;
	else					return false;
}

template<typename TYPE>
bool DLinkList<TYPE>::IsTail(NODE<TYPE>* pData)
{
	if (pData == m_pTail) return true;
	else					return false;
}

template<typename TYPE>
bool DLinkList<TYPE>::IsEnd(NODE<TYPE>* pData)
{
	return IsTail(pData);
}

} // namespace minLib

#endif // _DLINKLIST_H_