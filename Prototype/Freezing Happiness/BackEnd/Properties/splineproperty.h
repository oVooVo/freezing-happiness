#ifndef SPLINEPROPERTY_H
#define SPLINEPROPERTY_H

#include "BackEnd/Properties/property.h"


class SplineProperty : public Property
{
    Q_OBJECT
public:
    SplineProperty(QByteArray *data);
    SplineProperty(QString category, QString name);
    qreal getValue(qreal x);
    QByteArray toByteArray();
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);

    void addPoint(QPointF p);
    void setPoint(int index, QPointF p);
    QList<QPointF> points() const { return _points; }
    void update();

private:
    void calcCoef();


private:
    REGISTER_DECL_PROPERTYTYPE(SplineProperty);
    QList<QPointF> _points;
    QList<qreal> _coef;
};

#endif // SPLINEPROPERTY_H
