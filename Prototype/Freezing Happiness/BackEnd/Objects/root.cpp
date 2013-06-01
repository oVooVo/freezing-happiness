#include "root.h"

REGISTER_DEFN_OBJECTTYPE(Root);

Root::Root(Project *project, QString name) : Object(project, name, true)
{
    polish();
}
