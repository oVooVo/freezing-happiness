#ifndef BOOLPROPERTY_H
#define BOOLPROPERTY_H

#include "property.h"

class BoolProperty : public Property
{
    Q_OBJECT
public:
    BoolProperty(QByteArray *data = 0);
    BoolProperty(QString category, QString name, bool value);
    QByteArray toByteArray();
    QString toString() const;
    bool value() const { return _value; }
    static QWidget* createWidget(QList<Property*> props, QWidget* parent = 0);

public slots:
    void setValue(bool value);

private:
    REGISTER_DECL_PROPERTYTYPE(BoolProperty);
    bool _value;
};

#endif // BOOLPROPERTY_H
