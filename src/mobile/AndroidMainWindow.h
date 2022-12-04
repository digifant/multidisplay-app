#ifndef ANDROIDMAINWINDOW_H
#define ANDROIDMAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QGridLayout>
#include <QTime>
#include <QString>
#include <QPixmap>
#include "MdData.h"
#include "widgets/rtwidget.h"
#include "widgets/DFExtendedWidget.h"

namespace Ui {
class AndroidMainWindow;
}

class AndroidMainWindow : public QMainWindow
{
    Q_OBJECT
    
     friend class AppEngine;

public:
    explicit AndroidMainWindow(QWidget *parent = 0);
    ~AndroidMainWindow();

    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void closeEvent(QCloseEvent *event);
    void resizeEvent ( QResizeEvent * event );
    //void paintEvent(QPaintEvent *pe);
#if defined Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    static bool checkAndroidPermission(QString permission);
    static bool requestAndroidPermission(QString permission);
#endif
#endif
    enum ToastDuration {
        SHORT = 0,
        LONG = 1
    };

public slots:
    void showStatusMessage ( const QString &msg );
    void showToast ( const QString &msg, ToastDuration duration = ToastDuration::LONG );
    void btPortClosed ();
    void btPortOpened ();

    void fireSupportForumIntent();

signals:
    void writeSettings();

private:
    Ui::AndroidMainWindow *ui;

    QGridLayout* l;
    QElapsedTimer timer;
    MeasurementWidget* boostw;
    DFExtendedWidget *dfexw;
    MeasurementWidget *lw;
    MaxEgtWidget *egtw;
    QPixmap _pixmapBg;
#if defined (Q_OS_IOS) or defined (Q_OS_ANDROID)
    QLabel *statusBarLabel = nullptr;
#endif
};

#endif // ANDROIDMAINWINDOW_H
