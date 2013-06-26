#include "boolproperty.h"
#include <QCheckBox>
#include <QDebug>
#include <QPushButton>
REGISTER_DEFN_PROPERTYTYPE(BoolProperty);

BoolProperty::BoolProperty(QByteArray* data)
{
    QString className, category, name;
    QDataStream in(data, QIODevice::ReadOnly);
    quint8 vb, tb;
    in >> className >> category >> name >> vb >> tb;
    _value = (bool) vb;
    _isTrigger = (bool) tb;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
}

BoolProperty::BoolProperty(QString category, QString name, bool value, bool isTrigger)
{
    setName(name);
    setCategory(category);
    _value = value;
    _isTrigger = isTrigger;
}

QByteArray BoolProperty::toByteArray()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << type() << category() << name();
    out << (quint8) _value << (quint8) _isTrigger;
    return array;
}

QString BoolProperty::toString() const
{
    return QString("BoolProperty[%1]").arg(value());
}

void BoolProperty::setValue(bool value)
{
    if (value == _value) return;

    _value = value;
    emit valueChanged();
}

QWidget* BoolProperty::createWidget(QList<Property*> props, QWidget* parent)
{

    QString name = props.first()->name();
    QAbstractButton* checkBox;
    if (((BoolProperty*) props.first())->_isTrigger) {
        checkBox = new QPushButton(parent);
    } else {
        checkBox = new QCheckBox(parent);
    }

    auto updateCheckBox = [=]() {
        bool multipleValues = false;
        bool value = ((BoolProperty*) props.first())->value();

        foreach (Property* p, props) {
            BoolProperty* boolProp = (BoolProperty*) p;
            if (boolProp->value() != value) {
                multipleValues = true;
            }
        }

        checkBox->setChecked(value);
        if (multipleValues) {
            checkBox->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            checkBox->setStyleSheet("");
        }

    };

    foreach (Property* p, props) {
        BoolProperty* boolProp = (BoolProperty*) p;
        connect(checkBox, &QAbstractButton::clicked, [=]() {
            if (boolProp->_isTrigger) boolProp->setValue(!boolProp->value());
            boolProp->setValue(checkBox->isChecked());
        });

        connect(boolProp, &BoolProperty::valueChanged, updateCheckBox);
    }

    updateCheckBox();

    checkBox->setText(name);
    return checkBox;
}




