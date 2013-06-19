#ifndef SPLINEEDIT_H
#define SPLINEEDIT_H

#include <QWidget>
#include "BackEnd/Properties/splineproperty.h"

class SplineEdit : public QWidget
{
    Q_OBJECT
public:
    explicit SplineEdit(SplineProperty* data, QWidget *parent = 0);
    SplineProperty* data() const { return _data; }

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void pointsChanged();

private:
    SplineProperty* _data;
    QPoint _topLeft;
    QSize _size;
    QPoint map(QPointF p);
    QPointF reMap(QPoint p);

    int _grabbedIndex = -1;
    int getIndexBelow(QPoint p);
    
};

#endif // SPLINEEDIT_H
