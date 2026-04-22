/* property sheet functions */
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "NekoCFG.h"
#include "../NekoCommon.h"
#include "NekoCFG_resource.h"
#include "NekoCFG_resource.hm"


/* help ID lookup table */
static DWORD dwHelpID[] = { 
    IDC_SCALESLIDER,    HIDC_SCALESLIDER,
    IDC_SCALEDISPLAY,   HIDC_SCALEDISPLAY,
    IDC_SET100,         HIDC_SET100,
    IDC_IMAGELIB,       HIDC_IMAGELIB,
    IDC_CHANGE,         HIDC_CHANGE,
    IDC_DEFAULT,        HIDC_DEFAULT,

    IDC_SPEEDSLIDER,    HIDC_SPEEDSLIDER,
    IDC_SENSESLIDER,    HIDC_SENSESLIDER,

    IDC_SOUNDFREQ,      HIDC_SOUNDFREQ,
    IDC_SOUNDSAVAIL,    HIDC_SOUNDSAVAIL,
    IDC_SOUNDNAME,      HIDC_SOUNDNAME,
    IDC_PREVIEW,        HIDC_PREVIEW,
    IDC_NONE,           HIDC_NONE,
    IDC_BROWSE,         HIDC_BROWSE,

    IDC_ACTION,         HIDC_ACTION,
    IDC_ACTIONDESC,     HIDC_ACTIONDESC,

	IDC_ALWAYSONTOP,	HIDC_ALWAYSONTOP,
    IDC_FOOTPRINTS,     HIDC_FOOTPRINTS,
    0,0
};


/**************************************************************************************/

//cat settings
LPCATSETTINGS g_lpCurrentCat = nullptr;
#define MY_WRITESETTINGS (WM_USER+32)
#define MY_READSETTINGS  (WM_USER+33)

// Post WM_MARKDIRTY to the parent (main config dialog) whenever a tab-page
// control's value is changed by the user. g_fInitialising gates out the
// storm of phantom WM_COMMAND / EN_CHANGE events emitted while we populate
// controls during WM_INITDIALOG / MY_READSETTINGS.
static inline void MarkDirty( HWND hPage )
{
    PostMessageW( GetParent(hPage), WM_MARKDIRTY, 0, 0 );
}

/**************************************************************************************/
//tab control data

#define NUM_PAGES 5
BOOL CALLBACK PropPage_Display( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Movement( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Sound( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Independence( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PropPage_Effects( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam );
TABINITDATA tibTabs[NUM_PAGES] = {
    { L"Display",      IDD_DISPLAY,      (DLGPROC)PropPage_Display,      IDI_TAB_DISPLAY,      nullptr },
    { L"Movement",     IDD_MOVEMENT,     (DLGPROC)PropPage_Movement,     IDI_TAB_MOVEMENT,     nullptr },
    { L"Sound",        IDD_SOUND,        (DLGPROC)PropPage_Sound,        IDI_TAB_SOUNDS,       nullptr },
    { L"Behavior",     IDD_INDEPENDENCE, (DLGPROC)PropPage_Independence, IDI_TAB_INDEPENDENCE, nullptr },
    { L"Effects",      IDD_EFFECTS,      (DLGPROC)PropPage_Effects,      IDI_TAB_EFFECTS,      nullptr },
};

HWND g_hDlgDisplay = nullptr;      //current child dialog
HWND g_hWndTab = nullptr;          //tab control
RECT g_rcTabStart;              //offset in dialog to draw tab pages

HIMAGELIST g_himgTabIcons = nullptr;

/**************************************************************************************/


/* InitialisePropertyDialog - loads all of the property sheet pages into the property sheet control */
void WINAPI InitialisePropertyDialog( HWND hWnd )
{
    int i;

    //create the image list
    g_himgTabIcons = ImageList_Create( 16, 16, ILC_COLOR4|ILC_MASK, NUM_PAGES, 0 );
    for( i = 0; i < NUM_PAGES; i++ )
    {
        HICON hIcon = (HICON)LoadImageW( g_hInstance, MAKEINTRESOURCEW(tibTabs[i].uIDIcon), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE );
        tibTabs[i].uIDIcon = ImageList_AddIcon( g_himgTabIcons, hIcon );
        DestroyIcon( hIcon );
    }
    TabCtrl_SetImageList( hWnd, g_himgTabIcons );


    //insert tabs for each of the dialog items and create the (hidden) dialog
    TCITEMW tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    for( i = 0; i < NUM_PAGES; i++ )
    {
        tie.iImage = tibTabs[i].uIDIcon;
        tie.pszText = (LPWSTR)tibTabs[i].szTitle;
        SendMessageW( hWnd, TCM_INSERTITEMW, (WPARAM)i, (LPARAM)&tie );

        tibTabs[i].hDlg = CreateDialogW( g_hInstance, MAKEINTRESOURCEW(tibTabs[i].uIDDialog), GetParent(hWnd), tibTabs[i].lpfnDlgProc );
    }

    //calculate where to stick each dialog
    GetWindowRect( hWnd, &g_rcTabStart );
    ScreenToClient( GetParent(hWnd), (LPPOINT)&g_rcTabStart.left );
    ScreenToClient( GetParent(hWnd), (LPPOINT)&g_rcTabStart.right );
    g_rcTabStart.top += 42;
    g_rcTabStart.left += 3;

    //store the handle and show the first page
    g_hWndTab = hWnd;
    OnSelChanged( hWnd );
}

/* ShutdownPropertyDialog - releases any resources owned by the propery sheet */
void WINAPI ShutdownPropertyDialog()
{
    ImageList_Destroy( g_himgTabIcons );
}

/* OnTabSelection - updates the tabs when one is selected */
void WINAPI OnSelChanged( HWND hWnd ) 
{ 
    int iSel = TabCtrl_GetCurSel( hWnd ); 
 
    //destroy the current child dialog box, if any. 
    if( g_hDlgDisplay != nullptr) ShowWindow( g_hDlgDisplay, SW_HIDE ); 
 
    //create the new child dialog box. 
    g_hDlgDisplay = tibTabs[iSel].hDlg;
    ShowWindow( g_hDlgDisplay, SW_SHOW );
    
}

/* SetActiveCat - selects which cat to configure */
void WINAPI SetActiveCat( LPCATSETTINGS lpCat )
{
    GetDialogSettings();
    g_lpCurrentCat = lpCat;
    // Programmatic updates from MY_READSETTINGS would otherwise be indistinguishable
    // from user edits — guard the broadcast so no phantom MarkDirty is posted.
    g_fInitialising = TRUE;
    for( int i = 0; i < NUM_PAGES; i++ ) SendMessageW( tibTabs[i].hDlg, MY_READSETTINGS, 0, 0 );
    g_fInitialising = FALSE;
}

/* GetDialogSettings - causes all property pages to write their settings into the data structure */
void WINAPI GetDialogSettings()
{
    if( g_lpCurrentCat ) 
        for( int i = 0; i < NUM_PAGES; i++ ) SendMessageW( tibTabs[i].hDlg, MY_WRITESETTINGS, 0, 0 );
}

/**************************************************************************************/

/* PropPage_Display - message processsing function for the 'display' property page */
BOOL CALLBACK PropPage_Display( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessageW( hPage, IDC_SCALESLIDER, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SCALE/10,MAX_SCALE/10) );
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uScale = SendDlgItemMessageW( hPage, IDC_SCALESLIDER, TBM_GETPOS, 0, 0 ) * 10;
            GetDlgItemTextW( hPage, IDC_IMAGELIB, g_lpCurrentCat->szLibrary, MAX_PATH );
			g_lpCurrentCat->fAlwaysOnTop = IsDlgButtonChecked( hPage, IDC_ALWAYSONTOP );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessageW( hPage, IDC_SCALESLIDER, TBM_SETPOS, TRUE, g_lpCurrentCat->uScale / 10 );
            SetDlgItemTextW( hPage, IDC_IMAGELIB, g_lpCurrentCat->szLibrary );
			CheckDlgButton( hPage, IDC_ALWAYSONTOP, g_lpCurrentCat->fAlwaysOnTop );
            SendMessageW( hPage, WM_HSCROLL, 0, 0 );
            break;

        case WM_HSCROLL:
        {
            //update the % indicator
            WCHAR szBuffer[32];
            wsprintfW( szBuffer, L"%d%%", (int)(SendDlgItemMessageW( hPage, IDC_SCALESLIDER, TBM_GETPOS, 0, 0 ) * 10) );
            SetDlgItemTextW( hPage, IDC_SCALEDISPLAY, szBuffer );
            MarkDirty( hPage );
            break;
        }

        case WM_COMMAND:

            switch( LOWORD(wParam) )
            {
                case IDC_SET100:
                    SendDlgItemMessageW( hPage, IDC_SCALESLIDER, TBM_SETPOS, TRUE, 10 );
                    SendMessageW( hPage, WM_HSCROLL, 0, 0 );
                    break;

                case IDC_DEFAULT:
                    SetDlgItemTextW( hPage, IDC_IMAGELIB, L"" );
                    MarkDirty( hPage );
                    break;

                case IDC_ALWAYSONTOP:
                    MarkDirty( hPage );
                    break;

                case IDC_CHANGE:
                {
                    OPENFILENAMEW ofn;
                    WCHAR lpstrLibrary[MAX_PATH];
                    LPCWSTR lpstrFileMasks = L"Icon Libraries\0*.icl;*.exe;*.dll\0All Files (*.*)\0*.*\0\0";

                    //get the current file
                    GetDlgItemTextW( hPage, IDC_IMAGELIB, lpstrLibrary, MAX_PATH-1 );

                    ofn.lStructSize = sizeof(OPENFILENAMEW);
                    ofn.hwndOwner = hPage;
                    ofn.hInstance = nullptr;
                    ofn.lpstrFilter = lpstrFileMasks;
                    ofn.lpstrCustomFilter = nullptr;
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = lpstrLibrary;
                    ofn.nMaxFile = MAX_PATH-1;
                    ofn.lpstrFileTitle = nullptr;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = nullptr;
                    ofn.lpstrTitle = L"Select Image Library";
                    ofn.Flags = OFN_EXPLORER|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
                    ofn.lpstrDefExt = L"icl";
                    if( GetOpenFileNameW( &ofn ) )
                    {
                        //check to make sure it's got enough icons in...
                        if( (UINT)(UINT_PTR)ExtractIconW( g_hInstance, ofn.lpstrFile, (UINT)-1 ) < 32 )
                            MessageBoxW( hPage, L"That file does not have enough icons in it - it must have at least 32", L"Change Image Library", MB_ICONEXCLAMATION );
                        else
                        {
                            SetDlgItemTextW( hPage, IDC_IMAGELIB, ofn.lpstrFile );
                            MarkDirty( hPage );
                        }
                    }
                    break;
                }
            }
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE );
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelpW( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (ULONG_PTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelpW( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (ULONG_PTR)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}


/* PropPage_Movement - message processsing function for the 'movement' property page */
BOOL CALLBACK PropPage_Movement( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessageW( hPage, IDC_SPEEDSLIDER, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SPEED,MAX_SPEED) );
            SendDlgItemMessageW( hPage, IDC_SENSESLIDER, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SENSE,MAX_SENSE) );
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uSpeed = SendDlgItemMessageW( hPage, IDC_SPEEDSLIDER, TBM_GETPOS, 0, 0 );
            g_lpCurrentCat->uMouseSensitivity = MAX_SENSE - SendDlgItemMessageW( hPage, IDC_SENSESLIDER, TBM_GETPOS, 0, 0 );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessageW( hPage, IDC_SPEEDSLIDER, TBM_SETPOS, TRUE, g_lpCurrentCat->uSpeed );
            SendDlgItemMessageW( hPage, IDC_SENSESLIDER, TBM_SETPOS, TRUE, MAX_SENSE - g_lpCurrentCat->uMouseSensitivity );
            break;

        case WM_HSCROLL:
            MarkDirty( hPage );
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE );
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelpW( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (ULONG_PTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelpW( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (ULONG_PTR)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

/* PropPage_Sound - message processsing function for the 'sound' property page */
BOOL CALLBACK PropPage_Sound( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    static HCURSOR s_hcPlaying = nullptr;

    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessageW( hPage, IDC_PREVIEW, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadImageW( g_hInstance, MAKEINTRESOURCEW(IDR_PLAYBITMAP), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS ) );
            SendDlgItemMessageW( hPage, IDC_SOUNDFREQ, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_SNDFREQ,MAX_SNDFREQ) );

            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)L"Idle One" );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)L"Idle Two" );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)L"Idle Three" );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)L"Sleep" );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_ADDSTRING, 0, (LPARAM)L"Awake" );

            //load the playing cursor
            s_hcPlaying = LoadCursorW( g_hInstance, MAKEINTRESOURCEW(IDC_PLAYING) );

            //select first item in the list & update
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_SETCURSEL, 0, 0 );
            break;

        case WM_DESTROY:
            DestroyCursor( s_hcPlaying );
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uSndFrequency = SendDlgItemMessageW( hPage, IDC_SOUNDFREQ, TBM_GETPOS, 0, 0 );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessageW( hPage, IDC_SOUNDFREQ, TBM_SETPOS, TRUE, g_lpCurrentCat->uSndFrequency );

            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 0, (LPARAM)g_lpCurrentCat->szSndIdle1 );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 1, (LPARAM)g_lpCurrentCat->szSndIdle2 );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 2, (LPARAM)g_lpCurrentCat->szSndIdle3 );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 3, (LPARAM)g_lpCurrentCat->szSndSleep );
            SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_SETITEMDATA, 4, (LPARAM)g_lpCurrentCat->szSndAwake );
            PostMessageW( hPage, WM_COMMAND, MAKEWPARAM( IDC_SOUNDSAVAIL, 0 ), 0 );
            break;

        case WM_HSCROLL:
            MarkDirty( hPage );
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDC_SOUNDSAVAIL:
                {
                    int iCurSel = SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );
                    wchar_t * lpszFile = (wchar_t*)SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 );

                    if( lpszFile[0] == L'\0' )
                    {
                        SetDlgItemTextW( hPage, IDC_SOUNDNAME, L"" );
                        EnableWindow( GetDlgItem( hPage, IDC_PREVIEW ), FALSE );
                        EnableWindow( GetDlgItem( hPage, IDC_NONE ), FALSE );
                    }
                    else
                    {
                        SetDlgItemTextW( hPage, IDC_SOUNDNAME, lpszFile );
                        EnableWindow( GetDlgItem( hPage, IDC_PREVIEW ), TRUE );
                        EnableWindow( GetDlgItem( hPage, IDC_NONE ), TRUE );
                    }

                    break;
                }

                case IDC_PREVIEW:
                {
                    int iCurSel = SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );
                    wchar_t * lpszFile = (wchar_t*)SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 );

                    if( lpszFile[0] != L'\0' )
                    {
                        SetCursor( s_hcPlaying );
                        if( !PlaySoundW( lpszFile, nullptr, SND_FILENAME|SND_NODEFAULT|SND_SYNC ) ) MessageBoxW( hPage, L"Unable to start playing wave file!", L"Preview Sound", MB_ICONEXCLAMATION );
                        SetCursor( LoadCursorW( nullptr, IDC_ARROW ) );
                    }
                    break;
                }

                case IDC_NONE:
                {
                    int iCurSel = SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );

                    ((wchar_t*)SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 ))[0] = L'\0';

                    //update the display
                    PostMessageW( hPage, WM_COMMAND, MAKEWPARAM( IDC_SOUNDSAVAIL, 0 ), 0 );
                    MarkDirty( hPage );
                    break;
                }

                case IDC_BROWSE:
                {
                    OPENFILENAMEW ofn;
                    WCHAR szPrompt[256] = L"Browse for ", szSoundName[256];
                    LPCWSTR lpstrFileMasks = L"Sounds (*.wav)\0*.wav\0All Files (*.*)\0*.*\0\0";
                    int iCurSel = SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETCURSEL, 0, 0 );

                    //build browse dialog prompt
                    SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETTEXT, iCurSel, (LPARAM)szSoundName );
                    lstrcatW( szPrompt, szSoundName );
                    lstrcatW( szPrompt, L" sound" );

                    lstrcpyW( szSoundName, (wchar_t*)SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 ) );

                    //get the current file
                    ofn.lStructSize = sizeof(OPENFILENAMEW);
                    ofn.hwndOwner = hPage;
                    ofn.hInstance = nullptr;
                    ofn.lpstrFilter = lpstrFileMasks;
                    ofn.lpstrCustomFilter = nullptr;
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = szSoundName;
                    ofn.nMaxFile = MAX_PATH-1;
                    ofn.lpstrFileTitle = nullptr;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = nullptr;
                    ofn.lpstrTitle = szPrompt;
                    ofn.Flags = OFN_EXPLORER|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
                    ofn.lpstrDefExt = L"WAV";
                    if( GetOpenFileNameW( &ofn ) )
                    {
                        //update everything with the new sound
                        SetDlgItemTextW( hPage, IDC_SOUNDNAME, ofn.lpstrFile );
                        lstrcpyW( (wchar_t*)SendDlgItemMessageW( hPage, IDC_SOUNDSAVAIL, LB_GETITEMDATA, iCurSel, 0 ), szSoundName );

                        //update the display
                        PostMessageW( hPage, WM_COMMAND, MAKEWPARAM( IDC_SOUNDSAVAIL, 0 ), 0 );

						//make sure it will make a sound
						if( SendDlgItemMessageW( hPage, IDC_SOUNDFREQ, TBM_GETPOS, 0, 0 ) == 0 )
							SendDlgItemMessageW( hPage, IDC_SOUNDFREQ, TBM_SETPOS, TRUE, 2 );

                        MarkDirty( hPage );
                    }
                    break;
                }
            }
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE );
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelpW( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (ULONG_PTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelpW( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (ULONG_PTR)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}


/* PropPage_Independence - message processsing function for the 'independence' property page */
BOOL CALLBACK PropPage_Independence( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            SendDlgItemMessageW( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)L"Chase the mouse (default)" );
            SendDlgItemMessageW( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)L"Run away from the mouse");
            SendDlgItemMessageW( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)L"Run around randomly");
            SendDlgItemMessageW( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)L"Pace around the screen");
            SendDlgItemMessageW( hPage, IDC_ACTION, CB_ADDSTRING, 0, (LPARAM)L"Run around the screen");
            break;

        case MY_WRITESETTINGS:
            g_lpCurrentCat->uAction = SendDlgItemMessageW( hPage, IDC_ACTION, CB_GETCURSEL, 0, 0 );
            break;

        case MY_READSETTINGS:
            SendDlgItemMessageW( hPage, IDC_ACTION, CB_SETCURSEL, g_lpCurrentCat->uAction, 0 );
            SendMessageW( hPage, WM_COMMAND, MAKEWPARAM(IDC_ACTION, CBN_SELCHANGE), 0 );
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE );
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDC_ACTION:
                    if( HIWORD(wParam) == CBN_SELCHANGE )
                    {
                        LPCWSTR szActionDesc[] = {
                            L"Neko will follow the mouse cursor wherever it goes",
                            L"Neko will run away when the mouse cursor gets too near, and sleep when it isn't near",
                            L"Neko will run to a random spot on the desktop, sleep for a while, and then continue running around",
                            L"Neko will run around the outside of the desktop",
                            L"Neko will run around the desktop, without stopping"
                        };

                        SetDlgItemTextW( hPage, IDC_ACTIONDESC, szActionDesc[SendDlgItemMessageW( hPage, IDC_ACTION, CB_GETCURSEL, 0, 0 )] );
                        MarkDirty( hPage );
                    }
                    break;
            }
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelpW( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (ULONG_PTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelpW( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (ULONG_PTR)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}




/* PropPage_Effects - message processsing function for the 'effects' property page */
BOOL CALLBACK PropPage_Effects( HWND hPage, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case MY_WRITESETTINGS:
            GetDlgItemTextW( hPage, IDC_IMAGELIB, g_lpCurrentCat->szFootprintLib, MAX_PATH );
			g_lpCurrentCat->bFootprints = IsDlgButtonChecked( hPage, IDC_FOOTPRINTS );
            break;

        case MY_READSETTINGS:
            SetDlgItemTextW( hPage, IDC_IMAGELIB, g_lpCurrentCat->szFootprintLib );
			CheckDlgButton( hPage, IDC_FOOTPRINTS, g_lpCurrentCat->bFootprints );
            break;

        case WM_COMMAND:

            switch( LOWORD(wParam) )
            {
                case IDC_FOOTPRINTS:
                    MarkDirty( hPage );
                    break;

                case IDC_DEFAULT:
                    SetDlgItemTextW( hPage, IDC_IMAGELIB, L"" );
                    MarkDirty( hPage );
                    break;

                case IDC_CHANGE:
                {
                    OPENFILENAMEW ofn;
                    WCHAR lpstrLibrary[MAX_PATH];
                    LPCWSTR lpstrFileMasks = L"Icon Libraries\0*.icl;*.exe;*.dll\0All Files (*.*)\0*.*\0\0";

                    //get the current file
                    GetDlgItemTextW( hPage, IDC_IMAGELIB, lpstrLibrary, MAX_PATH-1 );

                    ofn.lStructSize = sizeof(OPENFILENAMEW);
                    ofn.hwndOwner = hPage;
                    ofn.hInstance = nullptr;
                    ofn.lpstrFilter = lpstrFileMasks;
                    ofn.lpstrCustomFilter = nullptr;
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = lpstrLibrary;
                    ofn.nMaxFile = MAX_PATH-1;
                    ofn.lpstrFileTitle = nullptr;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = nullptr;
                    ofn.lpstrTitle = L"Select Footprint Image Library";
                    ofn.Flags = OFN_EXPLORER|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
                    ofn.lpstrDefExt = L"icl";
                    if( GetOpenFileNameW( &ofn ) )
                    {
                        //check to make sure it's got enough icons in...
                        if( (UINT)(UINT_PTR)ExtractIconW( g_hInstance, ofn.lpstrFile, (UINT)-1 ) < 8 )
                            MessageBoxW( hPage, L"That file does not have enough icons in it - it must have at least 8", L"Change Image Library", MB_ICONEXCLAMATION );
                        else
                        {
                            SetDlgItemTextW( hPage, IDC_IMAGELIB, ofn.lpstrFile );
			                CheckDlgButton( hPage, IDC_FOOTPRINTS, TRUE );
                            MarkDirty( hPage );
                        }
                    }
                    break;
                }
            }
            break;

        case WM_SHOWWINDOW:
            if( wParam ) SetWindowPos( hPage, HWND_TOP, g_rcTabStart.left, g_rcTabStart.top, 0, 0, SWP_NOSIZE );
            break;

        /* help stuff */
        case WM_HELP:
            if( ((LPHELPINFO)lParam)->iCtrlId != (-1) )
                WinHelpW( (HWND)((LPHELPINFO)lParam)->hItemHandle, szHelpFile, HELP_WM_HELP, (ULONG_PTR)dwHelpID );
            else
                return FALSE;
            break;

        case WM_CONTEXTMENU:
            WinHelpW( (HWND)wParam, szHelpFile, HELP_CONTEXTMENU, (ULONG_PTR)dwHelpID );
            break;

        default:
            return FALSE;
    }
    return TRUE;
}
