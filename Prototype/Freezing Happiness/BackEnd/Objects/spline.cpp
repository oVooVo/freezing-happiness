#include "spline.h"
#include "point.h"
#include "BackEnd/Tags/pointtag.h"

REGISTER_DEFN_OBJECTTYPE(Spline);

Spline::Spline(Project* project, QString name) : Object(project, name)
{
    polish();
}

void Spline::customDraw(QPainter &p)
{
    QList<QPointF> points;
    QList<Object*> extras;
    for (Object* child : directChildren()) {
        if (child->type() == "Point") {
            points.append(child->localePosition());
        } else if (child->hasTag("PointTag")) {
            extras.append(child);
        }
    }
    for (Object* extraObject : extras) {
        points.insert(((PointTag*) extraObject->tag("PointTag"))->index(), extraObject->localePosition());
    }

    for (int i = 0; i < points.size() - 1; i++) {
        p.drawLine(points[i], points[i+1]);
    }
}


