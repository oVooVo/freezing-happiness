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
        path.addEllipse(ctrlAPosition(), s, s);
        path.addEllipse(ctrlBPosition(), s, s);
        p.fillPath(path, Qt::black);
        p.drawLine(ctrlAPosition(), QPointF(0,0));
        p.drawLine(ctrlBPosition(), QPointF(0,0));
        p.restore();
    }
}

QPointF Point::ctrlAPosition() const
{
    return QPointF(((RealProperty*) properties()["xctrlA"])->value(),
                   ((RealProperty*) properties()["yctrlA"])->value());
}

QPointF Point::ctrlBPosition() const
{
    return QPointF(((RealProperty*) properties()["xctrlB"])->value(),
                   ((RealProperty*) properties()["yctrlB"])->value());
}

QPointF Point::ctrlA() const
{
    QTransform t = localeTransform().translate(ctrlAPosition().x(), ctrlAPosition().y());
    return QPointF(t.dx(), t.dy());
}

QPointF Point::ctrlB() const
{
    QTransform t = localeTransform().translate(ctrlBPosition().x(), ctrlBPosition().y());
    return QPointF(t.dx(), t.dy());
}

QPointF Point::globaleCtrlA()
{
    QTransform t = globaleTransform().translate(ctrlAPosition().x(), ctrlAPosition().y());
    return QPointF(t.dx(), t.dy());
}


QPointF Point::globaleCtrlB()
{
    QTransform t = globaleTransform().translate(ctrlBPosition().x(), ctrlBPosition().y());
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


void Point::setLocaleCtrlA(QPointF p)
{
    QTransform t = localeTransform().inverted().translate(p.x(), p.y());
    ((RealProperty*) properties()["xctrlA"])->setValue(t.dx());
    ((RealProperty*) properties()["yctrlA"])->setValue(t.dy());
}

void Point::setLocaleCtrlB(QPointF p)
{
    QTransform t = localeTransform().inverted().translate(p.x(), p.y());
    ((RealProperty*) properties()["xctrlB"])->setValue(t.dx());
    ((RealProperty*) properties()["yctrlB"])->setValue(t.dy());
}
