#include "propertystring.h"
#include <QDebug>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <BackEnd/project.h>

REGISTER_DEFN_PROPERTYTYPE(PropertyString);

PropertyString::PropertyString(QByteArray* data)
{
    QString className, category, name;
    QDataStream in(data, QIODevice::ReadOnly);
    in >> className >> category >> name >> _string;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
}

PropertyString::PropertyString(QString category, QString name, QString string)
{
    _string = string;
    setCategory(category);
    setName(name);
}

QByteArray PropertyString::toByteArray()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << type() << category() << name();
    out << _string;

    return array;
}

void PropertyString::setString(QString s)
{
    if (s == _string) return;

    _string = s;
    emit valueChanged();
}

QString PropertyString::toString() const
{
    return QString("PropertyString[%1]").arg(_string);
}

QWidget* PropertyString::createWidget(QList<Property*> props, QWidget* parent)
{

    QString name = props.first()->name();
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setAcceptDrops(false);

    auto updateLineEdit = [=]() {
        bool multipleValues = false;
        QString string = ((PropertyString*) props.first())->string();

        foreach (Property* p, props) {
            PropertyString* propString = (PropertyString*) p;
            if (propString->string() != string) {
                multipleValues = true;
            }
        }

        lineEdit->setText(string);
        if (multipleValues) {
            lineEdit->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            lineEdit->setStyleSheet("");
        }

    };

    foreach (Property* p, props) {
        PropertyString* propString = (PropertyString*) p;
        connect(lineEdit, &QLineEdit::editingFinished, [=]() {
            propString->setString(lineEdit->text());
        });

        connect(propString, &PropertyString::valueChanged, updateLineEdit);
    }

    updateLineEdit();

    if (name.isEmpty()) {
        return lineEdit;
    } else {
        QWidget* w = new QWidget(parent);
        lineEdit->setParent(w);
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(new QLabel(QString("%1:").arg(name), parent));
        layout->addWidget(lineEdit);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        w->setLayout(layout);
        return w;
    }
}













