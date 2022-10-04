#pragma once

#include "../EngineUIlib/GUInterface/UIGroup.h"
#include "../EngineUILib/GUInterface/BasicAnimationBox.h"
#include "GLItem.h"
#include "UIWindowEx.h"

class CGambleAnimationBox : public CUIGroup
{
private:

protected:
	enum
	{
		GAMBLE_ANIMATION_SHUFFLE1 = NO_ID + 1,
	};

public:
	CGambleAnimationBox(void);
	virtual ~CGambleAnimationBox(void);

public:
	void	CreateSubControl ();

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );


private:
	CUIControl * m_pControl;
	CBasicAnimationBox* m_pAniBox;
	float m_TotElapsedTime;

};