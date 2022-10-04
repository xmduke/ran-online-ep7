#pragma once
#include "../engineuilib\guinterface\uigroup.h"
#include "../EngineUILib/GUInterface/BasicAnimationBox.h"
#include "UIWindowEx.h"

class CPetSkinMixImage : public CUIWindowEx
{
protected:
	enum
	{
		PETSKIN_MIX_IMAGE = NO_ID + 1,
	};

public:
	CPetSkinMixImage(void);
	~CPetSkinMixImage(void);


public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
	inline void	SetItemData( WORD wPosX, WORD wPosY, SNATIVEID sItemData ) 
	{ 
		m_ItemPosX  = wPosX; 
		m_ItemPosY  = wPosY; 
		m_sItemID	= sItemData;
	}


public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicAnimationBox*		m_pAniBox;	

	WORD					m_ItemPosX;
	WORD					m_ItemPosY;
	SNATIVEID				m_sItemID;

};
