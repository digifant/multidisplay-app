#ifndef SWIPEGESTURERECOGNIZER_H
#define SWIPEGESTURERECOGNIZER_H

#include <QGestureRecognizer>
#include <QSwipeGesture>

class SwipeGestureRecognizer : public QGestureRecognizer
{
public:
    SwipeGestureRecognizer();


private:
   static const int MINIMUM_DISTANCE = 10;

   typedef QGestureRecognizer parent;

   bool IsValidMove(int dx, int dy);

   qreal ComputeAngle(int dx, int dy);

   virtual QGesture* create(QObject* pTarget);

   virtual QGestureRecognizer::Result recognize(QGesture* pGesture, QObject *pWatched, QEvent *pEvent);

   void reset (QGesture *pGesture);
};


class SwipeGestureUtil {
public:
    static QSwipeGesture::SwipeDirection GetHorizontalDirection(QSwipeGesture *pSwipeGesture);
    static QSwipeGesture::SwipeDirection GetVerticalDirection(QSwipeGesture *pSwipeGesture);
};

#endif // SWIPEGESTURERECOGNIZER_H
