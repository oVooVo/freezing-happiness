#ifndef REALPROPERTY_H
#define REALPROPERTY_H

#include "property.h"

class RealProperty : public Property
{
    Q_OBJECT
public:
    RealProperty(QByteArray *data);
    RealProperty(QString category, QString name, qreal min, qreal max, qreal value = 0);
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
};

#endif // REALPROPERTY_H
