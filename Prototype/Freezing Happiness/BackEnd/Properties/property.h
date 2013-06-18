#ifndef PROPERTY_H
#define PROPERTY_H

#include <QByteArray>
#include <QObject>
#include <QMap>
#include <QDataStream>
#include <QWidget>

class Object;
class Property;

template<typename T> Property *createT(QByteArray* data) { return new T(data); }
template<typename T> QWidget *createWidgetT(QList<Property*> props, QWidget* parent)
                                { return T::createWidget(props, parent); }
class Project;
class Property : public QObject
{
    Q_OBJECT
public:
    static const QString MULTIPLE_VALUES_COLOR;
    virtual QByteArray toByteArray();
    QString type() const;
    QString category() const { return _category; }
    QString name() const { return _name; }
    virtual QString toString() const;

    static QWidget* createWidget(QList<Property*> props, QWidget* parent = 0);
    static Property* createInstance(QByteArray* data);
    void setProject(Project* project);
    Project* project() const { return _project; }

signals:
    void valueChanged();

private:
    QString _category;
    QString _name;
    Project* _project = 0;

protected:
    //contains ctor for each Property
    static QMap<QString, Property* (*)(QByteArray*)> *_creatorMap;
    static QMap<QString, QWidget* (*)(QList<Property*>, QWidget*)> *_widgetCreatorMap;
    void setCategory(QString category);
    void setName(QString name);
    Property(QByteArray* data = 0);


};

template<typename T>
struct PropertyRegister : Property
{
    PropertyRegister(QString className) : Property(0)
    {
        if (!_creatorMap)
            _creatorMap = new QMap<QString, Property* (*)(QByteArray*)>();
        if (!_widgetCreatorMap) {
            _widgetCreatorMap = new QMap<QString, QWidget* (*)(QList<Property*>, QWidget*)>();
        }
        _creatorMap->insert(className, &createT<T>);
        _widgetCreatorMap->insert(className, &createWidgetT<T>);
    }
};

QDataStream& operator<<(QDataStream& out, Property* p);
QDataStream& operator>>(QDataStream& in, Property* &p);

#define REGISTER_DECL_PROPERTYTYPE(CLASSNAME) \
    static PropertyRegister<CLASSNAME> reg

#define REGISTER_DEFN_PROPERTYTYPE(CLASSNAME) \
    PropertyRegister<CLASSNAME> CLASSNAME::reg(#CLASSNAME)

#endif // PROPERTY_H
