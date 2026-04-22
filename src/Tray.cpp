/************************************

   Tray Class: Very simple interface
   onto the system tray for Neko-ng

 *************************************/

#include <windows.h>
#include "Tray.h"
#include "resource.h"

BOOL CTray::AddIcon( HWND hWnd, HICON hIcon, UINT uID )
{
    BOOL res;
    NOTIFYICONDATAW tnid = {};

    //fill in the structure
    tnid.cbSize = sizeof(NOTIFYICONDATAW);
    tnid.hWnd = hWnd;
    tnid.uID = uID;
    tnid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
    tnid.uCallbackMessage = MY_NOTIFYICON;
    tnid.hIcon = hIcon;
    lstrcpynW( tnid.szTip, L"Neko", sizeof(tnid.szTip)/sizeof(tnid.szTip[0]) );

    //give the command
    res = Shell_NotifyIconW( NIM_ADD, &tnid );

    //increment the counter
    if( res ) m_uIconCount++;

    return res; 
}

BOOL CTray::RemoveIcon( HWND hWnd, UINT uID )
{
    BOOL res;
    NOTIFYICONDATAW tnid = {};

    //fill in the structure
    tnid.cbSize = sizeof(NOTIFYICONDATAW);
    tnid.hWnd = hWnd;
    tnid.uID = uID;

    //give the notify command
    res = Shell_NotifyIconW( NIM_DELETE, &tnid );

    //decrement the counter
    if( res ) m_uIconCount--;

    return res;
}

void CTray::ShowPopupMenu( HWND hWnd )
{
    //get the mouse position
    POINT mouse;
    GetCursorPos( &mouse );

    //create the popup menu
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW( hMenu, MF_STRING, ID_SETTINGS, L"&Settings" );
    AppendMenuW( hMenu, MF_STRING, ID_EXIT,     L"E&xit" );
    SetMenuDefaultItem( hMenu, ID_SETTINGS, FALSE );

    //display the menu
    TrackPopupMenu( hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, mouse.x, mouse.y, 0, hWnd, NULL );
 
    //destroy it
    DestroyMenu(hMenu); 
}

