#ifndef _NEKO_VERSION_H_
#define _NEKO_VERSION_H_

// This file is for specifying the target windows version, as well as application
// version constants.

// We need to define _UNICODE and UNICODE for TCHAR
#ifndef UNICODE
 #define UNICODE
#endif

#ifndef _UNICODE
 #define _UNICODE
#endif

#if defined(__clang__) && defined(_UNICODE)
 #pragma code_page(65001) // UTF-8
#endif

/* Including SDKDDKVer.h defines the highest available Windows platform.
   If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
   set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h. */

#ifndef __MINGW32__
 #include <WinSDKVer.h> // Doesn't exist in MinGW
#endif

#ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x0500 // Windows 2000
#endif

#ifndef WINVER
 #define WINVER 0x0500 // Same as _WIN32_WINNT above
#endif

#ifndef _WIN64_WINNT
 #define _WIN64_WINNT 0x0502 // Minimum version for 64 bit, Windows Server 2003
#endif

#ifndef _WIN32_IE
 #define _WIN32_IE 0x0501 // Minimum Internet Explorer version for common controls
#endif

#if _WIN32_WINNT < 0x0601 // If we are less than Windows 7, use old ATL for safety
 #ifndef _ATL_XP_TARGETING
  #define _ATL_XP_TARGETING // For using XP-compatible ATL/MFC functions
 #endif
#endif

#ifndef __MINGW32__
 #include <SDKDDKVer.h> // Doesn't exist in MinGW
#endif

// clang-format off: Version DEFINES left alone

// Macro to convert to string
#if !defined(_STRINGIZER_)
 #define _STRINGIZER_
 #define _STRINGIZER(in) #in
 #define STRINGIZE(in) _STRINGIZER(in)
 // Wide-string variant: L ## "x" -> L"x". Two levels so the argument is
 // expanded before the L## paste widens the resulting narrow literal.
 #define _WIDEN(in) L ## in
 #define WIDEN(in) _WIDEN(in)
#endif // !defined(_STRINGIZER_)

// Main version constant. The whole dotted version is stringized in ONE step so
// it expands to a SINGLE wide string literal (e.g. L"4.0.5") rather than several
// adjacent ones (L"4" L"." L"0" ...). GNU windres concatenates adjacent literals
// anywhere, but llvm-rc does not concatenate them in a dialog control's text
// field and fails with 'expected ',', got "4"'. A single literal works for
// both toolchains.
#ifndef _VERSION
 // Run stringizer above
 #define _VERSION(major,minor,build) WIDEN(STRINGIZE(major.minor.build))
#endif // _VERSION

// These next few lines are where we control version number and copyright year
// Adhere to semver > semver.org
#define MAJOR_VERSION 4
#define MINOR_VERSION 0
#define BUILD_VERSION 5

// String constants
#define VERSION_STRING _VERSION(MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION)
// Stringize the name and version together so this is one wide literal
// (L"Neko-ng 4.0.5"). See the note on _VERSION above for why this must not be a
// run of adjacent literals (L"Neko-ng " VERSION_STRING) under llvm-rc.
#define ABOUT_CONTENT  WIDEN(STRINGIZE(Neko-ng MAJOR_VERSION.MINOR_VERSION.BUILD_VERSION))
#define LEGAL_COPYRIGHT L"Copyright \251 2026 Alex313031 "

#ifndef _PACKVERSION
 #define _PACKVERSION(major,minor) MAKELONG((minor), ((major) << 8))
#endif

// clang-format on: Done with version DEFINES

#endif // _NEKO_VERSION_H_
