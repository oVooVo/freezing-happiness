#include "compositeproperty.h"
#include <QDebug>

CompositeProperty::CompositeProperty(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);

    QString className, category, name;
    quint16 size;
    stream >> className >> category >> name >> size;
    //Q_ASSERT(className == type());    //type() returns always CompositeProperty!
    setCategory(category);
    setName(name);


    QList<QByteArray> childrenData;
    stream >> childrenData;

    for (QByteArray array : childrenData) {
        Property* p = Property::createInstance(&array);
        addProperty(p->name(), p);
    }
}

Property* CompositeProperty::addProperty(QString key, Property *property)
{
    property->setProject(project());
    _properties.insert(key, property);
    connect(property, SIGNAL(valueChanged()), this, SLOT(emitValueChanged()));
    return property;
}

QByteArray CompositeProperty::toByteArray()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    stream << type() << category() << name() << (quint16) _properties.size();

    QList<QByteArray> childrenData;

    for (Property* p : properties()) {
        childrenData.append(p->toByteArray());
    }
    stream << childrenData;

    return array;
}

QList<Property*> CompositeProperty::getChildProperties(QString key, QList<Property*> props)
{
    QList<Property*> children;
    for (Property* property : props) {
        children.append(((CompositeProperty*) property)->property(key));
    }
    return children;
}

void CompositeProperty::emitValueChanged()
{
    emit valueChanged();
}
