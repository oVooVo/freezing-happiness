#ifndef SPLINE_H
#define SPLINE_H

#include <BackEnd/Objects/pathobject.h>

class Spline : public PathObject
{
    Q_OBJECT
public:
    Spline(Project* project, QString name = "Spline");
    bool isPointObject() const { return true; }
    QTransform getLocaleTransformAt(qreal pos);

protected:
    void childrenHasChanged();
    bool eventFilter(QObject *o, QEvent *e);
    void updatePath();

private:
    REGISTER_DECL_OBJECTTYPE(Spline);
};

#endif // SPLINE_H
