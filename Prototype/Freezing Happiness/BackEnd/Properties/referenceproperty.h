#ifndef REFERENCEPROPERTY_H
#define REFERENCEPROPERTY_H

#include "property.h"
#include "BackEnd/Objects/object.h"

class ReferenceProperty : public Property
{
    Q_OBJECT
public:
    ReferenceProperty(QString category, QString name);
    ReferenceProperty(QByteArray* data);

    QByteArray toByteArray();
    void setId(quint64 id, bool isEmpty);
    quint64 id() const { return _id; }
    bool isEmpty() const { return _empty; }
    QString toString();
    static QWidget* createWidget(QList<Property*> props, QWidget* parent = 0);

signals:
    void watchedObjectChanged();    //the object is still the same one, but a property changed.


private:
    REGISTER_DECL_PROPERTYTYPE(ReferenceProperty);

    bool _empty;
    quint64 _id;

private slots:
    void emitWatchedObjectChanged();
};

#endif // REFERENCEPROPERTY_H

