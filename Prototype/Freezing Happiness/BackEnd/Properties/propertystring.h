#ifndef PROPERTYSTRING_H
#define PROPERTYSTRING_H

#include "property.h"

class PropertyString : public Property
{
    Q_OBJECT
public:
    PropertyString(QByteArray* data);
    PropertyString(QString category, QString name, QString string = "", bool singleLine = true, QFont font = QFont());
    QByteArray toByteArray();
    QString toString() const;

    QString string() const { return _string; }
    static QWidget* createWidget(QList<Property*> props, QWidget* parent = 0);

public slots:
    void setString(QString s);
    void setFont(QFont font);


private:
    REGISTER_DECL_PROPERTYTYPE(PropertyString);
    QString _string;
    QFont _font;
    bool _singleLine = true;
};

#endif // PROPERTYSTRING_H
