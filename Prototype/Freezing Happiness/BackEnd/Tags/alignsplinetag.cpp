#include "alignsplinetag.h"
#include "BackEnd/Properties/referenceproperty.h"
#include "BackEnd/Properties/realproperty.h"
#include "BackEnd/project.h"
#include "BackEnd/Properties/boolproperty.h"
#include <QVBoxLayout>

REGISTER_DEFN_TAGTYPE(AlignSplineTag);

AlignSplineTag::AlignSplineTag(Object *owner, QByteArray *data)
{
    setOwner(owner);
    if (data) {
        QDataStream stream(data, QIODevice::ReadOnly);
        QString className;
        stream >> className;
        restoreProperties(stream);
        Q_ASSERT(className == type());
    } else {
        addProperty("object", new ReferenceProperty("AlignSplineTag", tr("Object")));
        addProperty("position", new RealProperty("AlignSplineTag", tr("Position"), 0, 100, 0));
        addProperty("align", new BoolProperty("AlignSplineTag", tr("Align"), true));
    }
}

void AlignSplineTag::exec()
{
    ReferenceProperty* reference =((ReferenceProperty*) property("object"));
    if (!reference->isEmpty()) {
        Object* object = owner()->project()->getObject(reference->id());
        if (!owner()->isAncestorOf(object)) {
            qreal pos = ((RealProperty*) property("position"))->value() / 100.0;
            QTransform t = object->getLocaleTransformAt(pos) * object->globaleTransform();
            if (((BoolProperty*) property("align"))->value())
                owner()->setGlobaleTransform(t);
            else {
                owner()->setGlobalePosition(QPointF(t.dx(), t.dy()));
            }
        }
    }
}

QWidget* AlignSplineTag::createWidget(QList<Tag *> tags, QWidget *parent)
{
    QWidget* w = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(Tag::closeButton(tags, parent));
    layout->addWidget(RealProperty::createWidget(Tag::getProperties("position", tags), w));
    layout->addWidget(ReferenceProperty::createWidget(Tag::getProperties("object", tags), w));
    layout->addWidget(BoolProperty::createWidget(Tag::getProperties("align", tags), w));
    w->setLayout(layout);
    return w;
}
