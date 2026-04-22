// DesktopPet.h: interface for the CDesktopPet class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NEKO_DESKTOP_PET_H_
#define _NEKO_DESKTOP_PET_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Pet.h"
#include "DesktopHack.h"

class CDesktopPet : public CDesktopHack, virtual public CPet  
{
public:
	CDesktopPet();
	virtual ~CDesktopPet();

    virtual void DrawOnTarget( int x, int y, HICON hIcon );

protected:
	int m_nUpdateHWndCounter;
	virtual void Erase();
	virtual void Draw( int nImage );
};

#endif // _NEKO_DESKTOP_PET_H_
