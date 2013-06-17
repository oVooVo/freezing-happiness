#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "object.h"


class Ellipse : public Object
{
    Q_OBJECT
public:
    Ellipse(Project *project, QString name = "Ellipse");
    void customDraw(QPainter &p);

private:
    REGISTER_DECL_OBJECTTYPE(Ellipse);

protected:
    void adjustProperties();
};

#endif // ELLIPSE_H
