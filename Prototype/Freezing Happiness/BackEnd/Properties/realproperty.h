#ifndef REALPROPERTY_H
#define REALPROPERTY_H

#include "property.h"

class RealProperty : public Property
{
    Q_OBJECT
public:
    RealProperty(QByteArray *data);
    RealProperty(QString category, QString name, qreal min = -std::numeric_limits<qreal>::max(), qreal max = std::numeric_limits<qreal>::max(),
                 qreal value = 0, qreal singleStep = 1);
    QByteArray toByteArray();
    QString toString() const;
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
