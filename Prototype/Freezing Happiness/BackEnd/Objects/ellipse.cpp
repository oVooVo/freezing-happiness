#include "ellipse.h"
#include "BackEnd/Properties/realproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include <QDebug>
#include "qmath.h"

REGISTER_DEFN_OBJECTTYPE(Ellipse);

Ellipse::Ellipse(Project* project, QString name) : PathObject(project, name)
{
    addProperty("xrad", new RealProperty("Ellipse", "x-Radius", 0, 10, 1));
    addProperty("yrad", new RealProperty("Ellipse", "y-Radius", 0, 10, 1));
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

void Ellipse::updatePath()
{
    _path = QPainterPath();
    _path.addEllipse(QPointF(0,0), ((RealProperty*) properties()["xrad"])->value(), ((RealProperty*) properties()["yrad"])->value());
}
