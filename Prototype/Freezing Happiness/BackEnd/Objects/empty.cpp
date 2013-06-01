#include "empty.h"
#include <QDebug>

REGISTER_DEFN_OBJECTTYPE(Empty);

Empty::Empty(Project* project, QString name) : Object(project, name)
{
    polish();
}
