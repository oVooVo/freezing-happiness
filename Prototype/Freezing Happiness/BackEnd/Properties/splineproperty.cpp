#include "splineproperty.h"

REGISTER_DEFN_PROPERTYTYPE(SplineProperty);

SplineProperty::SplineProperty(QByteArray* data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString name, className, category;
    stream >> className >> category >> name >> _points >> _coef;

    Q_ASSERT(className == type());
    setCategory(category);
    setName(name);
}

SplineProperty::SplineProperty(QString category, QString name)
{
    setName(name);
    setCategory(category);
}

QByteArray SplineProperty::toByteArray()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    stream << type() << category() << name() << _points << _coef;

    return array;
}

qreal SplineProperty::getValue(qreal x)
{
    // a0 + a1(x-x0) + a2(x-x0)(x-x1) + ...

    qreal fac = 1;
    qreal sum = 0;

    for (int i = 0; i < _coef.size(); i++) {
        sum += _coef[i] * fac;
        fac *= (x-_points[i].x());
    }
    return sum;
}

void SplineProperty::calcCoef()
{
    _coef.clear();
    for (int i = 0; i < _points.size(); i++) {
        _coef.append(_points[i].y());
    }

    for (int i = 1; i < _points.size(); i++) {
        for (int j = _points.size() - 1; j >= i; j--) {
            _coef[j] = (_coef[j-1] - _coef[j]) / (_points[j-i].x() - _points[j].x());
        }
    }
}

void SplineProperty::addPoint(QPointF p)
{
    bool overwritten = false;
    for (QPointF point : _points) {
        if (qFuzzyCompare(p.x(), point.x())) {
            point.setX(p.x());
            point.setY(p.y());
            overwritten = true;
        }
    }
    if (!overwritten)
        _points.append(p);
}

QWidget* SplineProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    return new QWidget(parent);
}

void SplineProperty::update()
{
    calcCoef();
}

void SplineProperty::setPoint(int index, QPointF p)
{
    _points[index] = p;
}

