#include "fontproperty.h"
#include "QPushButton"
#include <QFontDialog>

REGISTER_DEFN_PROPERTYTYPE(FontProperty);

FontProperty::FontProperty(QByteArray *data)
{
    QString className, category, name;
    QDataStream in(data, QIODevice::ReadOnly);
    in >> className >> category >> name >> _font;
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
}

FontProperty::FontProperty(QString category, QString name)
{
    setCategory(category);
    setName(name);
}

QByteArray FontProperty::toByteArray()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << type() << category() << name() << _font;
    return array;
}

QString FontProperty::toString()
{
    return QString();
}

QWidget* FontProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QString name = props.first()->name();
    QAbstractButton* checkBox = new QPushButton(parent);

    QFont value = ((FontProperty*) props.first())->font();
    auto updateCheckBox = [=]() {
        bool multipleValues = false;

        foreach (Property* p, props) {
            FontProperty* fontProp = (FontProperty*) p;
            if (fontProp->font() != value) {
                multipleValues = true;
            }
        }

        if (multipleValues) {
            checkBox->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            checkBox->setStyleSheet("");
        }

    };

    connect(checkBox, &QAbstractButton::clicked, [=]() {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, value, parent, tr("Font"));
        if (ok) {
            foreach (Property* p, props) {
                FontProperty* fontProp = (FontProperty*) p;
                fontProp->setFont(font);
            }
        }
    }
    );
    for (Property* p : props) {
        FontProperty* fontProp = (FontProperty*) p;
        connect(fontProp, &FontProperty::valueChanged, updateCheckBox);
    }

    updateCheckBox();

    checkBox->setText(name);
    return checkBox;
}

void FontProperty::setFont(QFont font)
{
    if (_font == font) return;
    _font = font;
    emit valueChanged();
}
