#include "spline.h"
#include "point.h"
#include "BackEnd/Tags/pointtag.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/styleproperty.h"

REGISTER_DEFN_OBJECTTYPE(Spline);

Spline::Spline(Project* project, QString name) : Object(project, name)
{
    addProperty("closed", new BoolProperty("Spline", "Closed", false));
    addProperty("pen-style", new SelectProperty("Spline", "Pen style", 0, SelectProperty::PEN_STYLES));
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
    qSort(extras.begin(), extras.end(), [](Object* a, Object* b)
        { return ((PointTag*) a->tag("PointTag"))->index() > ((PointTag*) b->tag("PointTag"))->index(); } );
    for (Object* extraObject : extras) {
        points.insert(((PointTag*) extraObject->tag("PointTag"))->index(), extraObject->localePosition());
    }

    StyleProperty* sp = ((StyleProperty*) properties()["style"]);
    QPen pen;
    pen.setColor(sp->drawColor());
    pen.setWidth(sp->width());
    pen.setStyle(sp->penStyle());
    p.setPen(pen);

    QBrush brush;
    brush.setColor(sp->fillColor());
    brush.setStyle(sp->brushStyle());
    p.setBrush(brush);

    if (((BoolProperty*) properties()["closed"])->value() && !points.isEmpty()) {
        QPolygonF poly;
        for (QPointF point : points) {
            poly.append(point);
        }
        p.drawPolygon(poly);
    } else for (int i = 0; i < points.size() - 1; i++) {
        p.drawLine(points[i], points[i+1]);
    }
}


