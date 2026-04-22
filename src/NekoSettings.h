/************************************
 
    Neko's configuration header file

 *************************************/

#ifndef _NEKO_NEKO_SETTINGS_H_
#define _NEKO_NEKO_SETTINGS_H_

#include "framework.h"

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

#endif // _NEKO_NEKO_SETTINGS_H_
