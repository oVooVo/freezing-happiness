#include "pointtag.h"
#include <QSpinBox>
#include <QVBoxLayout>
#include <QDebug>
#include "BackEnd/Properties/integerproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Properties/boolproperty.h"

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
        addProperty("This", new BoolProperty("PointTag", tr("This"), true));
        addProperty("Direct children", new BoolProperty("PointTag", tr("direct Children"), false));
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
