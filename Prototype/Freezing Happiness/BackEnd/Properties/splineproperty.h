#ifndef SPLINEPROPERTY_H
#define SPLINEPROPERTY_H

#include "BackEnd/Properties/property.h"


class SplineProperty : public Property
{
    Q_OBJECT
public:
    enum Initialization { Null, One, Up, Down, None};
    SplineProperty(QByteArray *data);
    SplineProperty(QString category, QString name, Initialization init = Up);
    qreal getValue(qreal x);
    QByteArray toByteArray();
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);

    void addPoint(QPointF p);
    void setPoints(QList<QPointF> points);
    void removePoint(int index);
    void setPoint(int index, QPointF p);
    QList<QPointF> points() const { return _points; }
    void update();


private:
    void calcCoef();


private:
    REGISTER_DECL_PROPERTYTYPE(SplineProperty);
    QList<QPointF> _points;
    QList<qreal> _coef;
    quint64 hash() const;
};

#endif // SPLINEPROPERTY_H
