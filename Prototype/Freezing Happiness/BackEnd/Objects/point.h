#ifndef POINT_H
#define POINT_H

#include "object.h"
class Point : public Object
{

    Q_OBJECT
public:
    Point(Project* p, QString name = "Point");
    bool canHaveChildren() const { return false; }
    bool visibleInObjectTree() const { return false; }
    QPointF ctrlA() const;
    QPointF ctrlB() const;
    QPointF globaleCtrlA();
    QPointF globaleCtrlB();
    void setGlobaleCtrlA(QPointF p);
    void setGlobaleCtrlB(QPointF p);
    void setLocaleCtrlA(QPointF p);
    void setLocaleCtrlB(QPointF p);
    QPointF ctrlAPosition() const;
    QPointF ctrlBPosition() const;

protected:
    void customDraw(QPainter &p);

private:
    REGISTER_DECL_OBJECTTYPE(Point);


};

#endif // POINT_H
