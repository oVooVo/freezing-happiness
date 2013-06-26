#include "fontproperty.h"

REGISTER_DEFN_PROPERTYTYPE(FontProperty);

FontProperty::FontProperty(QByteArray *data)
{
}

FontProperty::FontProperty(QString category, QString name)
{

}

QByteArray FontProperty::toByteArray()
{
    return QByteArray();
}

QString FontProperty::toString()
{
    return QString();
}

QWidget* FontProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    return new QWidget(parent);
}
