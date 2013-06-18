#include "realproperty.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

REGISTER_DEFN_PROPERTYTYPE(RealProperty);

RealProperty::RealProperty(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString className, category, name;
    stream >> className >> category >> name;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
    stream >> _min >> _max >> _value;
}

RealProperty::RealProperty(QString category, QString name, qreal min, qreal max, qreal value)
{
    _min = min < max ? min : max;
    _max = min < max ? max : min;
    _value = value > max ? max : value < min ? min : value;
    setCategory(category);
    setName(name);
}

QByteArray RealProperty::toByteArray()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << type() << category() <<  name() << _min << _max << _value;
    return array;
}

QString RealProperty::toString() const
{
    return QString("RealProperty(%1,%2,%3)").arg(_min).arg(_value).arg(_max);
}

void RealProperty::setValue(qreal value)
{
    if (_value == value) return;

    _value = value;
    emit valueChanged();
}

QWidget* RealProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QString name = props.first()->name();
    QDoubleSpinBox* spinBox = new QDoubleSpinBox(parent);
    spinBox->setRange(((RealProperty*) props.first())->_min, ((RealProperty*) props.first())->_max);

    auto updateSpinBox = [=]() {
        bool multipleValues = false;
        qreal value = ((RealProperty*) props.first())->value();

        foreach (Property* p, props) {
            RealProperty* realProp = (RealProperty*) p;
            if (realProp->value() != value) {
                multipleValues = true;
            }
        }

        spinBox->setValue(value);
        if (multipleValues) {
            spinBox->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            spinBox->setStyleSheet("");
        }

    };

    void (QDoubleSpinBox:: *valueChanged)(qreal) = &QDoubleSpinBox::valueChanged;

    foreach (Property* p, props) {
        RealProperty* realProp = (RealProperty*) p;
        connect(spinBox, valueChanged, [=]() {
            realProp->setValue(spinBox->value());
        });

        connect(realProp, &RealProperty::valueChanged, updateSpinBox);
    }

    updateSpinBox();

    if (name.isEmpty()) {
        return spinBox;
    } else {
        QWidget* w = new QWidget(parent);
        spinBox->setParent(w);
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(new QLabel(QString("%1:").arg(name), parent));
        layout->addWidget(spinBox);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        w->setLayout(layout);
        return w;
    }
}


