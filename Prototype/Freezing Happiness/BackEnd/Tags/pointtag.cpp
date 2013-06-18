#include "pointtag.h"
#include <QSpinBox>
#include <QVBoxLayout>

REGISTER_DEFN_TAGTYPE(PointTag);

PointTag::PointTag(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString className;
    stream >> className >> _index;
    Q_ASSERT(className == type());
}

PointTag::PointTag()
{
    _index = 0;
}

QByteArray PointTag::toByteArray() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << type() << _index;
    return data;
}

void PointTag::setIndex(quint64 index)
{
    if (_index == index) return;

    _index = index;
    emit valueChanged();
}

QWidget* PointTag::createWidget(QList<Tag *> tags, QWidget *parent)
{
    QSpinBox* spinBox = new QSpinBox(parent);
    spinBox->setRange(0, INT_MAX);

    auto update = [=]() {
        spinBox->blockSignals(true);
        bool multipleValues = false;
        quint64 index = ((PointTag*) tags.first())->index();
        spinBox->setValue((int) index);
        for (Tag* t : tags) {
            if (((PointTag*) t)->index() != index) multipleValues = true;
        }
        if (multipleValues) {
            spinBox->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            spinBox->setStyleSheet("");
        }
        spinBox->blockSignals(false);
    };

    void (QSpinBox:: *valueChangedInt)(int) = &QSpinBox::valueChanged;

    for (Tag* t : tags) {
        PointTag* pt = (PointTag*) t;
        connect(pt, &Tag::valueChanged, update);
        connect(spinBox, valueChangedInt, [pt](int value) { pt->setIndex((quint64) value); } );
    }
    update();

    QWidget* w = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(Tag::closeButton(tags, parent));
    layout->addWidget(spinBox);
    w->setLayout(layout);
    return w;
}
