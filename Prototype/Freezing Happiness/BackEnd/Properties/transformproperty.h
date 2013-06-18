#ifndef TRANSFORMPROPERTY_H
#define TRANSFORMPROPERTY_H

#include "compositeproperty.h"

class TransformProperty : public CompositeProperty
{
    Q_OBJECT
public:
    TransformProperty(QByteArray *data);
    TransformProperty(QString category, QString name);
    ~TransformProperty();
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);


    QString toString() const;
    QPointF position() const;
    qreal scalation() const;
    qreal rotation() const;
    QTransform transform() const;

public slots:
    void setX(qreal x);
    void setY(qreal y);
    void setTransform(QTransform t);
    void setPosition(QPointF pos);
    void setScalation(qreal scale);
    void setRotation(qreal r);

private:
    void init();
    REGISTER_DECL_PROPERTYTYPE(TransformProperty);
};

#endif // TRANSFORMPROPERTY_H
