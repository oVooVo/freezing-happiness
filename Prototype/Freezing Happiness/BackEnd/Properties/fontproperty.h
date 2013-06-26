#ifndef FONTPROPERTY_H
#define FONTPROPERTY_H

#include "property.h"

class FontProperty : public Property
{
public:
    FontProperty(QString category, QString name);
    FontProperty(QByteArray *data);
    QByteArray toByteArray();
    QString toString();
    QFont font() const { return _font; }
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);


private:
    REGISTER_DECL_PROPERTYTYPE(FontProperty);
    QFont _font;
};

#endif // FONTPROPERTY_H
