#ifndef PATHOBJECT_H
#define PATHOBJECT_H

#include <QPainterPath>
#include "BackEnd/Objects/object.h"

class PathObject : public Object
{
    Q_OBJECT
protected:
    PathObject(Project* project, QString name);
    virtual void updatePath() = 0;
    QTransform getLocaleTransformAt(qreal pos);
    void customDraw(QPainter &p);
    QPainterPath _path;
    virtual void polish();

public slots:
    void emitObjectChanged();
};

#endif // PATHOBJECT_H
