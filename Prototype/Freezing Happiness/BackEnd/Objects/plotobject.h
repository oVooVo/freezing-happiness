#ifndef PLOTOBJECT_H
#define PLOTOBJECT_H

#include "pathobject.h"

class PlotObject : public PathObject
{
    Q_OBJECT
public:
    PlotObject(Project* project, QString name = tr("Plot"));
protected:
    void updatePath();
    void updatePropertiesVisibility();
    void connectPropertyTriggers();
private:
    REGISTER_DECL_OBJECTTYPE(PlotObject);
    static const QStringList CARTESIAN;
    static const QStringList POLAR;
};

#endif // PLOTOBJECT_H
