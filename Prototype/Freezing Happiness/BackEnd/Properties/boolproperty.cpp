#include "boolproperty.h"
#include <QCheckBox>
#include <QDebug>
REGISTER_DEFN_PROPERTYTYPE(BoolProperty);

BoolProperty::BoolProperty(QByteArray* data)
{
    QString className, category, name;
    QDataStream in(data, QIODevice::ReadOnly);
    quint8 vb;
    in >> className >> category >> name >> vb;
    _value = (bool) vb;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
}

BoolProperty::BoolProperty(QString category, QString name, bool value)
{
    setName(name);
    setCategory(category);
    _value = value;
}

QByteArray BoolProperty::toByteArray()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << type() << category() << name();
    out << (quint8) _value;
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
    QCheckBox* checkBox = new QCheckBox(parent);

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
        connect(checkBox, &QCheckBox::clicked, [=]() {
            boolProp->setValue(checkBox->isChecked());
        });

        connect(boolProp, &BoolProperty::valueChanged, updateCheckBox);
    }

    updateCheckBox();

    checkBox->setText(name);
    return checkBox;
}




