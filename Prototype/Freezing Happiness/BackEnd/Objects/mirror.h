#ifndef MIRROR_H
#define MIRROR_H

#include "object.h"

class Mirror : public Object
{
    Q_OBJECT
public:
    Mirror(Project *project, QString name);
    void customDraw(QPainter &p);
    bool drawChildren() const;
    Object* convert();

private:
    REGISTER_DECL_OBJECTTYPE(Mirror);
};

#endif // MIRROR_H
