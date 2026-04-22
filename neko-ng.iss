; Neko-ng Inno Setup Script File
; Only for Inno Setup 5.x to support Windows 2000/XP
; Tested with ISS 5.6.1

#define AppVer "4.0.4"
#define AppName "Neko-ng"
#define CFGAppName "Neko-ng Config"
#define ExeName "neko_win32"
#define CFGExeName "neko_cfg"
#define Developer "Alex313031"
#define CopyRightYear "© 2026"
#define GitURL "https://github.com/Alex313031/neko-ng"

[Setup]
MinVersion=5.0
AppName={#AppName}
AppVersion={#AppVer}
AppVerName={#AppName} v{#AppVer}
OutputBaseFilename={#ExeName}_{#AppVer}_setup
UninstallDisplayName={#AppName} {#AppVer}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\{#ExeName}.exe
DisableWelcomePage=no
AlwaysShowDirOnReadyPage=yes
Compression=lzma
SolidCompression=yes
VersionInfoVersion={#AppVer}
AppPublisher={#Developer}
AppPublisherURL={#GitURL}
AppSupportURL={#GitURL}/#readme
AppUpdatesURL={#GitURL}/releases
VersionInfoCompany={#Developer}
AppCopyright={#CopyRightYear}
VersionInfoCopyright={#CopyRightYear} {#Developer}
VersionInfoProductName={#AppName}
InfoBeforeFile="assets\Readme.txt"
SetupIconFile="src\NekoCFG\res\default.ico"
WizardImageFile="assets\installer_background.bmp"
;WizardStyle=modern
UseSetupLdr=yes

[Files]
Source: "release\{#ExeName}.exe"; DestDir: "{app}"
Source: "release\{#CFGExeName}.exe"; DestDir: "{app}"
Source: "assets\neko-ng.hlp"; DestDir: "{app}"
Source: "assets\Readme.txt"; DestDir: "{app}";

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#ExeName}.exe"
Name: "{group}\{#CFGAppName}"; Filename: "{app}\{#CFGExeName}.exe"

[Run]
;Filename: "{app}\Readme.txt"; Description: "View Readme.txt"; Flags: postinstall nowait skipifsilent shellexec unchecked
Filename: "{app}\{#CFGExeName}.exe"; Description: "Launch Neko-ng when finished"; Flags: postinstall nowait skipifsilent
