#include "integerproperty.h"
#include "FrontEnd/EditorWidgets/integeredit.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

REGISTER_DEFN_PROPERTYTYPE(IntegerProperty);

IntegerProperty::IntegerProperty(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString className, category, name;
    stream >> className >> category >> name;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
    stream >> _min >> _max >> _value;
}

IntegerProperty::IntegerProperty(QString category, QString name, int min, int max, qint64 value)
{
    _min = min < max ? min : max;
    _max = min < max ? max : min;
    _value = value > max ? max : value < min ? min : value;
    setCategory(category);
    setName(name);
}

QByteArray IntegerProperty::toByteArray()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << type() << category() <<  name() << _min << _max << _value;
    return array;
}

QString IntegerProperty::toString() const
{
    return QString("IntegerProperty(%1,%2,%3)").arg(_min).arg(_value).arg(_max);
}

void IntegerProperty::setValue(qint64 value)
{
    if (_value == value) return;

    _value = value;
    qDebug() << "value changed int";
    emit valueChanged();
}

QWidget* IntegerProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QString name = props.first()->name();
    IntegerEdit* spinBox = new IntegerEdit(parent);
    spinBox->setRange(((IntegerProperty*) props.first())->_min, ((IntegerProperty*) props.first())->_max);

    auto updateSpinBox = [=]() {
        bool multipleValues = false;
        qint64 value = ((IntegerProperty*) props.first())->value();

        foreach (Property* p, props) {
            IntegerProperty* realProp = (IntegerProperty*) p;
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

    foreach (Property* p, props) {
        IntegerProperty* realProp = (IntegerProperty*) p;
        connect(spinBox, &QSpinBox::editingFinished, [=]() {
            realProp->setValue(spinBox->value());
        });

        connect(realProp, &IntegerProperty::valueChanged, updateSpinBox);
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


