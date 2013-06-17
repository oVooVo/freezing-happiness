#include "ellipse.h"
#include "BackEnd/Properties/realproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include <QDebug>

REGISTER_DEFN_OBJECTTYPE(Ellipse);

Ellipse::Ellipse(Project* project, QString name) : Object(project, name, false)
{
    addProperty("xrad", new RealProperty("Ellipse", "x-Radius", 0, 100000000, 100));
    addProperty("yrad", new RealProperty("Ellipse", "y-Radius", 0, 100000000, 100));
    addProperty("iscircle", new BoolProperty("Ellipse", "Circle", true));
    polish();
}

void Ellipse::customDraw(QPainter &p)
{
    applyStyleOptions(p);
    p.drawEllipse(QPointF(0,0),
                ((RealProperty*) properties()["xrad"])->value(),
                ((RealProperty*) properties()["yrad"])->value());
}

void Ellipse::adjustProperties()
{
    if (((BoolProperty*) properties()["iscircle"])->value()) {
        ((RealProperty*) properties()["yrad"])->setValue(
                    ((RealProperty*) properties()["xrad"])->value());
    }
}
