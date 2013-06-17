#ifndef CLONER_H
#define CLONER_H

#include "object.h"

class Cloner : public Object
{
public:
    Cloner(Project *project, QString name = "Cloner");
    void customDraw(QPainter &p);
    bool valid() const;
protected:
    bool drawChildren() const { return false; }
private:
    REGISTER_DECL_OBJECTTYPE(Cloner);
    static const QStringList MODES;
};

#endif // CLONER_H
