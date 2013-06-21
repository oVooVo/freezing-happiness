#include "pathobject.h"
#include <qmath.h>
#include "BackEnd/Tags/styletag.h"

PathObject::PathObject(Project *project, QString name) : Object(project, name, false)
{
}

void PathObject::polish()
{
    Object::polish();
    updatePath();
}

QTransform PathObject::getLocaleTransformAt(qreal pos)
{
    qreal eps = 0.0001;
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
    t.rotate(r);
    return t;
}

void PathObject::customDraw(QPainter &p)
{
    applyStyleOptions(p);
    if (hasTag("StyleTag") && ((StyleTag*) tag("StyleTag"))->brushStyle() != Qt::NoBrush) {
        p.fillPath(_path, p.brush());
    }
    p.drawPath(_path);
}

void PathObject::emitObjectChanged()
{
    updatePath();
    Object::emitObjectChanged();
}


