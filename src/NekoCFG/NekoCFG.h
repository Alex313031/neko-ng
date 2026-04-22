#ifndef _NEKOCFG_H
#define _NEKOCFG_H

#include "../version.h"
#include "../NekoCommon.h"

//structures
typedef struct tagTABINITDATA {
    LPCWSTR szTitle;
    UINT uIDDialog;
    DLGPROC lpfnDlgProc;
    UINT uIDIcon;
    HWND hDlg;
} TABINITDATA, *LPTABINITDATA;

typedef struct tagCATSETTINGS {
    WCHAR szName[MAX_NEKO_NAME];

    //display
    WCHAR szLibrary[MAX_PATH];
    DWORD uScale;
	BOOL fAlwaysOnTop;

    //movement
    DWORD uSpeed;
    DWORD uMouseSensitivity;

    //sounds
    WCHAR szSndIdle1[MAX_PATH];
    WCHAR szSndIdle2[MAX_PATH];
    WCHAR szSndIdle3[MAX_PATH];
	WCHAR szSndSleep[MAX_PATH];
    WCHAR szSndAwake[MAX_PATH];
	DWORD uSndFrequency;

    //independence
    DWORD uAction;

    //effects
    BOOL bFootprints;
    WCHAR szFootprintLib[MAX_PATH];

    //list items
    BOOL fDeleted;
    struct tagCATSETTINGS* next;

} CATSETTINGS, *LPCATSETTINGS;

//ranges for slider bars
#define MIN_SPEED 2
#define MAX_SPEED 48
#define MIN_SENSE 1
#define MAX_SENSE 64
#define MIN_SNDFREQ 0
#define MAX_SNDFREQ 100
#define MIN_SCALE 10
#define MAX_SCALE 400

//external functions
extern void WINAPI InitialisePropertyDialog( HWND hWnd );
extern void WINAPI ShutdownPropertyDialog();
extern void WINAPI OnSelChanged( HWND hWnd );
extern void WINAPI SetActiveCat( LPCATSETTINGS lpCat );
extern void WINAPI GetDialogSettings();

//external variables
extern HINSTANCE g_hInstance;
extern LPCWSTR szHelpFile;

#endif //_NEKOCFG_H
