; Snake Game NSIS Installer
; This script is used by the GitHub Actions workflow to create the Windows installer

Unicode true

!define PRODUCT_NAME "Snake Game"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "SnakeGame"
!define PRODUCT_WEB_SITE "https://github.com/Minokainduwara/snake-game"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\snake.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI (Modern User Interface)
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Language Selection
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_INSTFILES

; Languages
!insertmacro MUI_LANGUAGE "English"

; Installer properties
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "SnakeGame-Setup-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES64\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show
RequestExecutionLevel admin

Section "Install" SEC01
    SetOutPath "$INSTDIR"
    SetOverwrite try

    ; Main executable
    File "snake.exe"

    ; Create saves directory
    CreateDirectory "$INSTDIR\saves"

    ; Create Start Menu shortcut
    CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
    CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Snake Game.lnk" "$INSTDIR\snake.exe"
    CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"

    ; Create Desktop shortcut
    CreateShortCut "$DESKTOP\Snake Game.lnk" "$INSTDIR\snake.exe"

    ; Write uninstaller
    WriteUninstaller "$INSTDIR\uninst.exe"

    ; Registry information for Add/Remove Programs
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\snake.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
    WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoModify" 1
    WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoRepair" 1
SectionEnd

; Uninstaller
Section "Uninstall"
    ; Remove shortcuts
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\Snake Game.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
    Delete "$DESKTOP\Snake Game.lnk"
    RMDir "$SMPROGRAMS\${PRODUCT_NAME}"

    ; Remove installed files
    Delete "$INSTDIR\snake.exe"
    Delete "$INSTDIR\uninst.exe"
    RMDir /r "$INSTDIR\saves"
    RMDir "$INSTDIR"

    ; Remove registry keys
    DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
    DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
SectionEnd