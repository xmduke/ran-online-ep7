#include "StdAfx.h"
#include "ConftConfirm.h"
#include "../EngineUIlib/GUInterface/BasicProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CConftConfirm::CConftConfirm ()
{
}

CConftConfirm::~CConftConfirm ()
{
}

void	CConftConfirm::SetPercent ( const float fPercent )
{
	m_pProgress->SetPercent ( fPercent );
}

void CConftConfirm::CreateSubControl ()
{    
	CBasicProgressBar* pProgress = new CBasicProgressBar;
	pProgress->CreateSub ( this, "CONFT_CONFIRM_PROGRESS", UI_FLAG_DEFAULT, CONFT_CONFIRM );
	pProgress->CreateOverImage ( "CONFT_CONFIRM_PROGRESS_OVER" );
	RegisterControl ( pProgress );
	m_pProgress = pProgress;
}