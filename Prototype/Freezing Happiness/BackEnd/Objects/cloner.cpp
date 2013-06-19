#include "cloner.h"
#include <QDebug>
#include "BackEnd/Properties/integerproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/transformproperty.h"
#include "BackEnd/Properties/splineproperty.h"

REGISTER_DEFN_OBJECTTYPE(Cloner);
const QStringList Cloner::MODES = QStringList() << tr("Linear") << tr("Circle");

Cloner::Cloner(Project* project, QString name) : Object(project, name, false)
{
    polish();
    addProperty("mode", new SelectProperty(tr("Cloner"), tr("Mode"), 0, MODES));

    addProperty("count", new IntegerProperty(tr("Cloner"), tr("Count"), 10));
    addProperty("offset", new TransformProperty(tr("Cloner"), tr("Offset"), 100,0,0,0));
    addProperty("xpos-curve", new SplineProperty(tr("Cloner"), tr("Pos x spread")));
    addProperty("ypos-curve", new SplineProperty(tr("Cloner"), tr("Pos y spread")));
    addProperty("rot-curve", new SplineProperty(tr("Cloner"), tr("Rotation spread")));
    addProperty("scale-curve", new SplineProperty(tr("Cloner"), tr("Scalation spread")));
    addProperty("stepmode", new BoolProperty(tr("Cloner"), tr("Step mode"), true));
    addProperty("Align", new BoolProperty(tr("Cloner"), tr("Align"), true));
}

void Cloner::customDraw(QPainter &p)
{
    int count = ((IntegerProperty*) properties()["count"])->value();
    int c = ((BoolProperty*) properties()["stepmode"])->value() ? count : 1;
    for (int i = 0; i < count; i++) {
        qreal x = (double) i / count;
        p.save();
        p.translate(c*((SplineProperty*) properties()["xpos-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->position().x(),
                    c*((SplineProperty*) properties()["ypos-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->position().y());
        p.rotate(c*((SplineProperty*) properties()["rot-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->rotation());
        qreal s = ((SplineProperty*) properties()["scale-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->scalation();
        p.scale(1+c*s, 1+c*s);
        directChildren()[i % directChildren().size()]->paint(p);
        p.restore();
    }
}

bool Cloner::valid() const
{
    return directChildren().size() > 0
            && ((IntegerProperty*) properties()["count"])->value() > 0;
}

