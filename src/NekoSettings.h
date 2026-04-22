/************************************
 
    Neko's configuration header file

 *************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "NekoCommon.h"

class CNekoSettings {
public:
    CNekoSettings( LPCWSTR key, BOOL fCreate = TRUE );
    ~CNekoSettings();

	BOOL IsOpen();

    BOOL GetString( LPCWSTR keyname, LPWSTR result, ULONG size );
    BOOL GetInt( LPCWSTR keyname, DWORD * result );
    BOOL GetBool( LPCWSTR keyname, BOOL * result );

    BOOL SetString( LPCWSTR keyname, LPCWSTR value );
    BOOL SetInt( LPCWSTR keyname, DWORD value );
    BOOL SetBool( LPCWSTR keyname, BOOL value );

private:
    HKEY m_hKey;
};
