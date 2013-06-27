#include "mirror.h"
#include "BackEnd/Properties/boolproperty.h"

REGISTER_DEFN_OBJECTTYPE(Mirror);

Mirror::Mirror(Project* project, QString name = "Mirror") : Object(project, name, false)
{
    addProperty("original", new BoolProperty("Mirror", tr("Original"), true));
    addProperty("hor", new BoolProperty("Mirror", tr("Vertical"), true));
    addProperty("vert", new BoolProperty("Mirror", tr("Horizontal"), false));
    polish();
}

void Mirror::customDraw(QPainter &p)
{
    if (((BoolProperty*) properties()["hor"])->value()) {
        p.save();
        p.scale(-1, 1);
        for (Object* o : directChildren()) {
            p.setMatrix(o->localeTransform().toAffine(), true);
            o->paint(p);
        }
        p.restore();
    }

    if (((BoolProperty*) properties()["vert"])->value()) {
        p.save();
        p.scale(1, -1);
        for (Object* o : directChildren()) {
            p.setMatrix(o->localeTransform().toAffine(), true);
            o->paint(p);
        }
        p.restore();
    }

    if (((BoolProperty*) properties()["hor"])->value() &&
            ((BoolProperty*) properties()["vert"])->value()) {
        p.save();
        p.scale(-1, -1);
        for (Object* o : directChildren()) {
            p.setMatrix(o->localeTransform().toAffine(), true);
            o->paint(p);
        }
        p.restore();
    }
}

bool Mirror::drawChildren() const
{
    return ((BoolProperty*) properties()["original"])->value();
}

Object* Mirror::convert()
{
    return new Mirror(project(), name());
}
