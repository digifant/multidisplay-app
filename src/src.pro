TEMPLATE = app
TARGET = mUI
#TARGET = digifantview
QT += core \
    gui \
    widgets \
    opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += bluetooth sensors positioning serialport
}


CONFIG += rtti exceptions c++14

#qDebug() to console
win32: CONFIG += console

#turn off debug messages in release build
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

android {
    QT += svg androidextras
    ##FIX hack! fix for "java.lang.UnsatisfiedLinkError: dlopen failed: library libQt5Concurrent_x86.so libQt5PrintSupport_x86.so not found.
    QT += printsupport concurrent
}

maemo5 {
    QT += maemo5 mobility svg
    MOBILITY += location systeminfo sensors
}

android {
    #necessitas sets Q_OS_ANDROID
    DEFINES += ANDROID
    #legacy -> remove!
    DEFINES+= Q_WS_ANDROID
    message ("android define set!")
}

#simulator {
#    QT += maemo5
#    DEFINES+=QT_SIMULATOR
#}

HEADERS += evaluation/EvalSpectrogramPlot.h \
    com/MdBluetoothCom.h \
    com/MdBluetoothLECom.h \
    com/MdBluetoothWrapper.h \
    evaluation/MdSpectrogramData.h \
    evaluation/evaluationwindow.h \
    evaluation/EvalPlot.h \
    evaluation/EvalPlotData.h \
    MdDataRecordV1.h \
    visconfigdialog.h \
    MdPlotPicker.h \
    MdPlotZoomer.h \
    MdPlotData.h \
    MdPlot.h \
    VisualizationPlot.h \
    BoostPlot.h \
    MdData.h \
    serialoptions.h \
    multidisplayuimainwindow.h \
    mdutil.h \
    mobile/mobilemainwindow.h \
    AppEngine.h \
    mobile/MobileBoostPidWindow.h \
    mobile/MobileVis1MainWindow.h \
    mobile/MobileCommandWindow.h \
    thread/replayworker.h \
    thread/jobrunnerthread.h \
    thread/workerjob.h \
    widgets/realtimevis.h \
    widgets/rtwidget.h \
    MdDataRecordV2.h \
    Map16x1.h \
    N75OptionsDialog.h \
    V2N75SetupDialog.h \
    widgets/MyTableWidget.h \
    widgets/N75PidSettingsWidget.h \
    V2SettingsDialog.h \
    GearSettingsDialog.h \
    AboutDialog.h \
    widgets/DFExtendedWidget.h \
    TransferFunction.h \
    DigifantApplicationWindow.h \
    ColorOverBlend.h \
    DataTableConfigDialog.h \
    mobile/MobileSensorRecord.h \
    PowerPlot.h \
    V2PowerDialog.h \
    MdGpsSerial.h \
    WotEventsDialog.h \
    widgets/Overlay.h \
    com/MdAbstractCom.h \    
    com/MdBinaryProtocol.h \
    mobile/AndroidMainWindow.h \
    mobile/SwipeGestureRecognizer.h \
    mobile/AndroidN75Dialog.h \
    widgets/VR6Widget.h \
    com/BluetoothDeviceInfo.h \
    com/MdQSerialPortCom.h



lessThan(QT_MAJOR_VERSION, 5) {
    win32|unix:HEADERS+=com/MdQextSerialCom.h
} else {
    win32|unix:HEADERS+=mobile/MobileGPS.h
}


maemo5:HEADERS+=mobile/MobileEvaluationDialog.h \
    mobile/MobileGPS.h \
    mobile/Accelerometer.h \
    com/MdQextSerialCom.h

android:HEADERS+=mobile/Accelerometer.h \
    mobile/AndroidDashboardDialog.h

SOURCES += evaluation/EvalSpectrogramPlot.cpp \
    com/MdBluetoothCom.cpp \
    com/MdBluetoothLECom.cpp \
    com/MdBluetoothWrapper.cpp \
    evaluation/MdSpectrogramData.cpp \
    evaluation/evaluationwindow.cpp \
    evaluation/EvalPlot.cpp \
    evaluation/EvalPlotData.cpp \
    MdDataRecordV1.cpp \
    visconfigdialog.cpp \
    MdPlotPicker.cpp \
    MdPlotZoomer.cpp \
    MdPlotData.cpp \
    MdPlot.cpp \
    VisualizationPlot.cpp \
    BoostPlot.cpp \
    MdData.cpp \
    serialoptions.cpp \
    multidisplayuimainwindow.cpp \
    main.cpp \
    mdutil.cpp \
    mobile/mobilemainwindow.cpp \
    AppEngine.cpp \
    mobile/MobileBoostPidWindow.cpp \
    mobile/MobileVis1MainWindow.cpp \
    mobile/MobileCommandWindow.cpp \
    thread/workerjob.cpp \
    thread/replayworker.cpp \
    thread/jobrunnerthread.cpp \
    widgets/realtimevis.cpp \
    widgets/rtwidget.cpp \
    MdDataRecordV2.cpp \
    Map16x1.cpp \
    N75OptionsDialog.cpp \
    V2N75SetupDialog.cpp \
    widgets/MyTableWidget.cpp \
    widgets/N75PidSettingsWidget.cpp \
    V2SettingsDialog.cpp \
    GearSettingsDialog.cpp \
    AboutDialog.cpp \
    widgets/DFExtendedWidget.cpp \
    TransferFunction.cpp \
    DigifantApplicationWindow.cpp \
    ColorOverBlend.cpp \
    DataTableConfigDialog.cpp \
    mobile/MobileSensorRecord.cpp \
    PowerPlot.cpp \
    V2PowerDialog.cpp \
    MdGpsSerial.cpp \
    WotEventsDialog.cpp \
    widgets/Overlay.cpp \
    com/MdAbstractCom.cpp \
    com/MdBinaryProtocol.cpp \
    mobile/AndroidMainWindow.cpp \
    mobile/SwipeGestureRecognizer.cpp \
    mobile/AndroidN75Dialog.cpp \
    widgets/VR6Widget.cpp \
    com/BluetoothDeviceInfo.cpp \
    com/MdQSerialPortCom.cpp


lessThan(QT_MAJOR_VERSION, 5) {
    win32|unix:SOURCES+=com/MdQextSerialCom.cpp
} else {
    win32|unix:SOURCES+=mobile/MobileGPS.cpp
}

maemo5:SOURCES+=mobile/MobileEvaluationDialog.cpp \
    mobile/MobileGPS.cpp \
    mobile/Accelerometer.cpp

android:SOURCES+=mobile/Accelerometer.cpp \
    mobile/AndroidDashboardDialog.cpp

FORMS += evaluation/evaluationwindow.ui \
    visconfigdialog.ui \
    serialoptions.ui \
    multidisplayuimainwindow.ui \
    mobile/mobilemainwindow.ui \
    mobile/MobileBoostPidWindow.ui \
    mobile/MobileVis1MainWindow.ui \
    mobile/MobileCommandWindow.ui \
    N75OptionsDialog.ui \
    V2N75SetupDialog.ui \
    V2SettingsDialog.ui \
    GearSettingsDialog.ui \
    AboutDialog.ui \
    DataTableConfigDialog.ui \
    V2PowerDialog.ui \
    WotEventsDialog.ui \
    mobile/AndroidMainWindow.ui \
    mobile/AndroidDashboardDialog.ui \
    mobile/AndroidN75Dialog.ui

maemo5:FORMS += mobile/MobileEvaluationDialog.ui

unix:INCLUDEPATH = ../libs/qextserialport/src \
    ../libs/qwt-6.1.5/src



win32:INCLUDEPATH = $$quote(..\libs\qextserialport\src) \
                    $$quote(..\libs\qwt-6.1.5\src)

#win32 static
#2011-06-29 broken
#!debug_and_release|build_pass {
#    CONFIG(debug, debug|release) {
#        win32:LIBS += $$quote(../libs\qextserialport/src/build/libqextserialportd1.a) \
#                      $$quote(../libs\qextserialport/src/build/libqextserialportd.a) \
#                      ..\libs\qwt-6.1.5\lib\libqwtd.a

#    } else {
#        win32:LIBS += $$quote(../libs/qextserialport/src/build/libqextserialport1.a) \
#                      $$quote(../libs\qextserialport/src/build/libqextserialport.a) \
#                  ..\libs\qwt-6.1.5\lib\libqwt.a

#    }
#}

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        message ("debug")
        #win32 dynamic
        lessThan(QT_MAJOR_VERSION, 5) {
            LIBS += -L $$quote(..\libs\qextserialport\src\build) -lqextserialportd
        }
        win32:LIBS += ..\libs\qwt-6.1.5\lib\libqwtd.a
#        unix:LIBS += -L ../libs/qextserialport/src/build -lqextserialportd \
#                    ../libs/qwt-6.1.5/lib/libqwtd.a

        unix:{
            !android:!maemo5 {
                        message ("UNIX pure")
                        lessThan(QT_MAJOR_VERSION, 5) {
                            unix:LIBS += -L ../libs/qextserialport/src/build -lqextserialportd
                        }
                        unix:LIBS += -L../libs/qwt-6.1.5/lib -lqwt
                      }
            android:  {
                        #message("android: static linking!")
                        #unix:LIBS += ../libs/qwt-6.1.5/lib/libqwt.a
                        message("android AAB: dynamic linking!")                        
                        unix:LIBS += -L../libs/qwt-6.1.5/lib -lqwt_$${QT_ARCH}
                       }
            maemo5:    {
#                        message ("Maemo5: static linking!")
#                        unix:LIBS += ../libs/qextserialport/src/build/libqextserialportd.a \
#                                     ../libs/qwt-6.1.5/lib/libqwtd.a
                        message ("Maemo5: static qwt6 linking, dynamic qextserialport!")
                        unix:LIBS += ../libs/qwt-6.1.5/lib/libqwt.a -L../libs/qextserialport/src/build -lqextserialport
                        }
        }

    } else {
        message ("release")
        !android:!maemo5 {
                        message ("UNIX pure")
                        unix:LIBS += -L ../libs/qwt-6.1.5/lib -lqwt
        }
        win32:{
                lessThan(QT_MAJOR_VERSION, 5) {
                    LIBS += -L $$quote(..\libs\qextserialport\src\build) -lqextserialport1
                }
                    LIBS += -L $$quote(../libs/qwt-6.1.5/lib) -lqwt
        }
        android:  {
            #message("android: static linking!")
            #unix:LIBS += ../libs/qwt-6.1.5/lib/libqwt.a
            message("android AAB: dynamic linking!")
            unix:LIBS += -L../libs/qwt-6.1.5/lib -lqwt_$${QT_ARCH}
        }
        maemo5: {
            message ("Maemo5: static qwt6 linking, dynamic qextserialport!")
            unix:LIBS += ../libs/qwt-6.1.5/lib/libqwt.a -L../libs/qextserialport/src/build -lqextserialport
        }
    }
}



#if build fails with "dpkg-shlibdeps: failure: couldn't find library libqwt.so.5 needed by debian/mui/usr/bin/mUI (its RPATH is '')."
#uncomment line "dh_shlibdeps # Uncomment this line for use without Qt Creator"
#in mUI-build-maemo-Qt_for_Fremantle_PR1_3_Devices__Qt_SDK__Release/debian/rules
maemo5 {
    target.path = /usr/bin/
    INSTALLS += target
    desktopfile.files = ../maemofiles/mui.desktop
    desktopfile.path = /usr/share/applications/hildon
    INSTALLS += desktopfile
    icon48.path = /usr/share/icons/hicolor/48x48/apps
    icon48.files += ../maemofiles/mui.png
    INSTALLS += icon48
}

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android

OTHER_FILES += \
    ../android/AndroidManifest.xml
