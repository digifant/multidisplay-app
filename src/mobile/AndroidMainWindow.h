#ifndef ANDROIDMAINWINDOW_H
#define ANDROIDMAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QGridLayout>
#include <QTime>
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

    void closeEvent(QCloseEvent *event);

public slots:
    void visualize (MdDataRecord *d);

signals:
    void writeSettings();

private:
    Ui::AndroidMainWindow *ui;

    QGridLayout* l;
    QTime t;
    MeasurementWidget* boostw;
    DFExtendedWidget *dfexw;
    MeasurementWidget *lw;
    MaxEgtWidget *egtw;
};

#endif // ANDROIDMAINWINDOW_H
