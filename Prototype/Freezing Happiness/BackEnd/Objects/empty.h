#ifndef EMPTY_H
#define EMPTY_H

#include "object.h"

class Empty : public Object
{
    Q_OBJECT
public:
    Empty(Project *project, QString name = "Empty");

private:
    REGISTER_DECL_OBJECTTYPE(Empty);
};

#endif // EMPTY_H
