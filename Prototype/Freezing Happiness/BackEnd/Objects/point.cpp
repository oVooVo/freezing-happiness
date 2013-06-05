#include "point.h"

REGISTER_DEFN_OBJECTTYPE(Point);

Point::Point(Project *p, QString name) : Object(p, name)
{
    polish();
}
