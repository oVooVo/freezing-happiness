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
    void smoothTangents();

protected:
    void childrenHasChanged();
    bool eventFilter(QObject *o, QEvent *e);
    void updatePath();
    void connectPropertyTriggers();

private:
    REGISTER_DECL_OBJECTTYPE(Spline);
    QList<Object*> _points;
};

#endif // SPLINE_H
