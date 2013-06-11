#ifndef SELECTPROPERTY_H
#define SELECTPROPERTY_H

#include "property.h"

class SelectProperty : public Property
{
    Q_OBJECT
public:    

    SelectProperty(QByteArray *data);
    SelectProperty(QString category, QString name, quint64 index, QStringList options);
    QByteArray toByteArray();
    QString toString();
    void setCurrentIndex(quint64 index);
    QString item() const { return _options[_currentIndex]; }
    quint64 currentIndex() const { return _currentIndex; }
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);

private:
    REGISTER_DECL_PROPERTYTYPE(SelectProperty);
    quint64 _currentIndex;
    QStringList _options;
};

#endif // SELECTPROPERTY_H
