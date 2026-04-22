// DesktopPet.cpp: implementation of the CDesktopPet class.
//
//////////////////////////////////////////////////////////////////////

#include "DesktopPet.h"

//constants
#define UPDATE_COUNTER_MAX 50 //number of redraws until desktop handle is recalculated

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDesktopPet::CDesktopPet() : CPet(), CDesktopHack()
{
	//clear the update counter
	m_nUpdateHWndCounter = 0;

	// Span the virtual screen (union of all monitor bounds). The virtual-screen
	// origin can be negative — SM_XVIRTUALSCREEN/SM_YVIRTUALSCREEN — when a
	// secondary monitor sits to the left of or above the primary.
	const int x  = GetSystemMetrics(SM_XVIRTUALSCREEN);
	const int y  = GetSystemMetrics(SM_YVIRTUALSCREEN);
	const int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	const int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	SetRect(&m_rcBounds, x, y, x + cx - 1, y + cy - 1);
}

CDesktopPet::~CDesktopPet()
{
	RedrawWindow( m_hWndDesktop, nullptr, nullptr, RDW_INVALIDATE|RDW_ERASE );
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

void CDesktopPet::Draw( int nImage )
{
	//grab the device context of the desktop
	HDC hDC = GetDC( m_hWndDesktop );
	
	//draw the icon on it
    DrawIconEx( hDC, m_ptPosition.x, m_ptPosition.y, m_hIcons[nImage], m_sizeImage.cx, m_sizeImage.cy, 0, nullptr, DI_NORMAL );

	//release the device context
    ReleaseDC( m_hWndDesktop, hDC );

	//increment the update counter and update if required
	if( ++m_nUpdateHWndCounter > UPDATE_COUNTER_MAX )
	{
		FindDesktopHandle();
		const int x  = GetSystemMetrics(SM_XVIRTUALSCREEN);
		const int y  = GetSystemMetrics(SM_YVIRTUALSCREEN);
		const int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		const int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		SetRect(&m_rcBounds, x, y, x + cx - 1, y + cy - 1);
	}
}

void CDesktopPet::Erase()
{
	//set up rect to be erased
    RECT rcErase = { m_ptOldPosition.x, m_ptOldPosition.y, 
                     m_ptOldPosition.x + m_sizeImage.cx, m_ptOldPosition.y + m_sizeImage.cy };

	//invalidate it and tell it to update
    InvalidateRect( m_hWndDesktop, &rcErase, TRUE ); 
	UpdateWindow( m_hWndDesktop );
}

void CDesktopPet::DrawOnTarget( int x, int y, HICON hIcon )
{
    //grab the device context of the desktop
    HDC hDC = GetDC( m_hWndDesktop );

    //draw the icon on it
    DrawIconEx( hDC, x, y, hIcon, 0, 0, 0, nullptr, DI_NORMAL );

    //release the device context
    ReleaseDC( m_hWndDesktop, hDC );
}

