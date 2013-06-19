#include "splineedit.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

SplineEdit::SplineEdit(SplineProperty* data, QWidget *parent) :
    QWidget(parent)
{
    _data = data;
    _topLeft = QPoint(5,5);
}

void SplineEdit::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    _size = QSize(this->width() - 10, this->height() - 10);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawRect(QRect(_topLeft, _size));
    painter.setBrush(Qt::black);
    for (int i = 0; i < _data->points().size(); i++) {
        painter.drawEllipse(map(_data->points()[i]), 3, 3);
    }
    qreal stepwide = 1.0/_size.width();
    QPainterPath curve(map(QPoint(0,_data->getValue(0))));
    _data->update();
    for (qreal x = 0; x <= 1; x += stepwide) {
        curve.lineTo(map(QPointF(x, _data->getValue(x))));
    }
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(curve);
    painter.restore();

    QWidget::paintEvent(event);
}

QPoint SplineEdit::map(QPointF p)
{
    return QPoint(
                p.x() * _size.width() + _topLeft.x(),
                (1 - p.y()) * _size.height() + _topLeft.y()
                );

}

QPointF SplineEdit::reMap(QPoint p)
{
    return QPointF(
                (double)(p.x() - _topLeft.x())/_size.width(),
                1-(double)(p.y() - _topLeft.y())/_size.height()
                );
}


void SplineEdit::mousePressEvent(QMouseEvent *event)
{
    _grabbedIndex = -1;
    for (int i = 0; i < _data->points().size(); i++) {
        qreal dist = (map(_data->points()[i]) - event->pos()).manhattanLength();
        if (dist < 5) {
            _grabbedIndex = i;
            qDebug() << "Treffer";
            return;
        }
    }
}

void SplineEdit::mouseReleaseEvent(QMouseEvent *event)
{
    _grabbedIndex = -1;
}

void SplineEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (_grabbedIndex < 0) return;
    _data->setPoint(_grabbedIndex, reMap(event->pos()));
    _data->update();
    update();
}

void SplineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    _data->addPoint(reMap(event->pos()));
    _data->update();
    update();
}
