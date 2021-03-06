#include "cloner.h"
#include <QDebug>
#include "BackEnd/Properties/integerproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/transformproperty.h"
#include "BackEnd/Properties/splineproperty.h"
#include "BackEnd/Properties/realproperty.h"
#include <qmath.h>
#include "BackEnd/Properties/referenceproperty.h"
#include <BackEnd/project.h>
#include "BackEnd/mathutility.h"
#include "empty.h"

const QStringList Cloner::LINEAR_PROPERTIES = QStringList() << "offset-begin" << "offset-end" << "xpos-curve" << "ypos-curve" << "linrot-curve" << "linscale-curve" << "stepmode";
const QStringList Cloner::CIRCLE_PROPERTIES = QStringList() << "align" << "start" << "angle-curve" << "end" << "radius" << "radius-curve"
                                                      << "scal" << "rot" << "circscale-curve" << "circrot-curve";
const QStringList Cloner::SPLINE_PROPERTIES = QStringList() << "spline-start" << "spline-end" << "spline-position-curve" << "spline-scale" << "spline-rot"
                                                            << "spline-scale-curve" << "spline-rot-curve" << "spline-align" << "spline-reference";
const QStringList Cloner::MODES = QStringList() << tr("Linear") << tr("Circle") << tr("Spline");

REGISTER_DEFN_OBJECTTYPE(Cloner);

Cloner::Cloner(Project* project, QString name) : Object(project, name, false)
{
    addProperty("mode", new SelectProperty(tr("Cloner"), tr("Mode"), 0, MODES));
    addProperty("count", new IntegerProperty(tr("Cloner"), tr("Count"), 0, std::numeric_limits<int>::max(), 10));

    addProperty("offset-begin", new TransformProperty(tr("Cloner"), tr("Offset"), 0,0,0,0));
    addProperty("offset-end", new TransformProperty(tr("Cloner"), tr("Spread"), 100,0,0,0));
    addProperty("xpos-curve", new SplineProperty(tr("Cloner"), tr("Pos x spread")));
    addProperty("ypos-curve", new SplineProperty(tr("Cloner"), tr("Pos y spread")));
    addProperty("linrot-curve", new SplineProperty(tr("Cloner"), tr("Rotation spread")));
    addProperty("linscale-curve", new SplineProperty(tr("Cloner"), tr("Scalation spread")));
    addProperty("stepmode", new BoolProperty(tr("Cloner"), tr("Step mode"), true));

    addProperty("align", new BoolProperty(tr("Cloner"), tr("Align"), true));
    addProperty("start", new RealProperty(tr("Cloner"), tr("Start")));
    addProperty("angle-curve", new SplineProperty(tr("Cloner"), tr("Angle spread")));
    addProperty("end", new RealProperty(tr("Cloner"), tr("End"), 360));
    addProperty("radius", new RealProperty(tr("Cloner"), tr("Radius"), 0, 100, 1));
    addProperty("radius-curve", new SplineProperty(tr("Cloner"), tr("Radius spread"), SplineProperty::One));
    addProperty("scal", new RealProperty(tr("Cloner"), tr("Scalation"), 0, 0.1));
    addProperty("rot", new RealProperty(tr("Cloner"), tr("Rotation"), 0, 0.1));
    addProperty("circscale-curve", new SplineProperty(tr("Cloner"), tr("Scalation spread")));
    addProperty("circrot-curve", new SplineProperty(tr("Cloner"), tr("Rotaion spread")));

    addProperty("spline-start", new RealProperty(tr("Cloner"), tr("Start"), 0, 100, 0, 1));
    addProperty("spline-end", new RealProperty(tr("Cloner"), tr("End"), 0, 100, 100, 1));
    addProperty("spline-position-curve", new SplineProperty(tr("Cloner"), tr("Position spread"), SplineProperty::Up));
    addProperty("spline-scale", new RealProperty(tr("Cloner"), tr("Scalation"), 0, 0.1));
    addProperty("spline-rot", new RealProperty(tr("Cloner"), tr("Rotation"), 0, 0.1));
    addProperty("spline-scale-curve", new SplineProperty(tr("Cloner"), tr("Scalation spread")));
    addProperty("spline-rot-curve", new SplineProperty(tr("Cloner"), tr("Rotaion spread")));
    addProperty("spline-align", new BoolProperty(tr("Cloner"), tr("Align"), true));
    addProperty("spline-reference", new ReferenceProperty(tr("Cloner"), tr("Spline")));

    updatePropertiesVisibility();
    polish();
}

void Cloner::customDraw(QPainter &p)
{

    if (_dirtyMatrices) updateMatrices();
    for (int i = 0; i < _matrices.size(); i++) {
        p.save();
        p.setMatrix(_matrices[i] * p.matrix());
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

    TransformProperty* sOff = ((TransformProperty*) properties()["offset-begin"]);
    TransformProperty* eOff = ((TransformProperty*) properties()["offset-end"]);
    for (int i = 0; i < count; i++) {
        QMatrix m;
        qreal x = (double) i / count;
        m.translate(sOff->position().x() + c*((SplineProperty*) properties()["xpos-curve"])->getValue(x) * eOff->position().x(),
                    sOff->position().y() + c*((SplineProperty*) properties()["ypos-curve"])->getValue(x) * eOff->position().y());
        m.rotate(sOff->rotation() + c*((SplineProperty*) properties()["linrot-curve"])->getValue(x) * eOff->rotation());
        qreal s = sOff->scalation() + ((SplineProperty*) properties()["linscale-curve"])->getValue(x) * eOff->scalation();
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

void Cloner::alignSpline()
{
    _matrices.clear();
    ReferenceProperty* refProp = (ReferenceProperty*) properties()["spline-reference"];
    if (refProp->isEmpty()) return;
    Object* spline = project()->getObject(refProp->id());
    int count = ((IntegerProperty*) properties()["count"])->value();
    bool align = ((BoolProperty*) properties()["spline-align"])->value();

    qreal c = ((BoolProperty* ) properties()["stepmode"])->value() ? count : 1;
    qreal span = ((RealProperty*) properties()["spline-end"])->value() - ((RealProperty*) properties()["spline-start"])->value();
    qreal start = ((RealProperty*) properties()["spline-start"])->value();

    for (int i = 0; i < count; i++) {
        QMatrix m;
        qreal x = (double)i/count;
        qreal position = start/100.0 + span/100.0 * ((SplineProperty*) properties()["spline-position-curve"])->getValue(x);
        QTransform t = spline->getLocaleTransformAt(position) * spline->globaleTransform() * globaleTransformInverted();

        m.translate(t.dx(), t.dy());
        if (align) {
          m.rotate(MathUtility::rotation(t));
        }
        m.rotate(c*((SplineProperty*) properties()["spline-rot-curve"])->getValue(x) * ((RealProperty*) properties()["spline-rot"])->value());
        qreal s = ((SplineProperty*) properties()["spline-scale-curve"])->getValue(x) * ((RealProperty*) properties()["spline-scale"])->value();
        m.scale(1+c*s, 1+c*s);
        _matrices.append(m);
    }
}

void Cloner::updatePropertiesVisibility()
{
    for (QString lin : LINEAR_PROPERTIES) {
        properties()[lin]->isVisible = (((SelectProperty*) properties()["mode"])->currentIndex() == 0);
    }
    for (QString circ : CIRCLE_PROPERTIES) {
        properties()[circ]->isVisible = (((SelectProperty*) properties()["mode"])->currentIndex() == 1);
    }
    for (QString spline : SPLINE_PROPERTIES) {
        properties()[spline]->isVisible = (((SelectProperty*) properties()["mode"])->currentIndex() == 2);
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
    qDebug() << "updateMatrices";
    if (_dirtyMatrices) {
        updateMatrices();
    }
    return QList<QPointF>();
}

void Cloner::updateMatrices()
{
    if (!_dirtyMatrices)  return;
    if (((SelectProperty*) properties()["mode"])->currentIndex() == 0) {
        alignLinear();
    } else if (((SelectProperty*) properties()["mode"])->currentIndex() == 1) {
        alignCircle();
    } else if (((SelectProperty*) properties()["mode"])->currentIndex() == 2) {
        alignSpline();
    }
    _dirtyMatrices = false;
}

void Cloner::connectPropertyTriggers()
{
    connect(properties()["mode"], &Property::valueChanged, [=]() {
        updatePropertiesVisibility();
    });

    connect((ReferenceProperty*)properties()["spline-reference"], &ReferenceProperty::watchedObjectChanged, [=]() {
        updateMatrices();
    });
}

Object* Cloner::convert()
{
    Object* object = new Empty(project(), name());
    if (!directChildren().isEmpty()) {
        updateMatrices();
        QList<QByteArray> data;
        for (Object* o : directChildren()) {
            qDebug() << "1";
            QByteArray array;
            QDataStream stream(&array, QIODevice::WriteOnly);
            o->serialize(stream);
            data.append(array);
        }
        for (int i = 0; i < _matrices.size(); i++) {
            QDataStream stream(&data[i % data.size()], QIODevice::ReadOnly);
            Object* o = Object::deserialize(stream, project(), false);
            o->setTreeParent(object);
            o->setLocaleTransform(QTransform(_matrices[i]));
        }

    }
    object->setLocaleTransform(localeTransform());
    object->setTreeParent(treeParent());
    return object;
}
