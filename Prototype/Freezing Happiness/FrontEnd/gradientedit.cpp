#include "gradientedit.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QColorDialog>

#define GRABBER_WIDTH 8
#define LOWER_MARGIN 5
#define PEAK_HEIGHT 4

GradientEdit::GradientEdit(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(50);
    _colors.append(QPair<QColor,qreal>(Qt::red, 0));
    _colors.append(QPair<QColor,qreal>(Qt::blue, .5));
    _colors.append(QPair<QColor,qreal>(Qt::green, 1));
}

QList<QPair<QColor, qreal> > GradientEdit::gradient() const
{
    return _colors;
}

void GradientEdit::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    _startPosition = 10;
    _width = width() - 2*_startPosition;
    _sliderY = height() - 20;

    paintGradient(painter);
    paintSlider(painter);
    QWidget::paintEvent(event);
}

void GradientEdit::paintSlider(QPainter &painter)
{
    QPen pen;
    pen.setWidth(2);
    painter.setPen(pen);
    for (int i = 0; i < _colors.size(); i++) {
        if (i == _grabbedIndex) {
            pen.setColor(QColor(255, 128, 0));
            painter.setPen(pen);
        } else {
            pen.setColor(QColor(0,0,0));
            painter.setPen(pen);
        }
        int pos = relToAbs(_colors[i].second);
        QPainterPath path;
        path.addPolygon(QPolygon(QVector<QPoint>() << QPoint(pos - GRABBER_WIDTH/2, _sliderY)
                                 << QPoint(pos, _sliderY - PEAK_HEIGHT)
                                 << QPoint(pos + GRABBER_WIDTH/2, _sliderY)
                                 << QPoint(pos + GRABBER_WIDTH/2, height() - LOWER_MARGIN)
                                 << QPoint(pos - GRABBER_WIDTH/2, height() - LOWER_MARGIN)
                                 << QPoint(pos - GRABBER_WIDTH/2, _sliderY)));
        painter.fillPath(path, QBrush(_colors[i].first));
        painter.drawPath(path);
    }
}

void GradientEdit::paintGradient(QPainter &painter)
{
    QLinearGradient grad;
    for (QPair<QColor, qreal> p : _colors) {
        grad.setColorAt(p.second, p.first);
    }
    grad.setStart(_startPosition, 0);
    grad.setFinalStop(_startPosition + _width, 0);
    painter.save();
    painter.setBrush(grad);
    QRect r = QRect(_startPosition, _startPosition, _width, _sliderY - _startPosition);
    painter.drawRect(r);
    painter.restore();
}

int GradientEdit::relToAbs(qreal rel)
{
    if (rel > 1) rel = 1;
    if (rel < 0) rel = 0;
    return rel * _width + _startPosition;
}

qreal GradientEdit::absToRel(int abs)
{
    if (_width == 0) return 0;
    qreal rel = (qreal)(abs - _startPosition)/_width;
    return rel >= 0 ? (rel <= 1 ? rel : 1) : 0;
}

void GradientEdit::mousePressEvent(QMouseEvent *event)
{
    _grabbedIndex = itemAt(event->pos());
    if (_grabbedIndex != -1 && event->button() == Qt::RightButton) {
        QColor color = QColorDialog::getColor(_colors[_grabbedIndex].first, this, "Choose color", QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            _colors[_grabbedIndex].first = color;
            emit positionChanged(absToRel(event->x()));
            update();
        }
    } else if (_grabbedIndex != -1) {
        emit positionChanged(absToRel(event->x()));
        update();
    }
    QWidget::mousePressEvent(event);
}

void GradientEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void GradientEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (_grabbedIndex != -1) {
        qreal r = absToRel(event->pos().x());
        emit positionChanged(r);
        _colors[_grabbedIndex].second = r;
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void GradientEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    int item = itemAt(event->pos());
    if (item != -1) {
        _colors.removeAt(_grabbedIndex);
    } else {
        qreal pos = absToRel(event->x());
        QColor color = grab(QRect(event->x(), _startPosition + 1, 1,1)).toImage().pixel(0,0);
        _colors.append(QPair<QColor, qreal>(color, pos));
        _grabbedIndex = _colors.size() - 1;
        emit positionChanged(absToRel(event->x()));
    }
    update();
    QWidget::mouseDoubleClickEvent(event);
}

int GradientEdit::itemAt(QPoint pos)
{
    if (pos.y() >= _sliderY && pos.y() <= height() - LOWER_MARGIN) {
        for (int i = 0; i < _colors.size(); i++) {
            int abs = relToAbs(_colors[i].second);
            if (pos.x()>= abs - GRABBER_WIDTH/2 && pos.x() <= abs + GRABBER_WIDTH/2) {
                return i;
            }
        }
    }
    return -1;
}

void GradientEdit::setCurrentColor(QColor r)
{
    if (_grabbedIndex < 0) return;
    _colors[_grabbedIndex].first = r;
    update();
}

void GradientEdit::setCurrentPosition(qreal r)
{
    if (_grabbedIndex < 0) return;
    _colors[_grabbedIndex].second = r;
    update();
}
