#ifndef COMPOSITEPROPERTY_H
#define COMPOSITEPROPERTY_H

#include "property.h"

class CompositeProperty : public Property
{
    Q_OBJECT
public:
    QByteArray toByteArray();

protected:
    Property *addProperty(QString key, Property* p);
    CompositeProperty(QByteArray* data);
    CompositeProperty() {}
    QList<Property*> properties() const { return _properties.values(); }
    Property* property(QString key) const { return _properties[key]; }

    static QList<Property*> getChildProperties(QString key, QList<Property *> props);

protected slots:
    void emitValueChanged();

private:
    QMap<QString,Property*> _properties;


};

#endif // COMPOSITEPROPERTY_H
