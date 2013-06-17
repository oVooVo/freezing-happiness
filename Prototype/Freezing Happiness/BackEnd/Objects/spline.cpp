#include "spline.h"
#include "point.h"
#include "BackEnd/Tags/pointtag.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Tags/styletag.h"
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
    for (int i = 0; i < _points.size() - 1; i++) {
        p.drawLine(_points[i], _points[i+1]);
    }
}

void Spline::updatePath()
{
    _points.clear();
    QList<Object*> extras;
    for (Object* child : directChildren()) {
        if (child->type() == "Point") {
            _points.append(child->localePosition());
        } else if (child->hasTag("PointTag")) {
            extras.append(child);
        }
    }
    qSort(extras.begin(), extras.end(), [](Object* a, Object* b)
        { return ((PointTag*) a->tag("PointTag"))->index() > ((PointTag*) b->tag("PointTag"))->index(); } );
    for (Object* extraObject : extras) {
        _points.insert(((PointTag*) extraObject->tag("PointTag"))->index(), extraObject->localePosition());
    }

    if (((BoolProperty*) properties()["closed"])->value() && !_points.isEmpty()) {
        _points.append(_points.first());
    }
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


