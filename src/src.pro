TEMPLATE = app
TARGET = mUI
#TARGET = digifantview
QT += core \
    gui \
    widgets \
    opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += bluetooth sensors positioning
    !ios {
      QT += serialport
    }
}

#Qt-6.2 still missing required modules
#equals(QT_MAJOR_VERSION, 6):lessThan(QT_MINOR_VERSION, 3) {
equals(QT_MAJOR_VERSION, 6) {
    QT += opengl openglwidgets
}

CONFIG += rtti exceptions c++14

#qDebug() to console
win32: CONFIG += console

#turn off debug messages in release build
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

android {
    lessThan(QT_MAJOR_VERSION, 6) {
        QT += svg androidextras
    } else {
        #HACK to get jndi androidextras stuff! https://doc-snapshots.qt.io/qt6-dev/extras-changes-qt6.html
        QT += svg core-private
    }
    ##FIX hack! fix for "java.lang.UnsatisfiedLinkError: dlopen failed: library libQt5Concurrent_x86.so libQt5PrintSupport_x86.so not found.
    QT += printsupport concurrent
}

maemo5 {
    QT += maemo5 mobility svg
    MOBILITY += location systeminfo sensors
}

ANDROID_VERSION_NAME = "1.3"
ANDROID_VERSION_CODE = "9"

DEFINES += ANDROID_VERSION_NAME=$${ANDROID_VERSION_NAME}
DEFINES += ANDROID_VERSION_CODE=$${ANDROID_VERSION_CODE}
android {
    #necessitas sets Q_OS_ANDROID
    DEFINES += ANDROID
    #legacy -> remove!
    DEFINES+= Q_WS_ANDROID
    message ("android define set!")
}

ios {
    QT += svg
}

#simulator {
#    QT += maemo5
#    DEFINES+=QT_SIMULATOR
#}

HEADERS += evaluation/EvalSpectrogramPlot.h \
    builddatetime.h \
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

ios:HEADERS+=mobile/Accelerometer.h \
    mobile/AndroidDashboardDialog.h
ios:HEADERS-=com/MdQSerialPortCom.h \
             MdGpsSerial.h

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

ios:SOURCES+=mobile/Accelerometer.cpp \
    mobile/AndroidDashboardDialog.cpp
ios:SOURCES-=com/MdQSerialPortCom.cpp \
             MdGpsSerial.cpp

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
    ../libs/qwt-6.2.0/src



win32:INCLUDEPATH = $$quote(..\libs\qextserialport\src) \
                    $$quote(..\libs\qwt-6.2.0\src)

#win32 static
#2011-06-29 broken
#!debug_and_release|build_pass {
#    CONFIG(debug, debug|release) {
#        win32:LIBS += $$quote(../libs\qextserialport/src/build/libqextserialportd1.a) \
#                      $$quote(../libs\qextserialport/src/build/libqextserialportd.a) \
#                      ..\libs\qwt-6.2.0\lib\libqwtd.a

#    } else {
#        win32:LIBS += $$quote(../libs/qextserialport/src/build/libqextserialport1.a) \
#                      $$quote(../libs\qextserialport/src/build/libqextserialport.a) \
#                  ..\libs\qwt-6.2.0\lib\libqwt.a

#    }
#}

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        message ("debug")
        #win32 dynamic
        lessThan(QT_MAJOR_VERSION, 5) {
            LIBS += -L $$quote(..\libs\qextserialport\src\build) -lqextserialportd
        }
        win32:LIBS += ..\libs\qwt-6.2.0\lib\libqwtd.a
#        unix:LIBS += -L ../libs/qextserialport/src/build -lqextserialportd \
#                    ../libs/qwt-6.2.0/lib/libqwtd.a

        unix:{
            !android:!maemo5:!ios {
                        message ("UNIX pure")
                        lessThan(QT_MAJOR_VERSION, 5) {
                            unix:LIBS += -L ../libs/qextserialport/src/build -lqextserialportd
                        }
                        unix:LIBS += -L../libs/qwt-6.2.0/lib -lqwt
                      }
            android:  {
                        #message("android: static linking!")
                        #unix:LIBS += ../libs/qwt-6.2.0/lib/libqwt.a
                        message("android AAB: dynamic linking (debug)! QT_ARCH=$${QT_ARCH} ANDROID_TARGET_ARCH=$${ANDROID_TARGET_ARCH}")
                        unix:LIBS += -L../libs/qwt-6.2.0/lib -lqwt_$${QT_ARCH}
                       }
            maemo5:    {
#                        message ("Maemo5: static linking!")
#                        unix:LIBS += ../libs/qextserialport/src/build/libqextserialportd.a \
#                                     ../libs/qwt-6.2.0/lib/libqwtd.a
                        message ("Maemo5: static qwt6 linking, dynamic qextserialport!")
                        unix:LIBS += ../libs/qwt-6.2.0/lib/libqwt.a -L../libs/qextserialport/src/build -lqextserialport
                        }
           ios: {
                  message ("IOS debug")
                  message ("IOS: static qwt6 linking")
                  LIBS += ../libs/qwt-6.2.0/lib/libqwt.a
                  #LIBS += UIKit
                  message( $$QMAKESPEC )
                }
        }

    } else {
        message ("release")
        !android:!maemo5 {
                        message ("UNIX pure")
                        unix:LIBS += -L ../libs/qwt-6.2.0/lib -lqwt
        }
        win32:{
                lessThan(QT_MAJOR_VERSION, 5) {
                    LIBS += -L $$quote(..\libs\qextserialport\src\build) -lqextserialport1
                }
                    LIBS += -L $$quote(../libs/qwt-6.2.0/lib) -lqwt
        }
        android:  {
            #message("android: static linking!")
            #unix:LIBS += ../libs/qwt-6.2.0/lib/libqwt.a
            message("android AAB: dynamic linking (release)! QT_ARCH=$${QT_ARCH} ANDROID_TARGET_ARCH=$${ANDROID_TARGET_ARCH}")
            unix:LIBS += -L../libs/qwt-6.2.0/lib -lqwt_$${QT_ARCH}
        }
        maemo5: {
            message ("Maemo5: static qwt6 linking, dynamic qextserialport!")
            unix:LIBS += ../libs/qwt-6.2.0/lib/libqwt.a -L../libs/qextserialport/src/build -lqextserialport
        }
        ios: {
               message ("IOS release")
               message ("IOS: static qwt6 linking")
               LIBS += ../libs/qwt-6.2.0/lib/libqwt.a
               #LIBS += UIKit
               message( $$QMAKESPEC )
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

ios {
    QMAKE_INFO_PLIST = ../ios/Info.plist
    ios_icon.files = $$files(../ios/AppIcon*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    #TODO launch.xib storyboard ios >= 8
    #app_launch_images.files = ../ios/Launch.xib $$files(../ios/LaunchImage*.png)
    app_launch_images.files = $$files(../ios/LaunchImage*.png)
    QMAKE_BUNDLE_DATA += app_launch_images
}

ANDROID_ABIS = arm64-v8a x86_64 x86 armeabi-v7a

ANDROID_EXTRA_LIBS = $$OUT_PWD/../libs/qwt-6.2.0/lib/libqwt_x86_64.so $$OUT_PWD/../libs/qwt-6.2.0/lib/libqwt_x86.so $$OUT_PWD/../libs/qwt-6.2.0/lib/libqwt_arm64-v8a.so $$OUT_PWD/../libs/qwt-6.2.0/lib/libqwt_armeabi-v7a.so




