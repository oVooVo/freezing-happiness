#include "spline.h"

REGISTER_DEFN_OBJECTTYPE(Spline);

Spline::Spline(Project* project, QString name) : Object(project, name)
{
    polish();
}

void Spline::customDraw(QPainter &p)
{
    for (int i = 0; i < directChildren().size() - 1; i++) {
        p.drawLine(directChildren()[i]->localePosition(), directChildren()[i+1]->localePosition());
    }
}


