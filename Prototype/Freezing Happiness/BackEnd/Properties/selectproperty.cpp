#include "selectproperty.h"
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>

REGISTER_DEFN_PROPERTYTYPE(SelectProperty);

SelectProperty::SelectProperty(QByteArray* data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString className, category, name;
    QStringList options;
    quint64 index;
    stream >> className >> category >> name >> options >> index;
    Q_ASSERT(type() == className);
    setCategory(category);
    setName(name);
    _options = options;
    _currentIndex = index;
}

QByteArray SelectProperty::toByteArray()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << type() << category() << name() << _options << _currentIndex;
    return data;
}

SelectProperty::SelectProperty(QString category, QString name, quint64 index, QStringList options)
{
    setCategory(category);
    setName(name);
    _currentIndex = index;
    _options = options;
}

void SelectProperty::setCurrentIndex(quint64 index)
{
    if (index == _currentIndex) return;

    _currentIndex = index;
    emit valueChanged();
}

QWidget* SelectProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QString name = props.first()->name();
    QComboBox* optionsCB = new QComboBox(parent);
    optionsCB->addItems(((SelectProperty*) props.first())->_options);
    optionsCB->setAcceptDrops(false);

    auto updateComboBox = [=]() {
        bool multipleValues = false;
        quint64 index = ((SelectProperty*) props.first())->currentIndex();

        foreach (Property* p, props) {
            SelectProperty* selectProp = (SelectProperty*) p;
            if (selectProp->currentIndex() != index) {
                multipleValues = true;
            }
        }

        optionsCB->setCurrentIndex((int) index);
        if (multipleValues) {
            optionsCB->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            optionsCB->setStyleSheet("");
        }

    };

    //void (QComboBox:: *currentIndexChangedInt)(int index) = (*QComboBox::currentIndexChanged)(int index);

    void (QComboBox:: *currentIndexChangedInt)(int) = &QComboBox::currentIndexChanged;

    foreach (Property* p, props) {
        SelectProperty* selectProp = (SelectProperty*) p;
        connect(optionsCB, currentIndexChangedInt, [=](int index) {
            selectProp->setCurrentIndex((quint64) index);
        });

        connect(selectProp, &SelectProperty::valueChanged, updateComboBox);
    }

    updateComboBox();

    if (name.isEmpty()) {
        return optionsCB;
    } else {
        QWidget* w = new QWidget(parent);
        optionsCB->setParent(w);
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(new QLabel(QString("%1:").arg(name), parent));
        layout->addWidget(optionsCB);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        w->setLayout(layout);
        return w;
    }
}
