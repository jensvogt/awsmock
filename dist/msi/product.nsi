; =====================================================
; AwsMock NSIS Installer Script
; =====================================================

!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"
!include "WordFunc.nsh"
!include "StrFunc.nsh"

; =====================================================
; General Settings
; =====================================================
Name "AwsMock 1.18.20"
OutFile "AwsMock-1.18.20-amd64.exe"
InstallDir "$PROGRAMFILES64\awsmock"
InstallDirRegKey HKLM "Software\AwsMock" "InstallDir"
RequestExecutionLevel admin
SetCompressor /SOLID lzma

; =====================================================
; Version Info
; =====================================================
VIProductVersion "1.18.20.0"
VIAddVersionKey "ProductName"     "AWS Mock"
VIAddVersionKey "CompanyName"     "Jens Vogt"
VIAddVersionKey "FileDescription" "AWS Cloud Service Simulator"
VIAddVersionKey "FileVersion"     "1.18.20"
VIAddVersionKey "ProductVersion"  "1.18.20"
VIAddVersionKey "LegalCopyright"  "© Jens Vogt"

; =====================================================
; MUI Settings
; =====================================================
!define MUI_ABORTWARNING
!define MUI_ICON   "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; =====================================================
; Source directory variable
; =====================================================
!define VERSION "1.18.20"
!define SRCDIR "C:\work\private\awsmock"

; =====================================================
; Pages
; =====================================================
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${SRCDIR}\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

; =====================================================
; Update check on startup
; =====================================================
Function CheckForUpdates

  inetc::get /SILENT \
    "https://api.github.com/repos/jensvogt/awsmock/releases/latest" \
    "$TEMP\awsmock_release.json" /END
  Pop $0

  ${If} $0 == "OK"

    ; Parse JSON properly with nsJSON
    nsJSON::Set /file "$TEMP\awsmock_release.json"
    Pop $0
    ${If} $0 == "ok"
      nsJSON::Get "tag_name" /end
      Pop $R0  ; $R0 = "v1.18.17"

      ; Strip leading "v"
      StrCpy $R1 $R0 "" 1  ; $R1 = "1.18.17"

      ${VersionCompare} $R1 "${VERSION}" $R2
      ${If} $R2 == 1
        MessageBox MB_YESNO \
          "Version $R1 is available.$\nOpen download page?" \
          IDYES open_browser IDNO done
        open_browser:
          ExecShell "open" "https://github.com/jensvogt/awsmock/releases/latest"
          Quit
        done:
      ${EndIf}
    ${EndIf}

  ${EndIf}

  Delete "$TEMP\awsmock_release.json"

FunctionEnd

; Called automatically before installer UI shows
Function .onInit
  Call CheckForUpdates
FunctionEnd

; =====================================================
; Installer Sections
; =====================================================
Section "Main Application" SecMain

  SetOutPath "$INSTDIR\bin"
  File "${SRCDIR}\cmake-build-release\Release\awsmockmgr.exe"
  File "${SRCDIR}\cmake-build-release\Release\awslocal.exe"
  File "${SRCDIR}\cmake-build-release\Release\awsmockctl.exe"

  SetOutPath "$INSTDIR\etc"
  File "/oname=awsmock.json" "${SRCDIR}\dist\etc\awsmock_win32.json"

  SetOutPath "$INSTDIR\init"
  File "${SRCDIR}\dist\msi\init.json"

  ; Create empty directories
  CreateDirectory "$INSTDIR\log"
  CreateDirectory "$INSTDIR\tmp"
  CreateDirectory "$INSTDIR\data"
  CreateDirectory "$INSTDIR\data\application"
  CreateDirectory "$INSTDIR\data\backup"
  CreateDirectory "$INSTDIR\data\lambda"
  CreateDirectory "$INSTDIR\data\s3"
  CreateDirectory "$INSTDIR\data\sqs"
  CreateDirectory "$INSTDIR\data\sns"
  CreateDirectory "$INSTDIR\data\tmp"
  CreateDirectory "$INSTDIR\data\transfer"
  CreateDirectory "$INSTDIR\frontend"

  ; Copy samples recursively
  SetOutPath "$INSTDIR\samples"
  File /r "${SRCDIR}\dist\samples\*.*"

  ; Copy frontend recursively
  SetOutPath "$INSTDIR\frontend"
  File /r "${SRCDIR}\awsmock-ui\dist\awsmock-ui\browser\*.*"

  ; Write registry for uninstaller
  WriteRegStr HKLM "Software\AwsMock" "InstallDir" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AwsMock" \
    "DisplayName" "AWS Mock"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AwsMock" \
    "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AwsMock" \
    "DisplayVersion" "1.18.20"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AwsMock" \
    "Publisher" "Jens Vogt"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AwsMock" \
    "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AwsMock" \
    "NoRepair" 1

  ; Write uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; =====================================================
; Windows Service
; =====================================================
Section "Windows Service" SecService

  ; Stop and remove existing service if present
  SimpleSC::StopService "awsmock" 1 30
  SimpleSC::RemoveService "awsmock"

  ; Install service
  SimpleSC::InstallService "awsmock" "AWS Mock" 16 2 \
    `"$INSTDIR\bin\awsmockmgr.exe" --config "$INSTDIR\etc\awsmock.json"` "" "" ""
  SimpleSC::SetServiceDescription "awsmock" "AWS Cloud Service Simulator"

  ; Start service
  SimpleSC::StartService "awsmock" "" 30

  Pop $0
  ${If} $0 <> 0
    MessageBox MB_OK "Service could not be started. Error: $0"
  ${EndIf}

SectionEnd

; =====================================================
; Uninstaller
; =====================================================
Section "Uninstall"

  ; Stop and remove service
  SimpleSC::StopService "awsmock" 1 30
  SimpleSC::RemoveService "awsmock"

  ; Remove files
  Delete "$INSTDIR\bin\awsmockmgr.exe"
  Delete "$INSTDIR\bin\awslocal.exe"
  Delete "$INSTDIR\bin\awsmockctl.exe"
  Delete "$INSTDIR\etc\awsmock.json"
  Delete "$INSTDIR\init\init.json"
  Delete "$INSTDIR\uninstall.exe"

  ; Remove samples recursively
  RMDir /r "$INSTDIR\samples"

  ; Remove directories (only if empty)
  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\log"
  RMDir /r "$INSTDIR\tmp"
  RMDir /r "$INSTDIR\frontend"
  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\etc"
  RMDir "$INSTDIR\init"
  RMDir "$INSTDIR"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\AwsMock"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AwsMock"

SectionEnd
