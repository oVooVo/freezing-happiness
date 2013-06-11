#ifndef GRADIENTEDIT_H
#define GRADIENTEDIT_H

#include <QWidget>
#include <QGradient>

class GradientEdit : public QWidget
{
    Q_OBJECT
public:
    GradientEdit(QWidget* parent = 0);
    QList<QPair<QColor, qreal>> gradient() const;
    void setCurrentPosition(qreal r);
    void setCurrentColor(QColor r);

signals:
    void positionChanged(qreal);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QList<QPair<QColor,qreal>> _colors;
    int _startPosition;
    int _width;
    int _sliderY;
    int _grabbedIndex = -1;
    void paintSlider(QPainter &painter);
    void paintGradient(QPainter &painter);
    int relToAbs(qreal rel);
    qreal absToRel(int abs);
    int itemAt(QPoint pos);
    QGradient::InterpolationMode _interpolationMode;
};

#endif // GRADIENTEDIT_H
