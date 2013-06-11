#include "spline.h"
#include "point.h"
#include "BackEnd/Tags/pointtag.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/styleproperty.h"
#include <QEvent>

REGISTER_DEFN_OBJECTTYPE(Spline);

Spline::Spline(Project* project, QString name) : Object(project, name)
{
    addProperty("closed", new BoolProperty("Spline", "Closed", false));
    polish();
    installEventFilter(this);
}

void Spline::childrenHasChanged()
{
    updatePath();
}

void Spline::customDraw(QPainter &p)
{
    p.drawPath(_path);
    p.fillPath(_path, p.brush());
}

void Spline::updatePath()
{
    _path = QPainterPath();
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

    if (((BoolProperty*) properties()["closed"])->value() && !points.isEmpty()) {
        points.append(points.first());
    }
    QPolygonF poly;
    for (QPointF point : points) {
        poly.append(point);
    }
    _path.addPolygon(poly);
}

void Spline::emitObjectChanged()
{
    updatePath();
    Object::emitObjectChanged();
}

bool Spline::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && (e->type() == QEvent::ChildAdded || e->type() == QEvent::ChildRemoved)) {
        updatePath();
    }
    return Object::eventFilter(o,e);
}


