/************************************
 
    Neko's configuration

 *************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "NekoSettings.h"

CNekoSettings::CNekoSettings( LPCWSTR key, BOOL fCreate /*TRUE*/ )
{
	m_hKey = nullptr;
	if( fCreate )
	{
		DWORD dwDisposition;
		RegCreateKeyExW( HKEY_CURRENT_USER, key, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &m_hKey, &dwDisposition );
	}
	else
	{
		if( RegOpenKeyExW( HKEY_CURRENT_USER, key, 0, KEY_READ|KEY_WRITE, &m_hKey ) != ERROR_SUCCESS )
			m_hKey = nullptr;
	}
}

CNekoSettings::~CNekoSettings()
{
    if( m_hKey ) RegCloseKey( m_hKey );
}

BOOL CNekoSettings::IsOpen()
{
	return( m_hKey != nullptr );
}

BOOL CNekoSettings::GetString( LPCWSTR keyname, LPWSTR result, ULONG size )
{
    DWORD dwType;
    return ( RegQueryValueExW( m_hKey, keyname, nullptr, &dwType, (LPBYTE)result, &size ) == ERROR_SUCCESS );
}

BOOL CNekoSettings::GetInt( LPCWSTR keyname, DWORD * result )
{
    DWORD dwType;
    ULONG ulSize = sizeof(DWORD);
    return ( RegQueryValueExW( m_hKey, keyname, nullptr, &dwType, (LPBYTE)result, &ulSize ) == ERROR_SUCCESS );
}

BOOL CNekoSettings::GetBool( LPCWSTR keyname, BOOL * result )
{
    DWORD dwType;
    ULONG ulSize = sizeof(BOOL);
    return ( RegQueryValueExW( m_hKey, keyname, nullptr, &dwType, (LPBYTE)result, &ulSize ) == ERROR_SUCCESS );
}


BOOL CNekoSettings::SetString( LPCWSTR keyname, LPCWSTR value )
{
    ULONG ulSize = (lstrlenW( value ) + 1) * sizeof(WCHAR);
    return ( RegSetValueExW( m_hKey, keyname, 0, REG_SZ, (const BYTE*)value, ulSize) == ERROR_SUCCESS );
}

BOOL CNekoSettings::SetInt( LPCWSTR keyname, DWORD value )
{
    ULONG ulSize = sizeof(DWORD);
    return ( RegSetValueExW( m_hKey, keyname, 0, REG_DWORD, (const BYTE*)&value, ulSize ) == ERROR_SUCCESS );
}

BOOL CNekoSettings::SetBool( LPCWSTR keyname, BOOL value )
{
    ULONG ulSize = sizeof(BOOL);
    return ( RegSetValueExW( m_hKey, keyname, 0, REG_BINARY, (const BYTE*)&value, ulSize ) == ERROR_SUCCESS );
}
