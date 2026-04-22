/************************************
 
   Tray Class: Very simple interface
   into the system tray

 *************************************/

#ifndef _NEKO_TRAY_H_
#define _NEKO_TRAY_H_

#include "framework.h"

//taskbar notification message
#define MY_NOTIFYICON (WM_USER+32)

class CTray {

public:
    CTray( HINSTANCE hInstance ) { m_uIconCount = 0; m_hInstance = hInstance; };

    BOOL AddIcon( HWND hWnd, HICON hIcon, UINT uID );
    BOOL RemoveIcon( HWND hWnd, UINT uID );

    void ShowPopupMenu( HWND hWnd );

    UINT GetCount() { return m_uIconCount; };

private:
    UINT m_uIconCount;
    HINSTANCE m_hInstance;
};

#endif // _NEKO_TRAY_H_
