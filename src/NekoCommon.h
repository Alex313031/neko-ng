/************************************

   Common Definitions header

   Contains definitions used by both
   neko_win32 and neko_cfg applications

 *************************************/

#ifndef _NEKO_COMMON_H_
#define _NEKO_COMMON_H_

//class & window names
#define szNekoClassName   L"Neko98Class"
#define szNekoWindowTitle L"Neko-ng"

//registry key for Neko's settings
#define szNekoRegKey      L"Software\\Alex313031\\Neko-ng"

//sub-key names
#define szNekoTaskbarKey      L"Taskbar Icon"

#define szNekoSpeedKey        L"Speed"
#define szNekoSenseKey        L"Sensitivity"
#define szNekoLibraryKey      L"Library"
#define szNekoSndIdle1Key     L"Snd_Idle1"
#define szNekoSndIdle2Key     L"Snd_Idle2"
#define szNekoSndIdle3Key     L"Snd_Idle3"
#define szNekoSndSleepKey     L"Snd_Sleep"
#define szNekoSndAwakeKey     L"Snd_Awake"
#define szNekoSndFreqKey      L"Sound Frequency"
#define szNekoScaleKey        L"Size"
#define szNekoOnTopKey        L"On Top"
#define szNekoFootprintLibKey L"Footprint Library"
#define szNekoFootprintKey    L"Footprints"
#define szNekoActionKey       L"Action"
#define szNekoNumCatsKey      L"NumNekos"


//identifiers for different Neko actions
#define CHASE_MOUSE           0
#define RUN_AWAY_FROM_MOUSE   1
#define RUN_AROUND_RANDOMLY   2
#define PACE_AROUND_SCREEN    3
#define RUN_AROUND            4


//message sent from NekoCFG to Neko to signal settings have changed
#define MY_UPDATENEKO (WM_USER+33)

//maximum length of a neko 'Name'
#define MAX_NEKO_NAME (64)

#endif // _NEKO_COMMON_H_
