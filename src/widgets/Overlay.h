#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>

class Overlay : public QWidget
{
    Q_OBJECT
public:
    explicit Overlay(QWidget *parent = 0);
    
signals:
    
public slots:

protected:
     void paintEvent(QPaintEvent *event);
     void resizeEvent(QResizeEvent *event);
    
};

#endif // OVERLAY_H
