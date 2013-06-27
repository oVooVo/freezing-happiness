#include "propertystring.h"
#include <QDebug>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <BackEnd/project.h>
#include "FrontEnd/EditorWidgets/textedit.h"

REGISTER_DEFN_PROPERTYTYPE(PropertyString);

PropertyString::PropertyString(QByteArray* data)
{
    QString className, category, name;
    QDataStream in(data, QIODevice::ReadOnly);
    quint8 singleLine;
    in >> className >> category >> name >> _string >> singleLine >> _font;
    _singleLine = (bool) singleLine;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
}

PropertyString::PropertyString(QString category, QString name, QString string, bool singleLine, QFont font)
{
    _font = font;
    _string = string;
    _singleLine = singleLine;
    setCategory(category);
    setName(name);
}

QByteArray PropertyString::toByteArray()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << type() << category() << name() << _string << (quint8) _singleLine << _font;

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
    bool singleLine = ((PropertyString*) props.first())->_singleLine;
    QString name = props.first()->name();
    QWidget* lineEdit;
    if (singleLine)
        lineEdit = new QLineEdit(parent);
    else
        lineEdit = new TextEdit(parent);
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

        singleLine ? ((QLineEdit*) lineEdit)->setText(string) : ((TextEdit*) lineEdit)->setPlainText(string);
        lineEdit->setFont(((PropertyString*) props.first())->_font);
        if (multipleValues) {
            lineEdit->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            lineEdit->setStyleSheet("");
        }

    };

    foreach (Property* p, props) {
        PropertyString* propString = (PropertyString*) p;
        if (singleLine) {
            connect((QLineEdit*) lineEdit, &QLineEdit::editingFinished, [=]() {
                propString->setString(((QLineEdit*) lineEdit)->text());
            });
        } else {
            connect((TextEdit*) lineEdit, &TextEdit::editingFinished, [=]() {
                propString->setString(((TextEdit*) lineEdit)->document()->toPlainText());
            });
        }

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

void PropertyString::setFont(QFont font)
{
    if (_font == font) return;

    _font = font;
    emit valueChanged();
}













