#ifndef REALPROPERTY_H
#define REALPROPERTY_H

#include "property.h"

class RealProperty : public Property
{
    Q_OBJECT
public:
    RealProperty(QByteArray *data);
    RealProperty(QString category, QString name, qreal min, qreal max,
                 qreal value, qreal singleStep);
    RealProperty(QString category, QString name, qreal min, qreal value, qreal singleStep);
    RealProperty(QString category, QString name, qreal value, qreal singleStep);
    RealProperty(QString category, QString name, qreal value);
    RealProperty(QString category, QString name);
    QByteArray toByteArray();
    QString toString() const;
    void init(QString category, QString name, qreal min, qreal max, qreal value, qreal singleStep);
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);
    qreal value() const { return _value; }

public slots:
    void setValue(qreal value);

private:
    REGISTER_DECL_PROPERTYTYPE(RealProperty);
    qreal _min;
    qreal _max;
    qreal _value;
    qreal _singleStep;
};

#endif // REALPROPERTY_H
