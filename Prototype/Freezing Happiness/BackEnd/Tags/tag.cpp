#include "tag.h"
#include <QDebug>

QMap<QString, Tag* (*)(QByteArray*)> *Tag::_creatorMap = 0;

Tag::Tag(Object *owner)
{
    _owner = 0;
    setOwner(owner);
}

Tag::Tag(QByteArray data)
{
    Q_UNUSED(data);
}

void Tag::setOwner(Object *owner)
{
    _owner = owner;
}

Tag* Tag::createInstance(QByteArray *data)
{
    if (!_creatorMap)
    {
        Q_ASSERT(false);
        //_creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
    }

    QString className;
    QDataStream in(data, QIODevice::ReadOnly);
    in >> className;

    QMap<QString, Tag* (*)(QByteArray*)>::iterator it = _creatorMap->find(className);
    if (it == _creatorMap->end()) {
        qDebug() << "Classname = " << className << ". I will crash now.";
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(data);
}

QString Tag::type() const
{
    return metaObject()->className();
}

QByteArray Tag::toByteArray() const
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << type();
    return array;
}

QDataStream& operator<<(QDataStream& out, Tag* t)
{
    QByteArray a = t->toByteArray();
    out << a;
    return out;
}

QDataStream& operator>>(QDataStream& in, Tag* &t)
{
    QByteArray a;
    in >> a;
    t = Tag::createInstance(&a);
    return in;
}

