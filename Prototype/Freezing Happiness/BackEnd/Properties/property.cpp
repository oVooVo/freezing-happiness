#include "property.h"
#include <QDataStream>
#include <QDebug>

QMap<QString, Property* (*)(QByteArray*)> *Property::_creatorMap = 0;
QMap<QString, QWidget* (*)(QList<Property*>, QWidget*)> *Property::_widgetCreatorMap = 0;

//TODO: store "background-color:yellow" instead of "yellow";
const QString Property::MULTIPLE_VALUES_COLOR = "yellow";

Property::Property(QByteArray *data)
{
    Q_UNUSED(data);
    _category = "none";
    _name = "none";
}

Property* Property::createInstance(QByteArray *data)
{
    if (!_creatorMap)
    {
        Q_ASSERT(false);
        //_creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
    }

    QString className;
    QDataStream in(data, QIODevice::ReadOnly);
    in >> className;

    QMap<QString, Property* (*)(QByteArray*)>::iterator it = _creatorMap->find(className);
    if (it == _creatorMap->end()) {
        qDebug() << "Classname = " << className << ". I will crash now.";
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(data);
}

QString Property::type() const
{
    return metaObject()->className();
}

QByteArray Property::toByteArray()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << type() << category();
    return array;
}

QDataStream& operator<<(QDataStream& out, Property* p)
{
    QByteArray a = p->toByteArray();
    out << a;
    return out;
}

QDataStream& operator>>(QDataStream& in, Property* &p)
{
    QByteArray a;
    in >> a;
    p = Property::createInstance(&a);
    return in;
}

void Property::setCategory(QString category)
{
    _category = category;
}

void Property::setName(QString name)
{
    _name = name;
}

void Property::setProject(Project *project)
{
    _project = project;
}

void Property::setOwner(Object* owner)
{
    _owner = owner;
}


QString Property::toString() const
{
    return type();
}

QWidget* Property::createWidget(QList<Property*> props, QWidget* parent)
{
    if (!_creatorMap) {
        Q_ASSERT(false);
        //_creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
    }
    if (props.isEmpty()) {
        return new QWidget(parent);
    }
    QString className = props.first()->metaObject()->className();
    QString name = props.first()->name();
    foreach (Property* p, props) {
        Q_ASSERT(p->name() == name);
        Q_ASSERT(p->metaObject()->className() == className);
    }

    QMap<QString, QWidget* (*)(QList<Property*>, QWidget*)>::iterator it = _widgetCreatorMap->find(className);
    if (it == _widgetCreatorMap->end()) {
        qDebug() << "Classname = " << className << ". I will crash now.";
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(props, parent);
}
