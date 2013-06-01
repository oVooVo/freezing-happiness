#ifndef OBJECT_H
#define OBJECT_H

#include <QList>
#include <QObject>
#include <QDataStream>
#include "BackEnd/Properties/property.h"
#include "BackEnd/Properties/propertystring.h"
#include <QPainter>

class Tag;
class Object;
class Project;

template<typename T> Object *createT(Project *parent) { return new T(parent); }

class Object : public QObject
{
    Q_OBJECT
    friend class Project;
public:

    enum Mode { AxesMode,       //manipulat objects Axes without effect to geometry or children objects
                ObjectMode      //manipulate Axes, children and geometry
              };

    Object(Project* project, QString name = "Object", bool isRoot = false);
    virtual ~Object();
    QString type() const;
    Project* project() const { return _project; }

    //tree administration
    bool setTreeParent(Object* parent, bool invariantGlobalTransform = true);
    bool moveBelow(Object* above);
    bool moveAbove(Object* below);
    QList<Object*> directChildren() const;  //Returns all Objects o with o->treeParent() == this.
    bool isRoot() const;
    bool deletable() const { return !isRoot(); }

    //object means child.

    Object* treeParent() const { return _parent; }

    //debug
    QString toString() const;
    QString visualize(int level = 0) const;

    //general properties
    QMap<QString, Property*> properties() const { return _properties; }
    bool hasProperty(QString role) const;
    Property* property(QString role) const  { return _properties.value(role, 0); }
    QList<Tag*> tags() const { return _tags; }

    //id administration
    quint64 id() const { return _id; }

    //selection administration
    bool isSelected() const { return _isSelected; }
    void setSelected(bool selected);


    //object specific properties
    QString name() const;
    void setName(QString newName);

    //locale transforms
    void setLocaleTransform(QTransform t);
    QTransform localeTransform() const;
    void setLocalePosition(QPointF pos);
    QPointF localePosition() const;
    void setLocaleScalation(qreal scale);
    qreal localeScalation() const;
    void setLocaleRotation(qreal rot);
    qreal localeRotation() const;

    //globaleTransforms
    void setGlobaleTransform(QTransform t);
    QTransform globaleTransform();
    QTransform globaleTransformInverted();
    void setGlobalePosition(QPointF pos, Mode mode);
    QPointF globalePosition();
    void setGlobaleScalation(qreal scale, Mode mode, QPointF center);
    void setGlobaleScalation(qreal scale, Mode mode);
    qreal globaleScalation();
    void setGlobaleRotation(qreal r, Mode mode, QPointF center);
    void setGlobaleRotation(qreal r, Mode mode);
    qreal globaleRotation();
    bool isAncestorOf(Object* o) const;
    bool isDescedantOf(Object* o) const;
    void serialize(QDataStream &stream);
    static Object* deserialize(QDataStream &stream, Project* project, bool assertId = true,
                               bool recursive = true, quint64 *childrenCount_ = 0);

    //use for serilization issues only!
    void addChild(Object* o);
    void setId(quint64 id);

    //auto generate
    static Object* createInstance(QString className, Project* project);

    //render
    void paint(QPainter &p);
    virtual void customDraw(QPainter &p) { Q_UNUSED(p) }
    virtual bool drawChildren() { return true; }
    virtual bool valid() { return true; }

    //export
    virtual QString toTikz() const;

    void polish();  //some things should be done only if class object has its final classname

    QPointF mapToGlobal(QPointF point);
    QPointF mapToLocal(QPointF point);


public slots:
    void emitObjectChanged();

private:
    Project* _project;
    Object* _parent;
    QList<Object*> _children;
    QMap<QString, Property*> _properties;
    QList<Tag*> _tags;
    quint64 _id;
    bool _isSelected;
    QList<Object*> selectedObjects(); //Returns all selected Children.
    QList<Object*> subjects();        //Returns all objects.
    QList<Object*> selectedParents(); //Returns selected Objects, but never a child of a selected Object.
    QList<Object*> selectedChildren();//Returns selected Objects and every children of a selected Object.
    void setProperties(QMap<QString, Property*> props);
    void setTags(QList<Tag*>) { Q_ASSERT_X(false, "Object::setProperties", "not implemented"); }
    QTransform _globaleTransformCache;
    QTransform _globaleTransformInvertedCache;
    bool _globaleTransformUpToDate;
    bool _globaleTransformInvertedUpToDate;
    void dumpGlobaleTransformationCache();

protected:
    void addProperty(QString key, Property* property);
    //contains ctor for each Object
    static QMap<QString, Object* (*)(Project*)> *_creatorMap;
};

template<typename T>
struct ObjectRegister : Object
{
    ObjectRegister(QString className) : Object(0, 0)
    {
        if (!_creatorMap)
            _creatorMap = new QMap<QString, Object* (*)(Project*)>();
        _creatorMap->insert(className, &createT<T>);
    }
};

#define REGISTER_DECL_OBJECTTYPE(CLASSNAME) \
    static ObjectRegister<CLASSNAME> reg

#define REGISTER_DEFN_OBJECTTYPE(CLASSNAME) \
    ObjectRegister<CLASSNAME> CLASSNAME::reg(#CLASSNAME)

#endif // OBJECT_H
