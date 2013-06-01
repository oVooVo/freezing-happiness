#ifndef SPLINE_H
#define SPLINE_H

#include <BackEnd/Objects/object.h>

class Spline : public Object
{
    Q_OBJECT
public:
    Spline(Project* project, QString name = "Spline");
    void customDraw(QPainter &p);

private:
    REGISTER_DECL_OBJECTTYPE(Spline);
};

#endif // SPLINE_H
