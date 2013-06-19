#include "cloner.h"
#include <QDebug>
#include "BackEnd/Properties/integerproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/transformproperty.h"
#include "BackEnd/Properties/splineproperty.h"

REGISTER_DEFN_OBJECTTYPE(Cloner);
const QStringList Cloner::MODES = QStringList() << tr("Linear") << tr("Grid") << tr("Circle");

Cloner::Cloner(Project* project, QString name) : Object(project, name, false)
{
    polish();
    addProperty("mode", new SelectProperty(tr("Cloner"), tr("Mode"), 0, MODES));

    addProperty("count", new IntegerProperty(tr("Cloner"), tr("Count")));
    addProperty("offset", new TransformProperty(tr("Cloner"), tr("Offset")));
    addProperty("curve", new SplineProperty(tr("Cloner"), tr("Curve")));
}

void Cloner::customDraw(QPainter &p)
{
    if (!valid()) return;
    for (int i = 0; i < 10; i++) {
        directChildren()[i % directChildren().size()]->customDraw(p);
        p.translate(100, 0);
    }
}

bool Cloner::valid() const
{
    return directChildren().size() > 0;
}

