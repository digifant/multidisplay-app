PROJECT                 = qextserialport
TEMPLATE                = lib
VERSION                 = 1.2.0
DESTDIR                 = build
CONFIG                 += qt warn_on debug_and_release
CONFIG                  += dll
DEFINES                 += QEXTSERIALPORT_LIB

unix: CONFIG +=  dll staticlib

android: CONFIG += staticlib
maemo5: {
        message("Maemo5 qextserial")
#            CONFIG += staticlib
#            CONFIG -= dll
            CONFIG -= staticlib
            CONFIG += dll
}

# event driven device enumeration on windows requires the gui module
!win32:QT               -= gui

OBJECTS_DIR             = tmp
MOC_DIR                 = tmp
DEPENDDIR               = .
INCLUDEDIR              = .
HEADERS                 = qextserialport.h \
                          qextserialenumerator.h \
                          qextserialport_global.h
SOURCES                 = qextserialport.cpp

unix:SOURCES           += posix_qextserialport.cpp
unix:!macx:SOURCES     += qextserialenumerator_unix.cpp
macx {
  SOURstaticlibCES          += qextserialenumerator_osx.cpp
  LIBS             += -framework IOKit -framework CoreFoundation
}

win32 {
  SOURCES          += win_qextserialport.cpp qextserialenumerator_win.cpp
  DEFINES          += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
  LIBS             += -lsetupapi
}

CONFIG(debug, debug|release) {
    TARGET = qextserialportd
} else {
    TARGET = qextserialport
}

maemo5 {
    target.path = /usr/lib
    INSTALLS += target
}
