// DesktopHack.h: interface for the CDesktopHack class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NEKO_DESKTOP_HACK_H_
#define _NEKO_DESKTOP_HACK_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "framework.h"

class CDesktopHack  
{
public:
	void GetDesktopRect( RECT& rcDesktop );
	void FindDesktopHandle();
	CDesktopHack();
	virtual ~CDesktopHack();

protected:
	HWND m_hWndDesktop;

	BOOL m_fActiveDesktop;    //the desktop is an active (web) desktop
	BOOL m_fNoChicagoDesktop; //the desktop is not a windows 95/nt4 desktop

};

#endif // _NEKO_DESKTOP_HACK_H_
