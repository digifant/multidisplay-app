#include "VR6Widget.h"

VR6Widget::VR6Widget( QWidget *parent, QString caption, double lo, double mid, double hi,
                      QColor loColor, QColor midColor, QColor hiColor )
          : MeasurementWidget (parent, caption, lo, mid, hi, loColor, midColor, hiColor) {

}

void VR6Widget::paint() {
//    qDebug() << "DFExtendedWidget::paint()";

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bc = loColor;

    painter.fillRect( QRect(0,0,size().width(),size().height()), bc);

    //    painter.setBackgroundMode( Qt::OpaqueMode );
    painter.setBackgroundMode( Qt::TransparentMode );
    painter.setBackground( QBrush ( bc ) );

    textFont.setItalic(true);
    textFont.setBold(true);
    painter.setFont(textFont);

    //scale font size to fit heigth
    //down
    while ( ( QFontMetrics(textFont).lineSpacing() * 13 ) > height() )
        textFont.setPointSizeF( textFont.pointSizeF() - 0.5 );
    //up
    while ( ( ( QFontMetrics(textFont).lineSpacing() * 14 ) < height() ) &&
            ( QFontMetrics(textFont).boundingRect(QString("LC OFF   knock detection OFF")).width() < width() ) )
        textFont.setPointSizeF( textFont.pointSizeF() + 0.5 );

    QFontMetrics fm = painter.fontMetrics();

    h = 0;
    w = 0;
    h = fm.height() + fm.leading();
    painter.drawText( QPoint(0,h), caption );

    painter.end();
}
