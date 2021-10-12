#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QTimer>

/**
 * @brief Overlay Widget
 * @broken 2021-07: broken again! :(
 * Works in Android, IOS, Linux, Windows and MacOS (Catalina, not on HighSierra)
 */
class Overlay : public QWidget
{
    Q_OBJECT
public:
    explicit Overlay(QWidget *parent = 0);
    virtual ~Overlay();
    
signals:
    
public slots:

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    //! installs the event filter
    void newParent();
    //! catches resize and child events from parent widget
    bool eventFilter(QObject *obj, QEvent *ev) override;
    //! tracks parent widget changes
    bool event(QEvent *ev) override;

};

class MessageOverlay : public Overlay
{
    Q_OBJECT
public:
    explicit MessageOverlay(QWidget *parent = nullptr,
                            const int drawAlignFlags=Qt::AlignHCenter | Qt::AlignJustify,
                            const QColor &textColor=QColor(Qt::red));
    virtual ~MessageOverlay();
public slots:
    void showMessage (const QString msg, const int forSeconds=0);
    void hideMessage ();
protected:
    void paintEvent(QPaintEvent *event) override;

    bool m_visible = false;
    QString m_text;
    QColor m_textColor = QColor (Qt::red);
    int m_drawAlignFlags = Qt::AlignHCenter | Qt::AlignJustify;
};

#endif // OVERLAY_H
