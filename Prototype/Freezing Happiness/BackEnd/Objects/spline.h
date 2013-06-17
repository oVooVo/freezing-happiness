#ifndef SPLINE_H
#define SPLINE_H

#include <BackEnd/Objects/object.h>

class Spline : public Object
{
    Q_OBJECT
public:
    Spline(Project* project, QString name = "Spline");
    void customDraw(QPainter &p);
    bool isPointObject() const { return true; }

public slots:
    void emitObjectChanged();

protected:
    void childrenHasChanged();
    bool eventFilter(QObject *o, QEvent *e);

private:
    REGISTER_DECL_OBJECTTYPE(Spline);
    QVector<QPointF> _points;
    void updatePath();
};

#endif // SPLINE_H
