#ifndef INTEGERPROPERTY_H
#define INTEGERPROPERTY_H

#include "property.h"

class IntegerProperty : public Property
{
    Q_OBJECT
public:
    IntegerProperty(QByteArray *data);
    IntegerProperty(QString category, QString name, int min = 0, int max = std::numeric_limits<int>::max(), qint64 value = 0);
    QByteArray toByteArray();
    QString toString() const;
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);
    qint64 value() const { return _value; }

public slots:
    void setValue(qint64 value);

private:
    REGISTER_DECL_PROPERTYTYPE(IntegerProperty);
    qint64 _min;
    qint64 _max;
    qint64 _value;
};

#endif // INTEGERPROPERTY_H
