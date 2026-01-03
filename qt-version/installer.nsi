; Kindle DRM Converter - NSIS Installer Script
; Requires NSIS 3.0 or later
; Download from: https://nsis.sourceforge.io/

!define APP_NAME "Kindle DRM Converter"
!define COMP_NAME "Kindle DRM Converter Team"
!define VERSION "1.0.0"
!define COPYRIGHT "Copyright © 2024"
!define DESCRIPTION "Download, Remove DRM, and Convert Kindle Books"
!define INSTALLER_NAME "KindleDRMConverter-Setup.exe"
!define MAIN_APP_EXE "KindleDRMConverter.exe"
!define INSTALL_TYPE "SetShellVarContext current"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

; Modern UI
!include "MUI2.nsh"
!include "FileFunc.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\${MAIN_APP_EXE}"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--help"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.md"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language
!insertmacro MUI_LANGUAGE "English"

; Version Information
VIProductVersion "${VERSION}.0"
VIAddVersionKey "ProductName" "${APP_NAME}"
VIAddVersionKey "CompanyName" "${COMP_NAME}"
VIAddVersionKey "LegalCopyright" "${COPYRIGHT}"
VIAddVersionKey "FileDescription" "${DESCRIPTION}"
VIAddVersionKey "FileVersion" "${VERSION}"

; Installer attributes
Name "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
InstallDir "$PROGRAMFILES64\${APP_NAME}"
BrandingText "${APP_NAME}"
RequestExecutionLevel admin

; Main install section
Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    SetOverwrite ifnewer

    ; Copy all files from deployment directory
    File /r "KindleDRMConverter-Windows\*.*"

    ; Create shortcuts
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\Configure.lnk" "$INSTDIR\config.yaml"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\Output Folder.lnk" "$INSTDIR\output"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"

    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    ; Registry entries
    WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
    WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}" "DisplayName" "${APP_NAME}"
    WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}" "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
    WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}" "DisplayVersion" "${VERSION}"
    WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}" "Publisher" "${COMP_NAME}"

    ; Get install size
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD ${REG_ROOT} "${UNINSTALL_PATH}" "EstimatedSize" "$0"

SectionEnd

; Uninstaller section
Section Uninstall
    ; Remove shortcuts
    Delete "$DESKTOP\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Configure.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Output Folder.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk"
    RMDir "$SMPROGRAMS\${APP_NAME}"

    ; Remove files and directories
    RMDir /r "$INSTDIR\*.*"
    RMDir "$INSTDIR"

    ; Remove registry entries
    DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
    DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"

    ; Ask to remove user data
    MessageBox MB_YESNO|MB_ICONQUESTION "Do you want to remove all downloaded books and converted files?" IDNO skip_user_data
        RMDir /r "$INSTDIR\downloads"
        RMDir /r "$INSTDIR\output"
        RMDir /r "$INSTDIR\temp"
        RMDir /r "$INSTDIR\logs"
    skip_user_data:

SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Install ${APP_NAME} application files and create shortcuts."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Check for Calibre installation
Function .onInit
    ; Check if already installed
    ReadRegStr $R0 ${REG_ROOT} "${UNINSTALL_PATH}" "UninstallString"
    StrCmp $R0 "" done

    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
    "${APP_NAME} is already installed. $\n$\nClick 'OK' to remove the previous version or 'Cancel' to cancel this upgrade." \
    IDOK uninst
    Abort

    uninst:
        ClearErrors
        ExecWait '$R0 _?=$INSTDIR'

    done:

    ; Check for Calibre
    ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Calibre" "InstallLocation"
    StrCmp $R1 "" no_calibre calibre_found

    no_calibre:
        MessageBox MB_YESNO|MB_ICONQUESTION \
        "Calibre is not detected. Calibre is required for ebook conversion.$\n$\nDo you want to continue installation?" \
        IDYES calibre_found
        Abort

    calibre_found:

FunctionEnd

; After installation info
Function .onInstSuccess
    MessageBox MB_OK|MB_ICONINFORMATION \
    "Installation complete!$\n$\n\
    Next steps:$\n\
    1. Edit config.yaml with your Amazon credentials$\n\
    2. Install Calibre if not already installed$\n\
    3. Run ${APP_NAME} from Start Menu$\n$\n\
    See QUICKSTART.txt for detailed instructions."
FunctionEnd
