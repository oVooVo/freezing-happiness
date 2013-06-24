#include "pointtag.h"
#include <QSpinBox>
#include <QVBoxLayout>
#include <QDebug>
#include "BackEnd/Properties/integerproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/realproperty.h"

REGISTER_DEFN_TAGTYPE(PointTag);

PointTag::PointTag(Object* owner, QByteArray *data)
{
    setOwner(owner);
    if (data) {
        QDataStream stream(data, QIODevice::ReadOnly);
        QString className;
        stream >> className;
        restoreProperties(stream);
        Q_ASSERT(className == type());
    } else {
        addProperty("Index", new IntegerProperty("PointTag", tr("Index"), 0, std::numeric_limits<int>::max()));

        addProperty("xctrlA", new RealProperty("Point", tr("x Left"), 10));
        addProperty("yctrlA", new RealProperty("Point", tr("y Left"), 0));
        addProperty("xctrlB", new RealProperty("Point", tr("x Right"), -10));
        addProperty("yctrlB", new RealProperty("Point", tr("y Right"), 0));
    }
}

void PointTag::setIndex(qint64 ind)
{
    if (ind == index()) return;

    ((IntegerProperty*) property("Index"))->setValue(ind);
    emit valueChanged();
}

QWidget* PointTag::createWidget(QList<Tag *> tags, QWidget *parent)
{
    QWidget* w = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(Tag::closeButton(tags, parent));
    layout->addWidget(IntegerProperty::createWidget(Tag::getProperties("Index", tags), w));
    w->setLayout(layout);
    return w;
}

qint64 PointTag::index() const
{
    return ((IntegerProperty*) property("Index"))->value();
}

QPointF PointTag::ctrlAPosition() const
{
    return QPointF(((RealProperty*) property("xctrlA"))->value(),
                   ((RealProperty*) property("yctrlA"))->value());
}

QPointF PointTag::ctrlBPosition() const
{
    return QPointF(((RealProperty*) property("xctrlB"))->value(),
                   ((RealProperty*) property("yctrlB"))->value());
}

QPointF PointTag::ctrlA() const
{
    QTransform t = owner()->localeTransform().translate(ctrlAPosition().x(), ctrlAPosition().y());
    return QPointF(t.dx(), t.dy());
}

QPointF PointTag::ctrlB() const
{
    QTransform t = owner()->localeTransform().translate(ctrlBPosition().x(), ctrlBPosition().y());
    return QPointF(t.dx(), t.dy());
}

QPointF PointTag::globaleCtrlA()
{
    QTransform t = owner()->globaleTransform().translate(ctrlAPosition().x(), ctrlAPosition().y());
    return QPointF(t.dx(), t.dy());
}


QPointF PointTag::globaleCtrlB()
{
    QTransform t = owner()->globaleTransform().translate(ctrlBPosition().x(), ctrlBPosition().y());
    return QPointF(t.dx(), t.dy());
}

void PointTag::setGlobaleCtrlA(QPointF p)
{
    QTransform t = owner()->globaleTransformInverted().translate(p.x(), p.y());
    ((RealProperty*) property("xctrlA"))->setValue(t.dx());
    ((RealProperty*) property("yctrlA"))->setValue(t.dy());
}

void PointTag::setGlobaleCtrlB(QPointF p)
{
    QTransform t = owner()->globaleTransformInverted().translate(p.x(), p.y());
    ((RealProperty*) property("xctrlB"))->setValue(t.dx());
    ((RealProperty*) property("yctrlB"))->setValue(t.dy());
}

