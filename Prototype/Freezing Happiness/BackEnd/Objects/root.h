#ifndef ROOT_H
#define ROOT_H

#include <BackEnd/Objects/object.h>
#include <QDebug>

class Root : public Object
{
    Q_OBJECT
public:
    Root(Project* project, QString name = "Root");
private:
    REGISTER_DECL_OBJECTTYPE(Root);
};

#endif // ROOT_H
