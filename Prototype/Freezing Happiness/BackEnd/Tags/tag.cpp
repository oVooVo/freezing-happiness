#include "tag.h"
#include <QDebug>
#include "BackEnd/project.h"
#include <QPushButton>

QMap<QString, Tag* (*)(QByteArray*)> *Tag::_creatorMap = 0;
QMap<QString, QWidget* (*)(QList<Tag*>, QWidget*)> *Tag::_widgetCreatorMap = 0;


Tag::Tag(QByteArray *data)
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

Tag* Tag::createInstance(QString className)
{
    if (!_creatorMap)
    {
        Q_ASSERT(false);
        //_creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
    }

    QMap<QString, Tag* (*)(QByteArray*)>::iterator it = _creatorMap->find(className);
    if (it == _creatorMap->end()) {
        qDebug() << "Classname = " << className << ". I will crash now.";
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(0);
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
            tags.first()->owner()->project()->createNewUndoRecord();
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

void Tag::createUndoRecord(bool force)
{
    if (owner() && owner()->project()) {
        owner()->project()->createNewUndoRecord(force);
    }
}
