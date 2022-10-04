#pragma	once

#include "../EngineUIlib/GUInterface/BasicScrollBar.h"

class	CBasicScrollBarEx : public CBasicScrollBar
{
public:
	CBasicScrollBarEx ();
	virtual	~CBasicScrollBarEx ();

public:
    void	CreateBaseScrollBar ( char* szBaseScrollBar, const bool& bVERTICAL = true );
};