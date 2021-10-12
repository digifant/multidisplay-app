#ifndef DIGIFANTAPPLICATIONWINDOW_H
#define DIGIFANTAPPLICATIONWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QTime>
#include <QElapsedTimer>
#include "MdData.h"
#include "widgets/rtwidget.h"
#include "widgets/DFExtendedWidget.h"

class DigifantApplicationWindow : public QDialog
{
    Q_OBJECT
public:
    explicit DigifantApplicationWindow(QWidget *parent = 0);
    
signals:
    
public slots:
    void visualize (MdDataRecord *d);

protected:
    QGridLayout* l;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTime t;
#else
    QElapsedTimer t;
#endif
    MeasurementWidget* boostw;
    DFExtendedWidget *dfexw;
    MeasurementWidget *lw;
    MaxEgtWidget *egtw;
#if defined (DIGIFANTVANAPP)
    MeasurementWidget* rpmw;
#endif
};

#endif // DIGIFANTAPPLICATIONWINDOW_H
