/****************
  NekoCFG

  Configuration
  program for
  Neko-ng

*****************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <wctype.h>
#include "NekoCFG.h"
#include "../NekoCommon.h"
#include "../NekoSettings.h"
#include "NekoCFG_resource.h"
#include "NekoCFG_resource.hm"

/*************************************************************************************************/

LPCWSTR szWindowTitle = L"Configure Neko-ng";
LPCWSTR szDefaultName = L"Neko-ng (default)";
LPCWSTR szHelpFile = L"Neko98.hlp";

/* help ID lookup table */
static DWORD dwHelpID[] = { 
    IDC_NAME,           HIDC_NAME, 
    IDC_NEW,            HIDC_NEW, 
    IDC_DELETE,         HIDC_DELETE,
    IDC_TASKBAR,        HIDC_TASKBAR, 
    IDC_ABOUT,          HIDC_ABOUT,
    IDOK,               HIDOK,
    IDCANCEL,           HIDCANCEL,
    IDC_APPLY,          HIDC_APPLY,
    IDC_HELP32,         HIDC_HELP,
    IDC_TABS,           HIDC_TABS,
    0,0
};

//global variables
HINSTANCE g_hInstance = NULL;
HWND g_hWndNeko = NULL;


//global settings
BOOL g_fShowTaskbar = TRUE;

//list of all cats
LPCATSETTINGS catSettings = NULL;

//function forward declaration
void WINAPI WriteSettings();

/*************************************************************************************************/

/* DeleteCatSettings - removes the given cat from the list */
BOOL WINAPI DeleteCatSettings( LPCATSETTINGS cat )
{
    cat->fDeleted = TRUE;
    return TRUE;
}

/* DlgProc_NewNeko - dialog box procedure to add a neko */
BOOL CALLBACK DlgProc_NewNeko( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            //make sure the user isn't adding *too* many Nekos!
            LPCATSETTINGS cat = catSettings;
            int nCats = 0;
            while( cat )
            {
                if( cat->fDeleted == FALSE ) nCats++;
                cat = cat->next;
            }

            if( nCats > 5 )
            {
                WCHAR szBuffer[128];
                wsprintfW( szBuffer, L"You already have %d Nekos!\nAdding more is likely to slow down your computer - do you want to add one anyway?", nCats );
                if( IDNO == MessageBoxW( hDlg, szBuffer, L"Add Neko", MB_YESNO|MB_ICONQUESTION ) )
                {
                    EndDialog( hDlg, FALSE );
                    return TRUE;
                }
            }

            //initialise the text box
            SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_LIMITTEXT, IDC_NEWNEKONAME-2, 0 );

			//add some demo items
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Arthur" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Boris" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Kitty" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Lucy" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Garfield" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Tom" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Bast" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Ginger" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Bob" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Tabs" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Tigger" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"James" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Cooper" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Hey You" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Greebo" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Furball" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Sylvester" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Buffy" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Willow" );
			SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Sarah" );
			//SendDlgItemMessageW( hDlg, IDC_NEWNEKONAME, CB_ADDSTRING, 0, (LPARAM)L"Vicious Bastard" );
            break;
        }

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK:
                {
                    WCHAR szName[MAX_NEKO_NAME];
                    GetDlgItemTextW( hDlg, IDC_NEWNEKONAME, szName, MAX_NEKO_NAME );
                    wchar_t* lpszPtr = szName;

                    //make sure it's not empty
                    if( lstrlenW( szName ) == 0 )
                    {
                        MessageBoxW( hDlg, L"Invalid Name: You must type a name", L"New Neko", MB_ICONINFORMATION );
                        return TRUE;
                    }

                    //make sure it only has letters, numbers and spaces in
                    while( lpszPtr && *lpszPtr != L'\0' )
                    {
                        if( !( iswalpha(*lpszPtr) || iswdigit(*lpszPtr) || *lpszPtr == L' ' ) )
                        {
                            MessageBoxW( hDlg, L"Invalid Name: Sorry - a name can only contain letters, numbers and spaces", L"Add Neko", MB_ICONINFORMATION );
                            return TRUE;
                        }
                        lpszPtr++;
                    }

                    //make sure it's unique
                    LPCATSETTINGS cat = catSettings;
                    while( cat )
                    {
                        if( !cat->fDeleted && (lstrcmpiW( cat->szName, szName ) == 0 || lstrcmpiW( cat->szName, szDefaultName ) == 0 ))
                        {
                            MessageBoxW( hDlg, L"Invalid Name: This neko already exists!", L"New Neko", MB_ICONINFORMATION );
                            return TRUE;
                        }
                        cat = cat->next;
                    }


                    //create a new neko
                    cat = new CATSETTINGS;
                    lstrcpyW( cat->szName, szName );

                    //set defaults
                    cat->szLibrary[0] = L'\0';
                    cat->uScale = 100;
                    cat->uSpeed = 16;
                    cat->uMouseSensitivity = 6;
                    cat->szSndIdle1[0] = L'\0';
                    cat->szSndIdle2[0] = L'\0';
                    cat->szSndIdle3[0] = L'\0';
	                cat->szSndSleep[0] = L'\0';
                    cat->szSndAwake[0] = L'\0';
	                cat->uSndFrequency = 0;
                    cat->uAction = CHASE_MOUSE;
					cat->fAlwaysOnTop = FALSE;
                    cat->bFootprints = FALSE;
                    *cat->szFootprintLib = L'\0';

                    //link it in
                    cat->fDeleted = FALSE;
                    cat->next = catSettings;
                    catSettings = cat;

                    //add it to the list box & select it
                    int i = SendDlgItemMessageW( GetParent(hDlg), IDC_NAME, LB_ADDSTRING, 0, (LPARAM)szName );
                    SendDlgItemMessageW( GetParent(hDlg), IDC_NAME, LB_SETCURSEL, i, 0 );
                    PostMessageW( GetParent(hDlg), WM_COMMAND, MAKEWPARAM(IDC_NAME, CBN_SELCHANGE), 0 );

                    //close the dialog
                    EndDialog( hDlg, TRUE );
                    break;
                }

                case IDCANCEL:
                    EndDialog( hDlg, FALSE );
                    break;
            }
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

/* DlgProc_About - dialog box procedure for about dialog */
BOOL CALLBACK DlgProc_About( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( ( uMsg == WM_CLOSE ) || ( uMsg == WM_COMMAND && LOWORD(wParam)==IDOK ) ) EndDialog( hDlg, TRUE ); else return FALSE;
    return TRUE;
}

/* DlgProc_Config - dialog box procedure for configuration dialog */
BOOL CALLBACK DlgProc_Config( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            SendDlgItemMessageW( hDlg, IDC_TASKBAR, BM_SETCHECK, g_fShowTaskbar, 0 );

            LPCATSETTINGS cat = catSettings;
            while( cat )
            {
                if( !cat->fDeleted ) SendDlgItemMessageW( hDlg, IDC_NAME, LB_ADDSTRING, 0, ( cat->szName[0] == L'\0' ) ? (LPARAM)szDefaultName : (LPARAM)cat->szName );
                cat = cat->next;
            }
            SendDlgItemMessageW( hDlg, IDC_NAME, LB_SELECTSTRING, 0, (LPARAM)szDefaultName );
            EnableWindow( GetDlgItem( hDlg, IDC_DELETE ), FALSE );
            EnableWindow( GetDlgItem( hDlg, IDC_APPLY ), (g_hWndNeko != NULL ) ? TRUE : FALSE );

            InitialisePropertyDialog( GetDlgItem( hDlg, IDC_TABS ) );
            SendMessageW( hDlg, WM_COMMAND, MAKEWPARAM(IDC_NAME, LBN_SELCHANGE), 0 );
            return TRUE;
        }

        case WM_DESTROY:
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK:
                    SetCursor( LoadCursorW( NULL, IDC_WAIT ) );
                    g_fShowTaskbar = ( IsDlgButtonChecked( hDlg, IDC_TASKBAR ) == BST_CHECKED );
                    WriteSettings();
                    if( IsWindow( g_hWndNeko ) == FALSE ) g_hWndNeko = FindWindowW( szNekoClassName, szNekoWindowTitle );
                    SendMessageW( g_hWndNeko, MY_UPDATENEKO, 0, 0 );
                    SetCursor( LoadCursorW( NULL, IDC_ARROW ) );
                    EndDialog( hDlg, TRUE );
                    break;

                case IDCANCEL:
                    EndDialog( hDlg, FALSE ); break;

                case IDC_APPLY:
                    SetCursor( LoadCursorW( NULL, IDC_WAIT ) );
                    g_fShowTaskbar = ( IsDlgButtonChecked( hDlg, IDC_TASKBAR ) == BST_CHECKED );
                    WriteSettings();
                    if( IsWindow( g_hWndNeko ) == FALSE ) g_hWndNeko = FindWindowW( szNekoClassName, szNekoWindowTitle );
                    SendMessageW( g_hWndNeko, MY_UPDATENEKO, 0, 0 );
                    SetCursor( LoadCursorW( NULL, IDC_ARROW ) );
                    break;

                case IDC_ABOUT:
                    DialogBoxW( g_hInstance, MAKEINTRESOURCEW(IDD_ABOUTBOX), hDlg, (DLGPROC)DlgProc_About );
                    break;

                case IDC_NEW:
                    DialogBoxW( g_hInstance, MAKEINTRESOURCEW(IDD_NEWNEKO), hDlg, (DLGPROC)DlgProc_NewNeko );
                    break;

                case IDC_DELETE:
                {
                    int iItem = SendDlgItemMessageW( hDlg, IDC_NAME, LB_GETCURSEL, 0, 0 );
                    WCHAR szDoomed[MAX_NEKO_NAME] = L"";

                    //get string
                    SendDlgItemMessageW( hDlg, IDC_NAME, LB_GETTEXT, iItem, (LPARAM)szDoomed );
                    if( lstrcmpW( szDoomed, szDefaultName ) == 0 ) szDoomed[0] = L'\0';

                    //find it in the settings list and continue
                    LPCATSETTINGS cat = catSettings;
                    while( cat )
                    {
                        if( !cat->fDeleted )
                            if( lstrcmpW( cat->szName, szDoomed ) == 0 ) break;

                        cat = cat->next;
                    }

                    if( cat )
                    {
                        //remove it from the list box
                        SendDlgItemMessageW( hDlg, IDC_NAME, LB_DELETESTRING, iItem, 0 );

                        //select another item
                        SendDlgItemMessageW( hDlg, IDC_NAME, LB_SETCURSEL, (iItem ? iItem-1 : 0), 0 );
                        SendMessageW( hDlg, WM_COMMAND, MAKEWPARAM(IDC_NAME,LBN_SELCHANGE), 0 );

                        //remove it from memory
                        if( DeleteCatSettings( cat ) == FALSE )
                            MessageBoxW( hDlg, L"Internal Error: Could not delete cat!", szWindowTitle, MB_ICONERROR );
                    }
                    else
                        MessageBoxW( hDlg, L"Internal Error: Dropped off the end of the cat list!", szWindowTitle, MB_ICONERROR );

                    break;
                }

                case IDC_HELP32:
                    WinHelpW( hDlg, szHelpFile, HELP_FINDER, 0 );
                    break;

                case IDC_NAME:
                    if( HIWORD(wParam) == LBN_SELCHANGE )
                    {
                        int iNew = SendDlgItemMessageW( hDlg, IDC_NAME, LB_GETCURSEL, 0, 0 );
                        WCHAR szNew[MAX_NEKO_NAME] = L"";

                        //get string and enable delete if it's not the default item (first)
                        SendDlgItemMessageW( hDlg, IDC_NAME, LB_GETTEXT, iNew, (LPARAM)szNew );
                        if( lstrcmpW( szNew, szDefaultName ) == 0 )
                        {
                            szNew[0] = L'\0';
                            EnableWindow( GetDlgItem( hDlg, IDC_DELETE ), FALSE );
                        }
                        else EnableWindow( GetDlgItem( hDlg, IDC_DELETE ), TRUE );

                        //the user has selected a different name - find it in the settings list and continue
                        LPCATSETTINGS cat = catSettings;
                        while( cat )
                        {
                            if( !cat->fDeleted )
                                if( lstrcmpW( cat->szName, szNew ) == 0 ) break;

                            cat = cat->next;
                        }

                        if( cat )
                            SetActiveCat( cat );
                        else
                            MessageBoxW( hDlg, L"Internal Error: Dropped off the end of the cat list!", szWindowTitle, MB_ICONERROR );
                    }
                    break;
            }
            break;

        case WM_NOTIFY:
        {
            LPNMHDR nmhdr = (LPNMHDR)lParam;
            switch( nmhdr->idFrom )
            {
                case IDC_TABS:
                    if( nmhdr->code == TCN_SELCHANGE ) OnSelChanged( GetDlgItem( hDlg, IDC_TABS ) );
                    break;
            }
            break;
        }

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

/*************************************************************************************************/

/* WriteCatSetting - write a cat from the registry with the given cat settings block */
void WINAPI WriteCatSettings( LPCATSETTINGS cat, LPCWSTR szName )
{
    //see if this cat is flagged as deleted
    if( cat->fDeleted == TRUE )
    {
        //remove this item from the registry
        if( lstrlenW( szName ) > 0 )
        {
            HKEY hKey;
            WCHAR szKeyName[MAX_PATH];
            lstrcpyW( szKeyName, szNekoRegKey );
            if( RegOpenKeyExW( HKEY_CURRENT_USER, szKeyName, 0, KEY_WRITE, &hKey ) == ERROR_SUCCESS )
            {
                LONG l = RegDeleteKeyW( hKey, cat->szName );
                if( l != ERROR_SUCCESS )
                {
/*
                    LPVOID lpMsgBuf;

                    FormatMessageW(
                        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        l,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPWSTR) &lpMsgBuf,
                        0,
                        NULL
                    );
                    // Display the string.
                    MessageBoxW( NULL, (LPCWSTR)lpMsgBuf, L"Can't Delete Key", MB_OK|MB_ICONINFORMATION );

                    // Free the buffer.
                    LocalFree( lpMsgBuf );
*/

                }
                RegCloseKey( hKey );
            }
            else
                MessageBoxW( NULL, L"Internal Error: Couldn't open registry key!", 0, MB_TASKMODAL );
        }
        else
            MessageBoxW( NULL, L"Internal Error: Tried to delete default Neko!", 0, MB_TASKMODAL );
    }
    else
    {
        //build & open key
	    WCHAR szKey[1024];
	    lstrcpyW( szKey, szNekoRegKey );
	    if( lstrlenW( szName ) > 0 )
	    {
		    lstrcatW( szKey, L"\\" );
		    lstrcatW( szKey, szName );
	    }

        //write cat to the registry
        CNekoSettings NekoSettings( szKey );

        //display
        NekoSettings.SetString( szNekoLibraryKey, cat->szLibrary );
        NekoSettings.SetInt( szNekoScaleKey, cat->uScale );
		NekoSettings.SetInt( szNekoOnTopKey, cat->fAlwaysOnTop );

        //movement
        NekoSettings.SetInt( szNekoSpeedKey, cat->uSpeed );
        NekoSettings.SetInt( szNekoSenseKey, cat->uMouseSensitivity );

        //sounds
        NekoSettings.SetString( szNekoSndIdle1Key, cat->szSndIdle1 );
        NekoSettings.SetString( szNekoSndIdle2Key, cat->szSndIdle2 );
        NekoSettings.SetString( szNekoSndIdle3Key, cat->szSndIdle3 );
	    NekoSettings.SetString( szNekoSndSleepKey, cat->szSndSleep );
        NekoSettings.SetString( szNekoSndAwakeKey, cat->szSndAwake );
	    NekoSettings.SetInt( szNekoSndFreqKey, cat->uSndFrequency );

        //independence
        NekoSettings.SetInt( szNekoActionKey, cat->uAction );

        //effects
        NekoSettings.SetBool( szNekoFootprintKey, cat->bFootprints );
        NekoSettings.SetString( szNekoFootprintLibKey, cat->szFootprintLib );
    }
}


/* WriteSettings - writes all of the settings to the registry */
void WINAPI WriteSettings()
{
    //tell all dialogs to write what they've got so far into the data structure
    GetDialogSettings();

    //write global settings
    CNekoSettings NekoSettings( szNekoRegKey );
    NekoSettings.SetBool( szNekoTaskbarKey, g_fShowTaskbar );
    
    int nNumCats = 0;
    LPCATSETTINGS cat = catSettings;
    while( cat )
    {
        //write this cat's settings
        WriteCatSettings( cat, cat->szName );

        //unlink it if it's been deleted, or write out the name index key if not
        if( cat->fDeleted == FALSE )
        {
            //build the key index
            WCHAR szSubKey[MAX_PATH];
            wsprintfW( szSubKey, L"%d", nNumCats );

            //write this cat's name to the registry
		    NekoSettings.SetString( szSubKey, cat->szName );

            //advance the list
            nNumCats++;
            cat = cat->next;
        }
        else
        {
            cat = cat->next;
        }
    }

    NekoSettings.SetInt( szNekoNumCatsKey, nNumCats );
}

/* ReadCatSetting - loads a cat from the registry into the given cat settings block */
void WINAPI ReadCatSettings( LPCATSETTINGS cat, LPCWSTR szName )
{
    lstrcpyW( cat->szName, szName );

    //set defaults
    cat->szLibrary[0] = L'\0';
    cat->uScale = 100;
    cat->uSpeed = 16;
    cat->uMouseSensitivity = 6;
    cat->szSndIdle1[0] = L'\0';
    cat->szSndIdle2[0] = L'\0';
    cat->szSndIdle3[0] = L'\0';
	cat->szSndSleep[0] = L'\0';
    cat->szSndAwake[0] = L'\0';
	cat->uSndFrequency = 0;
    cat->uAction = CHASE_MOUSE;
    cat->fDeleted = FALSE;
    cat->fAlwaysOnTop = FALSE;
    cat->bFootprints = FALSE;
    *cat->szFootprintLib = L'\0';

    //build & open key
	WCHAR szKey[1024];
	lstrcpyW( szKey, szNekoRegKey );
	if( lstrlenW( szName ) > 0 )
	{
		lstrcatW( szKey, L"\\" );
		lstrcatW( szKey, szName );
	}

    CNekoSettings NekoSettings( szKey );

    //display
    NekoSettings.GetString( szNekoLibraryKey, cat->szLibrary, (MAX_PATH-1) * sizeof(WCHAR) );
    NekoSettings.GetInt( szNekoScaleKey, &cat->uScale );
    DWORD fAlwaysOnTop = cat->fAlwaysOnTop;
	NekoSettings.GetInt( szNekoOnTopKey, &fAlwaysOnTop );
    cat->fAlwaysOnTop = fAlwaysOnTop;

    //movement
    NekoSettings.GetInt( szNekoSpeedKey, &cat->uSpeed );
    NekoSettings.GetInt( szNekoSenseKey, &cat->uMouseSensitivity );

    //sounds
    NekoSettings.GetString( szNekoSndIdle1Key, cat->szSndIdle1, (MAX_PATH-1) * sizeof(WCHAR) );
    NekoSettings.GetString( szNekoSndIdle2Key, cat->szSndIdle2, (MAX_PATH-1) * sizeof(WCHAR) );
    NekoSettings.GetString( szNekoSndIdle3Key, cat->szSndIdle3, (MAX_PATH-1) * sizeof(WCHAR) );
	NekoSettings.GetString( szNekoSndSleepKey, cat->szSndSleep, (MAX_PATH-1) * sizeof(WCHAR) );
    NekoSettings.GetString( szNekoSndAwakeKey, cat->szSndAwake, (MAX_PATH-1) * sizeof(WCHAR) );
	NekoSettings.GetInt( szNekoSndFreqKey, &cat->uSndFrequency );

    //independence
    NekoSettings.GetInt( szNekoActionKey, &cat->uAction );

    NekoSettings.GetBool( szNekoFootprintKey, &cat->bFootprints );
    NekoSettings.GetString( szNekoFootprintLibKey, cat->szFootprintLib, (MAX_PATH-1) * sizeof(WCHAR) );
}

/* ReadSettings - reads all of the settings from the registry and creates the global settings array */
void WINAPI ReadSettings()
{
    LPCATSETTINGS cat;

    CNekoSettings * NekoSettings = new CNekoSettings( szNekoRegKey );
    NekoSettings->GetBool( szNekoTaskbarKey, &g_fShowTaskbar );

    //load in new cats list
    DWORD nNumCats = 0;
    NekoSettings->GetInt( szNekoNumCatsKey, &nNumCats );

    if( nNumCats == 0 )
    {
        //the user hasn't run the config program, or there are no Nekos - use default
        cat = new CATSETTINGS;
        cat->next = NULL;
        ReadCatSettings( cat, L"" );

        catSettings = cat;
    }
    else
    {
        for( DWORD i = 0; i < nNumCats; i++ )
        {
            //build the key index
            WCHAR szSubKey[MAX_PATH], szName[MAX_NEKO_NAME];
            wsprintfW( szSubKey, L"%d", i );

            //load this cat's name from the registry
		    NekoSettings->GetString( szSubKey, szName, (MAX_NEKO_NAME-1) * sizeof(WCHAR) );

            //create a new setting for it and put the object into the list
            cat = new CATSETTINGS;
            cat->next = catSettings;
            ReadCatSettings( cat, szName );
            catSettings = cat;
        }
    }

    delete NekoSettings;
}

/* DeleteConfigList - deletes the list of settings from memory */
void WINAPI DeleteConfigList()
{
    LPCATSETTINGS cat = catSettings;
    while( catSettings )
    {
        cat = catSettings;
        catSettings = catSettings->next;
        delete cat;
    }
}


/* FindAndActivateOldInstance - activates the old instance of the configuration program
   if it has been loaded twice */
BOOL FindAndActivateOldInstance()
{
    HWND hWnd = FindWindowW( NULL, szWindowTitle );
    if( hWnd )
    {
        OpenIcon(hWnd);
        SetForegroundWindow(hWnd);
        return TRUE;
    }

    return FALSE;
}

/* WinMain - main program start point */
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine,
                      int iCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
    //see if the configuration program is loaded already
    if( FindAndActivateOldInstance() ) return 0;

    //ensure the common control library is loaded
    InitCommonControls();

    //store the instance handle and find the Neko window
    g_hInstance = hInstance;
    g_hWndNeko = FindWindowW( szNekoClassName, szNekoWindowTitle );

    //initialise program and display dialog
    ReadSettings();
    DialogBoxW( g_hInstance, MAKEINTRESOURCEW(IDD_CONFIG), NULL, (DLGPROC)DlgProc_Config );
    DeleteConfigList();
    ShutdownPropertyDialog();
    return 0;
}
