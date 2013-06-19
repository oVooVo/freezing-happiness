#include "splineedit.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QStyleOption>

SplineEdit::SplineEdit(SplineProperty* data, QWidget *parent) :
    QWidget(parent)
{
    _data = data;
    _topLeft = QPoint(0,0);
}

void SplineEdit::paintEvent(QPaintEvent *event)
{
    // apply stylesheet
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    // end stylesheet

    _size = QSize(this->width() - 0, this->height() - 0);

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
    _grabbedIndex = getIndexBelow(event->pos());
}

int SplineEdit::getIndexBelow(QPoint p)
{
    for (int i = 0; i < _data->points().size(); i++) {
        qreal dist = (map(_data->points()[i]) - p).manhattanLength();
        if (dist < 5) {
            return i;
        }
    }
    return -1;
}

void SplineEdit::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    _grabbedIndex = -1;
}

void SplineEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (_grabbedIndex < 0) return;
    _data->setPoint(_grabbedIndex, reMap(event->pos()));
    _data->update();
    emit pointsChanged();
    update();
}

void SplineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    int grab = getIndexBelow(event->pos());
    if (grab < 0) {
        _data->addPoint(reMap(event->pos()));
        _grabbedIndex = _data->points().size() - 1;
        _data->update();
        update();
    } else {
        _data->removePoint(grab);
        _data->update();
        update();
    }
    emit pointsChanged();
}
