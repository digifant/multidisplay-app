#include "Overlay.h"

#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QResizeEvent>

Overlay::Overlay(QWidget *parent)
    : QWidget(parent)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);

}

void Overlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red));
    painter.drawLine(width()/8, height()/8, 7*width()/8, 7*height()/8);
    painter.drawLine(width()/8, 7*height()/8, 7*width()/8, height()/8);
    qDebug() << "Overlay::paintEvent";
}

void Overlay::resizeEvent ( QResizeEvent * event ) {
    QWidget::resizeEvent( event );
    qDebug() << "Overlay::resizeEvent old=" << event->oldSize() << "  new=" << event->size();
}

