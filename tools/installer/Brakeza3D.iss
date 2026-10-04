; Brakeza3D Windows installer (Inno Setup 6).
; Built from the command line by tools/package_windows.ps1, which passes:
;   /DMyAppVersion=<ENGINE_VERSION of master>  /DSourceDir=<package folder>  /DOutputDir=<output folder>
;   /DRepoDir=<repo root>
; The defaults below only apply when the script is opened by hand in the Inno Setup IDE.
; Based on the original wizard script (Desktop\InnoDBSetupBrakeza.iss). Non-commercial use only.

#ifndef MyAppVersion
  #define MyAppVersion "dev"
#endif
#ifndef SourceDir
  #define SourceDir "..\..\..\build"
#endif
#ifndef OutputDir
  #define OutputDir "..\..\.."
#endif
#ifndef RepoDir
  #define RepoDir "..\.."
#endif

#define MyAppName "Brakeza3D for Windows"
#define MyAppPublisher "Brakeza3D"
#define MyAppURL "https://www.brakeza.com"
#define MyAppExeName "Brakeza3D.exe"

[Setup]
; AppId identifies the application across versions: keep it, so a new installer upgrades the old one.
AppId={{521D5FA9-6A94-4356-B50F-E5DB788D7346}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
UninstallDisplayIcon={app}\bin\{#MyAppExeName}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
; Same file name as the previous releases: the website download links only change the release tag.
OutputBaseFilename=Brakeza3D-x64-86-Windows-installer
SetupIconFile={#RepoDir}\resources\windows\application.ico
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\bin\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
