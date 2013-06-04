#ifndef TAG_H
#define TAG_H

#include <QObject>
#include "../Objects/object.h"

class Tag;
template<typename T> Tag *createT(QByteArray* data = 0) { return new T(data); }
template<typename T> QWidget *createWidgetT(QList<Tag*> tags, QWidget* parent = 0) { return T::createWidget(tags, parent); }

class Tag : public QObject
{
    Q_OBJECT
public:
    explicit Tag(QByteArray *data = 0);
    void setOwner(Object* owner);
    Object* owner() const { return _owner; }
    QString type() const;
    virtual QByteArray toByteArray() const;
    static Tag* createInstance(QByteArray* data);
    static Tag* createInstance(QString className);
    static QStringList tags() { return _creatorMap->keys(); }
    static QWidget* createWidget(QList<Tag*> tags, QWidget* parent = 0);
    virtual void exec(QPainter &p) { Q_UNUSED(p) Q_ASSERT_X(false, "Tag::exec()", "pure virtual call"); }

signals:
    void valueChanged();

protected:
    //contains ctor for each Tag
    static QMap<QString, Tag* (*)(QByteArray*)> *_creatorMap;
    static QMap<QString, QWidget* (*)(QList<Tag*>, QWidget*)> *_widgetCreatorMap;
    void emitValueChanged() { emit valueChanged(); }

    
public:  //TODO private
    Object* _owner;
};

template<typename T>
struct TagRegister : Tag
{
    TagRegister(QString className) : Tag()
    {
        if (!_creatorMap)
            _creatorMap = new QMap<QString, Tag* (*)(QByteArray*)>();
        if (!_widgetCreatorMap)
            _widgetCreatorMap = new QMap<QString, QWidget* (*)(QList<Tag*>, QWidget*)>();
        _creatorMap->insert(className, &createT<T>);
        _widgetCreatorMap->insert(className, &createWidgetT<T>);
    }
};

QDataStream& operator<<(QDataStream& out, Tag* t);
QDataStream& operator>>(QDataStream& in, Tag* &t);

#define REGISTER_DECL_TAGTYPE(CLASSNAME) \
    static TagRegister<CLASSNAME> reg

#define REGISTER_DEFN_TAGTYPE(CLASSNAME) \
    TagRegister<CLASSNAME> CLASSNAME::reg(#CLASSNAME)

#endif // TAG_H
