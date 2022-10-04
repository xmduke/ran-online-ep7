#include "StdAfx.h"
#include ".\glnote.h"
#include <strsafe.h>

GLNote& GLNote::operator= ( const GLNote &value )
{
	m_bSelected = value.m_bSelected;
	StringCchCopy ( m_szSendChar, CHAR_SZNAME, value.m_szSendChar );
	StringCchCopy ( m_szReciChar, CHAR_SZNAME, value.m_szReciChar );
	StringCchCopy ( m_szSentence, CHAR_SZNOTE, value.m_szSentence );

	return *this;
}
