#http://www.qtwiki.de/wiki/Vom_Qt-Programm_zum_Windows_Installer

!define PRODUCT_NAME "multidisplayUI"
!define EXE_NAME "mUI.exe"
!define PRODUCT_VERSION "0.21-release-x86"
!define PRODUCT_PUBLISHER "Dominik Gummel"
!define PRODUCT_WEB_SITE "http://code.google.com/p/multidisplay/"

!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${EXE_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY  "HKLM"

;!define MUI_ICON pfad_zu_ihrem_icon.ico
;!define MUI_UNICON pfad_zu_uninstaller_icon.ico


!include "mui.nsh"

; Zeige im Falle eines Installationsabbruchs eine Warnung
!define MUI_ABORTWARNING
; Definiere den Text der Warnung
!define MUI_ABORTWARNING_TEXT "Are you sure you want to quit the installation process?"
; Setze den "Abbrechen" Button als default
!define MUI_ABORTWARNING_CANCEL_DEFAULT

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "gpl-3.0.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
;füge Häkchen zur sofortigen Ausführung ein
!define MUI_FINISHPAGE_RUN "$INSTDIR\${EXE_NAME}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; setze Sprache
!insertmacro MUI_LANGUAGE "English"

Name "${PRODUCT_NAME}"
OutFile "mUI-setup-0.21-release-x86.exe"
InstallDir $PROGRAMFILES\${PRODUCT_NAME}
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
   SetOutPath $INSTDIR
   SetOverwrite ifnewer
   ;kopiere die .exe-Datei ins Installationsverzeichnis
   File ${EXE_NAME}
   ;kopiere die Bibliotheken ins Installationsverzeichnis
   File mingwm10.dll
   File QtCore4.dll
   ;File QtCored4.dll
   File QtGui4.dll
   ;File QtGuid4.dll
   File qextserialport1.dll
   File libgcc_s_dw2-1.dll
   File qextserialport1.dll
   ;Lege Startmenü-Eintrag an ($SMPROGRAMS entspricht dem Startmenü-Ordner)
   CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
   CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${EXE_NAME} "" "$INSTDIR\${EXE_NAME}"
   ;Erstelle Icons für Uninstaller
   CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe"
SectionEnd

Section -Post
   WriteUninstaller $INSTDIR\uninstall.exe
   WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "$(^Name)"
   WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_NAME}"
   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
   WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
   WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoModify" 1
   WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoRepair" 1
SectionEnd

;Meldung bei Erfolg
Function un.onUninstSuccess
   HideWindow
   MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer"
FunctionEnd

;Meldung bei Initialisierung
Function un.onInit
   MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
   Abort
FunctionEnd

;Sektion Uninstall
Section Uninstall
   ;Lösche die Dateien aus dem Programmordner
   Delete $INSTDIR\uninstall.exe
   Delete $INSTDIR\freshcode.exe
   Delete $INSTDIR\mingwm10.dll
   Delete $INSTDIR\QtCore4.dll
   Delete $INSTDIR\QtGui4.dll
   ;Delete $INSTDIR\QtCored4.dll
   ;Delete $INSTDIR\QtGuid4.dll
   Delete $INSTDIR\qextserialport1.dll
   Delete $INSTDIR\libgcc_s_dw2-1.dll
   Delete $INSTDIR\${EXE_NAME}
   ;Entferne den Programmordner
   RMDir  $INSTDIR
   ;Lösche alle Dateien aus dem Startmenü-Ordner
   Delete "$SMPROGRAMS\${PRODUCT_NAME}\*.*"
   ;Lösche den Startmenü-Ordner
   RMDir "$SMPROGRAMS\${PRODUCT_NAME}"
   DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
SectionEnd