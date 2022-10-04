#pragma once

#include "GLCharDefine.h"

struct GLNote
{
	bool m_bSelected;
	char m_szSendChar[CHAR_SZNAME];
	char m_szReciChar[CHAR_SZNAME];
	char m_szSentence[CHAR_SZNOTE];

	GLNote(void) 
		: m_bSelected(false)
	{
		memset ( m_szSendChar, 0, sizeof(char)*CHAR_SZNAME );
		memset ( m_szReciChar, 0, sizeof(char)*CHAR_SZNAME );
		memset ( m_szSentence, 0, sizeof(char)*CHAR_SZNOTE );
	}

	GLNote& operator= ( const GLNote &value );

	void Select ()   { m_bSelected = true; }
	void UnSelect () { m_bSelected = false; }
};

typedef std::vector<GLNote>			VECNOTE;
