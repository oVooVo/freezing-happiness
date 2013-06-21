#include "cloner.h"
#include <QDebug>
#include "BackEnd/Properties/integerproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/transformproperty.h"
#include "BackEnd/Properties/splineproperty.h"
#include "BackEnd/Properties/realproperty.h"
#include <qmath.h>
#include <BackEnd/project.h>

const QStringList Cloner::LINEAR_PROPERTIES = QStringList() << "offset" << "xpos-curve" << "ypos-curve" << "linrot-curve" << "linscale-curve" << "stepmode";
const QStringList Cloner::CIRCLE_PROPERTIES = QStringList() << "align" << "start" << "angle-curve" << "end" << "radius" << "radius-curve"
                                                      << "scal" << "rot" << "circscale-curve" << "circrot-curve";

REGISTER_DEFN_OBJECTTYPE(Cloner);
const QStringList Cloner::MODES = QStringList() << tr("Linear") << tr("Circle");

Cloner::Cloner(Project* project, QString name) : Object(project, name, false)
{
    addProperty("mode", new SelectProperty(tr("Cloner"), tr("Mode"), 0, MODES));
    addProperty("count", new IntegerProperty(tr("Cloner"), tr("Count"), 0, std::numeric_limits<int>::max(), 10));

    addProperty("offset", new TransformProperty(tr("Cloner"), tr("Offset"), 100,0,0,0));
    addProperty("xpos-curve", new SplineProperty(tr("Cloner"), tr("Pos x spread")));
    addProperty("ypos-curve", new SplineProperty(tr("Cloner"), tr("Pos y spread")));
    addProperty("linrot-curve", new SplineProperty(tr("Cloner"), tr("Rotation spread")));
    addProperty("linscale-curve", new SplineProperty(tr("Cloner"), tr("Scalation spread")));
    addProperty("stepmode", new BoolProperty(tr("Cloner"), tr("Step mode"), true));

    addProperty("align", new BoolProperty(tr("Cloner"), tr("Align"), true));
    addProperty("start", new RealProperty(tr("Cloner"), tr("Start")));
    addProperty("angle-curve", new SplineProperty(tr("Cloner"), tr("Angle spread")));
    addProperty("end", new RealProperty(tr("Cloner"), tr("End"), -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), 360));
    addProperty("radius", new RealProperty(tr("Cloner"), tr("Radius"), 0, std::numeric_limits<qreal>::max(), 100));
    addProperty("radius-curve", new SplineProperty(tr("Cloner"), tr("Radius spread"), SplineProperty::One));
    addProperty("scal", new RealProperty(tr("Cloner"), tr("Scalation"), -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), 0));
    addProperty("rot", new RealProperty(tr("Cloner"), tr("Rotation"), -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), 0));
    addProperty("circscale-curve", new SplineProperty(tr("Cloner"), tr("Scalation spread")));
    addProperty("circrot-curve", new SplineProperty(tr("Cloner"), tr("Rotaion spread")));

    updatePropertiesVisibility();
    polish();
}

void Cloner::customDraw(QPainter &p)
{

    if (_dirtyMatrices) updateMatrices();
    int count = ((IntegerProperty*) properties()["count"])->value();
    for (int i = 0; i < count; i++) {
        p.save();
        p.setMatrix((QTransform(_matrices[i]) * globaleTransform()).toAffine());
        directChildren()[i % directChildren().size()]->paint(p);
        p.restore();
    }

}

bool Cloner::valid() const
{
    return directChildren().size() > 0
            && ((IntegerProperty*) properties()["count"])->value() > 0;
}

void Cloner::alignLinear()
{
    _matrices.clear();
    int count = ((IntegerProperty*) properties()["count"])->value();
    int c = ((BoolProperty*) properties()["stepmode"])->value() ? count : 1;
    for (int i = 0; i < count; i++) {
        QMatrix m;
        qreal x = (double) i / count;
        m.translate(c*((SplineProperty*) properties()["xpos-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->position().x(),
                    c*((SplineProperty*) properties()["ypos-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->position().y());
        m.rotate(c*((SplineProperty*) properties()["linrot-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->rotation());
        qreal s = ((SplineProperty*) properties()["linscale-curve"])->getValue(x) * ((TransformProperty*) properties()["offset"])->scalation();
        m.scale(1+c*s, 1+c*s);
        _matrices.append(m);
    }
}

void Cloner::alignCircle()
{
    _matrices.clear();
    int count = ((IntegerProperty*) properties()["count"])->value();
    qreal r = ((RealProperty*) properties()["radius"])->value();
    bool align = ((BoolProperty*) properties()["align"])->value();

    qreal c = ((BoolProperty* ) properties()["stepmode"])->value() ? count : 1;
    qreal span = ((RealProperty*) properties()["end"])->value() - ((RealProperty*) properties()["start"])->value();
    qreal start = ((RealProperty*) properties()["start"])->value();
    span *= M_PI / 180.0;
    start *= M_PI / 180.0;

    for (int i = 0; i < count; i++) {
        QMatrix m;
        qreal x = (double)i/count;
        qreal alpha = start + span * ((SplineProperty*) properties()["angle-curve"])->getValue(x);
        qreal radius = r * ((SplineProperty*) properties()["radius-curve"])->getValue(x);
        QPointF pos = QPointF(radius * qCos(alpha), radius * qSin(alpha));
        m.translate(pos.x(), pos.y());
        if (align) {
          m.rotate(alpha * M_1_PI * 180);
        }
        m.rotate(c*((SplineProperty*) properties()["circrot-curve"])->getValue(x) * ((RealProperty*) properties()["rot"])->value());
        qreal s = ((SplineProperty*) properties()["circscale-curve"])->getValue(x) * ((RealProperty*) properties()["scal"])->value();
        m.scale(1+c*s, 1+c*s);
        _matrices.append(m);
    }
}

void Cloner::updatePropertiesVisibility()
{
    qDebug() << "updatePropertiesVisibility();";
    for (QString lin : LINEAR_PROPERTIES) {
        properties()[lin]->isVisible = (((SelectProperty*) properties()["mode"])->currentIndex() == 0);
    }
    for (QString circ : CIRCLE_PROPERTIES) {
        properties()[circ]->isVisible = (((SelectProperty*) properties()["mode"])->currentIndex() == 1);
    }
    project()->emitSelectionChanged();
}

void Cloner::emitObjectChanged()
{
    _dirtyMatrices = true;
    Object::emitObjectChanged();
}

QList<QPointF> Cloner::matrices()
{
    if (_dirtyMatrices) {
        updateMatrices();
    }
    return QList<QPointF>();
}

void Cloner::updateMatrices()
{
    _dirtyMatrices = false;
    if (((SelectProperty*) properties()["mode"])->currentIndex() == 0) {
        alignLinear();
    } else {
        alignCircle();
    }
}

void Cloner::connectVisibilityTriggers()
{
    connect(properties()["mode"], &Property::valueChanged, [=]() {
        updatePropertiesVisibility();
    });
}
