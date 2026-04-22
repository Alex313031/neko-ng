/************************************
	Neko for Windows NT
 *************************************/

#include "Neko98.h"

#include "Tray.h"
#include "NekoSettings.h"
#include "NekoCommon.h"
#include "resource.h"
#include "Neko.h"

//global variables
HINSTANCE g_hInstance = NULL;
HWND g_hWndMain = NULL;
BOOL g_fTaskbar = TRUE;
CTray * Tray = NULL;

int g_nNumCats = 0;
typedef CNeko* PNeko;
PNeko* g_NekoArray = NULL; //array of CNeko pointers
DWORD g_dwSleepTime = 100;

BOOL g_fNeedReinit = TRUE;

static HICON kMainIcon = nullptr;
static HICON kSmallIcon = nullptr;

/* ExecuteConfigProcess - runs the configuration program *******************************/
static bool ExecuteConfigProcess() {
  if ((int)(INT_PTR)ShellExecuteW(nullptr, L"open", L"neko_cfg.exe", 0, L"", SW_SHOWNORMAL) <= 32 ) {
    MessageBoxW( NULL, L"Neko was unable to start the configuration program\n'neko_cfg.exe'\n\nMake sure that this file is in the same folder as the main Neko-ng program.", L"Configure Neko-ng", MB_ICONEXCLAMATION );
    return false;
  } else {
    return true;
  }
}

/* LoadConfiguration - reloads the program configuration *******************************/
void LoadConfiguration()
{
    //load in the new global settings
    CNekoSettings * NekoSettings = new CNekoSettings( szNekoRegKey );
    NekoSettings->GetBool( szNekoTaskbarKey, &g_fTaskbar );

    //delete all existing Nekos
    int i;
    for( i = 0; i < g_nNumCats; i++ ) delete g_NekoArray[i];
    delete[] g_NekoArray;

    //load in new cats list
    DWORD dwNumCats = 0;
    NekoSettings->GetInt( szNekoNumCatsKey, &dwNumCats );
    g_nNumCats = dwNumCats;

    if( g_nNumCats == 0 )
    {
        //the user hasn't run the config program, or there are no Nekos - use default
        g_NekoArray = new PNeko[1];
        g_NekoArray[0] = new CNeko();
        g_nNumCats = 1;
    }
    else
    {
        g_NekoArray = new PNeko[g_nNumCats];
        for( i = 0; i < g_nNumCats; i++ )
        {
            //build the key index
            WCHAR szSubKey[MAX_PATH], szName[MAX_NEKO_NAME];
            wsprintfW( szSubKey, L"%d", i );

            //load this cat's name from the registry
		    NekoSettings->GetString( szSubKey, szName, (MAX_NEKO_NAME-1) * sizeof(WCHAR) );

            //add this Neko to the array
			g_NekoArray[i] = new CNeko( szName );
        }
    }

	//erase the Neko settings
    delete NekoSettings;

	//calculate new sleep time
	g_dwSleepTime = 100;
	if( g_nNumCats <= 20 ) g_dwSleepTime = 200 - ( 5 * g_nNumCats );

    //update task bar
    if( g_fTaskbar )
    {
        if( Tray->GetCount() == 0 )
			Tray->AddIcon( g_hWndMain, LoadIconW( g_hInstance, MAKEINTRESOURCEW( IDI_AWAKE )), 1 );
    }
    else
    {
        if( Tray->GetCount() > 0 ) 
			Tray->RemoveIcon( g_hWndMain, 1 );
    }

}


/* WndProc - message processing function for the hidden Neko window ********************/
static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
        case WM_DESTROY:
			//terminate the program
            PostQuitMessage( 0 );
            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_ABOUT:
                    DialogBoxW( g_hInstance, MAKEINTRESOURCEW(IDD_ABOUTBOX), hWnd, (DLGPROC)AboutDlgProc );
                    break;
                case ID_SETTINGS:
                    ExecuteConfigProcess();
                    break;

                case ID_EXIT: 
                    PostQuitMessage( 0 ); 
                    break;
            }
            break;

        case WM_DEVMODECHANGE:
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
        case MY_UPDATENEKO:
            g_fNeedReinit = TRUE;
            break;

        case MY_NOTIFYICON:
            switch( lParam )
            {
                case WM_LBUTTONDBLCLK:
                    ExecuteConfigProcess();
                    break;

                case WM_RBUTTONUP:
                    //this line is needed to make the menu go away!
                    //(from Windows Developer FAQ [Robert Mashlan])
                    SetForegroundWindow( hWnd );

                    //show the context menu
                    Tray->ShowPopupMenu( hWnd );
                    break;
            }
            break;

        case WM_TIMER:
        {
            //check to see if stuff needs reinitialisation
            if( g_fNeedReinit )
            {
                LoadConfiguration();
                g_fNeedReinit = FALSE;
            }

			//update all Nekos
			for( int i = 0; i < g_nNumCats; i++ ) g_NekoArray[i]->Update();
            break;
        }

        default:
            return DefWindowProcW( hWnd, message, wParam, lParam );
    }

    return 0;
}


/* InitApplication - program initialisation function ***********************************/
static BOOL InitApplication(HINSTANCE hInstance) {
    //store the instance handle
    g_hInstance = hInstance;

  kMainIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDI_AWAKE));
  kSmallIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDI_AWAKE));

	//create the (hidden) window
    WNDCLASSW wc;
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szNekoClassName;

    if( RegisterClassW(&wc) )
    {
        //create the window
        g_hWndMain = CreateWindowW( szNekoClassName, szNekoWindowTitle, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

		if( g_hWndMain == NULL ) return FALSE;
    }
	else
		return FALSE;

    //initialise random number generator
    srand( GetTickCount() );
    return TRUE;
}


/* WinMain - program entry point *******************************************************/
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine,
                      int iCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
	//check for previous version & unload both if it's found
	HWND hWndOld = FindWindowW( szNekoClassName, szNekoWindowTitle );
	if( hWndOld != NULL )
	{
		PostMessageW( hWndOld, WM_QUIT, 0, 0 );
		return FALSE;
	}

    //ensure the common control library is loaded
    InitCommonControls();

    //initialise the program
    if( !InitApplication( hInstance ) ) return FALSE;

    //create the tray object
    Tray = new CTray( hInstance );

    //set a timer to update Neko
    SetTimer( g_hWndMain, ID_TIMER_NEKO, 200, NULL );

    MSG msg;
    while( GetMessageW( &msg, NULL, 0, 0 ) )
    {
		//pass the message onto our WndProc
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	}

    //remove the timer
    KillTimer( g_hWndMain, ID_TIMER_NEKO );

    //delete all Nekos
    for( int i = 0; i < g_nNumCats; i++ ) delete g_NekoArray[i];
    delete[] g_NekoArray;

    //remove the taskbar icon
    if( g_fTaskbar ) Tray->RemoveIcon( g_hWndMain, 1 );
    delete Tray;

    return (int)msg.wParam;
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (uMsg) {
    case WM_INITDIALOG:
      // Set icon in titlebar of about dialog
      static const HICON kAboutIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDI_ABOUTICON));
      SendMessageW(hDlg, WM_SETICON, ICON_BIG, (LPARAM)kAboutIcon);
      SendMessageW(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)kAboutIcon);
      return TRUE;
    case WM_CLOSE:
      EndDialog( hDlg, TRUE );
      return TRUE;
    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
        EndDialog(hDlg, LOWORD(wParam));
        return TRUE;
      }
    default:
      break;
  }

  return FALSE;
}
