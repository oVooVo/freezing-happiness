#include "transformproperty.h"
#include "realproperty.h"
#include <stdlib.h>
#include <QVBoxLayout>
#include "BackEnd/mathutility.h"
#include <QDebug>

REGISTER_DEFN_PROPERTYTYPE(TransformProperty);

TransformProperty::TransformProperty(QByteArray *data) : CompositeProperty(data)
{

}

TransformProperty::~TransformProperty()
{
    for (Property* prop : properties()) {
        delete prop;
    }
}

TransformProperty::TransformProperty(QString category, QString name)
{
    setCategory(category);
    setName(name);
    addProperty("x", new RealProperty(category, "x", -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()));
    addProperty("y", new RealProperty(category, "y", -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()));
    addProperty("Rotation", new RealProperty(category, "Rotation", -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()));
    addProperty("Scalation", new RealProperty(category, "Scalation", std::numeric_limits<qreal>::min(), std::numeric_limits<qreal>::max(), 1));
}

QWidget* TransformProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QWidget* par = new QWidget(parent);
    QWidget* xWidget = RealProperty::createWidget(getChildProperties("x", props), par);
    QWidget* yWidget = RealProperty::createWidget(getChildProperties("y", props), par);
    QWidget* rotWidget = RealProperty::createWidget(getChildProperties("Rotation", props), par);
    QWidget* scalWidget = RealProperty::createWidget(getChildProperties("Scalation", props), par);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(xWidget);
    layout->addWidget(yWidget);
    layout->addWidget(rotWidget);
    layout->addWidget(scalWidget);
    par->setLayout(layout);
    return par;
}


QString TransformProperty::toString() const
{
    return QString("PT[%1,%2,%3|%4,%5,%6|%7,%8,%9]")
            .arg(transform().m11()).arg(transform().m12()).arg(transform().m13())
            .arg(transform().m21()).arg(transform().m22()).arg(transform().m23())
            .arg(transform().m31()).arg(transform().m32()).arg(transform().m33());
}


QTransform TransformProperty::transform() const
{
    qreal x = ((RealProperty*) property("x"))->value();
    qreal y = ((RealProperty*) property("y"))->value();
    qreal rot = ((RealProperty*) property("Rotation"))->value();
    qreal scale = ((RealProperty*) property("Scalation"))->value();

    QTransform t = QTransform(qCos(rot) * scale, -qSin(rot) * scale, 0,
                      qSin(rot) * scale, qCos(rot) * scale, 0,
                      x, y, 1);
    return t;
}

void TransformProperty::setTransform(QTransform t)
{
    ((RealProperty*) property("x"))->setValue(MathUtility::translation(t).rx());
    ((RealProperty*) property("y"))->setValue(MathUtility::translation(t).ry());
    ((RealProperty*) property("Rotation"))->setValue(MathUtility::rotation(t));
    ((RealProperty*) property("Scalation"))->setValue(MathUtility::scalation(t));
}


void TransformProperty::setPosition(QPointF pos)
{
    QTransform t = transform();
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), pos.x(), pos.y(), t.m33());
    setTransform(t);
}

void TransformProperty::setX(qreal x)
{
    QTransform t = transform();
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), x, t.m32(), t.m33());
    setTransform(t);
}

void TransformProperty::setY(qreal y)
{
    QTransform t = transform();
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), t.m31(), y, t.m33());
    setTransform(t);
}

void TransformProperty::setScalation(qreal scale)
{
    QTransform t = transform();
    qreal s = scale / scalation();
    t.setMatrix(t.m11()*s, t.m12()*s, t.m13(),
                t.m21()*s, t.m22()*s, t.m23(),
                t.m31(), t.m32(), t.m33());
    setTransform(t);
}

void TransformProperty::setRotation(qreal r)
{
    QTransform t = transform();
    qreal s = scalation();
    t.setMatrix(qCos(r)*s, -qSin(r)*s, t.m13(),
                qSin(r)*s,  qCos(r)*s, t.m23(),
                t.m31(), t.m32(), t.m33());
    setTransform(t);
}

QPointF TransformProperty::position() const
{
    return MathUtility::translation(transform());
}

qreal TransformProperty::rotation() const
{
    return MathUtility::rotation(transform());
}

qreal TransformProperty::scalation() const
{
    return MathUtility::scalation(transform());
}

