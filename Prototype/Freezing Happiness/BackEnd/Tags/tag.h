#ifndef TAG_H
#define TAG_H

#include <QObject>
#include "../Objects/object.h"

class Tag;
template<typename T> Tag *createT(Object* owner, QByteArray *data = 0) { return new T(owner, data); }
template<typename T> QWidget *createWidgetT(QList<Tag*> tags, QWidget* parent = 0) { return T::createWidget(tags, parent); }

class Tag : public QObject
{
    Q_OBJECT
public:
    explicit Tag(Object* owner = 0, QByteArray *data = 0);
    Object* owner() const { return _owner; }
    QString type() const;
    virtual QByteArray toByteArray() const;
    static Tag* createInstance(Object *owner, QString className);
    static Tag* createInstance(Object *owner, QByteArray *data);
    static QStringList tags() { return _creatorMap->keys(); }
    static QWidget* createWidget(QList<Tag*> tags, QWidget* parent = 0);
    static QWidget* closeButton(QList<Tag*> tags, QWidget* parent = 0);
    static QList<Property*> getProperties(QString key, QList<Tag *> tags);
    virtual void exec(QPainter &p) { Q_UNUSED(p) }

signals:
    void valueChanged();

protected:
    //contains ctor for each Tag
    static QMap<QString, Tag* (*)(Object*, QByteArray*)> *_creatorMap;
    static QMap<QString, QWidget* (*)(QList<Tag*>, QWidget*)> *_widgetCreatorMap;
    Property* addProperty(QString key, Property* property);
    QList<Property*> properties() const { return _properties.values(); }
    Property* property(QString key) const { return _properties[key]; }
    void setOwner(Object* owner) { _owner = owner; }
    void saveProperties(QDataStream& stream) const;
    void restoreProperties(QDataStream& stream);


private:
    Object* _owner;
    QMap<QString, Property*> _properties;

private slots:
    void emitValueChanged();

public:
    static Object* AUX_OWNER_BUFFER;
};

template<typename T>
struct TagRegister : Tag
{
    TagRegister(QString className) : Tag()
    {
        if (!_creatorMap)
            _creatorMap = new QMap<QString, Tag* (*)(Object*, QByteArray*)>();
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
