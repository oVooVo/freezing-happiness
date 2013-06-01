#ifndef PROPERTYTRANSFORM_H
#define PROPERTYTRANSFORM_H

#include "property.h"
#include <QTransform>

class PropertyTransform : public Property
{
    Q_OBJECT
public:
    PropertyTransform(QString category, QString name, QTransform t = QTransform());
    PropertyTransform(QByteArray* data);
    QByteArray toByteArray();
    QString toString() const;
    QPointF position() const;
    qreal scalation() const;
    qreal rotation() const;

    QTransform transform() const;
    static QWidget* createWidget(QList<Property*> props, QWidget* parent = 0);


public slots:
    void setX(qreal x);
    void setY(qreal y);
    void setTransform(QTransform t);
    void setPosition(QPointF pos);
    void setScalation(qreal scale);
    void setRotation(qreal r);

signals:
    void xPositionChanged();
    void yPositionChanged();
    void rotationChanged();
    void scalationChanged();

private:
    REGISTER_DECL_PROPERTYTYPE(PropertyTransform);
    QTransform _transform;
    QList<qreal> values() const;
};

#endif // PROPERTYTRANSFORM_H
