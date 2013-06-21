#include "ellipse.h"
#include "BackEnd/Properties/realproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include <QDebug>
#include "qmath.h"

REGISTER_DEFN_OBJECTTYPE(Ellipse);

Ellipse::Ellipse(Project* project, QString name) : PathObject(project, name)
{
    addProperty("xrad", new RealProperty("Ellipse", "x-Radius", 0, 100000000, 10));
    addProperty("yrad", new RealProperty("Ellipse", "y-Radius", 0, 100000000, 10));
    addProperty("iscircle", new BoolProperty("Ellipse", "Circle", true));
    polish();
}

void Ellipse::adjustProperties()
{
    if (((BoolProperty*) properties()["iscircle"])->value()) {
        ((RealProperty*) properties()["yrad"])->setValue(
                    ((RealProperty*) properties()["xrad"])->value());
    }
}

QTransform Ellipse::getLocaleTransformAt(qreal pos)
{
    qreal alpha = pos * 2 * M_PI;
    qreal r1 = ((RealProperty*) properties()["xrad"])->value();
    qreal r2 = ((RealProperty*) properties()["yrad"])->value();
    QTransform trans;
    trans.translate(qCos(alpha) * r1, qSin(alpha) * r2);
    trans.rotate(alpha * 180 * M_1_PI);
    return trans;
}

void Ellipse::updatePath()
{
    _path = QPainterPath();
    _path.addEllipse(QPointF(0,0), ((RealProperty*) properties()["xrad"])->value(), ((RealProperty*) properties()["yrad"])->value());
}
