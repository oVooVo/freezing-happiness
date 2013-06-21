#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "pathobject.h"


class Ellipse : public PathObject
{
    Q_OBJECT
public:
    Ellipse(Project *project, QString name = "Ellipse");
    QTransform getLocaleTransformAt(qreal pos);

private:
    REGISTER_DECL_OBJECTTYPE(Ellipse);

protected:
    void adjustProperties();
    void updatePath();
};

#endif // ELLIPSE_H
