#include "point.h"
#include "BackEnd/Properties/realproperty.h"

REGISTER_DEFN_OBJECTTYPE(Point);

Point::Point(Project *p, QString name) : Object(p, name)
{
    polish();
    addProperty("xctrlA", new RealProperty("Point", tr("x Left"), 10));
    addProperty("yctrlA", new RealProperty("Point", tr("y Left"), 0));
    addProperty("xctrlB", new RealProperty("Point", tr("x Right"), -10));
    addProperty("yctrlB", new RealProperty("Point", tr("y Right"), 0));
}

void Point::customDraw(QPainter &p)
{
    if (isSelected()) {
        p.save();
        QPen cosPen;
        qreal s = 2/globaleScalation();
        cosPen.setCosmetic(true);
        p.setPen(cosPen);
        QPainterPath path;
        path.addEllipse(ctrlAPrivate(), s, s);
        path.addEllipse(ctrlBPrivate(), s, s);
        p.fillPath(path, Qt::black);
        p.drawLine(ctrlAPrivate(), QPointF(0,0));
        p.drawLine(ctrlBPrivate(), QPointF(0,0));
        p.restore();
    }
}

QPointF Point::ctrlAPrivate() const
{
    return QPointF(((RealProperty*) properties()["xctrlA"])->value(),
                   ((RealProperty*) properties()["yctrlA"])->value());
}

QPointF Point::ctrlBPrivate() const
{
    return QPointF(((RealProperty*) properties()["xctrlB"])->value(),
                   ((RealProperty*) properties()["yctrlB"])->value());
}

QPointF Point::ctrlA() const
{
    QTransform t = localeTransform().translate(ctrlAPrivate().x(), ctrlAPrivate().y());
    return QPointF(t.dx(), t.dy());
}

QPointF Point::ctrlB() const
{
    QTransform t = localeTransform().translate(ctrlBPrivate().x(), ctrlBPrivate().y());
    return QPointF(t.dx(), t.dy());
}

QPointF Point::globaleCtrlA()
{
    QTransform t = globaleTransform().translate(ctrlAPrivate().x(), ctrlAPrivate().y());
    return QPointF(t.dx(), t.dy());
}


QPointF Point::globaleCtrlB()
{
    QTransform t = globaleTransform().translate(ctrlBPrivate().x(), ctrlBPrivate().y());
    return QPointF(t.dx(), t.dy());
}

void Point::setGlobaleCtrlA(QPointF p)
{
    QTransform t = globaleTransformInverted().translate(p.x(), p.y());
    ((RealProperty*) properties()["xctrlA"])->setValue(t.dx());
    ((RealProperty*) properties()["yctrlA"])->setValue(t.dy());
}

void Point::setGlobaleCtrlB(QPointF p)
{
    QTransform t = globaleTransformInverted().translate(p.x(), p.y());
    ((RealProperty*) properties()["xctrlB"])->setValue(t.dx());
    ((RealProperty*) properties()["yctrlB"])->setValue(t.dy());
}
