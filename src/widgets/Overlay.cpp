#include "Overlay.h"

#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QResizeEvent>

Overlay::Overlay(QWidget *parent)
    : QWidget(parent)
{
#if not defined ( Q_OS_IOS )
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::ToolTip | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
#endif
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    newParent();
}

Overlay::~Overlay()
{

}

void Overlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //X
    painter.setPen(QPen(Qt::red));
    painter.drawLine(width()/8, height()/8, 7*width()/8, 7*height()/8);
    painter.drawLine(width()/8, 7*height()/8, 7*width()/8, height()/8);
    //Test String
    painter.setFont({"arial,helvetica", 48});
    painter.drawText(rect(), "TEST 1234", Qt::AlignHCenter | Qt::AlignTop);

    //qDebug() << "Overlay::paintEvent";
}

void Overlay::resizeEvent ( QResizeEvent * event ) {
    QWidget::resizeEvent( event );
    //qDebug() << "Overlay::resizeEvent old=" << event->oldSize() << "  new=" << event->size();
}

void Overlay::newParent()
{
    if ( !parent() )
        return;
    parent()->installEventFilter(this);
    raise();
}

bool Overlay::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == parent()) {
        if (ev->type() == QEvent::Resize)
            resize(static_cast<QResizeEvent*>(ev)->size());
        else if (ev->type() == QEvent::ChildAdded)
            raise();
    }
    return QWidget::eventFilter(obj, ev);
}

bool Overlay::event(QEvent *ev)
{
    if (ev->type() == QEvent::ParentAboutToChange) {
        if ( parent() )
            parent()->removeEventFilter(this);
    } else {
        if (ev->type() == QEvent::ParentChange)
            newParent();
    }
    return QWidget::event(ev);
}

/* =========================== */
MessageOverlay::MessageOverlay(QWidget *parent, const int drawAlignFlags, const QColor &textColor)
    : Overlay(parent)
{
    m_drawAlignFlags = drawAlignFlags;
    m_textColor = textColor;
}

MessageOverlay::~MessageOverlay()
{

}
void MessageOverlay::showMessage(const QString msg, const int forSeconds)
{
    m_text = msg;
    m_visible = true;
    if ( forSeconds > 0 )
        QTimer::singleShot(forSeconds * 1000, this, SLOT(hideMessage()));
}

void MessageOverlay::hideMessage()
{
    m_visible = false;
    update();
}

void MessageOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if ( m_visible ) {
        painter.setPen(m_textColor);
        QFont f;
#if defined (Q_OS_ANDROID) or defined (Q_OS_IOS)
        f.setPointSize(18);
#else
        f.setPointSize(36);
#endif
        //painter.setFont({"arial,helvetica", 18});
        painter.setFont(f);
        painter.drawText(rect(), m_drawAlignFlags, m_text);
    }
}
