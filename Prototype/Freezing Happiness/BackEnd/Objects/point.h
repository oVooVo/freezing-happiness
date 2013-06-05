#ifndef POINT_H
#define POINT_H

#include "object.h"
class Point : public Object
{

    Q_OBJECT
public:
    Point(Project* p, QString name = "Point");
    bool canHaveChildren() const { return false; }
    bool visibleInObjectTree() const { return false; }

private:
    REGISTER_DECL_OBJECTTYPE(Point);
};

#endif // POINT_H
