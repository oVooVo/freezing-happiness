#include "instance.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/referenceproperty.h"
#include "BackEnd/project.h"

REGISTER_DEFN_OBJECTTYPE(Instance);

Instance::Instance(Project *project, QString name) : Object(project, name)
{
    addProperty("reference", new ReferenceProperty("Instance", "Reference"));
    addProperty("ownStyle", new BoolProperty("Instance", "Use Own Style", false));
    polish();
}

void Instance::customDraw(QPainter &p)
{
    Object* original = project()->getObject(((ReferenceProperty*) properties()["reference"])->id());
    if (original) {
        p.save();
        applyStyleOptions(p);
        original->paint(p, !hasTag("StyleTag"));
        p.restore();
    }
}

bool Instance::valid() const
{
    Object* original = project()->getObject(((ReferenceProperty*) properties()["reference"])->id());
    if (original == 0) return false;
    return !isDescedantOf(original);
}

Object* Instance::convert()
{
    ReferenceProperty* refProp = (ReferenceProperty*) properties()["reference"];
    if (refProp->isEmpty()) return 0;

    Object* reference = project()->getObject(refProp->id());
    QByteArray data;
    QDataStream readStream(&data, QIODevice::WriteOnly);
    reference->serialize(readStream);

    QDataStream writeStream(&data, QIODevice::ReadOnly);
    Object* copy = Object::deserialize(writeStream, project(), false);
    copy->setTreeParent(treeParent(), false);
    copy->setLocaleTransform(localeTransform());
    copy->setName(name());
    return copy;
}
