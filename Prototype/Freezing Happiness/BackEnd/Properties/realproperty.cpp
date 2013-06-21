#include "realproperty.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include "FrontEnd/EditorWidgets/doubleedit.h"

REGISTER_DEFN_PROPERTYTYPE(RealProperty);

RealProperty::RealProperty(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString className, category, name;
    stream >> className >> category >> name;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
    stream >> _min >> _max >> _value >> _singleStep;
}

RealProperty::RealProperty(QString category, QString name, qreal min, qreal max, qreal value, qreal singleStep)
{
    init(category, name, min, max, value, singleStep);
}
RealProperty::RealProperty(QString category, QString name, qreal min, qreal value, qreal singleStep)
{
    init(category, name, min, std::numeric_limits<qreal>::max(), value, singleStep);
}

RealProperty::RealProperty(QString category, QString name, qreal value, qreal singleStep)
{
    init(category, name, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), value, singleStep);
}

RealProperty::RealProperty(QString category, QString name, qreal value)
{
    init(category, name, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), value, 1);
}

RealProperty::RealProperty(QString category, QString name)
{
    init(category, name, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), 0, 1);
}

void RealProperty::init(QString category, QString name, qreal min, qreal max, qreal value, qreal singleStep)
{
    _min = min < max ? min : max;
    _max = min < max ? max : min;
    _value = value > max ? max : value < min ? min : value;
    _singleStep = singleStep > 0 ? singleStep : std::numeric_limits<qreal>::min();
    setCategory(category);
    setName(name);
}

QByteArray RealProperty::toByteArray()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << type() << category() <<  name() << _min << _max << _value << _singleStep;
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
    DoubleEdit* spinBox = new DoubleEdit(parent);
    spinBox->setRange(((RealProperty*) props.first())->_min, ((RealProperty*) props.first())->_max);
    spinBox->setSingleStep(((RealProperty*) props.first())->_singleStep);

    auto updateSpinBox = [=]() {
        bool multipleValues = false;
        qreal value = ((RealProperty*) props.first())->value();

        foreach (Property* p, props) {
            RealProperty* realProp = (RealProperty*) p;
            if (!qFuzzyCompare(realProp->value(), value)) {
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

    foreach (Property* p, props) {
        RealProperty* realProp = (RealProperty*) p;
        connect(spinBox, &QDoubleSpinBox::editingFinished, [=]() {
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


