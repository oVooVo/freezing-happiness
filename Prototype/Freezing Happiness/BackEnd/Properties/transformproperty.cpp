#include "transformproperty.h"
#include "realproperty.h"
#include <stdlib.h>
#include <QVBoxLayout>
#include "BackEnd/mathutility.h"
#include <QDebug>
#include <QLabel>

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

TransformProperty::TransformProperty(QString category, QString name, qreal x, qreal y, qreal rot, qreal scale)
{
    setCategory(category);
    setName(name);
    addProperty("x", new RealProperty(category, "x", x));
    addProperty("y", new RealProperty(category, "y", y));
    addProperty("Rotation", new RealProperty(category, "Rotation", rot, 0.1));
    addProperty("Scalation", new RealProperty(category, "Scalation", scale, 0.1));
}

QWidget* TransformProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QWidget* par = new QWidget(parent);
    QWidget* label = new QLabel(props.first()->name(), parent);
    QWidget* xWidget = RealProperty::createWidget(getChildProperties("x", props), par);
    QWidget* yWidget = RealProperty::createWidget(getChildProperties("y", props), par);
    QWidget* rotWidget = RealProperty::createWidget(getChildProperties("Rotation", props), par);
    QWidget* scalWidget = RealProperty::createWidget(getChildProperties("Scalation", props), par);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(label);
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
    qreal rot = ((RealProperty*) property("Rotation"))->value() * M_PI /180 ;
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
    ((RealProperty*) property("Rotation"))->setValue(MathUtility::rotation(t) * 180 * M_1_PI);
    ((RealProperty*) property("Scalation"))->setValue(MathUtility::scalation(t));
}


void TransformProperty::setX(qreal x)
{
    ((RealProperty*) property("x"))->setValue(x);
}

void TransformProperty::setY(qreal y)
{
    ((RealProperty*) property("y"))->setValue(y);
}

void TransformProperty::setScalation(qreal scale)
{
((RealProperty*) property("Rotation"))->setValue(scale);
}

void TransformProperty::setRotation(qreal r)
{
    ((RealProperty*) property("Rotation"))->setValue(r);
}

QPointF TransformProperty::position() const
{
    return QPointF(((RealProperty*) property("x"))->value(), ((RealProperty*) property("y"))->value());
}

qreal TransformProperty::rotation() const
{
    return ((RealProperty*) property("Rotation"))->value();
}

qreal TransformProperty::scalation() const
{
    return ((RealProperty*) property("Scalation"))->value();
}

void TransformProperty::setPosition(QPointF pos)
{
    setX(pos.x());
    setY(pos.y());
}

