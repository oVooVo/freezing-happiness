#ifndef TAG_H
#define TAG_H

#include <QObject>
#include "../Objects/object.h"

class Tag;
template<typename T> Tag *createT(QByteArray* data) { return new T(data); }

class Tag : public QObject
{
    Q_OBJECT
public:
    explicit Tag(Object *owner = 0);
    explicit Tag(QByteArray data);
    void setOwner(Object* owner);
    Object* owner() const { return _owner; }
    QString type() const;
    QByteArray toByteArray() const;
    static Tag* createInstance(QByteArray* data);

signals:
    void valueChanged();

protected:
    //contains ctor for each Tag
    static QMap<QString, Tag* (*)(QByteArray*)> *_creatorMap;
    
private:
    Object* _owner;
};

template<typename T>
struct TagRegister : Tag
{
    TagRegister(QString className) : Tag(0)
    {
        if (!_creatorMap)
            _creatorMap = new QMap<QString, Tag* (*)(QByteArray*)>();
        _creatorMap->insert(className, &createT<T>);
    }
};

QDataStream& operator<<(QDataStream& out, Tag* t);
QDataStream& operator>>(QDataStream& in, Tag* &t);

#define REGISTER_DECL_TAGTYPE(CLASSNAME) \
    static TagRegister<CLASSNAME> reg

#define REGISTER_DEFN_TAGTYPE(CLASSNAME) \
    TagRegister<CLASSNAME> CLASSNAME::reg(#CLASSNAME)

#endif // TAG_H
