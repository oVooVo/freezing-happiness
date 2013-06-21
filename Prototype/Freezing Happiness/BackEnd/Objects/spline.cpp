#include "spline.h"
#include "point.h"
#include "BackEnd/Tags/pointtag.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Tags/styletag.h"
#include <QEvent>
#include <QDebug>
#include <qmath.h>
#include "BackEnd/Objects/cloner.h"

REGISTER_DEFN_OBJECTTYPE(Spline);

Spline::Spline(Project* project, QString name) : PathObject(project, name)
{
    addProperty("closed", new BoolProperty("Spline", "Closed", false));
    polish();
    installEventFilter(this);
}

void Spline::childrenHasChanged()
{
    updatePath();
}

void Spline::updatePath()
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

    if (((BoolProperty*) properties()["closed"])->value() && !points.isEmpty()) {
        points.append(points.first());
    }

    if (!points.isEmpty()) {
        _path = QPainterPath(points.first());
        for (int i = 1; i < points.size(); i++) {
            _path.lineTo(points[i]);
        }
    } else {
        _path = QPainterPath();
    }
}

bool Spline::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && (e->type() == QEvent::ChildAdded || e->type() == QEvent::ChildRemoved)) {
        updatePath();
    }
    return Object::eventFilter(o,e);
}

QTransform Spline::getLocaleTransformAt(qreal pos)
{
    qreal eps = 0.001;
    qreal s1 = pos - eps;
    qreal s2 = pos + eps;

    if (s1 < 0) {
        s1 = 0;
        s2 = eps;
    } else if (s2 > 1) {
        s1 = 1-eps;
        s2 = 1;
    }


    QPointF p = _path.pointAtPercent(pos);
    QPointF sek1 = _path.pointAtPercent(s1);
    QPointF sek2 = _path.pointAtPercent(s2);
    qreal r = qAtan2((sek1.y() - sek2.y()),
                     (sek1.x() - sek2.x()));
    QTransform t;
    t.translate(p.x(), p.y());
    t.rotate(r * 180 * M_1_PI);
    return t;
}


