#include "instance.h"
#include "BackEnd/Properties/referenceproperty.h"
#include <QDebug>

REGISTER_DEFN_OBJECTTYPE(Instance);

Instance::Instance(Project *project, QString name) : Object(project, name)
{
    addProperty("reference", new ReferenceProperty("Instance", "Reference"));
    polish();
}

void Instance::customDraw(QPainter &p)
{
    Object* original = project()->getObject(((ReferenceProperty*) properties()["reference"])->id());
    if (original) {
        p.save();
        original->paint(p);
        p.restore();
    }
}

bool Instance::valid()
{
    Object* original = project()->getObject(((ReferenceProperty*) properties()["reference"])->id());
    if (original == 0) return false;
    return !isDescedantOf(original);
}
