#include "cloner.h"
#include <QDebug>

REGISTER_DEFN_OBJECTTYPE(Cloner);
QStringList Cloner::MODES = QStringList() << tr("Linear") << tr("Grid") << tr("Circle");

Cloner::Cloner(Project* project, QString name) : Object(project, name, false)
{
    polish();
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

