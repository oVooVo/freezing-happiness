#include "tag.h"
#include <QDebug>
#include "BackEnd/project.h"
#include <QPushButton>

QMap<QString, Tag* (*)(Object*, QByteArray*)> *Tag::_creatorMap = 0;
QMap<QString, QWidget* (*)(QList<Tag*>, QWidget*)> *Tag::_widgetCreatorMap = 0;
Object* Tag::AUX_OWNER_BUFFER = 0;


Tag::Tag(Object *owner, QByteArray *data)
{
    Q_UNUSED(data);
    _owner = owner;
}

Tag* Tag::createInstance(Object* owner, QByteArray *data)
{
    if (!_creatorMap)
    {
        Q_ASSERT(false);
        //_creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
    }

    QString className;
    QDataStream stream(data, QIODevice::ReadOnly);
    stream >> className;

    QMap<QString, Tag* (*)(Object*, QByteArray*)>::iterator it = _creatorMap->find(className);
    if (it == _creatorMap->end()) {
        qDebug() << "Classname = " << className << ". I will crash now.";
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(owner, data);
}

Tag* Tag::createInstance(Object* owner, QString className)
{
    if (!_creatorMap)
    {
        Q_ASSERT(false);
        //_creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
    }

    QMap<QString, Tag* (*)(Object*, QByteArray*)>::iterator it = _creatorMap->find(className);
    if (it == _creatorMap->end()) {
        qDebug() << "Classname = " << className << ". I will crash now.";
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(owner, 0);
}


QString Tag::type() const
{
    return metaObject()->className();
}

QByteArray Tag::toByteArray() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << type();
    saveProperties(stream);
    return data;
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
    t = Tag::createInstance(Tag::AUX_OWNER_BUFFER, &a);
    return in;
}

QWidget* Tag::createWidget(QList<Tag*> tags, QWidget* parent)
{
    if (!_creatorMap) {
        Q_ASSERT(false);
        //_creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
    }
    if (tags.isEmpty()) {
        return new QWidget(parent);
    }
    QString className = tags.first()->type();
    foreach (Tag* t, tags) {
        Q_ASSERT(t->type() == className);
    }

    QMap<QString, QWidget* (*)(QList<Tag*>, QWidget*)>::iterator it = _widgetCreatorMap->find(className);
    if (it == _widgetCreatorMap->end()) {
        qDebug() << "Classname = " << className << ". I will crash now.";
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(tags, parent);
}

QWidget* Tag::closeButton(QList<Tag *> tags, QWidget *parent)
{
    QPushButton* button = new QPushButton(tr("X"), parent);
    connect(button, &QPushButton::clicked, [=](){
        if (!tags.isEmpty()) {
            tags.first()->owner()->project()->blockSignals(true);
            for (Tag* t : tags) {
                t->owner()->deleteTag(t);
            }
            tags.first()->owner()->project()->blockSignals(false);
            tags.first()->owner()->project()->emitStructureChanged();
        }
    } );
    return button;
}

Property* Tag::addProperty(QString key, Property *property)
{
    property->setProject(owner()->project());
    _properties.insert(key, property);
    connect(property, SIGNAL(valueChanged()), this, SLOT(emitValueChanged()));
    return property;
}

void Tag::emitValueChanged()
{
    emit valueChanged();
}

void Tag::saveProperties(QDataStream &stream) const
{
    stream << _properties;
}

void Tag::restoreProperties(QDataStream &stream)
{
    QMap<QString, Property*> props;
    stream >> props;
    for (QString key : props.keys()) {
        addProperty(key, props[key]);
    }
}

QList<Property*> Tag::getProperties(QString key, QList<Tag *> tags)
{
    QList<Property*> properties;
    for (Tag* tag : tags) {
        properties.append(tag->property(key));
    }
    return properties;
}

void Tag::setOwner(Object *owner)
{
    _owner = owner;
    connect(this, &Tag::valueChanged, [=]() { owner->emitObjectChanged(); });
}
