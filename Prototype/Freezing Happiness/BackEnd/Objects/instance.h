#ifndef INSTANCE_H
#define INSTANCE_H

#include "object.h"
#include "BackEnd/project.h"

class Instance : public Object
{
    Q_OBJECT
public:
    Instance(Project* project, QString name = "Instance");
    void customDraw(QPainter &p);
    bool valid() const;

private:
    REGISTER_DECL_OBJECTTYPE(Instance);
};

#endif // INSTANCE_H
